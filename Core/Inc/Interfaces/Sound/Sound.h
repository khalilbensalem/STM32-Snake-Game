#ifndef SOUND_H_
#define SOUND_H_

/**
 * @file Sound.h
 * @brief Déclaration de la classe abstraite Sound pour la génération sonore.
 *
 * Cette interface définit les fonctions nécessaires pour produire du son :
 * - activer/désactiver le DAC,
 * - définir la fréquence ou une note,
 * - sélectionner la forme d’onde,
 * - ajuster l’amplitude.
 *
 * @date Novembre 2025
 * @defgroup ELE3312
 * @{
 */

#include "Interfaces/Sound/Note.h"

namespace ELE3312 {

/**
 * @brief Types de formes d’onde supportées.
 */
enum class Waveform {
    Sinus,
    Square
};

/**
 * @brief Interface abstraite définissant les fonctions minimales d'un générateur sonore.
 */
class Sound {
public:
    /** @brief Active la sortie sonore. */
    virtual void enableSound() = 0;

    /** @brief Désactive la sortie sonore. */
    virtual void disableSound() = 0;

    /**
     * @brief Définit la fréquence générée.
     * @param freq Fréquence en Hz.
     */
    virtual void setFrequency(float freq) = 0;

    /**
     * @brief Définit une note musicale.
     * @param note Note à jouer.
     */
    virtual void setNote(const Note &note) = 0;

    /**
     * @brief Met à jour l’état audio.
     * @note Peut être vide si l’implémentation utilise le DMA.
     */
    virtual void update() = 0;

    /**
     * @brief Change la forme d’onde.
     * @param newWaveform Forme d’onde désirée.
     */
    virtual void setWaveform(Waveform newWaveform) { waveform = newWaveform; }

    /**
     * @brief Indique si le son est actuellement actif.
     * @return true si actif, false sinon.
     */
    virtual bool isEnabled() const { return soundEnabled; }

    /**
     * @brief Définit l’amplitude (0–100%).
     */
    virtual void setAmplitude(uint16_t value) = 0;

    /**
     * @brief Retourne l’amplitude actuelle.
     */
    virtual uint16_t getAmplitude() const = 0;

protected:
    Waveform waveform;     //Forme d’onde sélectionnée.
    bool soundEnabled;     //Indique si le son est actif.
    uint16_t amplitude = 100; //Amplitude (0–100%).
};

} // namespace ELE3312

/**
 * @}
 */

#endif /* SOUND_H_ */
