/**
 * @file UltrasonicSensor.h
 * @brief Déclaration de la classe UltrasonicSensor pour le capteur HC-SR04.
 *
 * Cette classe gère :
 * - la configuration du timer en Input Capture,
 * - la génération de l’impulsion TRIG,
 * - le calcul de la distance à partir de la durée ECHO.
 *
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 */

#ifndef ULTRASONICSENSOR_H
#define ULTRASONICSENSOR_H

#include "stm32f4xx_hal.h"
#include <cstdint>

namespace ELE3312 {

/**
 * @brief Gestion du capteur ultrason HC-SR04.
 *
 * Utilise :
 * - TIM2_CH1 (PA0) en mode Input Capture pour le signal ECHO,
 * - PA1 en sortie pour le signal TRIG.
 */
class UltrasonicSensor {
public:
    /**
     * @brief Constructeur par défaut.
     */
    UltrasonicSensor();

    /**
     * @brief Configure le capteur ultrason.
     * @param htim Pointeur vers le timer utilisé (TIM2).
     * @param trigPort Port GPIO connecté à TRIG.
     * @param trigPin Broche GPIO connectée à TRIG.
     */
    void setup(TIM_HandleTypeDef *htim, GPIO_TypeDef *trigPort, uint16_t trigPin);

    /**
     * @brief Déclenche une mesure de distance.
     *
     * Envoie une impulsion d’environ 10 µs sur TRIG et
     * réinitialise l’état interne de capture.
     */
    void trigger();

    /**
     * @brief Met à jour la distance mesurée.
     *
     * À appeler depuis le callback d’Input Capture du timer.
     *
     * @param captureValue Valeur de capture du timer (compteur au front).
     */
    void updateDistance(uint32_t captureValue);

    /**
     * @brief Retourne la distance mesurée.
     * @return Distance en centimètres.
     */
    float getDistance() const;

    /**
     * @brief Vérifie si la distance est dans la zone cible [10–11] cm.
     * @return true si la distance est dans la zone, false sinon.
     */
    bool isInTargetZone() const;

private:
    TIM_HandleTypeDef *htim;  //Timer utilisé pour l’Input Capture.
    GPIO_TypeDef *trigPort;   //Port GPIO pour TRIG.
    uint16_t trigPin;         //Broche GPIO pour TRIG.

    uint32_t icValue1;        //Première capture (front montant).
    uint32_t icValue2;        //Deuxième capture (front descendant).
    uint32_t difference;      //Durée de l’impulsion ECHO (en µs).
    bool isFirstCaptured;     //Indique si la première capture a été faite.

    float distance;           //Distance mesurée en cm.

    // Constantes de fonctionnement
    static constexpr float TARGET_MIN = 10.0f;   //Distance minimale de la zone cible (cm).
    static constexpr float TARGET_MAX = 11.0f;   //Distance maximale de la zone cible (cm).
    static constexpr float SPEED_OF_SOUND = 0.0343f; //Vitesse du son
};

} // namespace ELE3312

#endif // ULTRASONICSENSOR_H
