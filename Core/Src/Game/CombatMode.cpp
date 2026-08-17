/**
 * @file CombatMode.cpp
 * @brief Implémentation de la classe CombatMode
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 */

#include "Game/CombatMode.h"
#include "main.h"
#include <cstdio>
#include <cstring>

namespace ELE3312 {

/**
 * @brief Constructeur
 */
CombatMode::CombatMode()
    : display(nullptr), sensor(nullptr), huart(nullptr),
      localLives(2), remoteLives(2), state(CombatState::WAITING),
      isMario(true), timeInZoneStart(0), isInZone(false),
      currentDistance(0.0f), lastMeasurementTime(0)
{
}

/**
 * @brief Configuration du mode combat
 */
void CombatMode::setup(Display *display, UltrasonicSensor *sensor, UART_HandleTypeDef *huart) {
    this->display = display;
    this->sensor = sensor;
    this->huart = huart;

    printf("2216169 2216665 - CombatMode initialise\n");
}

/**
 * @brief Initialise un nouveau combat
 */
void CombatMode::startCombat(bool isMario) {
    this->isMario = isMario;
    localLives = 2;
    remoteLives = 2;
    state = CombatState::FIGHTING;
    isInZone = false;
    timeInZoneStart = 0;
    currentDistance = 0.0f;

    // Effacer l'écran et dessiner l'interface de combat
    display->fillScreen(Color::BLACK);
    drawCombatScreen();

    printf("2216169 2216665 - Combat demarre! Joueur: %s\n", isMario ? "MARIO" : "LUIGI");
}

/**
 * @brief Met à jour le combat
 */
CombatState CombatMode::update() {
    if (state != CombatState::FIGHTING) {
        return state;
    }

    // Déclencher une mesure de distance toutes les 100ms
    uint32_t currentTime = HAL_GetTick();
    if (currentTime - lastMeasurementTime >= MEASUREMENT_INTERVAL) {
        sensor->trigger();
        lastMeasurementTime = currentTime;

        // Attendre un peu pour que la mesure se fasse
        HAL_Delay(10);

        // Récupérer la distance
        currentDistance = sensor->getDistance();

        // Afficher la distance
        drawDistance();
    }

    // Vérifier si on est dans la zone cible (10-11 cm)
    bool inTargetZone = sensor->isInTargetZone();

    if (inTargetZone) {
        if (!isInZone) {
            // On vient d'entrer dans la zone
            isInZone = true;
            timeInZoneStart = HAL_GetTick();
            printf("2216169 2216665 - Entre dans la zone cible!\n");
        } else {
            // On est toujours dans la zone, vérifier le temps écoulé
            uint32_t timeInZone = HAL_GetTick() - timeInZoneStart;

            // Afficher le timer
            drawTimer();

            if (timeInZone >= TIME_TO_WIN) {
                // Victoire du round!
                printf("2216169 2216665 - Round gagne!\n");
                handleRoundEnd(true);
                return state;
            }
        }
    } else {
        if (isInZone) {
            // On est sorti de la zone
            isInZone = false;
            printf("2216169 2216665 - Sorti de la zone cible\n");
        }
    }

    return state;
}

/**
 * @brief Traite les données UART reçues
 */
void CombatMode::processUartByte(uint8_t receivedByte) {
    // Format du message combat:
    // 0xC0 = L'adversaire a gagné le round
    // 0xC1 = L'adversaire a perdu le round

    if (receivedByte == 0xC0) {
        // L'adversaire a gagné → on a perdu
        printf("2216169 2216665 - Adversaire a gagne le round\n");
        handleRoundEnd(false);
    } else if (receivedByte == 0xC1) {
        // L'adversaire a perdu → on a gagné
        printf("2216169 2216665 - Adversaire a perdu le round\n");
        handleRoundEnd(true);
    }
}

/**
 * @brief Gère la fin d'un round
 */
void CombatMode::handleRoundEnd(bool won) {

    // Mettre à jour les vies
    if (won) {
        remoteLives--;
        state = CombatState::ROUND_WON;

        sendCombatData(true);
        display->fillRect(Color::BLACK, 0, 100, 320, 40);
        display->drawString(80, 110, "ROUND GAGNEE!", Color::GREEN);
    } else {
        localLives--;
        state = CombatState::ROUND_LOST;

        sendCombatData(false);
        display->fillRect(Color::BLACK, 0, 100, 320, 40);
        display->drawString(80, 110, "ROUND PERDUE!", Color::RED);
    }

    drawLives();

    // Petite pause pour lire le résultat
    HAL_Delay(300);

    // Fin DIRECTE du combat après une manche
    state = CombatState::GAME_OVER;
    printf("2216169 2216665 - GAME OVER (1 round unique)\n");

    if (won) {
        flashWhiteLedWinner();
    }
}


/**
 * @brief Envoie les données de combat via UART
 */
void CombatMode::sendCombatData(bool wonRound) {
    uint8_t data = wonRound ? 0xC0 : 0xC1;
    HAL_UART_Transmit(huart, &data, 1, 100);
}

/**
 * @brief Dessine l'écran de combat
 */
void CombatMode::drawCombatScreen() {
    // Titre
    display->drawString(80, 5, "MODE COMBAT", Color::YELLOW);

    // Instructions
    display->drawString(20, 30, "Gardez votre main entre", Color::WHITE);
    display->drawString(20, 45, "10-11 cm pendant 3 sec!", Color::WHITE);

    // Zone pour la distance
    display->drawString(20, 70, "Distance:", Color::CYAN);

    // Zone pour les vies
    drawLives();
}

/**
 * @brief Dessine les vies (coeurs)
 */
void CombatMode::drawLives() {
    // Effacer la zone des vies
    display->fillRect(Color::BLACK, 0, 190, 320, 50);

    // Déterminer les couleurs selon le joueur local
    Color localColor   = isMario ? Color::RED   : Color::GREEN;
    Color remoteColor  = isMario ? Color::GREEN : Color::RED;

    // Libellés
    display->drawString(10, 195, isMario ? "MARIO:" : "LUIGI:", Color::WHITE);
    display->drawString(160, 195, isMario ? "LUIGI:" : "MARIO:", Color::WHITE);

    // Dessiner vies locales
    for (int i = 0; i < localLives; i++) {
        display->fillCircle(localColor, 80 + i * 25, 205, 8);
    }

    // Dessiner vies adverses
    for (int i = 0; i < remoteLives; i++) {
        display->fillCircle(remoteColor, 230 + i * 25, 205, 8);
    }
}



void CombatMode::flashWhiteLedWinner() {
    for (int i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
        HAL_Delay(200);
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
        HAL_Delay(200);
    }
}



/**
 * @brief Dessine la distance actuelle
 */
void CombatMode::drawDistance() {
    // Effacer la zone de distance
    display->fillRect(Color::BLACK, 100, 70, 150, 18);

    // Afficher la distance SANS FLOAT dans snprintf
    char distStr[32];
    int intPart = (int)currentDistance;
    int decPart = (int)((currentDistance - intPart) * 10);
    snprintf(distStr, sizeof(distStr), "%d.%d cm", intPart, decPart);

    // Couleur selon si on est dans la zone ou non
    Color distColor = sensor->isInTargetZone() ? Color::GREEN : Color::WHITE;
    display->drawString(100, 70, distStr, distColor);

    // Afficher "ZONE OK!" si dans la zone
    if (sensor->isInTargetZone()) {
        display->drawString(100, 90, "ZONE OK!", Color::GREEN);
    } else {
        display->fillRect(Color::BLACK, 100, 90, 150, 18);
    }
}

/**
 * @brief Dessine le timer
 */
void CombatMode::drawTimer() {
    if (!isInZone) {
        return;
    }

    uint32_t timeInZone = HAL_GetTick() - timeInZoneStart;
    float secondsRemaining = (TIME_TO_WIN - timeInZone) / 1000.0f;

    if (secondsRemaining < 0) {
        secondsRemaining = 0;
    }

    // Effacer la zone du timer
    display->fillRect(Color::BLACK, 100, 120, 150, 18);

    // Afficher le temps restant SANS FLOAT dans snprintf
    char timerStr[32];
    int intPart = (int)secondsRemaining;
    int decPart = (int)((secondsRemaining - intPart) * 10);
    snprintf(timerStr, sizeof(timerStr), "%d.%d sec", intPart, decPart);
    display->drawString(100, 120, timerStr, Color::YELLOW);
}

/**
 * @brief Retourne le nombre de vies locales
 */
int CombatMode::getLocalLives() const {
    return localLives;
}

/**
 * @brief Retourne le nombre de vies distantes
 */
int CombatMode::getRemoteLives() const {
    return remoteLives;
}

/**
 * @brief Vérifie si c'est Game Over
 */
bool CombatMode::isGameOver() const {
    return state == CombatState::GAME_OVER;
}

} // namespace ELE3312
