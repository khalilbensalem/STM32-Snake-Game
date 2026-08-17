/**
 * @file cpp_main.cpp
 * @brief Point d'entrée principal du projet PolySnake.
 *
 * Version refactorisée : orchestration séparée de la logique de jeu.
 *
 * Ce fichier gère :
 * - l'initialisation des périphériques (écran, clavier, IMU, UART, DAC, LED),
 * - le menu de sélection Mario/Luigi,
 * - la synchronisation UART entre les deux cartes,
 * - le système audio (mélodies Mario/Luigi),
 * - la LED RGB selon le personnage,
 * - la boucle principale et le game over,
 * - le mode combat avec capteur ultrason HC-SR04 (2216169, 2216665).
 *
 * La logique de jeu (serpents, fruits, collisions) est dans SnakeGame.
 *
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 *
 * @defgroup ELE3312
 * @{
 */

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "main.h"
#include "cpp_main.h"

#include "NucleoImp/Display/ILI9341Display.h"
#include "NucleoImp/Keypad/GPIOKeypad.h"
#include "NucleoImp/MotionInput/MPU6050MotionInput.h"
#include "NucleoImp/Sound/DACSound.h"
#include "NucleoImp/RGBLight/RGBLED.h"

#include "Game/SnakeGame.h"
#include "Interfaces/Sound/Melody.h"

// Mode combat + ultrason
#include "Game/CombatMode.h"
#include "Game/UltrasonicSensor.h"

extern UART_HandleTypeDef huart5;

using namespace ELE3312;

// Instances des périphériques

static ILI9341Display display;
static GPIOKeypad keypad;
static MPU6050MotionInput imuInput;
static DACSound dacSound;
static RGBLED rgbLed;

// Mode combat et capteur ultrason (2216169 2216665)

static UltrasonicSensor ultrasonicSensor;
static CombatMode combatMode;

// Pointeur global pour les callbacks DMA du DAC

extern DACSound* g_dacSound;

// Pointeur global pour le callback Input Capture (2216169 2216665)
// Utilisé dans stm32f4xx_it.c pour appeler updateDistance depuis l’IRQ TIM2

void* g_ultrasonicSensor = nullptr;

/**
 * @brief Wrapper C pour appeler ultrasonicSensor.updateDistance() depuis le callback.
 * @param value Valeur capturée par le timer (durée de l'impulsion Echo).
 */
extern "C" void ultrasonicSensor_updateDistance(uint32_t value) {
    ultrasonicSensor.updateDistance(value);
}

// Système audio

static uint8_t currentNoteIndex = 0;
static uint32_t noteStartTick = 0;
static bool useMarioMelody = true;

/**
 * @brief Met à jour la mélodie (Mario ou Luigi) en fonction du temps écoulé.
 */
void updateMelody() {
    uint32_t currentTick = HAL_GetTick();

    if (useMarioMelody) {
        if (currentTick - noteStartTick >= marioMelody[currentNoteIndex].duration_ms) {
            currentNoteIndex = (currentNoteIndex + 1) % marioMelodyLength;
            dacSound.setPhaseIncrementDirect(marioMelody[currentNoteIndex].phaseIncrement);
            noteStartTick = currentTick;
        }
    } else {
        if (currentTick - noteStartTick >= luigiMelody[currentNoteIndex].duration_ms) {
            currentNoteIndex = (currentNoteIndex + 1) % luigiMelodyLength;
            dacSound.setPhaseIncrementDirect(luigiMelody[currentNoteIndex].phaseIncrement);
            noteStartTick = currentTick;
        }
    }
}

// UART - Ring buffer (défini dans stm32f4xx_it.c)

extern uint8_t rxByte;
#define RX_BUFFER_SIZE 256
extern uint8_t rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxHead;
extern volatile uint16_t rxTail;

/**
 * @brief Indique si des données UART sont disponibles dans le ring buffer.
 * @return true si au moins un octet est disponible, false sinon.
 */
inline bool uartDataAvailable() {
    return rxHead != rxTail;
}

/**
 * @brief Lit un octet depuis le ring buffer UART.
 * @return Octet lu.
 */
inline uint8_t uartReadByte() {
    uint8_t data = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
    return data;
}

// Lecture clavier

/**
 * @brief Lit l'état du clavier matriciel 4x4.
 *
 * Parcourt les lignes, force une ligne à 0 et lit les colonnes.
 *
 * @param gpio Pointeur vers le port GPIO utilisé pour le clavier.
 * @return Un masque avec un bit à 1 pour la touche appuyée, 0 si aucune.
 */
