/**
  * @file DACSound.cpp
  * @date Novembre 2025
  * @brief Implémentation de la classe DACSound qui réalise l'interface Sound
  *        pour le jeu PolySnake.
  *
  * Cette implémentation utilise :
  * - un double buffer de taille fixe (2048 échantillons par buffer),
  * - une table de consultation (LUT) de forme d'onde (256 échantillons par période),
  * - un accumulateur de phase pour générer les échantillons,
  * - des callbacks DMA pour remplir les moitiés de buffer pendant la lecture.
  *
  * @authors
  * - Inès Tazi Riffi (2216169)
  * - Zakaria Chikri (2216665)
  *
  * @defgroup ELE3312
  * @{
  */

#include "NucleoImp/Sound/DACSound.h"
#include <cstdint>
#include <cmath>
#include <cstdio>

namespace ELE3312 {

#ifdef __cplusplus
extern "C" {
#endif

// Pointeur externe vers l'instance DACSound
extern DACSound* g_dacSound;

/**
 * @brief Callback de mi-transfert DMA pour le canal 1 du DAC.
 * @param hdac Pointeur vers le gestionnaire DAC.
 * @details Appelé lorsque la première moitié du buffer a été transférée.
 * Remplit la première moitié avec de nouveaux échantillons pendant que
 * la deuxième moitié est jouée.
 */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac) {
	if (g_dacSound != nullptr) {
		g_dacSound->fillBufferHalf(g_dacSound->currentBuffer, 0,
		                           g_dacSound->fixedBufferSize / 2);
	}
}

/**
 * @brief Callback de transfert complet DMA pour le canal 1 du DAC.
 * @param hdac Pointeur vers le gestionnaire DAC.
 * @details Appelé lorsque le buffer complet a été transféré.
 * Remplit la deuxième moitié avec de nouveaux échantillons pendant que
 * la première moitié est jouée.
 */
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac) {
	DACSound::dacCpltEvent = 1;

	if (g_dacSound != nullptr) {
		g_dacSound->fillBufferHalf(g_dacSound->currentBuffer,
		                           g_dacSound->fixedBufferSize / 2,
		                           g_dacSound->fixedBufferSize / 2);
	}
}

#ifdef __cplusplus
}
#endif

// Définition du pointeur global vers l'instance active de DACSound
DACSound* g_dacSound = nullptr;

// Variable statique pour signaler la fin de transfert DMA
volatile uint16_t DACSound::dacCpltEvent = 0;

/**
 * @brief Constructeur de la classe DACSound.
 */
DACSound::DACSound() {
	soundEnabled = false;
	currentBuffer = sampleBuffer1;
}

/**
 * @brief Configure la classe DACSound.
 * @param hdac Pointeur vers le gestionnaire DAC (sortie analogique).
 * @param htim Pointeur vers le timer qui déclenche le DAC.
 * @param baseFreq Fréquence de base du microcontrôleur (en Hz).
 * @details
 * - Calcule la fréquence d'échantillonnage à partir de la configuration du timer.
 * - Configure la forme d'onde par défaut.
 * - Génère la LUT de forme d'onde.
 * - Démarre le timer utilisé comme base de temps du DAC.
 */
void DACSound::setup(DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim, uint64_t baseFreq){
	this->hdac = hdac;
	this->htim = htim;
	this->baseFreq = baseFreq;

	// Fs = F_base / (PSC + 1) / (ARR + 1)
	sampleFrequency = (baseFreq/(htim->Instance->PSC + 1))/htim->Instance->ARR;

	waveform = Waveform::Sinus;

	// Génère la LUT de forme d'onde initiale
	generateWaveformLUT(waveform);

	// Démarre le timer qui déclenche le DAC
	HAL_TIM_Base_Start(htim);
}

/**
 * @brief Génère la table de consultation pour une période de la forme d'onde.
 * @param waveform Type de forme d'onde (sinus, carré, etc.).
 * @details Remplit waveformLUT avec une période complète, à amplitude courante,
 * mise à l'échelle pour un DAC 12 bits (0–4095).
 */
void DACSound::generateWaveformLUT(Waveform waveform) {
	for(uint32_t i = 0; i < waveformLUTSize; i++) {
		float phase = (2.0f * pi * i) / waveformLUTSize;

		switch(waveform) {
			case Waveform::Sinus:
				// sin(phase) dans [-1, 1] → mise à l'échelle 0–4095
				waveformLUT[i] = (uint32_t)((sin(phase) + 1.0f) * (2047.5f * amplitude / 100.0f));
				break;

			case Waveform::Square:
				// Onde carrée 50% de rapport cyclique
				waveformLUT[i] = (i < waveformLUTSize/2) ? (4095 * amplitude / 100) : 0;
				break;

			default:
				// Par défaut : niveau DC au milieu de l'échelle
				waveformLUT[i] = 2048;
				break;
		}
	}

	printf("2216169 2216665 - LUT de forme d'onde generee, type=%d\n", (int)waveform);
}

