#include "SFMLView.hpp"
#include "ConwayRule.hpp"
#include "AliveState.hpp"
#include "DeadState.hpp"
#include "ObstacleState.hpp"
#include <iostream>
#include <sstream>
#include <random>

using namespace std;


SFMLView::SFMLView() 
    : running_(false), modeTorique_(false), iteration_(0), zoomActuel_(1.f), panning_(false) {
    
    window_.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jeu de la Vie");
    window_.setFramerateLimit(60);

    if (!font_.loadFromFile("arial.ttf")) {
        // Fallback Linux si arial n'est pas là
        if (!font_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            cerr << "Erreur police." << endl;
        }
    }

    gridView_.reset(sf::FloatRect(0.f, 0.f, (float)GRID_PIXEL_WIDTH, (float)GRID_PIXEL_HEIGHT));
    float vpW = (float)GRID_PIXEL_WIDTH / (float)WINDOW_WIDTH;
    float vpH = (float)GRID_PIXEL_HEIGHT / (float)WINDOW_HEIGHT;
    gridView_.setViewport(sf::FloatRect(0.f, 0.f, vpW, vpH));

    initBoutons();
}

SFMLView::~SFMLView() {
    if (window_.isOpen()) window_.close();
}

void SFMLView::initBouton(Bouton& btn, sf::Vector2f pos, sf::Vector2f size, sf::Color col, const string& lab) {
    btn.rect.setSize(size);
    btn.rect.setPosition(pos);
    btn.rect.setFillColor(col);
    btn.label = lab;
    btn.texte.setFont(font_);
    btn.texte.setString(lab);
    btn.texte.setCharacterSize(18);
    
    sf::FloatRect bounds = btn.texte.getLocalBounds();
    btn.texte.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    btn.texte.setPosition(pos.x + size.x / 2.f, pos.y + size.y / 2.f);
    
    if (col == sf::Color::Yellow || col == sf::Color::White) 
        btn.texte.setFillColor(sf::Color::Black);
}

