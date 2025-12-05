#pragma once
#include "CellState.hpp"
#include <memory>

class DeadState : public CellState {
public:
    bool estVivant() const override;
    std::unique_ptr<CellState> cloner() const override;
    const char* getNom() const override;
};
