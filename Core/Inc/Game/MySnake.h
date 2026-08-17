/**
 * @file MySnake.h
 * @brief Déclaration de la classe MySnake représentant le serpent du jeu.
 *
 * Cette classe implémente un serpent à l’aide d’un tampon circulaire
 * (head/tail) pour gérer efficacement le déplacement et la croissance.
 *
 * @date 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 */

#pragma once

#include "Game/Tiles.h"
#include "Interfaces/Display/Display.h"

namespace ELE3312 {

/**
 * @brief Directions possibles pour le serpent.
 */
enum Direction { NORD, EST, SUD, OUEST };

/**
 * @brief Représente le serpent du jeu à l’aide d’un tampon circulaire.
 */
class MySnake {
public:
    /**
     * @brief Constructeur du serpent.
     * @param startX Position X initiale de la tête.
     * @param startY Position Y initiale de la tête.
     * @param d Direction initiale.
     */
    MySnake(int startX = 5, int startY = 5, Direction d = EST);

    /**
     * @brief Initialise la position du serpent.
     * @param startX Position X de départ.
     * @param startY Position Y de départ.
     */
    void init(int startX, int startY);

    /**
     * @brief Change la direction du serpent.
     * @param d Nouvelle direction.
     * @return true si la direction a été acceptée, false sinon.
     */
    bool setDirection(Direction d);

    /**
     * @brief Retourne la direction actuelle.
     */
    Direction direction() const { return dir; }

    /**
     * @brief Retourne la direction actuelle (alias).
     */
    Direction getDirection() const { return dir; }

    /**
     * @brief Déplace le serpent.
     * @param eat Valeur non nulle si le serpent grandit (fruit mangé).
     */
    void move(int eat = 0);

    /**
     * @brief Fait tourner le serpent (non utilisée dans PolySnake).
     * @param direction Sens de rotation.
     */
    void turn(int direction);

    /**
     * @brief Retourne un pointeur constant sur le tampon du corps.
     */
    const tile* data() const { return body; }

    /**
     * @brief Retourne l’index de la tête dans le tampon.
     */
    int headIndex() const { return head; }

    /**
     * @brief Retourne l’index de la queue dans le tampon.
     */
    int tailIndex() const { return tail; }

    /**
     * @brief Retourne la taille maximale du serpent.
     */
    int maxSize() const { return MAX_SIZE; }

    /**
     * @brief Retourne la tuile correspondant à la tête.
     */
    tile headTile() const { return body[head]; }

    /**
     * @brief Retourne la tuile correspondant à la queue.
     */
    tile getTailTile() const { return body[tail]; }

    /**
     * @brief Retourne la longueur actuelle du serpent.
     */
    int getLength() const {
        int len = head - tail;
        if (len < 0) len += MAX_SIZE;
        return len + 1;
    }

    /**
     * @brief Fixe directement la position de la tête.
     * @param x Nouvelle coordonnée X.
     * @param y Nouvelle coordonnée Y.
     */
    void setHeadPosition(int x, int y) {
        body[head].x = x;
        body[head].y = y;
    }

    /**
     * @brief Fixe la longueur du serpent.
     * @param len Nouvelle longueur (bornée entre 1 et MAX_SIZE).
     */
    void setLength(int len) {
        if (len > MAX_SIZE) len = MAX_SIZE;
        if (len < 1) len = 1;
        tail = ring(head - len + 1);
    }

    /**
     * @brief Vérifie si une case est occupée par le serpent.
     * @param x Coordonnée X à tester.
     * @param y Coordonnée Y à tester.
     * @return true si la case appartient au serpent, false sinon.
     */
    bool containsPoint(int x, int y) const {
        int idx = tail;
        while (true) {
            if (body[idx].x == x && body[idx].y == y) return true;
            if (idx == head) break;
            idx = ring(idx + 1);
        }
        return false;
    }

    /**
     * @brief Vérifie si la tête touche une partie du corps.
     * @param collisionIdx Index de la case en collision (si true).
     * @return true en cas d’auto-collision, false sinon.
     */
    bool checkSelfCollision(int& collisionIdx) const {
        int idx = tail;
        while (idx != head) {
            if (body[head].x == body[idx].x && body[head].y == body[idx].y) {
                collisionIdx = idx;
                return true;
            }
            idx = ring(idx + 1);
        }
        return false;
    }

    /**
     * @brief Efface visuellement tout le serpent de l'écran.
     *
     * Utilisé notamment pour le serpent distant : on efface tout le corps,
     * car on ne connaît pas son historique complet de positions.
     *
     * @param display Pointeur vers l'afficheur.
     * @param yOffset Décalage vertical en pixels (ex. 25 pour laisser le titre).
     */
    void erase(Display* display, int yOffset = 0) const {
        int localHead = head;
        int localTail = tail;

        int len = localHead - localTail;
        if (len < 0) len += MAX_SIZE;
        if (len >= MAX_SIZE || len < 0) return;

        int idx = localTail;
        int count = 0;
        const int maxIterations = len + 1;

        while (count < maxIterations) {
            int px = body[idx].x * TILE_W;
            int py = body[idx].y * TILE_H + yOffset;
            display->fillRect(Color::BLACK, px, py, TILE_W, TILE_H);

            if (idx == localHead) break;
            idx = ring(idx + 1);
            count++;
        }
    }

    /**
     * @brief Dessine le serpent sur l’écran.
     *
     * @param display Pointeur vers l’afficheur.
     * @param bodyColor Couleur du corps.
     * @param yOffset Décalage vertical en pixels.
     * @param headColor Couleur de la tête (par défaut ORANGE).
     */
    void draw(Display* display, Color bodyColor, int yOffset = 0,
              Color headColor = Color::ORANGE) const {
        int localHead = head;
        int localTail = tail;

        int len = localHead - localTail;
        if (len < 0) len += MAX_SIZE;
        if (len >= MAX_SIZE || len < 0) return;

        int idx = localTail;
        int count = 0;
        const int maxIterations = len + 1;

        while (count < maxIterations) {
            int px = body[idx].x * TILE_W;
            int py = body[idx].y * TILE_H + yOffset;
            Color color = (idx == localHead) ? headColor : bodyColor;
            display->fillRect(color, px, py, TILE_W, TILE_H);

            if (idx == localHead) break;
            idx = ring(idx + 1);
            count++;
        }
    }

private:
    static constexpr int MAX_SIZE = 100; //Taille maximale du serpent.
    tile body[MAX_SIZE];                 //Tampon circulaire des cases du corps.
    int head;                            //Index de la tête.
    int tail;                            //Index de la queue.
    Direction dir;                       //Direction actuelle.

    /**
     * @brief Convertit un index en index circulaire [0, MAX_SIZE).
     * @param i Index (potentiellement négatif ou > MAX_SIZE).
     * @return Index replié dans [0, MAX_SIZE).
     */
    inline int ring(int i) const { return (i + MAX_SIZE) % MAX_SIZE; }
};

} // namespace ELE3312
