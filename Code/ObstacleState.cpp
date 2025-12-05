#include "ObstacleState.hpp"
#include <memory>

bool ObstacleState::estVivant() const {
    return false; // ou true si tu veux que ça compte comme vivant
}

std::unique_ptr<CellState> ObstacleState::cloner() const {
    return std::make_unique<ObstacleState>(*this);
}

const char* ObstacleState::getNom() const {
    return "Obstacle";
}
