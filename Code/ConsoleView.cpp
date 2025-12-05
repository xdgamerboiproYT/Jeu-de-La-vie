#include "ConsoleView.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

//Boucle principale pour le mode texte
ConsoleView::ConsoleView(int maxGen, int delai) 
    : maxGen_(maxGen), delai_(delai) {}

void ConsoleView::lancer(Game*& game) {
    if (!game) return;
    cout << "--- Mode Console ---" << endl;
    
    // Boucle simple sur un nombre max de générations
    for (int i = 0; i < maxGen_; ++i) {
        // Code ANSI pour effacer l'écran
        cout << "\033[2J\033[1;1H"; 
        cout << "Generation : " << game->numGeneration() << endl;
        dessiner(game);
        game->suivante();
        this_thread::sleep_for(chrono::milliseconds(delai_));// Pause pour lisibilité
    }
    arreter();
}
//Affiche la grille avec des caractères
void ConsoleView::dessiner(const Game* game) {
    if (!game) return;
    const Grid* g = game->grille();
    
    for (int i = 0; i < g->nbLignes(); ++i) {
        for (int j = 0; j < g->nbCols(); ++j) {
            string nom = g->cellule(i, j).getEtat()->getNom();
            
            if (nom == "Alive") cout << 'O';
            else if (nom == "Obstacle") cout << '#';
            else cout << '.';
            
            cout << ' ';
        }
        cout << '\n';
    }
}

void ConsoleView::arreter() {
    cout << "--- Fin de la simulation ---" << endl;
}