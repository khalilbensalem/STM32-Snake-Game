/**
 * @file SessionManager.h
 * @brief Gestion du Session ID pour synchroniser les générateurs aléatoires.
 *
 * Cette classe statique fournit un identifiant de session commun aux deux
 * microcontrôleurs, permettant de synchroniser les générateurs aléatoires
 * Par exemple, pour générer les mêmes positions de fruits dans le jeu.
 *
 * @date 2025
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 */

#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <cstdint>
#include <cstdlib>

namespace ELE3312 {

/**
 * @class SessionManager
 * @brief Classe utilitaire pour la gestion d’un identifiant de session partagé.
 *
 * Cette classe fournit des méthodes statiques permettant :
 * - d’obtenir un identifiant de session unique partagé entre les deux cartes ;
 * - d’initialiser le générateur aléatoire à partir de cet ID.
 */
class SessionManager {
public:
    /**
     * @brief Obtient le Session ID partagé.
     *
     * Cet identifiant doit être identique sur les deux cartes afin de garantir
     * une synchronisation parfaite des événements aléatoires (comme
     * la génération de fruits).
     *
     * @return Identifiant de session (uint32_t).
     */
    static uint32_t getSessionID() {
        return 0x12345678;  ///< ID fixe pour la synchronisation.
    }

    /**
     * @brief Initialise le générateur aléatoire standard à partir du Session ID.
     *
     * Cette méthode doit être appelée en début de session pour s’assurer que
     * les deux systèmes produisent la même séquence aléatoire.
     *
     */
    static void initRandom() {
        srand(getSessionID());
    }
};

} // namespace ELE3312

#endif // SESSION_MANAGER_H
