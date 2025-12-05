#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "IView.hpp"
#include "Game.hpp"
#include "Grid.hpp"

struct Bouton {
    sf::RectangleShape rect;
    sf::Text texte;
    std::string label;
};

class SFMLView : public IView {
public:
    static const int CELL_SIZE = 10;
    static const int ROWS = 80;
    static const int COLS = 100;
    static const int GRID_PIXEL_WIDTH = COLS * CELL_SIZE;
    static const int GRID_PIXEL_HEIGHT = ROWS * CELL_SIZE;
    static const int WINDOW_WIDTH = 1200;
    static const int WINDOW_HEIGHT = 800;
    static const int SIDEBAR_WIDTH = WINDOW_WIDTH - GRID_PIXEL_WIDTH;
    static const int SPEED_MS = 100;

    SFMLView();
    ~SFMLView() override;

    void lancer(Game*& game) override;
    void dessiner(const Game* game) override;
    void arreter() override;

private:
    sf::RenderWindow window_;
    sf::Font font_;
    sf::View gridView_;
    float zoomActuel_;
    bool panning_;
    sf::Vector2i dernierPosSouris_;

    bool running_;
    bool modeTorique_;
    int iteration_;

    Bouton btnStart_, btnStop_, btnStep_, btnClear_, btnToric_, btnRandom_;
    sf::Text txtIteration_, txtMode_;

    void initBoutons();
    void initBouton(Bouton& btn, sf::Vector2f pos, sf::Vector2f size, sf::Color col, const std::string& lab);
    
    void gererEvenements(Game*& game);
    void gererClic(Game*& game, sf::Vector2f posSouris);
    void ajusterVue();
    
    void changerEtat(Cell& cell);
    void resetJeu(Game*& game, bool torique);
    
    // C'est cette méthode qui manquait dans le CPP précédent
    void remplirAleatoire(Grid& grid, double probVie, double probObst);
};