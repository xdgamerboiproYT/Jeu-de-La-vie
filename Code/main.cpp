#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <utility> 
#include <random>
#include <thread>
#include <chrono>
#include <limits> 

//Model
#include "Game.hpp"
#include "ConwayRule.hpp"
//View
#include "SFMLView.hpp"
#include "ConsoleView.hpp"

using namespace std;

//pour stocker les données lues du fichier.
struct InitData {
    vector<pair<int, int>> aliveCoords;     // For 'O'
    vector<pair<int, int>> obstacleCoords;  // For '#'
};

//FUNCTIONS pour aider

//Vide le buffer d'entree en cas de mauvaise saisie
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Charge un fichier texte (O/#/.) et retourne les positions trouvees
InitData loadPatternFromFile(const string& filename, int offsetR, int offsetC) {
    InitData data;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erreur Impossible d'ouvrir " << filename << endl;
        return data;
    }
    string line;
    int r = 0;
    while (getline(file, line)) {
        for (int c = 0; c < line.size(); ++c) {
            if (line[c] == 'O' || line[c] == 'o' || line[c] == '1') {
                data.aliveCoords.push_back({r + offsetR, c + offsetC});
            }
            else if (line[c] == '#') {
                data.obstacleCoords.push_back({r + offsetR, c + offsetC});
            }
        }
        r++;
    }
    return data;
}

//genere des coordonnees aleatoires selon une densite donnee
vector<pair<int, int>> generateRandomCoordinates(int rows, int cols, double density) {
    vector<pair<int, int>> coords;
    static random_device rd; 
    static mt19937 gen(rd()); 
    uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (dis(gen) < density) {
                coords.push_back({r, c});
            }
        }
    }
    return coords;
}

// Retourne des coordonnees pour des motifs predefinis
vector<pair<int, int>> getPatternByName(const string& name, int offsetR, int offsetC) {
    vector<pair<int, int>> coords;
    
    if (name == "glider")       coords = {{0,1}, {1,2}, {2,0}, {2,1}, {2,2}};
    else if (name == "blinker") coords = {{0,1}, {1,1}, {2,1}};
    else if (name == "toad")    coords = {{1,1}, {1,2}, {1,3}, {2,0}, {2,1}, {2,2}};
    else if (name == "beacon")  coords = {{0,0}, {0,1}, {1,0}, {1,1}, {2,2}, {2,3}, {3,2}, {3,3}};
    
    for(auto& p : coords) { 
        p.first += offsetR; 
        p.second += offsetC; 
    }
    return coords;
}

//MAIN
int main() {
    cout << "=== JEU DE LA VIE ===" << endl;

    //1 Choix de l'interface (Texte ou Graphique)
    char interfaceChoice;
    cout << "Choisissez l'interface :\n";
    cout << "  (C)onsole\n";
    cout << "  (G)raphique (SFML)\n";
    cout << "> ";
    
    if (!(cin >> interfaceChoice)) {
        clearInput();
        interfaceChoice = 'c'; // Default to console on error
    }
    
    bool useGui = (interfaceChoice == 'g' || interfaceChoice == 'G');

    //Dimensions adaptees a la vue choisie
    int rows = useGui ? SFMLView::ROWS : 30;
    int cols = useGui ? SFMLView::COLS : 60;

    //2 Configuration du monde (Torique ou non)
    char toricChoice;
    cout << "\nMonde Torique (bords connectes) ? (y/n) : ";
    if (!(cin >> toricChoice)) {
        clearInput();
        toricChoice = 'n';
    }
    bool isToric = (toricChoice == 'y' || toricChoice == 'Y');

    //3 Creation du moteur de jeu
    // Utilisation de la regle de Conway par defaut
    auto rule = make_unique<ConwayRule>();
    // Creation du jeu
    Game* game = new Game(rows, cols, move(rule), isToric);

    // Pattern de depart
    InitData startData;
    int offsetR = 5, offsetC = 5;

    cout << "\nChoisissez le motif de depart :\n";
    cout << "  1. Fichier (ex: grille.txt)\n";
    cout << "  2. Aleatoire\n";
    cout << "  3. Motifs predefinis\n";
    cout << "  4. Vide\n";
    cout << "> ";
    
    int patternChoice;
    //4 Initialisation du contenu
    if (!(cin >> patternChoice)) {
        clearInput();
        patternChoice = 4;
    }

    if (patternChoice == 1) {
        string fname;
        cout << "Nom du fichier : ";
        cin >> fname;
        startData = loadPatternFromFile(fname, offsetR, offsetC);
    } 
    else if (patternChoice == 2) {
        double density;
        cout << "Densite (0.1 - 1.0) : ";
        if (!(cin >> density)) density = 0.2;
        startData.aliveCoords = generateRandomCoordinates(rows, cols, density);
    } 
    else if (patternChoice == 3) {
        cout << "Quel motif ? (glider, blinker, toad, beacon) : ";
        string patName;
        cin >> patName;
        // Center the pattern
        startData.aliveCoords = getPatternByName(patName, rows/2, cols/2);
    }

    //4 initialisation de jeu
    //Applique les coordonnees lues
    if (!startData.aliveCoords.empty()) {
        game->initGrille(startData.aliveCoords);
    }
    if (!startData.obstacleCoords.empty()) {
        game->initObstacles(startData.obstacleCoords);
    }

    //5 Lancement de la vue choisie
    unique_ptr<IView> view = nullptr;

    if (useGui) {
        cout << "\nLancement SFML..." << endl;
        view = make_unique<SFMLView>();
    } else {
        cout << "\nLancement Console..." << endl;
        this_thread::sleep_for(chrono::milliseconds(500));
        view = make_unique<ConsoleView>();
    }

    if (view) {
        // appelle IView::lancer(Game*&) et le vue choisi fait le reste
        view->lancer(game);
    }

    cout << "\nSimulation terminee." << endl;
    // Nettoyage final
    delete game;
    return 0;
}