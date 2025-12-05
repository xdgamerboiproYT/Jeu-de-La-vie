#include "AliveState.hpp"
// Une cellule vivante compte comme 1 voisin pour les autres.
bool AliveState::estVivant() const { return true; }
// Crée une nouvelle instance indépendante de AliveState.
std::unique_ptr<CellState> AliveState::cloner() const { return std::make_unique<AliveState>(*this); }
const char* AliveState::getNom() const { return "Alive"; }