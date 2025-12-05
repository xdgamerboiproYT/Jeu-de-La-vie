#pragma once
#include <vector>
#include <memory>
#include "Cell.hpp"

class IRule; 

class Grid {
private:
    std::vector<std::vector<Cell>> cellules_;
    int lignes_, cols_;
    bool torique_; // Si vrai, les bords gauche/droite et haut/bas sont connectés.

    void initCellules();
    

public:
    Grid(int l, int c, bool torique);
    //Vérifie si une coordonnée est valide.
    bool estDansGrille(int x, int y) const;

    int nbLignes() const { return lignes_; }
    int nbCols() const { return cols_; }

    Cell& cellule(int x, int y); 
    const Cell& cellule(int x, int y) const;
    //Retourne la liste des cellules voisins.
    std::vector<Cell*> voisins(int x, int y); 
    std::vector<const Cell*> voisins(int x, int y) const;
    //génère la nouvelle grille en appliquant la règle.
    void calculerSuivante(const IRule& regle); 
};