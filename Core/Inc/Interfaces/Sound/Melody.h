/**
 * @file Melody.h
 * @date Novembre 2025
 * @brief Définition des mélodies musicales (Mario & Luigi) pour le jeu PolySnake.
 *
 * Version optimisée : utilise phaseIncrement pré-calculé au lieu de frequency_hz
 * pour éviter les calculs flottants à chaque changement de note.
 * 
 * Ce fichier contient deux mélodies complètes :
 * - marioMelody : thème principal de Super Mario Bros,
 * - luigiMelody : thème Luigi (variation).
 *
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 *
 * @defgroup ELE3312
 * @{
 */

#ifndef MELODY_H_
#define MELODY_H_

#include <cstdint>

namespace ELE3312 {

/**
 * @brief Représente une note unique dans une séquence musicale.
 *
 * Contient le phaseIncrement pré-calculé et la durée en millisecondes.
 * Cela évite les calculs flottants lors des changements de note.
 *
 * Formule utilisée pour le pré-calcul :
 * phaseIncrement = (frequency_hz × waveformLUTSize) / sampleFrequency
 */
struct SimpleNote {
    float phaseIncrement;   //Incrément de phase pré-calculé
    uint32_t duration_ms;   //Durée de la note en millisecondes
};

// Mélodie 1 : Thème Mario (40 notes)

/**
 * @brief Mélodie principale de Super Mario Bros.
 */
const SimpleNote marioMelody[] = {
    // Introduction (8 notes)
    {3.3721f, 150}, // E5 (659.25 Hz)
    {3.3721f, 150}, // E5
    {3.3721f, 150}, // E5
    {2.6758f, 100}, // C5 (523.25 Hz)
    {3.3721f, 150}, // E5
    {4.0140f, 300}, // G5 (783.99 Hz)
    {2.6758f, 300}, // C5
    {2.0045f, 300}, // G4 (392.00 Hz)

    // Section A (8 notes)
    {1.6860f, 300}, // E4 (329.63 Hz)
    {2.2502f, 150}, // A4 (440.00 Hz)
    {2.5254f, 150}, // B4 (493.88 Hz)
    {2.3844f, 150}, // Bb4 (466.16 Hz)
    {2.2502f, 150}, // A4
    {2.0045f, 150}, // G4
    {3.3721f, 150}, // E5
    {4.0140f, 150}, // G5

    // Section B (7 notes)
    {4.5005f, 300}, // A5 (880.00 Hz)
    {3.5725f, 150}, // F5 (698.46 Hz)
    {4.0140f, 150}, // G5
    {3.3721f, 300}, // E5
    {2.6758f, 150}, // C5
    {3.0037f, 150}, // D5 (587.33 Hz)
    {2.5254f, 300}, // B4

    // Section C (9 notes)
    {2.6758f, 300}, // C5
    {2.0045f, 300}, // G4
    {1.6860f, 300}, // E4
    {2.2502f, 150}, // A4
    {2.5254f, 150}, // B4
    {2.3844f, 150}, // Bb4
    {2.2502f, 150}, // A4
    {2.0045f, 150}, // G4
    {3.3721f, 150}, // E5

    // Finale (8 notes)
    {4.0140f, 150}, // G5
    {4.5005f, 300}, // A5
    {3.5725f, 150}, // F5
    {4.0140f, 150}, // G5
    {3.3721f, 300}, // E5
    {2.6758f, 150}, // C5
    {3.0037f, 150}, // D5
    {2.5254f, 300}, // B4
};

/**
 * @brief Nombre total de notes dans la mélodie Mario.
 */
const uint8_t marioMelodyLength = sizeof(marioMelody) / sizeof(marioMelody[0]);

// Mélodie 2 : Thème Luigi (36 notes)

/**
 * @brief Mélodie Luigi (variation du thème de Mario, plus rapide et rythmée).
 */
const SimpleNote luigiMelody[] = {
    // Introduction rapide (6 notes)
    {3.3721f, 100}, // E5
    {3.3721f, 100}, // E5
    {3.3721f, 100}, // E5
    {2.6758f, 75},  // C5
    {3.3721f, 100}, // E5
    {4.0140f, 200}, // G5

    // Descente (4 notes)
    {2.6758f, 200}, // C5
    {2.0045f, 200}, // G4
    {1.6860f, 200}, // E4
    {2.2502f, 100}, // A4

    // Motif rythmique (8 notes)
    {2.5254f, 100}, // B4
    {2.3844f, 100}, // Bb4
    {2.2502f, 100}, // A4
    {2.0045f, 100}, // G4
    {3.3721f, 100}, // E5
    {4.0140f, 100}, // G5
    {4.5005f, 200}, // A5
    {3.5725f, 100}, // F5

    // Répétition variée (6 notes)
    {4.0140f, 100}, // G5
    {3.3721f, 200}, // E5
    {2.6758f, 100}, // C5
    {3.0037f, 100}, // D5
    {2.5254f, 200}, // B4
    {2.6758f, 200}, // C5

    // Section médiane (6 notes)
    {2.0045f, 200}, // G4
    {1.6860f, 200}, // E4
    {2.2502f, 100}, // A4
    {2.5254f, 100}, // B4
    {2.3844f, 100}, // Bb4
    {2.2502f, 100}, // A4

    // Finale accélérée (6 notes)
    {2.0045f, 100}, // G4
    {3.3721f, 100}, // E5
    {4.0140f, 100}, // G5
    {4.5005f, 200}, // A5
    {3.5725f, 100}, // F5
    {4.0140f, 150}, // G5
};

/**
 * @brief Nombre total de notes dans la mélodie Luigi.
 */
const uint8_t luigiMelodyLength = sizeof(luigiMelody) / sizeof(luigiMelody[0]);

} // namespace ELE3312

/**
  * @}
  */ // Fin du groupe ELE3312

#endif /* MELODY_H_ */
