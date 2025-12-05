#include "Game.hpp"
#include "IRule.hpp"
#include "AliveState.hpp"
#include "DeadState.hpp"
#include "ObstacleState.hpp"
#include <utility>
#include <iostream>
#include <string>

using namespace std;
//Constructeur : Prépare la grille et ouvre le fichier de log
Game::Game(int l, int c, unique_ptr<IRule> r, bool torique)
    : grid_(l, c, torique), regle_(move(r)), gen_(0) 
{
    fichierLog_.open("simulation_log.txt");
    if (!fichierLog_) cerr << "Erreur log." << endl;
    else journaliser();// Enregistre l'état initial (Gen 0)
}
//Active les cellules vivantes données par l'utilisateur
void Game::initGrille(const vector<pair<int, int>>& vivantes) {
    for (auto p : vivantes) {
        if (grid_.estDansGrille(p.first, p.second)) {
            grid_.cellule(p.first, p.second).setEtat(make_unique<AliveState>());
        }
    }
    //Si on est au début, on met à jour le log immédiatement
    if (gen_ == 0 && fichierLog_) { fichierLog_.seekp(0); journaliser(); }
}
//Place les obstacles 
void Game::initObstacles(const vector<pair<int, int>>& obstacles) {
    for (auto p : obstacles) {
        if (grid_.estDansGrille(p.first, p.second)) {
            grid_.cellule(p.first, p.second).setEtat(make_unique<ObstacleState>());
        }
    }
    if (gen_ == 0 && fichierLog_) { fichierLog_.seekp(0); journaliser(); }
}
//passage à la génération suivante
void Game::suivante() {
    grid_.calculerSuivante(*regle_);
    gen_++;
    journaliser();
}
//Écrit une représentation texte de la grille dans le fichier
void Game::journaliser() {
    if (!fichierLog_) return;
    fichierLog_ << "--- Gen " << gen_ << " ---\n";
    for (int i = 0; i < grid_.nbLignes(); ++i) {
        for (int j = 0; j < grid_.nbCols(); ++j) {
            string nom = grid_.cellule(i, j).getEtat()->getNom();
            //Symboles : O = Vivant, # = Obstacle, . = Mort
            if (nom == "Alive") fichierLog_ << 'O';
            else if (nom == "Obstacle") fichierLog_ << '#';
            else fichierLog_ << '.';
        }
        fichierLog_ << '\n';
    }
    fichierLog_ << "\n";
    fichierLog_.flush(); 
}