/**
 * @brief Remplit une moitié du buffer DAC avec des échantillons audio.
 * @param buffer Buffer à remplir.
 * @param startIdx Index de départ dans le buffer.
 * @param length Nombre d'échantillons à générer.
 * @details Utilise :
 * - la LUT de forme d’onde,
 * - l’accumulateur de phase,
 * - l’incrément de phase (lié à la fréquence de sortie).
 */
void DACSound::fillBufferHalf(uint32_t *buffer, uint32_t startIdx, uint32_t length) {
	for(uint32_t i = 0; i < length; i++) {
		uint32_t lutIndex = (uint32_t)phaseAccumulator % waveformLUTSize;
		buffer[startIdx + i] = waveformLUT[lutIndex];

		phaseAccumulator += phaseIncrement;

		if(phaseAccumulator >= waveformLUTSize) {
			phaseAccumulator -= waveformLUTSize;
		}
	}
}

/**
 * @brief Change la note à jouer (pour compatibilité avec l’interface Sound).
 * @param note Note à jouer.
 * @details
 * Dans le Lab 5, cette méthode n'est pas utilisée : le projet passe
 * directement par setFrequency() avec les fréquences en Hz définies dans Melody.h.
 */
void DACSound::setNote(const Note &note) {
	// Non utilisée dans le Lab 5
	(void)note; // éviter un avertissement de paramètre inutilisé
}

/**
 * @brief Démarre la lecture audio via DAC + DMA en mode circulaire.
 * @details
 * - Pré-remplit le buffer complet.
 * - Lance le transfert DMA vers le DAC.
 */
void DACSound::startPlayback() {
	// Pré-remplir le buffer complet avant de démarrer
	fillBufferHalf(sampleBuffer1, 0, fixedBufferSize);

	// Démarrer le DAC avec DMA en mode circulaire
	HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, sampleBuffer1, fixedBufferSize, DAC_ALIGN_12B_R);
	soundEnabled = true;

	printf("2216169 2216665 - Lecture DAC demarree\n");
}

/**
 * @brief Active la sortie sonore.
 * @details Démarre la lecture si elle n’est pas déjà active.
 */
void DACSound::enableSound() {
	if (!soundEnabled) {
		startPlayback();
	}
}

/**
 * @brief Désactive la sortie sonore.
 * @details Arrête le DAC et le DMA.
 */
void DACSound::disableSound(){
	HAL_DAC_Stop_DMA(hdac, DAC_CHANNEL_1);
	soundEnabled = false;
	printf("2216169 2216665 - Lecture DAC arretee\n");
}

/**
 * @brief Définit la fréquence du son produit.
 * @param freq Fréquence en Hz.
 * @details Met à jour l’incrément de phase en fonction de la fréquence donnée.
 */
void DACSound::setFrequency(float freq){
	currentFrequency = freq;
	phaseIncrement = (freq * waveformLUTSize) / sampleFrequency;
}

/**
 * @brief Met à jour le son généré.
 * @details Dans cette implémentation avec double buffer DMA, aucune action
 * n'est nécessaire ici : tout est géré par les callbacks DMA.
 */
void DACSound::update(){
	// Les callbacks DMA gèrent la mise à jour des buffers
}

/**
 * @brief Définit l’amplitude de l’onde sonore générée.
 * @param value Amplitude en pourcentage (0–100).
 * @details Régénère la LUT après la modification de l’amplitude.
 */
void DACSound::setAmplitude(uint16_t value) {
	amplitude = value <= 100 ? value : 100;
	generateWaveformLUT(waveform);
}

/**
 * @brief Retourne l’amplitude courante de l’onde générée.
 * @return Amplitude en pourcentage (0–100).
 */
uint16_t DACSound::getAmplitude() const {
	return amplitude;
}

/**
 * @brief Méthode héritée pour générer une LUT générique.
 * @param buffer Buffer de sortie.
 * @param bufferLength Longueur du buffer.
 * @retval 0 La génération s’est bien déroulée.
 * @retval -1 Une erreur est survenue.
 * @note Non utilisée dans l’implémentation du lab 5 avec double buffer fixe.
 */
int DACSound::generateWaveLUT(uint32_t *buffer, uint32_t *bufferLength){
	// Non utilisée dans le Lab 5
	(void)buffer;
	(void)bufferLength;
	return 0;
}

} /* namespace ELE3312 */

/**
  * @}
  */ // Fin du groupe ELE3312
