/**
 * @file SnakeGame.cpp
 * @brief Implémentation de la classe SnakeGame pour le jeu PolySnake.
 *
 * Ce fichier contient uniquement la logique de jeu :
 * - mouvement des serpents (local et distant),
 * - gestion des fruits,
 * - communication UART (direction + indicateur de fruit mangé),
 * - détection des collisions.
 *
 * L'orchestration (menu, audio, LED, synchronisation) est dans cpp_main.cpp.
 *
 * @date Novembre 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 *
 * @defgroup ELE3312
 * @{
 */

#include "Game/SnakeGame.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

namespace ELE3312 {

// Constructeur

/**
 * @brief Constructeur de la classe SnakeGame.
 * @param disp Pointeur vers l'afficheur à utiliser.
 * @param kpad Pointeur vers le clavier matriciel.
 * @param motion Pointeur vers l’IMU pour le contrôle par inclinaison.
 * @param uart Pointeur vers l’interface UART pour la communication avec l’autre carte.
 */
SnakeGame::SnakeGame(Display *disp, Keypad *kpad, MotionInput *motion,
                     UART_HandleTypeDef *uart)
    : display(disp), keypad(kpad), imu(motion), huart(uart),
      localSnake(POS_GAUCHE_X, POS_GAUCHE_Y, EST),
      remoteSnake(POS_DROITE_X, POS_DROITE_Y, OUEST),
      playerIsMario(true), numFruits(0)
{
    // Couleurs par défaut (Mario)
    localColors = {Color::BLUE, Color::RED};
    remoteColors = {Color::WHITE, Color::GREEN};
}

// Configuration des joueurs

/**
 * @brief Configure les serpents selon le choix local Mario/Luigi.
 * @param isMario true si le joueur local est Mario, false s’il est Luigi.
 */
void SnakeGame::setupPlayers(bool isMario) {
    playerIsMario = isMario;

    if (isMario) {
        // Mario = gauche, Luigi (distant) = droite
        localSnake = MySnake(POS_GAUCHE_X, POS_GAUCHE_Y, EST);
        remoteSnake = MySnake(POS_DROITE_X, POS_DROITE_Y, OUEST);

        // Couleurs Mario : corps BLEU, tête ROUGE
        localColors = {Color::BLUE, Color::RED};
        // Couleurs Luigi (distant) : corps BLANC, tête VERTE
        remoteColors = {Color::WHITE, Color::GREEN};
    } else {
        // Luigi = droite, Mario (distant) = gauche
        localSnake = MySnake(POS_DROITE_X, POS_DROITE_Y, OUEST);
        remoteSnake = MySnake(POS_GAUCHE_X, POS_GAUCHE_Y, EST);

        // Couleurs Luigi : corps BLANC, tête VERTE
        localColors = {Color::WHITE, Color::GREEN};
        // Couleurs Mario (distant) : corps BLEU, tête ROUGE
        remoteColors = {Color::BLUE, Color::RED};
    }

    printf("2216169 2216665 - Joueur configure: %s\n", isMario ? "MARIO" : "LUIGI");
}

// Initialisation

/**
 * @brief Initialise le jeu (affichage de base, fruits, serpents).
 */
void SnakeGame::init() {
    // Affichage du titre et du joueur
    display->drawString(5, 5, "PolySnake - 2216169 2216665");
    display->drawString(5, 220, playerIsMario ? "Vous: MARIO" : "Vous: LUIGI");

    // Seed fixe pour synchroniser les fruits entre les deux cartes
    srand(12345);

    // Générer les fruits
    spawnAllFruits();
    drawAllFruits();

    // Dessiner les serpents initiaux
    localSnake.draw(display, localColors.bodyColor, Y_OFFSET, localColors.headColor);
    remoteSnake.draw(display, remoteColors.bodyColor, Y_OFFSET, remoteColors.headColor);

    printf("2216169 2216665 - Jeu initialise\n");
}

// Traitement UART

/**
 * @brief Traite un octet reçu par UART.
 * @param receivedByte Octet reçu.
 * @return true si un signal de fin de partie (0xFF) est reçu, false sinon.
 */
bool SnakeGame::processUartByte(uint8_t receivedByte) {
    // Signal Game Over
    if (receivedByte == 0xFF) {
        return true;
    }

    // Ignorer quelques signaux de synchronisation
    if (receivedByte == 0xAA || receivedByte == 0xB1 || receivedByte == 0xB2) {
        return false;
    }

    // Décoder direction et indicateur de fruit mangé
    uint8_t direction = receivedByte & 0x0F;
    bool opAte = (receivedByte & 0x10) != 0;

    // Direction valide (0-3)
    if (direction > 3) {
        return false;
    }

    // Mise à jour du serpent distant
    remoteSnake.erase(display, Y_OFFSET);
    remoteSnake.setDirection(static_cast<Direction>(direction));
    remoteSnake.move(opAte ? 1 : 0);
    remoteSnake.draw(display, remoteColors.bodyColor, Y_OFFSET, remoteColors.headColor);

    // Si le serpent distant a mangé, on redessine tous les fruits
    if (opAte) {
        drawAllFruits();
    }

    return false;
}

// Entrées

/**
 * @brief Traite les entrées utilisateur (clavier ou IMU).
 * @param keyValue Valeur lue sur le clavier matriciel (bitfield), 0 si aucune touche.
 */
void SnakeGame::processInputs(uint16_t keyValue) {
    Direction currentDir = localSnake.getDirection();

    // Clavier matriciel (valeurs en bitfield)
    if (keyValue != 0) {
        // Touche 2 = NORD (valeur 2)
        if (keyValue == 2 && currentDir != SUD) {
            localSnake.setDirection(NORD);
        }
        // Touche 8 = SUD (valeur 512)
        else if (keyValue == 512 && currentDir != NORD) {
            localSnake.setDirection(SUD);
        }
        // Touche 6 = EST (valeur 64)
        else if (keyValue == 64 && currentDir != OUEST) {
            localSnake.setDirection(EST);
        }
        // Touche 4 = OUEST (valeur 16)
        else if (keyValue == 16 && currentDir != EST) {
            localSnake.setDirection(OUEST);
        }
    } else {
        // Pas de touche pressée → direction contrôlée par l’IMU
        updateDirectionFromIMU();
    }
}

/**
 * @brief Met à jour la direction du serpent local à partir de l’IMU.
 */
void SnakeGame::updateDirectionFromIMU() {
    imu->update();
    float ax = -imu->getX();
    float ay = imu->getY();

    Direction currentDir = localSnake.getDirection();
    const float THRESHOLD = 0.25f;

    if (fabs(ay) > fabs(ax)) {
        if (ay > THRESHOLD && currentDir != OUEST) {
            localSnake.setDirection(EST);
        } else if (ay < -THRESHOLD && currentDir != EST) {
            localSnake.setDirection(OUEST);
        }
    } else {
        if (ax > THRESHOLD && currentDir != SUD) {
            localSnake.setDirection(NORD);
        } else if (ax < -THRESHOLD && currentDir != NORD) {
            localSnake.setDirection(SUD);
        }
    }
}

// Tick de jeu

/**
 * @brief Effectue un tick de jeu (mouvement du serpent local).
 * @return true si le serpent local a mangé un fruit, false sinon.
 */
bool SnakeGame::tick() {
    // Vérifier si la tête touche un fruit
    int fruitIndex = checkFruitCollision();
    bool ate = (fruitIndex >= 0);

    // Si fruit mangé, le repositionner et le redessiner
    if (ate) {
        respawnFruit(fruitIndex);
        drawFruit(fruitIndex);
    }

    // Sauvegarder la queue avant de bouger
    tile oldTail = localSnake.getTailTile();

    // Déplacer le serpent
    localSnake.move(ate ? 1 : 0);

    // Effacer l’ancienne queue si aucun fruit n’a été mangé
    if (!ate) {
        eraseTile(oldTail.x, oldTail.y);
    }

    // Redessiner le serpent
    localSnake.draw(display, localColors.bodyColor, Y_OFFSET, localColors.headColor);

    return ate;
}

// Communication UART

/**
 * @brief Envoie la direction du serpent local sur l’UART.
 * @param ate true si le serpent vient de manger un fruit, false sinon.
 */
void SnakeGame::sendDirection(bool ate) {
    uint8_t data = static_cast<uint8_t>(localSnake.getDirection());
    if (ate) {
        data |= 0x10;  // Bit 4 = fruit mangé
    }
    HAL_UART_Transmit(huart, &data, 1, 10);
}

/**
 * @brief Envoie un signal de fin de partie sur l’UART.
 *
 * Envoie 3 fois l’octet 0xFF avec un court délai entre chaque envoi.
 */
void SnakeGame::sendGameOver() {
    uint8_t gameOverSignal = 0xFF;
    for (int i = 0; i < 3; i++) {
        HAL_UART_Transmit(huart, &gameOverSignal, 1, 100);
        HAL_Delay(10);
    }
}

// Collisions

/**
 * @brief Vérifie les collisions du serpent local.
 * @return Type de collision détectée (SELF, OPPONENT, WALL ou NONE).
 */
CollisionType SnakeGame::checkCollisions() {
    tile head = localSnake.headTile();

    // Collision avec soi-même
    int idx;
    if (localSnake.checkSelfCollision(idx)) {
        return CollisionType::SELF;
    }

    // Collision avec l’adversaire
    if (remoteSnake.containsPoint(head.x, head.y)) {
        return CollisionType::OPPONENT;
    }

    // Collision avec les bords de l’aire de jeu
    if (head.x < 0 || head.x >= GRID_W || head.y < 0 || head.y >= GRID_H) {
        return CollisionType::WALL;
    }

    return CollisionType::NONE;
}

// Gestion des fruits

/**
 * @brief Génère tous les fruits initiaux à des positions valides.
 */
void SnakeGame::spawnAllFruits() {
    numFruits = 0;
    for (int i = 0; i < MAX_FRUITS; i++) {
        fruits[i].x = 0;
        fruits[i].y = 0;
        respawnFruit(i);
        numFruits++;
    }
}

/**
 * @brief Repositionne un fruit à une nouvelle position libre.
 * @param index Index du fruit à repositionner.
 */
void SnakeGame::respawnFruit(int index) {
    if (index < 0 || index >= MAX_FRUITS) return;

    int attempts = 0;
    bool valid = false;

    while (!valid && attempts < 100) {
        int x = rand() % GRID_W;
        int y = rand() % GRID_H;

        // Vérifier que la case n’est pas occupée
        bool occupied = false;

        // Pas sur les serpents
        if (localSnake.containsPoint(x, y) || remoteSnake.containsPoint(x, y)) {
            occupied = true;
        }

        // Pas sur un autre fruit
        for (int i = 0; i < numFruits && !occupied; i++) {
            if (i != index && fruits[i].x == x && fruits[i].y == y) {
                occupied = true;
            }
        }

        if (!occupied) {
            fruits[index].x = x;
            fruits[index].y = y;
            valid = true;
        }

        attempts++;
    }
}

/**
 * @brief Vérifie si la tête du serpent local touche un fruit.
 * @return Index du fruit touché, ou -1 si aucun fruit n’est touché.
 */
int SnakeGame::checkFruitCollision() {
    tile head = localSnake.headTile();

    for (int i = 0; i < numFruits; i++) {
        if (head.x == fruits[i].x && head.y == fruits[i].y) {
            return i;
        }
    }

    return -1;
}

// Affichage

/**
 * @brief Dessine un fruit sous forme de cercle coloré.
 * @param index Index du fruit à dessiner.
 *
 * Les fruits d’index pair sont roses, ceux d’index impair sont orange.
 */
void SnakeGame::drawFruit(int index) {
    if (index < 0 || index >= numFruits) return;

    int px = fruits[index].x * TILE_W + TILE_W / 2;
    int py = fruits[index].y * TILE_H + Y_OFFSET + TILE_H / 2;
    int radius = TILE_W / 2 - 1;

    Color fruitColor = (index % 2 == 0) ? Color::PINK : Color::ORANGE;

    display->fillCircle(fruitColor, px, py, radius);
}

/**
 * @brief Dessine tous les fruits à l’écran.
 */
void SnakeGame::drawAllFruits() {
    for (int i = 0; i < numFruits; i++) {
        drawFruit(i);
    }
}

/**
 * @brief Efface une tuile sur l’aire de jeu.
 * @param x Coordonnée x de la tuile (en cases).
 * @param y Coordonnée y de la tuile (en cases).
 */
void SnakeGame::eraseTile(int x, int y) {
    int px = x * TILE_W;
    int py = y * TILE_H + Y_OFFSET;
    display->fillRect(Color::BLACK, px, py, TILE_W, TILE_H);
}

} // namespace ELE3312

/**
 * @}
 */ // Fin du groupe ELE3312
