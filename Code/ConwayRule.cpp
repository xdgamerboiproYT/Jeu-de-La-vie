#include "ConwayRule.hpp"
#include "AliveState.hpp"
#include "DeadState.hpp"
#include "ObstacleState.hpp"

std::unique_ptr<CellState> ConwayRule::calculerEtat(const Cell& cell, const Grid& grid) const {
    // Si obstacle, reste obstacle
    if (dynamic_cast<const ObstacleState*>(cell.getEtat())) 
        return std::make_unique<ObstacleState>();

    int vivants = 0;
    // Récupère les voisins autour de la cellule
    auto voisins = grid.voisins(cell.getPos().x, cell.getPos().y); 
    // Compte combien de voisins sont vivants.
    for (auto v : voisins) {
        if (v->estVivant()) ++vivants;
    }
    // Application des lois de la vie :
    if (cell.estVivant()) {
        if (vivants == 2 || vivants == 3) return std::make_unique<AliveState>();
        return std::make_unique<DeadState>();
    } else {
        if (vivants == 3) return std::make_unique<AliveState>();
        return std::make_unique<DeadState>();
    }
}