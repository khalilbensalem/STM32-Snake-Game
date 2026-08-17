#ifndef SERIALFRAME_INCLUDE_GUARD
#define SERIALFRAME_INCLUDE_GUARD

/**
 * @file SerialFrame.h
 * @brief Déclaration de la classe SerialFrame, qui représente une trame de
 *        données utilisée comme structure de base pour les échanges via
 *        l'interface série.
 *
 * Ajout de getSnakePositionMessage().
 *
 * @date Juin 2025
 * @defgroup ELE3312
 * @{
 *
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 */

#include "Interfaces/Communication/Message.h"
#include "Game/SnakePositionMessage.h"  // Utilisé pour décoder les positions de serpent
#include <memory>
#include <tuple>

namespace ELE3312 {

/**
 * @brief Classe définissant le protocole sous-jacent de communication série.
 *
 * Cette classe encapsule :
 * - le message courant (Message),
 * - les buffers utilisés pour l’encodage/décodage,
 * - le type de message transporté.
 */
class SerialFrame {
public:
    /**
     * @brief Constructeur par défaut.
     */
    SerialFrame();

    /**
     * @brief Constructeur à partir d’un message.
     * @param msg Pointeur vers le message à encapsuler.
     */
    SerialFrame(Message *msg);

    /**
     * @brief Configure les buffers internes utilisés pour l’encodage/décodage.
     *
     * @param p_encodeBuffer Pointeur vers le buffer d’encodage.
     * @param encodeSize Taille du buffer d’encodage.
     * @param p_tempBuffer Pointeur vers le buffer temporaire pour la sortie.
     * @param outputSize Taille du buffer temporaire.
     */
    void setup(uint8_t *p_encodeBuffer, size_t encodeSize,
               uint8_t *p_tempBuffer, size_t outputSize);

    // Getters

    /**
     * @brief Retourne le tableau d’octets représentant la trame série.
     * @return Tuple (pointeur sur les données, taille en octets).
     */
    std::tuple<uint8_t*, size_t> getByteArray();

    /**
     * @brief Retourne le type de message encapsulé dans la trame.
     * @return Type du message (MessageType).
     */
    MessageType getMessageType();

    // Setters

    /**
     * @brief Décode un tableau d’octets en message interne.
     *
     * @param byteArray Pointeur vers le tableau d’octets reçu.
     * @param size Taille du tableau en octets.
     */
    void setMessage(uint8_t* byteArray, size_t size);

    /**
     * @brief Construit un SnakePositionMessage à partir du buffer interne.
     *
     * Cette méthode interprète le contenu du buffer comme un message
     * de type Position, puis remplit un SnakePositionMessage avec les
     * données décodées.
     *
     * @return Un objet SnakePositionMessage construit à partir des données.
     *         Si le type ne correspond pas, le message retourné sera marqué
     *         comme invalide (via setValid(false)).
     */
    SnakePositionMessage getSnakePositionMessage();

private:
    Message *msg = nullptr;           //Message encapsulé (si présent)
    size_t tempBufferSize = 0;        //Taille du buffer temporaire
    size_t encodeBufferSize = 0;      //Taille du buffer d’encodage
    uint8_t *tempBuffer = nullptr;    //Buffer temporaire pour la sortie
    uint8_t *encodeBuffer = nullptr;  //Buffer utilisé pour l’encodage
    uint8_t *msgBuffer = nullptr;     //Pointeur vers les données du message
    size_t msgSize = 0;               //Taille des données du message
    MessageType messageType = MessageType::Unknown; //Type du message

    /**
     * @brief Convertit un message en tableau d’octets.
     *
     * @param message Pointeur vers le message à sérialiser.
     * @return Tuple (pointeur vers les octets, taille en octets).
     */
    std::tuple<uint8_t*, size_t> msgToByteArray(Message *message);
};

} // namespace ELE3312

/**
 * @}
 */ // Fin du groupe ELE3312

#endif
