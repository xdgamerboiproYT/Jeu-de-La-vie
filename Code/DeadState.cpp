#include "DeadState.hpp"
#include <memory>

bool DeadState::estVivant() const {
    return false;
}

std::unique_ptr<CellState> DeadState::cloner() const {
    return std::make_unique<DeadState>(*this);
}

const char* DeadState::getNom() const {
    return "Dead";
}
