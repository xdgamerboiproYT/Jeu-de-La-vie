#pragma once
class Game; 
//Interface pour toutes les vues (Console ou Graphique)
class IView {
public:
    virtual ~IView() = default;
    //Demarre la boucle principale de l'affichage
    //Game*& permet à la vue de reinitialiser (reset) le jeu si besoin
    virtual void lancer(Game*& game) = 0;
    virtual void dessiner(const Game* game) = 0;
    virtual void arreter() = 0;
};