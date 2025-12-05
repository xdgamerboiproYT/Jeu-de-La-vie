#include "Grid.hpp"
#include "IRule.hpp"
#include <stdexcept>
#include <thread>

using namespace std;
//initialise les dimensions et crée les cellules mortes
Grid::Grid(int l, int c, bool torique) : lignes_(l), cols_(c), torique_(torique) {
    if (l <= 0 || c <= 0) throw invalid_argument("Dimensions invalides");
    cellules_.assign(lignes_, vector<Cell>(cols_));
    initCellules();
}
// Donne à chaque cellule ses coordonnées (x,y)
void Grid::initCellules() {
    for (int i = 0; i < lignes_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            cellules_[i][j].setPos(i, j);
        }
    }
}
//Vérifie si une coordonnée est valide
bool Grid::estDansGrille(int x, int y) const {
    return (x >= 0 && x < lignes_ && y >= 0 && y < cols_);
}

Cell& Grid::cellule(int x, int y) {
    if (!estDansGrille(x, y)) throw out_of_range("Hors limite");
    return cellules_[x][y];
}

const Cell& Grid::cellule(int x, int y) const {
    if (!estDansGrille(x, y)) throw out_of_range("Hors limite");
    return cellules_[x][y];
}
//Trouve les voisins. Si le monde est torique, on utilise le modulo (%) pour boucler
vector<Cell*> Grid::voisins(int x, int y) {
    vector<Cell*> res;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            
            if (torique_) {
                nx = (nx % lignes_ + lignes_) % lignes_;
                ny = (ny % cols_ + cols_) % cols_;
                res.push_back(&cellules_[nx][ny]);
            } else if (estDansGrille(nx, ny)) {
                res.push_back(&cellules_[nx][ny]);
            }
        }
    }
    return res;
}

// (La version const de voisins est identique mais retourne const Cell*)
vector<const Cell*> Grid::voisins(int x, int y) const {
    vector<const Cell*> res;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (torique_) {
                nx = (nx % lignes_ + lignes_) % lignes_;
                ny = (ny % cols_ + cols_) % cols_;
                res.push_back(&cellules_[nx][ny]);
            } else if (estDansGrille(nx, ny)) {
                res.push_back(&cellules_[nx][ny]);
            }
        }
    }
    return res;
}
//Calcule la prochaine génération en utilisant le multithreading
void Grid::calculerSuivante(const IRule& regle) {
    Grid grilleSuivante = *this; //On travaille sur une copie pour ne pas corrompre la lecture
    //Détection auto du nombre de cœurs CPU
    unsigned int nbThreads = std::thread::hardware_concurrency();
    if (nbThreads == 0) nbThreads = 2;
    if ((unsigned int)lignes_ < nbThreads) nbThreads = lignes_;

    vector<thread> threads;
    int lignesParThread = lignes_ / nbThreads;
    //Découpage du travail : chaque thread s'occupe d'un bloc de lignes.
    for (unsigned int i = 0; i < nbThreads; ++i) {
        int debut = i * lignesParThread;
        int fin = (i == nbThreads - 1) ? lignes_ : debut + lignesParThread;

        threads.emplace_back([this, &grilleSuivante, &regle, debut, fin]() {
            for (int r = debut; r < fin; ++r) {
                for (int c = 0; c < cols_; ++c) {
                    //On demande à la regle quel sera le futur etat.
                    auto nouvelEtat = regle.calculerEtat(cellules_[r][c], *this);
                    grilleSuivante.cellules_[r][c].setEtat(move(nouvelEtat));
                }
            }
        });
    }
    // On attend que tous les threads aient fini.
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    // On remplace l'ancienne grille par la nouvelle calculée
    *this = move(grilleSuivante);
}