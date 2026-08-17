#ifndef DACSOUND_H_
#define DACSOUND_H_

/**
  * @file DACSound.h
  * @brief Déclaration de la classe DACSound, qui implémente l’interface Sound
  *        pour générer du son via le convertisseur numérique-analogique (DAC).
  *
  * Implémentation avec double buffer de taille fixe pour la musique.
  *
  * @date Novembre 2025
  * @authors
  * - Inès Tazi Riffi (2216169)
  * - Zakaria Chikri (2216665)
  *
  * @defgroup ELE3312
  * @{
  */

#include "main.h"
#include "Interfaces/Sound/Sound.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback de fin de transfert DMA pour le canal 1 du DAC.
 */
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac);

/**
 * @brief Callback de mi-transfert DMA pour le canal 1 du DAC.
 */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac);

#ifdef __cplusplus
}
#endif

namespace ELE3312 {

/**
 * @brief Classe gérant la génération de son via le DAC avec double buffer.
 *
 * @details
 * - Les échantillons audio sont générés à la volée à partir d’un accumulateur de phase.
 * - Une LUT (Look-Up Table) stocke une période de la forme d’onde.
 * - L’accumulateur de phase détermine l’index dans la LUT.
 * - Les callbacks DMA remplissent les moitiés de buffer pendant que l’autre moitié est jouée.
 */
class DACSound : public Sound {
public:
	/**
	 * @brief Constructeur de la classe DACSound.
	 */
	DACSound();

	/**
	 * @brief Routine de configuration de DACSound.
	 * @param hdac Pointeur vers le gestionnaire DAC.
	 * @param htim Pointeur vers le timer qui déclenche le DAC.
	 * @param baseFreq Fréquence de base du microcontrôleur (Hz) utilisée pour calculer la fréquence d’échantillonnage.
	 */
	void setup(DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim, uint64_t baseFreq = 84000000);

	/**
	 * @brief Destructeur virtuel par défaut.
	 */
	virtual ~DACSound() = default;

	/**
	 * @brief Active la sortie sonore.
	 */
	void enableSound() override;

	/**
	 * @brief Désactive la sortie sonore.
	 */
	void disableSound() override;

	/**
	 * @brief Définit la fréquence du son produit.
	 * @param freq Fréquence générée (Hz).
	 */
	void setFrequency(float freq) override;

	/**
	 * @brief Définit la note musicale à jouer (compatibilité interface Sound).
	 * @param note Note musicale à jouer.
	 * @note Le projet utilise principalement setFrequency() ou setPhaseIncrementDirect()
	 *       pour optimiser les performances.
	 */
	void setNote(const Note &note) override;

	/**
	 * @brief Met à jour le son généré.
	 * @note Avec le double buffer DMA, cette méthode n’est pas nécessaire pour
	 *       la lecture continue (callbacks DMA suffisent).
	 */
	void update() override;

	/**
	 * @brief Définit l’amplitude de l’onde sonore générée.
	 * @param value Amplitude en pourcentage (0–100).
	 */
	virtual void setAmplitude(uint16_t value) override;

	/**
	 * @brief Retourne l’amplitude de l’onde générée.
	 * @return Amplitude en pourcentage (0–100).
	 */
	virtual uint16_t getAmplitude() const override;

	// Optimisations audio (2216169 2216665)

	/**
	 * @brief Définit directement l’incrément de phase (version optimisée).
	 * @param increment Valeur pré-calculée de phaseIncrement.
	 * @details Permet d’éviter les calculs flottants au moment du changement de note
	 *          et réduit fortement le temps d’exécution.
	 */
	inline void setPhaseIncrementDirect(float increment) {
		phaseIncrement = increment;
	}

	/**
	 * @brief Retourne l’incrément de phase actuel.
	 * @return Valeur de phaseIncrement.
	 */
	inline float getPhaseIncrement() const {
		return phaseIncrement;
	}

