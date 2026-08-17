/**
 * @file SnakePositionMessage.cpp
 * @brief Implémentation du message de position du serpent pour la communication entre cartes.
 *
 * Ce message transporte la position de la tête, la direction, la longueur
 * du serpent et un indicateur de collision.
 *
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 *
 * @defgroup ELE3312
 * @{
 */

#include "Game/SnakePositionMessage.h"
#include <cstring>

namespace ELE3312 {

/**
 * @brief Constructeur paramétré.
 * @param x Position X de la tête.
 * @param y Position Y de la tête.
 * @param dir Direction actuelle (0–3).
 * @param len Longueur du serpent.
 * @param coll true si une collision est survenue, false sinon.
 */
SnakePositionMessage::SnakePositionMessage(uint16_t x, uint16_t y, uint16_t dir,
                                           uint16_t len, bool coll)
    : Message(MessageType::Position) {
    data.headX = x;
    data.headY = y;
    data.direction = dir;
    data.length = len;
    data.collision = coll;
    setValid(true);
}

/**
 * @brief Constructeur par défaut.
 *
 * Initialise le message avec des valeurs neutres et
 * le marque comme invalide (valid = false).
 */
SnakePositionMessage::SnakePositionMessage()
    : Message(MessageType::Position) {
    data.headX = 0;
    data.headY = 0;
    data.direction = 0;
    data.length = 1;
    data.collision = false;
    setValid(false);
}

/**
 * @brief Définit les données du message à partir d'un tableau d'octets.
 * @param newData Pointeur vers les octets reçus (payload).
 * @param length Taille en octets du tableau newData.
 *
 * Les données sont copiées dans la structure interne puis
 * légèrement validées / corrigées (longueur et direction).
 */
void SnakePositionMessage::setData(uint8_t *newData, size_t length) {
    if (newData == nullptr || length < sizeof(SnakePositionData)) {
        setValid(false);
        return;
    }

    // Copie les données brutes dans la structure
    std::memcpy(&data, newData, sizeof(SnakePositionData));

    // Validation simple pour éviter des valeurs corrompues
    if (data.length == 0) data.length = 1;
    if (data.length > 100) data.length = 100;
    if (data.direction > 3) data.direction = 0;  // Direction invalide, direction NORD

    setValid(true);
}

/**
 * @brief Retourne la taille en octets des données du message.
 * @return Taille de SnakePositionData.
 */
size_t SnakePositionMessage::getSize() const {
    return sizeof(SnakePositionData);
}

/**
 * @brief Retourne un pointeur constant vers les données brutes du message.
 * @return Pointeur sur la structure interne sérialisée.
 */
const uint8_t* SnakePositionMessage::getData() const {
    return reinterpret_cast<const uint8_t*>(&data);
}

/**
 * @brief Retourne une représentation textuelle du message.
 * @return Chaîne décrivant la position, la direction, la longueur et la collision.
 */
std::string SnakePositionMessage::toString() const {
    return "SnakePos[" + std::to_string(data.headX) + "," +
           std::to_string(data.headY) + "] dir=" +
           std::to_string(data.direction) + " len=" +
           std::to_string(data.length) +
           (data.collision ? " COLLISION" : "");
}

} // namespace ELE3312

/**
 * @}
 */ // Fin du groupe ELE3312
