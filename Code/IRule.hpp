#pragma once
#include "Cell.hpp"
#include <memory>

class Grid; 
// Interface pour définir des règles de jeu
class IRule {
public:
    virtual ~IRule() = default;
    // Calcule le état d'une cellule en fonction de ses voisins.
    virtual std::unique_ptr<CellState> calculerEtat(const Cell& cell, const Grid& grid) const = 0;
};