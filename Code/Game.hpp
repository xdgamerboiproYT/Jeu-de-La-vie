#pragma once
#include "Grid.hpp"
#include "IRule.hpp"
#include <fstream>
#include <vector>

class Game {
private:
    Grid grid_;
    std::unique_ptr<IRule> regle_;
    int gen_;// Compteur de générations
    std::ofstream fichierLog_; // Fichier de sortie pour journaliser les états

    void journaliser(); // Écrit l'état actuel dans le fichier de log

public:
    Game(int l, int c, std::unique_ptr<IRule> r, bool torique);
    ~Game() = default;
    // Initialisation des éléments de jeu.
    void initGrille(const std::vector<std::pair<int, int>>& vivantes);
    void initObstacles(const std::vector<std::pair<int, int>>& obstacles);
    // Avance le jeu d'un pas de temps.
    void suivante();
    // Accesseurs pour la Vue.
    const Grid* grille() const { return &grid_; }
    Grid* grille() { return &grid_; }
    
    int numGeneration() const { return gen_; }
};