void SFMLView::initBoutons() {
    float y = 10.f, h = 40.f, w = 100.f, m = 10.f;
    float x = (float)GRID_PIXEL_WIDTH + m;

    initBouton(btnStart_, {x, y}, {w, h}, sf::Color::Green, "Start");
    x += w + m;
    initBouton(btnStop_, {x, y}, {w, h}, sf::Color::Red, "Stop");

    y += h + m; x = (float)GRID_PIXEL_WIDTH + m;
    initBouton(btnStep_, {x, y}, {w, h}, sf::Color(0, 150, 200), "Step");
    x += w + m;
    initBouton(btnClear_, {x, y}, {w, h}, sf::Color::Blue, "Clear");

    y += h + m; x = (float)GRID_PIXEL_WIDTH + m;
    initBouton(btnToric_, {x, y}, {140.f, h}, sf::Color::Yellow, "Toric: OFF");

    y += h + m; x = (float)GRID_PIXEL_WIDTH + m;
    initBouton(btnRandom_, {x, y}, {140.f, h}, sf::Color::Magenta, "Random");

    txtIteration_.setFont(font_);
    txtIteration_.setCharacterSize(18);
    txtIteration_.setFillColor(sf::Color::White);
    txtIteration_.setPosition(GRID_PIXEL_WIDTH + m, y + h + 20.f);

    txtMode_.setFont(font_);
    txtMode_.setCharacterSize(18);
    txtMode_.setFillColor(sf::Color::White);
    txtMode_.setPosition(GRID_PIXEL_WIDTH + m, y + h + 50.f);
}
//Boucle principale de l'interface graphique
void SFMLView::lancer(Game*& game) {
    sf::Clock horloge;
    const sf::Time tempsParTour = sf::milliseconds(SPEED_MS);

    while (window_.isOpen()) {
        gererEvenements(game);//souris/clavier
        //Si le jeu tourne (bouton Start), on calcule la suite periodiquement
        if (running_ && game) {
            if (horloge.getElapsedTime() >= tempsParTour) {
                game->suivante();
                iteration_++;
                horloge.restart();
            }
        }
        dessiner(game); //Rafraichit l'ecran
    }
}
//Traite les evenements SFML(fermeture, clics, zoom)
void SFMLView::gererEvenements(Game*& game) {
    sf::Event ev;
    while (window_.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed) arreter();

        // Zoom/Pan
        if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Right) {
            panning_ = true;
            dernierPosSouris_ = sf::Mouse::getPosition(window_);
        } else if (ev.type == sf::Event::MouseButtonReleased && ev.mouseButton.button == sf::Mouse::Right) {
            panning_ = false;
        } else if (ev.type == sf::Event::MouseMoved && panning_) {
            sf::Vector2i curPos = sf::Mouse::getPosition(window_);
            sf::Vector2f before = window_.mapPixelToCoords(dernierPosSouris_, gridView_);
            sf::Vector2f after = window_.mapPixelToCoords(curPos, gridView_);
            gridView_.move(before - after);
            ajusterVue();
            dernierPosSouris_ = curPos;
        }

        if (ev.type == sf::Event::MouseWheelScrolled && ev.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            sf::Vector2i pixel(ev.mouseWheelScroll.x, ev.mouseWheelScroll.y);
            if (pixel.x < GRID_PIXEL_WIDTH) { 
                float fact = (ev.mouseWheelScroll.delta > 0) ? (1.f / 1.15f) : 1.15f;
                float zoom = zoomActuel_ * fact;
                if (zoom >= 0.1f && zoom <= 2.0f) {
                    sf::Vector2f avant = window_.mapPixelToCoords(pixel, gridView_);
                    gridView_.zoom(fact);
                    zoomActuel_ = zoom;
                    sf::Vector2f apres = window_.mapPixelToCoords(pixel, gridView_);
                    gridView_.move(avant - apres);
                    ajusterVue();
                }
            }
        }

        if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left) {
            gererClic(game, {(float)ev.mouseButton.x, (float)ev.mouseButton.y});
        }
    }
}
//Agit selon l'endroit clique(Menu ou Grille)
void SFMLView::gererClic(Game*& game, sf::Vector2f posSouris) {
    if (posSouris.x >= GRID_PIXEL_WIDTH) { // Interface
        if (btnStart_.rect.getGlobalBounds().contains(posSouris)) running_ = true;
        else if (btnStop_.rect.getGlobalBounds().contains(posSouris)) running_ = false;
        else if (btnClear_.rect.getGlobalBounds().contains(posSouris)) {
            running_ = false;
            resetJeu(game, modeTorique_);
            iteration_ = 0;
        } else if (btnToric_.rect.getGlobalBounds().contains(posSouris)) {
            modeTorique_ = !modeTorique_;
            resetJeu(game, modeTorique_);
            running_ = false;
            iteration_ = 0;
        } else if (btnStep_.rect.getGlobalBounds().contains(posSouris) && !running_) {
            if (game) { game->suivante(); iteration_++; }
        } else if (btnRandom_.rect.getGlobalBounds().contains(posSouris)) {
            running_ = false;
            double pVie = 0.0, pObst = 0.0;
            cout << "Probabilite Vie (0.0-1.0): "; cin >> pVie;
            cout << "Probabilite Obstacle (0.0-1.0): "; cin >> pObst;
            
            if (game) remplirAleatoire(*game->grille(), pVie, pObst);
            
            iteration_ = 0;
        }
    } else { // Grille
        sf::Vector2f monde = window_.mapPixelToCoords((sf::Vector2i)posSouris, gridView_);
        int c = (int)monde.x / CELL_SIZE;
        int r = (int)monde.y / CELL_SIZE;
        if (game && game->grille()->estDansGrille(r, c)) {
            changerEtat(game->grille()->cellule(r, c));
        }
    }
}