	/**
	 * @brief Calcule l’incrément de phase associé à une fréquence.
	 * @param freq Fréquence en Hz.
	 * @return phaseIncrement associé à cette fréquence.
	 *
	 * Formule : phaseIncrement = (freq × waveformLUTSize) / sampleFrequency
	 */
	inline float calculatePhaseIncrement(float freq) const {
		return (freq * waveformLUTSize) / sampleFrequency;
	}

	/**
	 * @brief Retourne la fréquence d’échantillonnage actuelle.
	 * @return Fréquence d’échantillonnage en Hz.
	 */
	inline float getSampleFrequency() const {
		return sampleFrequency;
	}

	/**
	 * @brief Arrête la lecture audio.
	 * @details Arrête le DAC et le DMA si le DAC est configuré.
	 */
	inline void stop() {
		if (hdac) {
			HAL_DAC_Stop_DMA(hdac, DAC_CHANNEL_1);
		}
	}

	// Méthodes spécifiques au Lab 5

	/**
	 * @brief Génère une LUT de forme d’onde pour une période complète.
	 * @param waveform Type de forme d’onde à générer.
	 */
	void generateWaveformLUT(Waveform waveform);

	/**
	 * @brief Remplit une moitié du buffer DAC avec des échantillons.
	 * @param buffer Pointeur vers le buffer DAC.
	 * @param startIdx Index de départ dans le buffer.
	 * @param length Nombre d’échantillons à générer.
	 */
	void fillBufferHalf(uint32_t *buffer, uint32_t startIdx, uint32_t length);

	/**
	 * @brief Démarre la lecture audio via DAC + DMA.
	 * @details Pré-remplit le buffer initial puis lance le DAC en mode DMA circulaire.
	 */
	void startPlayback();

	/**
	 * @brief Drapeau d’événement signalant la fin de transfert DMA du DAC.
	 */
	volatile static uint16_t dacCpltEvent;

	/**
	 * @brief Pointeur vers le buffer actuellement actif.
	 */
	uint32_t *currentBuffer;

	/**
	 * @brief Taille fixe de chaque buffer DAC.
	 */
	constexpr static uint32_t fixedBufferSize{2048};

private:
	// Pointeurs vers les périphériques
	DAC_HandleTypeDef *hdac = nullptr; // Gestionnaire DAC
	TIM_HandleTypeDef *htim = nullptr; //Gestionnaire Timer

	// Paramètres de fréquence
	float sampleFrequency = 0.0f;      //Fréquence d’échantillonnage (Hz)
	float currentFrequency = 440.0f;   //Fréquence courante (Hz)
	uint64_t baseFreq = 0;             //Fréquence de base du microcontrôleur (Hz)

	// Configuration du double buffer
	constexpr static uint32_t waveformLUTSize{256}; //Taille de la LUT (1 période)
	constexpr static float pi{3.1415926535f};       //Constante π

	uint32_t sampleBuffer1[fixedBufferSize]; //Premier buffer DMA
	uint32_t sampleBuffer2[fixedBufferSize]; //Second buffer DMA
	uint32_t waveformLUT[waveformLUTSize];   //LUT de forme d’onde

	bool needUpdate{false}; //Indique si la forme d’onde doit être régénérée (hérité)

	// Variables pour la génération
	float phaseAccumulator{0.0f}; //Position courante dans la LUT (fractionnaire)
	float phaseIncrement{0.0f};   //Incrément de phase par échantillon

	/**
	 * @brief Implémentation héritée de la génération de LUT générique.
	 * @param buffer Buffer de sortie à remplir.
	 * @param bufferLength Taille du buffer.
	 * @retval 0 Buffer écrit avec succès.
	 * @retval -1 Erreur.
	 * @note Non utilisée dans l’implémentation du Lab 5.
	 */
	int generateWaveLUT(uint32_t *buffer, uint32_t *bufferLength);
};

} /* namespace ELE3312 */

/**
  * @}
  */ // Fin du groupe ELE3312

#endif /* DACSOUND_H_ */