uint16_t readKeypad(GPIO_TypeDef* gpio) {
    for (int row = 0; row < 4; row++) {
        gpio->ODR = 0xFF & ~(1 << (row + 4));
        HAL_Delay(2);
        uint8_t cols = gpio->IDR & 0x0F;

        for (int col = 0; col < 4; col++) {
            if (!(cols & (1 << col))) {
                return (1 << (row * 4 + col));
            }
        }
    }
    return 0;
}

// Point d'entrée principal

/**
 * @brief Point d'entrée principal C++.
 *
 * Initialise les périphériques, gère le menu de sélection, la synchronisation
 * UART, la boucle de jeu PolySnake et le mode combat avec capteur ultrason.
 *
 * @param handles Pointeurs vers les structures HAL des périphériques.
 */
void cpp_main(peripheral_handles *handles)
{
    // Initialisation des périphériques

    // Écran
    display.setup(handles->hspi_tft);
    display.clearScreen();
    display.drawString(0, 0, "PolySnake - 2216169 2216665");

    // Clavier
    keypad.setup(handles->gpio_keypad);
    HAL_Delay(10);

    // IMU
    imuInput.setup(handles->hi2c);

    // Audio DAC
    g_dacSound = &dacSound;
    dacSound.setup(handles->hdac, handles->htim_dac, 84000000);
    dacSound.setWaveform(Waveform::Sinus);
    dacSound.setAmplitude(50);

    // UART
    HAL_UART_Receive_IT(&huart5, &rxByte, 1);

    // LED RGB (canal 2 = rouge, 3 = vert, 1 = bleu)
    rgbLed.setup(handles->htim_led, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_1);
    rgbLed.turnOn();
    rgbLed.setColorRGB(0, 0, 0);

    // Capteur ultrason HC-SR04 (2216169 2216665)
    // PA0 = ECHO (TIM2_CH1 Input Capture)
    // PA1 = TRIG (GPIO Output)
    ultrasonicSensor.setup(handles->htim_distance, GPIOA, GPIO_PIN_1);
    g_ultrasonicSensor = &ultrasonicSensor;

    // Mode combat (2216169 2216665)
    combatMode.setup(&display, &ultrasonicSensor, &huart5);


    // Boucle principale (retour au menu après chaque partie)

    while (1) {

        // Menu de sélection Mario / Luigi

        display.clearScreen();
        display.drawString(40, 40, "POLYSNAKE MULTIJOUEUR");
        display.drawString(60, 70, "2216169 - 2216665");
        display.drawString(20, 110, "1 = MARIO (rouge/bleu)");
        display.drawString(20, 140, "2 = LUIGI (vert/blanc)");
        display.drawString(50, 180, "Appuyez 1 ou 2...");

        // Démarrer la mélodie Mario par défaut
        useMarioMelody = true;
        currentNoteIndex = 0;
        dacSound.setPhaseIncrementDirect(marioMelody[0].phaseIncrement);
        noteStartTick = HAL_GetTick();
        dacSound.startPlayback();

        // Attendre le choix du joueur local
        uint16_t choice = 0;
        while (choice != 1 && choice != 2) {
            updateMelody();
            uint16_t k = readKeypad(handles->gpio_keypad);
            if (k == 1) choice = 1;
            if (k == 2) choice = 2;
            HAL_Delay(10);
        }

        // Allumer la LED immédiatement selon le personnage
        if (choice == 1) {
            rgbLed.setColorRGB(255, 0, 0);  // Rouge pour Mario
        } else {
            rgbLed.setColorRGB(0, 255, 0);  // Vert pour Luigi
        }

        // Vérification du choix (empêcher même personnage)

        display.clearScreen();
        display.drawString(40, 100, "Verification du choix...");

        while (uartDataAvailable()) uartReadByte();

        uint8_t choiceSignal = 0xB0 + choice;
        uint8_t otherChoice = 0;
        uint32_t choiceStart = HAL_GetTick();

        while (otherChoice == 0 && (HAL_GetTick() - choiceStart < 10000)) {
            updateMelody();
            HAL_UART_Transmit(&huart5, &choiceSignal, 1, 10);

            if (uartDataAvailable()) {
                uint8_t received = uartReadByte();
                if (received == 0xB1 || received == 0xB2) {
                    otherChoice = received - 0xB0;
                }
            }
            HAL_Delay(50);
        }

        if (otherChoice == choice) {
            display.clearScreen();
            display.drawString(20, 80, "ERREUR: Meme personnage!");
            display.drawString(20, 110, "Les deux joueurs ont");
            display.drawString(20, 130, choice == 1 ? "choisi MARIO!" : "choisi LUIGI!");
            display.drawString(20, 170, "Rechoisissez...");
            rgbLed.setColorRGB(0, 0, 0);
            HAL_Delay(3000);
            continue;
        }

        if (otherChoice == 0) {
            display.clearScreen();
            display.drawString(40, 110, "TIMEOUT - L'autre");
            display.drawString(40, 130, "joueur n'a pas choisi");
            rgbLed.setColorRGB(0, 0, 0);
            HAL_Delay(3000);
            continue;
        }

        // Changer la mélodie selon le personnage choisi
        useMarioMelody = (choice == 1);
        currentNoteIndex = 0;
        noteStartTick = HAL_GetTick();
        if (useMarioMelody) {
            dacSound.setPhaseIncrementDirect(marioMelody[0].phaseIncrement);
        } else {
            dacSound.setPhaseIncrementDirect(luigiMelody[0].phaseIncrement);
        }

        // Synchronisation de démarrage

        display.clearScreen();
        display.drawString(40, 100, "En attente de l'autre");
        display.drawString(40, 120, "joueur...");

        while (uartDataAvailable()) uartReadByte();

        uint32_t syncStart = HAL_GetTick();
        uint32_t lastSendTime = 0;
        bool otherReady = false;
        uint8_t readySignal = 0xAA;

        while (!otherReady && (HAL_GetTick() - syncStart < 30000)) {
            updateMelody();

            if (HAL_GetTick() - lastSendTime >= 100) {
                HAL_UART_Transmit(&huart5, &readySignal, 1, 10);
                lastSendTime = HAL_GetTick();
            }

            if (uartDataAvailable()) {
                uint8_t received = uartReadByte();
                if (received == 0xAA) {
                    otherReady = true;
                    HAL_UART_Transmit(&huart5, &readySignal, 1, 10);
                }
            }
            HAL_Delay(5);
        }

        if (!otherReady) {
            display.clearScreen();
            display.drawString(40, 110, "TIMEOUT - Pas de");
            display.drawString(40, 130, "connexion!");
            dacSound.disableSound();
            rgbLed.setColorRGB(0, 0, 0);
            HAL_Delay(3000);
            continue;
        }

        // Countdown 3-2-1-GO!

        for (int i = 0; i < 5; i++) {
            HAL_UART_Transmit(&huart5, &readySignal, 1, 10);
            HAL_Delay(50);
        }

        display.clearScreen();
        display.drawString(120, 100, "3");
        HAL_Delay(500);
        while (uartDataAvailable()) uartReadByte();

        display.drawString(120, 100, "2");
        HAL_Delay(500);
        while (uartDataAvailable()) uartReadByte();

        display.drawString(120, 100, "1");
        HAL_Delay(500);
        while (uartDataAvailable()) uartReadByte();

        display.drawString(100, 100, "GO!");
        HAL_Delay(300);

        while (uartDataAvailable()) uartReadByte();

        display.clearScreen();

        // Création du jeu (SnakeGame)

        bool isMario = (choice == 1);
        SnakeGame game(&display, &keypad, &imuInput, &huart5);
        game.setupPlayers(isMario);
        game.init();

        uint16_t lastKey = 0;
        uint32_t lastMoveTime = HAL_GetTick();
        bool gameOver = false;
        bool won = false;
        const char* deathMessage = nullptr;

        // Boucle de jeu principale

        while (!gameOver) {
            updateMelody();

            // Traitement UART normal (hors combat)

            while (uartDataAvailable()) {
                uint8_t receivedByte = uartReadByte();

                // Début combat
                if (receivedByte == 0xCC) {
                    combatMode.startCombat(isMario);

                    bool combatFinished = false;

                    while (!combatFinished) {

                        while (uartDataAvailable()) {
                            uint8_t combatByte = uartReadByte();
                            if (combatByte != 0xCC) {
                                combatMode.processUartByte(combatByte);
                            }
                        }

                        combatMode.update();

                        if (combatMode.isGameOver()) {
                            combatFinished = true;

                            if (combatMode.getLocalLives() <= 0) {
                                // Le joueur local perd le combat
                                won = false;

                                uint8_t msg = 0xC4;   // j'ai perdu
                                HAL_UART_Transmit(&huart5, &msg, 1, 10);

                            } else {
                                // Le joueur local gagne le combat
                                won = true;

                                uint8_t msg = 0xC3;   // j'ai gagné
                                HAL_UART_Transmit(&huart5, &msg, 1, 10);
                            }

                            gameOver = true;
                        }

                        HAL_Delay(5);
                    }

                    break;
                }

                // Traitement game over venant de l’adversaire
                if (game.processUartByte(receivedByte)) {
                    won = true;
                    gameOver = true;
                    break;
                }
            }

            if (gameOver) break;

            // Lecture des entrées (clavier + IMU)

            uint16_t k = readKeypad(handles->gpio_keypad);
            if (k != lastKey) {
                if (k != 0) {
                    game.processInputs(k);
                } else {
                    game.processInputs(0);  // Utiliser IMU
                }
                lastKey = k;
            } else if (k == 0) {
                game.processInputs(0);
            }

            // Mouvement du serpent toutes les 180 ms

            if (HAL_GetTick() - lastMoveTime >= 180) {
                lastMoveTime = HAL_GetTick();

                // Tick de jeu
                bool ate = game.tick();

                // Envoyer direction
                game.sendDirection(ate);

                // Vérifier collisions
                CollisionType collision = game.checkCollisions();

                if (collision != CollisionType::NONE) {
                    // Envoyer signal de début de combat (2216169 2216665)
                    uint8_t combatStartSignal = 0xCC;
                    for (int i = 0; i < 3; i++) {
                        HAL_UART_Transmit(&huart5, &combatStartSignal, 1, 100);
                        HAL_Delay(10);
                    }


                    // Initialiser le combat
                    combatMode.startCombat(isMario);

                    // Boucle de combat
                    bool combatFinished = false;
                    while (!combatFinished) {
                        updateMelody();  // Continuer la mélodie

                        // Traiter les données UART du combat
                        while (uartDataAvailable()) {
                            uint8_t receivedByte = uartReadByte();

                            // Ignorer les signaux 0xCC répétés
                            if (receivedByte != 0xCC) {
                                combatMode.processUartByte(receivedByte);
                            }
                        }

                        // Mettre à jour l'état du combat
                        CombatState combatState = combatMode.update();
                        (void)combatState; // évite un warning si non utilisé

                        if (combatMode.isGameOver()) {
                            combatFinished = true;

                            if (combatMode.getLocalLives() <= 0) {
                                // Défaite locale
                                gameOver = true;
                                won = false;
                                deathMessage = "DEFAITE AU COMBAT!";
                                // game.sendGameOver();
                            } else {
                                // Victoire locale
                                gameOver = true;      // fin du jeu
                                won = true;
                                deathMessage = "ADVERSAIRE EN DEFAITE!";
                            }
                        }
                        HAL_Delay(10);
                    }
                }
            }

            HAL_Delay(10);
        }

        // Synchronisation du résultat du combat
        // C3 = j’ai gagné, C4 = j’ai perdu

        HAL_Delay(150); // laisser le temps au message d'arriver

        while (uartDataAvailable()) {
            uint8_t result = uartReadByte();

            if (result == 0xC3) {
                // L'autre dit : "j'ai gagne"
                won = false;
            }
            if (result == 0xC4) {
                // L'autre dit : "j'ai perdu"
                won = true;
            }
        }

        // Fin de partie

        dacSound.disableSound();
        rgbLed.setColorRGB(0, 0, 0);
        display.clearScreen();

        if (won) {

            display.drawString(30, 100, "ADVERSAIRE A PERDU!");
            display.drawString(70, 130, "TU AS GAGNE!");

            // LED blanche (PA11)
            HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
            for (int i = 0; i < 3; i++) {
                HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
                HAL_Delay(200);
                HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
                HAL_Delay(200);
            }

        } else {

            display.drawString(30, 100, "DEFAITE AU COMBAT!");
            display.drawString(70, 130, "GAME OVER");

        }

        display.drawString(50, 160, "2216169 2216665");
        HAL_Delay(2000);

        // Décompte retour au menu

        display.clearScreen();
        display.drawString(40, 80, "** FIN DE PARTIE **");
        display.drawString(50, 110, "2216169 2216665");
        display.drawString(30, 150, "Retour au menu dans:");

        for (int countdown = 5; countdown >= 1; countdown--) {
            display.fillRect(Color::BLACK, 150, 180, 30, 20);
            char countStr[2] = {'0' + countdown, '\0'};
            display.drawString(155, 180, countStr);
            HAL_Delay(1000);
        }

        while (uartDataAvailable()) uartReadByte();

    }  // Fin boucle while(1) - retour au menu
}

/**
 * @}
 */ // Fin du groupe ELE3312