void SFMLView::dessiner(const Game* game) {
    window_.clear(sf::Color(10, 10, 30));

    window_.setView(gridView_);
    if (game) {
        const Grid* g = game->grille();
        for (int r = 0; r < g->nbLignes(); ++r) {
            for (int c = 0; c < g->nbCols(); ++c) {
                //Dessin des cellules vivantes(Vert) et obstacles(Gris)
                const Cell& cell = g->cellule(r, c);
                string nom = cell.getEtat()->getNom();
                if (nom != "Dead") {
                    sf::RectangleShape rect({(float)CELL_SIZE - 1.f, (float)CELL_SIZE - 1.f});
                    rect.setPosition((float)c * CELL_SIZE, (float)r * CELL_SIZE);
                    if (nom == "Alive") rect.setFillColor(sf::Color::Green);
                    else if (nom == "Obstacle") rect.setFillColor(sf::Color(120, 120, 120));
                    window_.draw(rect);
                }
            }
        }
        // Lignes
        sf::VertexArray lines(sf::Lines);
        for (int r = 0; r <= g->nbLignes(); ++r) {
            lines.append(sf::Vertex({0.f, (float)r * CELL_SIZE}, sf::Color(30, 30, 60)));
            lines.append(sf::Vertex({(float)GRID_PIXEL_WIDTH, (float)r * CELL_SIZE}, sf::Color(30, 30, 60)));
        }
        for (int c = 0; c <= g->nbCols(); ++c) {
            lines.append(sf::Vertex({(float)c * CELL_SIZE, 0.f}, sf::Color(30, 30, 60)));
            lines.append(sf::Vertex({(float)c * CELL_SIZE, (float)GRID_PIXEL_HEIGHT}, sf::Color(30, 30, 60)));
        }
        window_.draw(lines);
    }

    window_.setView(window_.getDefaultView());
    sf::RectangleShape sidebar;
    sidebar.setSize({(float)SIDEBAR_WIDTH, (float)WINDOW_HEIGHT});
    sidebar.setPosition({(float)GRID_PIXEL_WIDTH, 0.f});
    sidebar.setFillColor(sf::Color(20, 20, 40));
    window_.draw(sidebar);

    stringstream ss; ss << "Gen: " << iteration_;
    txtIteration_.setString(ss.str());
    txtMode_.setString(modeTorique_ ? "Mode: Torique" : "Mode: Normal");
    btnToric_.texte.setString(modeTorique_ ? "Torique: ON" : "Torique: OFF");

    auto dessinerBtn = [&](const Bouton& b) { window_.draw(b.rect); window_.draw(b.texte); };
    dessinerBtn(btnStart_); dessinerBtn(btnStop_); dessinerBtn(btnClear_);
    dessinerBtn(btnToric_); dessinerBtn(btnStep_); dessinerBtn(btnRandom_);
    window_.draw(txtIteration_); window_.draw(txtMode_);

    window_.display();
}

void SFMLView::arreter() { window_.close(); }

void SFMLView::ajusterVue() {
    sf::Vector2f size = gridView_.getSize();
    sf::Vector2f center = gridView_.getCenter();
    if (size.x >= GRID_PIXEL_WIDTH) center.x = GRID_PIXEL_WIDTH / 2.f;
    else { 
        float half = size.x / 2.f; 
        if (center.x < half) center.x = half; 
        if (center.x > GRID_PIXEL_WIDTH - half) center.x = GRID_PIXEL_WIDTH - half; 
    }
    if (size.y >= GRID_PIXEL_HEIGHT) center.y = GRID_PIXEL_HEIGHT / 2.f;
    else { 
        float half = size.y / 2.f; 
        if (center.y < half) center.y = half; 
        if (center.y > GRID_PIXEL_HEIGHT - half) center.y = GRID_PIXEL_HEIGHT - half; 
    }
    gridView_.setCenter(center);
}
//Cycle l'etat d'une cellule manuellement(Mort -> Vivant -> Obstacle -> Mort)
void SFMLView::changerEtat(Cell& c) {
    string nom = c.getEtat()->getNom();
    if (nom == "Dead") c.setEtat(make_unique<AliveState>());
    else if (nom == "Alive") c.setEtat(make_unique<ObstacleState>());
    else c.setEtat(make_unique<DeadState>());
}

// IMPLÉMENTATION DE LA MÉTHODE MANQUANTE 
void SFMLView::remplirAleatoire(Grid& grid, double probVie, double probObst) {
    static mt19937 gen(random_device{}()); 
    uniform_real_distribution<> dis(0.0, 1.0);
    for (int r = 0; r < grid.nbLignes(); ++r) {
        for (int c = 0; c < grid.nbCols(); ++c) {
            double v = dis(gen);
            if (v < probObst) grid.cellule(r, c).setEtat(make_unique<ObstacleState>());
            else if (v < probObst + probVie) grid.cellule(r, c).setEtat(make_unique<AliveState>());
            else grid.cellule(r, c).setEtat(make_unique<DeadState>());
        }
    }
}
//Recree un jeu neuf (utilise par le bouton Clear)
void SFMLView::resetJeu(Game*& game, bool torique) {
    if (game) delete game;
    auto regle = make_unique<ConwayRule>();
    game = new Game(ROWS, COLS, move(regle), torique);
}