/**
 * @file SnakeGame.h
 * @brief Déclaration de la classe SnakeGame pour le jeu PolySnake.
 *
 * Cette classe implémente toute la logique de jeu :
 * - gestion des serpents (local et distant),
 * - détection des collisions,
 * - génération et affichage des fruits,
 * - communication UART (direction + flag “ate”),
 * - support des thèmes Mario/Luigi.
 *
 * L’orchestration (audio, LED, menu, synchronisation) est gérée dans cpp_main.cpp.
 *
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 *
 * @defgroup ELE3312
 * @{
 */

#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include "main.h"
#include "Interfaces/Display/Display.h"
#include "Interfaces/Keypad/Keypad.h"
#include "Interfaces/MotionInput/MotionInput.h"
#include "Game/MySnake.h"
#include "Game/Tiles.h"

namespace ELE3312 {

/**
 * @brief Structure représentant les couleurs d’un serpent.
 */
struct SnakeColors {
    Color bodyColor;
    Color headColor;
};

/**
 * @brief Différents types de collisions possibles.
 */
enum class CollisionType {
    NONE,
    SELF,
    OPPONENT,
    WALL
};

/**
 * @brief Classe gérant la logique complète du mode multijoueur PolySnake.
 *
 * Cette classe gère :
 * - le déplacement/affichage des serpents,
 * - les fruits,
 * - les collisions,
 * - les messages UART simples (direction + ate),
 * - le thème Mario/Luigi.
 */
class SnakeGame {
public:
    /**
     * @brief Constructeur.
     * @param disp Afficheur
     * @param keypad Clavier matriciel
     * @param imu Accéléromètre/IMU
     * @param huart UART pour échanger direction + “ate”
     */
    SnakeGame(Display *disp, Keypad *keypad, MotionInput *imu,
              UART_HandleTypeDef *huart);

    /** @brief Destructeur. */
    ~SnakeGame() = default;

    /**
     * @brief Configure les serpents selon le thème choisi.
     * @param isMario true si le joueur local joue Mario, false pour Luigi
     */
    void setupPlayers(bool isMario);

    /**
     * @brief Initialise le jeu : fruits et rendu initial.
     * @details Doit être appelé après setupPlayers().
     */
    void init();

    // BOUCLE DE JEU

    /**
     * @brief Traite un octet UART provenant de l’autre carte.
     *
     * Format :
     * - Bits 0-3 : direction (0=NORD, 1=EST, 2=SUD, 3=OUEST)
     * - Bit 4    : flag “ate”
     * - 0xFF     : Game Over
     * - 0xAA, 0xB1, 0xB2 : sync (ignorés)
     *
     * @return true s’il s’agit du signal Game Over.
     */
    bool processUartByte(uint8_t receivedByte);

    /**
     * @brief Lit les entrées du joueur (clavier + IMU).
     * @param keyValue valeur du clavier (bitfield)
     */
    void processInputs(uint16_t keyValue);

    /**
     * @brief Exécute un tick de jeu pour le serpent local.
     * @return true si un fruit a été mangé.
     */
    bool tick();

    /**
     * @brief Envoie la direction + flag "ate" via UART.
     * @param ate true si le serpent a mangé durant ce tick.
     */
    void sendDirection(bool ate);

    /**
     * @brief Vérifie les collisions du serpent local.
     * @return CollisionType::NONE s’il n’y a pas collision.
     */
    CollisionType checkCollisions();

    /**
     * @brief Envoie le signal Game Over (3× 0xFF).
     */
    void sendGameOver();

    // ACCESSEURS
    /** @brief Retourne les couleurs du serpent local. */
    SnakeColors getLocalColors() const { return localColors; }

    /** @brief Retourne les couleurs du serpent distant. */
    SnakeColors getRemoteColors() const { return remoteColors; }

    /** @brief Indique si le joueur local est Mario. */
    bool isPlayerMario() const { return playerIsMario; }

private:
    // CONSTANTES
    static constexpr int GRID_W = 32;
    static constexpr int GRID_H = 24;
    static constexpr int TILE_W = 10;
    static constexpr int TILE_H = 10;
    static constexpr int Y_OFFSET = 25;
    static constexpr int MAX_FRUITS = 10;

    static constexpr int POS_GAUCHE_X = 8;
    static constexpr int POS_GAUCHE_Y = 12;
    static constexpr int POS_DROITE_X = 24;
    static constexpr int POS_DROITE_Y = 12;

    // PÉRIPHÉRIQUES
    Display *display;
    Keypad *keypad;
    MotionInput *imu;
    UART_HandleTypeDef *huart;

    // ÉTAT DU JEU
    MySnake localSnake;
    MySnake remoteSnake;

    bool playerIsMario = true;
    SnakeColors localColors;
    SnakeColors remoteColors;

    tile fruits[MAX_FRUITS];
    int numFruits = 0;

    // MÉTHODES PRIVÉES
    /** @brief Mise à jour direction via IMU. */
    void updateDirectionFromIMU();

    /** @brief Génère tous les fruits initiaux. */
    void spawnAllFruits();

    /** @brief Repositionne un fruit. */
    void respawnFruit(int index);

    /** @brief Dessine un fruit. */
    void drawFruit(int index);

    /** @brief Dessine tous les fruits. */
    void drawAllFruits();

    /** @brief Efface une tuile de la grille. */
    void eraseTile(int x, int y);

    /** @brief Vérifie une collision avec un fruit. */
    int checkFruitCollision();
};

} // namespace ELE3312

#endif // SNAKEGAME_H

/**
 * @}
 */ // Fin du groupe ELE3312
