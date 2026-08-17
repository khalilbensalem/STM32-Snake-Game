/**
 * @file SnakePositionMessage.h
 * @brief Définition du message de position du serpent pour la communication entre cartes.
 *
 * Cette classe encapsule les informations de position et d’état du serpent
 * afin de les transmettre entre les deux cartes dans le cadre du jeu PolySnake.
 * Le message contient la position de la tête, la direction actuelle, la longueur du serpent et un indicateur de collision.
 *
 * @date 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 */

#ifndef SNAKEPOSITIONMESSAGE_H
#define SNAKEPOSITIONMESSAGE_H

#include "Interfaces/Communication/Message.h"

namespace ELE3312 {

/**
 * @struct SnakePositionData
 * @brief Données brutes représentant l’état du serpent.
 *
 * Cette structure regroupe toutes les informations nécessaires
 * pour reconstruire la position et l’état d’un serpent distant.
 */
struct SnakePositionData {
    uint16_t headX;      //Coordonnée X de la tête du serpent.
    uint16_t headY;      //Coordonnée Y de la tête du serpent.
    uint16_t direction;  //Direction actuelle (0=NORD, 1=SUD, 2=OUEST, 3=EST).
    uint16_t length;     //Longueur du serpent (en tuiles).
    bool collision;      //Indique si une collision a été détectée.
};

/**
 * @class SnakePositionMessage
 * @brief Message réseau contenant la position et l’état du serpent.
 *
 * Hérite de la classe de base Message pour assurer la compatibilité avec
 * le protocole de communication. Ce message est échangé entre les cartes
 * afin de synchroniser les positions des serpents dans le mode multijoueur.
 */
class SnakePositionMessage : public Message {
public:
    /**
     * @brief Constructeur paramétré.
     * @param x Coordonnée X de la tête.
     * @param y Coordonnée Y de la tête.
     * @param dir Direction du serpent.
     * @param len Longueur du serpent.
     * @param coll Indique si le serpent est en collision (par défaut false).
     */
    SnakePositionMessage(uint16_t x, uint16_t y, uint16_t dir,
                         uint16_t len, bool coll = false);

    /** @brief Constructeur par défaut. */
    SnakePositionMessage();

    /**
     * @brief Définit les données du message à partir d’un tableau d’octets.
     * @param newData Pointeur vers les nouvelles données.
     * @param length Taille du tableau en octets.
     */
    void setData(uint8_t *newData, size_t length) override;

    /**
     * @brief Retourne la taille totale du message.
     * @return Taille en octets.
     */
    size_t getSize() const override;

    /**
     * @brief Retourne un pointeur vers les données brutes du message.
     * @return Pointeur constant vers le tableau de données.
     */
    const uint8_t *getData() const override;

    /**
     * @brief Retourne une représentation textuelle du message.
     * @return Chaîne de caractères décrivant la position et l’état du serpent.
     */
    std::string toString() const override;

    /** @brief Retourne la coordonnée X de la tête du serpent. */
    uint16_t getHeadX() const { return data.headX; }

    /** @brief Retourne la coordonnée Y de la tête du serpent. */
    uint16_t getHeadY() const { return data.headY; }

    /** @brief Retourne la direction actuelle du serpent. */
    uint16_t getDirection() const { return data.direction; }

    /** @brief Retourne la longueur actuelle du serpent. */
    uint16_t getLength() const { return data.length; }

    /** @brief Indique si une collision a été détectée. */
    bool hasCollision() const { return data.collision; }

private:
    SnakePositionData data; //Structure contenant les données de position.
};

} // namespace ELE3312

#endif //SNAKEPOSITIONMESSAGE_H
