#pragma once

// la classe Jeux initialiser les pieces dans leur positions
// selon la fin de partie choisie
// Jeux effectue les changement effectuer par l utilisateur sur l echiquier
// Pour plus de détail consulter la description de chanque methode dans Jeux.cpp.

#include <QObject>
#include <algorithm>
#include <random>

#include "Table.h"

//Fins de partie celebre
//Trouve sur WIKIPEDIA
enum Positions
{
	La_Bourdonnais_vs_McDonnell,
	J_Polgar_vs_Kasparov,
	Karpov_vs_Ftáčnik,
	Random,				// Génère des positions aléatoires chaque fois
	Random_avec_3_rois,	// Pour tester l'exeception du TD6
	Pat,				// Démonstration de Pat
	Pat_Inverse
};

namespace vue
{
	class Jeux : public QObject
	{
		Q_OBJECT

	public:
		Jeux(int game = 0);
		~Jeux() = default;

		modele::Table& getTable()
		{
			return table_;
		}
		bool tourFini()
		{
			return tourFini_;
		}
		bool partieFini()
		{
			return fin_;
		}
		void settourFini(bool tour)
		{
			tourFini_ = tour;
		}
		void tourEnCours();
		void mouvementEnCours();
		void createRoi(int x, int y, bool couleur);
		void createCavalier(int x, int y, bool couleur);
		void createTour(int x, int y, bool couleur);

	private:
		modele::Table table_;
		bool tourFini_ = false;
		bool fin_ = false;
		bool carreOriginalDejaClique_ = false;
		std::pair<int, int> carreOriginal_;
		std::pair<int, int> carreFinal_;
		std::vector<int> ranPositionsX = {0, 1, 2, 3, 4, 5, 6, 7};
		std::vector<int> ranPositionsY = {0, 1, 2, 3, 4, 5, 6, 7};

	signals:
		void sendReponse(bool tourFait, QString reponse,
						 std::pair<int, int> positonOriginale = {0,0},
						 std::pair<int, int> positionFinale = {0,0});

		void casesValides(std::vector<std::pair<int, int>> carreValide);
		void repeindre();
		void updateImage(std::pair<int, int>);
		void mat(bool);
		void pat(bool);

		public slots:
		void recevoirCarreClique(std::pair<int, int> carreClique);
		void recevoirCarreDeclique();
	};
}