#pragma once
#include "IView.hpp"
#include "Game.hpp"

class ConsoleView : public IView {
private:
    int maxGen_;
    int delai_;

public:
    // Constructeur avec valeurs par défaut
    ConsoleView(int maxGen = 200, int delai = 100);
    ~ConsoleView() override = default;

    void lancer(Game*& game) override;
    void dessiner(const Game* game) override;
    void arreter() override;
};