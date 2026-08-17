/**
 * @file Tiles.h
 * @brief Définition des constantes de grille et du type tuile pour PolySnake.
 *
 * Ce fichier définit la taille de la grille de jeu, les dimensions des tuiles,
 * ainsi que les identifiants et structures associés à chaque type de tuile
 * utilisée dans le jeu PolySnake (vide, serpent, fruit, etc.).
 *
 * @date 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 */

#pragma once
#include <cstdint>

namespace ELE3312 {

/** @brief Largeur de la grille en nombre de tuiles. */
constexpr int GRID_W = 32;

/** @brief Hauteur de la grille en nombre de tuiles. */
constexpr int GRID_H = 24;

/** @brief Largeur d’une tuile à l’écran (en pixels). */
constexpr int TILE_W = 10;

/** @brief Hauteur d’une tuile à l’écran (en pixels). */
constexpr int TILE_H = 10;

/**
 * @enum TileID
 * @brief Identifiants des différents types de tuiles.
 *
 * Ces identifiants permettent de distinguer le contenu d’une tuile
 * dans la grille : vide, tête du serpent, corps, ou fruit.
 */
enum : int {
  TILE_EMPTY      = 0,  // Tuile vide.
  TILE_SNAKE_HEAD = 1,  //Tête du serpent.
  TILE_SNAKE_BODY = 2,  //Corps du serpent.
  TILE_FRUIT_A    = 10, //Premier type de fruit.
  TILE_FRUIT_B    = 11, //Deuxième type de fruit.
};

/**
 * @struct tile
 * @brief Représente une tuile dans la grille du jeu.
 *
 * Chaque tuile contient ses coordonnées discrètes (x, y)
 * et un identifiant indiquant son contenu (vide, serpent, fruit, etc.).
 */
struct tile {
  int x;  //Coordonnée X dans la grille.
  int y;  //Coordonnée Y dans la grille.
  int id; //Identifiant du contenu de la tuile.
};

} // namespace ELE3312
