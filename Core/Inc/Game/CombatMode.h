/**
 * @file CombatMode.h
 * @brief Classe pour gérer le mode combat dans PolySnake
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 */

#ifndef COMBATMODE_H
#define COMBATMODE_H

#include "UltrasonicSensor.h"
#include "Interfaces/Display/Display.h"
#include "stm32f4xx_hal.h"
#include <cstdint>

namespace ELE3312 {

/**
 * @brief États du combat
 */
enum class CombatState {
    WAITING,        // En attente du début du combat
    FIGHTING,       // Combat en cours
    ROUND_WON,      // Round gagné
    ROUND_LOST,     // Round perdu
    GAME_OVER       // Fin du jeu (2 vies perdues)
};

/**
 * @brief Classe pour gérer le mode combat
 */
class CombatMode {
public:
    /**
     * @brief Constructeur
     */
    CombatMode();

    /**
     * @brief Configuration du mode combat
     * @param display Pointeur vers l'affichage
     * @param sensor Pointeur vers le capteur ultrason
     * @param huart Pointeur vers l'UART pour la communication
     */
    void setup(Display *display, UltrasonicSensor *sensor, UART_HandleTypeDef *huart);

    /**
     * @brief Initialise un nouveau combat
     * @param isMario true si le joueur local est Mario, false si Luigi
     */
    void startCombat(bool isMario);

    /**
     * @brief Met à jour le combat (à appeler dans la boucle principale)
     * @return CombatState État actuel du combat
     */
    CombatState update();

    /**
     * @brief Traite les données UART reçues pendant le combat
     * @param receivedByte Octet reçu via UART
     */
    void processUartByte(uint8_t receivedByte);

    /**
     * @brief Retourne le nombre de vies restantes du joueur local
     */
    int getLocalLives() const;

    /**
     * @brief Retourne le nombre de vies restantes du joueur distant
     */
    int getRemoteLives() const;

    /**
     * @brief Vérifie si le combat est terminé (Game Over)
     */
    bool isGameOver() const;

private:
    Display *display;
    UltrasonicSensor *sensor;
    UART_HandleTypeDef *huart;

    // Vies des joueurs
    int localLives;
    int remoteLives;

    // État du combat
    CombatState state;
    bool isMario;

    // Timer pour la zone cible
    uint32_t timeInZoneStart;
    bool isInZone;
    static constexpr uint32_t TIME_TO_WIN = 200; // 3 secondes en ms

    // Distance actuelle
    float currentDistance;

    // Timer pour les mesures ultrason
    uint32_t lastMeasurementTime;
    static constexpr uint32_t MEASUREMENT_INTERVAL = 100; // Mesure toutes les 100ms

    // Méthodes privées
    void drawCombatScreen();
    void drawLives();
    void drawDistance();
    void drawTimer();
    void sendCombatData(bool wonRound);
    void handleRoundEnd(bool won);
    void flashWhiteLedWinner();

};

} // namespace ELE3312

#endif // COMBATMODE_H
