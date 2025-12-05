#include "Cell.hpp"
#include "DeadState.hpp"

using namespace std;
// Initialise une cellule morte à la position (0,0) par défaut.
Cell::Cell() : pos_(), etat_(make_unique<DeadState>()) {}

Cell::Cell(int x, int y, unique_ptr<CellState> etat) 
    : pos_(x, y), etat_(move(etat)) {
    if (!etat_) etat_ = make_unique<DeadState>();
}
//on clone l'état de l'autre cellule.
Cell::Cell(const Cell& autre) 
    : pos_(autre.pos_), etat_(autre.etat_ ? autre.etat_->cloner() : make_unique<DeadState>()) {}

Cell& Cell::operator=(const Cell& autre) {
    if (this == &autre) return *this;
    pos_ = autre.pos_;
    etat_ = autre.etat_ ? autre.etat_->cloner() : make_unique<DeadState>();
    return *this;
}

Position Cell::getPos() const { return pos_; }
void Cell::setPos(int x, int y) { pos_.x = x; pos_.y = y; }

bool Cell::estVivant() const { return etat_ ? etat_->estVivant() : false; }
// Change l'état (la mémoire de l'ancien état est libérée automatiquement par unique_ptr).
void Cell::setEtat(unique_ptr<CellState> nouvEtat) {
    etat_ = move(nouvEtat);
    if (!etat_) etat_ = make_unique<DeadState>();
}

const CellState* Cell::getEtat() const { return etat_.get(); }

Cell Cell::cloner() const { return Cell(*this); }