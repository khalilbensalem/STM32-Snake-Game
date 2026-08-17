#ifndef CPP_MAIN_H_
#define CPP_MAIN_H_

/**
 * @file cpp_main.h
 * @brief Déclarations pour le jeu PolySnake multijoueur.
 *
 * Ce fichier déclare :
 * - la structure regroupant les handles des périphériques,
 * - la fonction d’entrée principale cpp_main() côté C++,
 * - les variables globales liées au ring buffer UART.
 *
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 *
 * @defgroup ELE3312
 * @{
 */

#include "main.h"

/**
 * @brief Structure contenant les handles vers les périphériques matériels.
 */
typedef struct {
    DAC_HandleTypeDef   *hdac;          //Handle du DAC utilisé pour l’audio
    TIM_HandleTypeDef   *htim_dac;      //Timer déclenchant le DAC
    SPI_HandleTypeDef   *hspi_tft;      //SPI pour l’écran TFT
    TIM_HandleTypeDef   *htim_distance; //Timer pour le capteur ultrason
    TIM_HandleTypeDef   *htim_led;      //Timer pour la LED RGB
    I2C_HandleTypeDef   *hi2c;          //I2C pour l’IMU
    UART_HandleTypeDef  *huart;         //UART de jeu entre les deux cartes
    UART_HandleTypeDef  *debug_uart;    //UART de debug (optionnel)
    ADC_HandleTypeDef   *hadc;          //ADC (si utilisé)
    GPIO_TypeDef        *gpio_keypad;   //Port GPIO du clavier matriciel
} peripheral_handles;

#ifdef __cplusplus
extern "C" {
#endif

// Variables globales pour la communication UART (ring buffer)

/**
 * @brief Octet reçu par interruption UART (défini dans main.c).
 */
extern uint8_t rxByte;

/**
 * @brief Taille du ring buffer UART.
 */
#define RX_BUFFER_SIZE 256

/**
 * @brief Buffer circulaire de réception UART (défini dans stm32f4xx_it.c).
 */
extern uint8_t rxBuffer[RX_BUFFER_SIZE];

/**
 * @brief Index d’écriture dans le ring buffer.
 */
extern volatile uint16_t rxHead;

/**
 * @brief Index de lecture dans le ring buffer.
 */
extern volatile uint16_t rxTail;

/**
 * @brief Point d'entrée principal en environnement C++.
 *
 * @param handles Pointeur vers la structure contenant les handles des périphériques.
 */
void cpp_main(peripheral_handles *handles);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */ // Fin du groupe ELE3312

#endif /* CPP_MAIN_H_ */
