#pragma once
#include <memory>

// Interface (classe abstraite) représentant un état générique d'une cellule
// Sert de base pour AliveState, DeadState et ObstacleState
class CellState {
public:
    virtual ~CellState() = default;
    // Indique si cet état est considéré comme "vivant" pour le calcul des voisins
    virtual bool estVivant() const = 0;
    // Crée une copie exacte de l'état
    virtual std::unique_ptr<CellState> cloner() const = 0;
    // Renvoie le nom de l'état ("Alive", "Dead", "Obstacle") pour l'affichage ou le débogage.
    virtual const char* getNom() const = 0;
};