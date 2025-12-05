#pragma once
#include "CellState.hpp"
// Représente une cellule vivante.
class AliveState : public CellState {
public:
    bool estVivant() const override;
    std::unique_ptr<CellState> cloner() const override;
    const char* getNom() const override;
};