#pragma once
#include "CellState.hpp"
#include <memory>
// Structure simple pour stocker les coordonnées (x = ligne, y = colonne).
struct Position {
    int x, y; 
    Position(int r = 0, int c = 0) : x(r), y(c) {}
};
// Classe représentant une case de la grille.
class Cell {
private:
    Position pos_;
    std::unique_ptr<CellState> etat_; // Pointeur vers l'état actuel (Alive/Dead/Obstacle)

public:
    Cell();
    Cell(int x, int y, std::unique_ptr<CellState> etat);
    // Constructeur de copie (nécessaire car unique_ptr ne se copie pas automatiquement).
    Cell(const Cell& autre);
    // Opérateur d'affectation pour copier une cellule vers une autre.
    Cell& operator=(const Cell& autre);

    Position getPos() const;
    void setPos(int x, int y);

    bool estVivant() const;
    
    void setEtat(std::unique_ptr<CellState> nouvEtat);
    const CellState* getEtat() const;
    // Crée une copie de la cellule.
    Cell cloner() const;
};