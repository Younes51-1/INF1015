#pragma once

#include <QObject>

#include "Table.h"
namespace modele
{
	class Jeux : public QObject
	{
		Q_OBJECT
		public:
		//Jeux() = default;
		Jeux(std::string game = "default");

		~Jeux() = default;

		Table& getTable()
		{
			return table_;
		}
		bool& tourFini()
		{
			return tourFini_;
		}
		bool& partieFini()
		{
			return fin_;
		}
		void tourEnCours();
		void mouvementEnCours();

		void createRoi(int x, int y, bool couleur);
		void createCavalier(int x, int y, bool couleur);
		void createTour(int x, int y, bool couleur); 
		void createPiece(std::string type, int x, int y, bool couleur);

		private:

		Table table_;
		bool tourFini_ = false;
		bool fin_ = false;

		bool carreOriginalDejaClique_ = false;
		std::pair<int, int> carreOriginal_;
		std::pair<int, int> carreFinal_;

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

namespace modele
{
	class PieceTemporaire
	{
		public:
		PieceTemporaire(Jeux& jeu, Table& table, std::string type, int x, int y, bool estBlanc);
		~PieceTemporaire();

		private:
		Table* table_;
		int x_, y_;
	};
}