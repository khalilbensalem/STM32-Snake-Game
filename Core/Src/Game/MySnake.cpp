/**
 * @file MySnake.cpp
 * @brief Implémentation de la classe MySnake représentant le serpent du jeu.
 *
 * Ce fichier contient l'implémentation des méthodes de la classe MySnake,
 * responsable du mouvement, de la direction et de la croissance
 * du serpent dans la grille de jeu.
 *
 * @date 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri (2216665)
 */

#include "Game/MySnake.h"
#include "Game/Tiles.h"

using namespace ELE3312;

/**
 * @brief Constructeur du serpent.
 *
 * Initialise un nouveau serpent avec une tête positionnée aux coordonnées (sx, sy)
 * et une direction initiale donnée.
 *
 * @param sx Coordonnée X initiale de la tête.
 * @param sy Coordonnée Y initiale de la tête.
 * @param d  Direction initiale du serpent.
 */
MySnake::MySnake(int sx, int sy, Direction d)
: head(0), tail(0), dir(d) {
  body[0] = {sx, sy, TILE_SNAKE_HEAD};
}

/**
 * @brief Réinitialise la position et la direction du serpent.
 *
 * Replace la tête du serpent aux coordonnées (sx, sy) et
 * réinitialise la direction à EST.
 *
 * @param sx Nouvelle coordonnée X de la tête.
 * @param sy Nouvelle coordonnée Y de la tête.
 */
void MySnake::init(int sx, int sy) {
  head = 0;
  tail = 0;
  dir = EST;
  body[0] = {sx, sy, TILE_SNAKE_HEAD};
}

/**
 * @brief Change la direction du serpent.
 *
 * Modifie la direction actuelle du serpent,
 * tout en interdisant le demi-tour immédiat (180 degrés).
 *
 * @param d Nouvelle direction souhaitée.
 * @return true si la direction a été changée avec succès, false sinon.
 */
bool MySnake::setDirection(Direction d) {
  if ((dir == NORD  && d == SUD ) || (dir == SUD  && d == NORD) ||
      (dir == EST   && d == OUEST) || (dir == OUEST && d == EST)) {
    return false;
  }
  dir = d;
  return true;
}

/**
 * @brief Déplace le serpent d'une case dans la direction actuelle.
 *
 * Gère le mouvement de la tête et du corps, ainsi que la
 * réapparition sur les bords de la grille. Si le paramètre eat est nul,
 * la queue avance également (le serpent garde la même longueur).
 * Sinon, la queue ne bouge pas et le serpent grandit.
 *
 * @param eat Indicateur de croissance (0 = ne mange pas, !0 = mange).
 */
void MySnake::move(int eat) {
  const int newHead = ring(head + 1);

  //Copie la position de l'ancienne tête
  body[newHead] = body[head];

  //Avance d'une case selon la direction courante
  switch (dir) {
    case NORD: body[newHead].y--; break;
    case SUD : body[newHead].y++; break;
    case EST : body[newHead].x++; break;
    case OUEST:body[newHead].x--; break;
  }

  //Réapparition
  if (body[newHead].x < 0)          body[newHead].x += GRID_W;
  if (body[newHead].y < 0)          body[newHead].y += GRID_H;
  if (body[newHead].x >= GRID_W)    body[newHead].x -= GRID_W;
  if (body[newHead].y >= GRID_H)    body[newHead].y -= GRID_H;

  //L'ancienne tête devient une tuile de corps
  body[head].id = TILE_SNAKE_BODY;

  //Nouvelle tête
  body[newHead].id = TILE_SNAKE_HEAD;
  head = newHead;

  if (eat == 0) {
    tail = ring(tail + 1);
  }
  //Sinon (eat != 0), la queue ne bouge pas, le serpent grandit
}

/**
 * @brief Fait tourner le serpent vers la gauche ou la droite.
 *
 * Modifie la direction du serpent en ajoutant un décalage
 * relatif (+1 ou -1) selon la rotation souhaitée.
 *
 * @param direction Sens de rotation : +1 (droite) ou -1 (gauche).
 */
void MySnake::turn(int direction) {
  dir = static_cast<Direction>((dir + 4 + direction) % 4);
}
