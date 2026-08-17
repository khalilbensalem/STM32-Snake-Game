/**
 * @file UltrasonicSensor.cpp
 * @brief Implémentation de la classe UltrasonicSensor pour le capteur HC-SR04.
 *
 * Cette classe gère :
 * - la configuration du timer en mode Input Capture,
 * - la génération de l’impulsion TRIG,
 * - le calcul de la distance à partir de la durée de l’impulsion ECHO.
 *
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 *
 * @defgroup ELE3312
 * @{
 */

#include "Game/UltrasonicSensor.h"
#include <cstdio>

namespace ELE3312 {

/**
 * @brief Constructeur par défaut.
 *
 * Initialise les pointeurs et variables internes à des valeurs neutres.
 */
UltrasonicSensor::UltrasonicSensor()
    : htim(nullptr), trigPort(nullptr), trigPin(0),
      icValue1(0), icValue2(0), difference(0),
      isFirstCaptured(false), distance(0.0f)
{
}

/**
 * @brief Configure le capteur ultrason.
 *
 * @param htim Pointeur vers le timer utilisé en Input Capture (ECHO).
 * @param trigPort Port GPIO utilisé pour la broche TRIG.
 * @param trigPin Broche GPIO utilisée pour TRIG.
 *
 * Met TRIG à l’état bas et démarre le timer en mode Input Capture
 * avec interruptions sur le canal 1.
 */
void UltrasonicSensor::setup(TIM_HandleTypeDef *htim, GPIO_TypeDef *trigPort, uint16_t trigPin) {
    this->htim = htim;
    this->trigPort = trigPort;
    this->trigPin = trigPin;

    // Mettre TRIG à LOW au départ
    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_RESET);

    // Démarrer le timer en mode Input Capture sur le canal 1
    HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
}

/**
 * @brief Déclenche une mesure de distance.
 *
 * Envoie une impulsion d’environ 10 µs sur la broche TRIG
 * et réinitialise l’état interne pour une nouvelle mesure.
 */
void UltrasonicSensor::trigger() {
    // Réinitialiser l'état de capture pour une nouvelle mesure
    isFirstCaptured = false;
    __HAL_TIM_SET_COUNTER(htim, 0);   // compteur TIM2 = 0

    // DEBUG: Compter les triggers
    static uint32_t triggerCount = 0;
    triggerCount++;

    // Impulsion de 10 µs sur TRIG
    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_SET);
    for (volatile int i = 0; i < 840; i++);   // ~10 µs à 84 MHz
    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_RESET);
}

/**
 * @brief Met à jour la distance mesurée à partir d’une valeur de capture.
 *
 * @param captureValue Valeur capturée par le timer (compteur à l’instant de l’interruption).
 *
 * Cette méthode doit être appelée depuis le callback d’Input Capture
 * (IRQ du timer). Elle utilise deux captures successives :
 * - premier front : début de l’impulsion ECHO,
 * - deuxième front : fin de l’impulsion ECHO.
 */
void UltrasonicSensor::updateDistance(uint32_t captureValue) {
    // 1ère capture : front montant → début de l'impulsion ECHO
    if (!isFirstCaptured) {
        icValue1 = captureValue;
        isFirstCaptured = true;

        // On remet le timer à 0 au moment du front montant
        __HAL_TIM_SET_COUNTER(htim, 0);
    }
    // 2ème capture : front descendant alors fin de l'impulsion ECHO
    else {
        icValue2 = captureValue;

        // Le compteur a été remis à 0, la valeur correspond à la durée en µs
        difference = icValue2;

        // Limiter à 30000 µs (~5 m) pour éviter les valeurs incorrectes
        if (difference > 30000) {
            difference = 30000;
        }

        float timeInMicroseconds = static_cast<float>(difference);  // 1 tick = 1 µs
        distance = (timeInMicroseconds * SPEED_OF_SOUND) / 2.0f;    // SPEED_OF_SOUND = 0.0343 cm/µs

        // Prêt pour la prochaine mesure
        isFirstCaptured = false;
    }
}

/**
 * @brief Retourne la distance mesurée.
 * @return Distance en centimètres.
 */
float UltrasonicSensor::getDistance() const {
    return distance;
}

/**
 * @brief Indique si la distance est dans la zone cible.
 *
 * @return true si la distance est comprise entre TARGET_MIN et TARGET_MAX,
 * false sinon.
 */
bool UltrasonicSensor::isInTargetZone() const {
    return (distance >= TARGET_MIN && distance <= TARGET_MAX);
}

} // namespace ELE3312

/**
 * @}
 */ // Fin du groupe ELE3312
