#include <iostream>
#include <utility>

#include "Jeux.h"

namespace vue
{
	using namespace std;
	Jeux::Jeux(int game)
	{
		try
		{
			switch(game)
			{
				case Positions::La_Bourdonnais_vs_McDonnell:
					createCavalier(0, 6, false);
					createTour(1, 7, true);
					createRoi(2, 4, true);
					createRoi(0, 4, false);
					break;

				case Positions::J_Polgar_vs_Kasparov:
					createCavalier(4, 1, false);
					createTour(4, 7, true);
					createTour(6, 0, false);
					createRoi(7, 3, true);
					createRoi(5, 2, false);
					break;

				case Positions::Karpov_vs_Ftáčnik:
					createCavalier(1, 5, false);
					createTour(5, 6, true);
					createRoi(4, 5, true);
					createRoi(7, 2, false);
					break;

				case Positions::Random:
					shuffle(ranPositionsX.begin(), ranPositionsX.end(), default_random_engine(rand()));
					shuffle(ranPositionsY.begin(), ranPositionsY.end(), default_random_engine(rand()));

					createCavalier(ranPositionsX[0], ranPositionsY[0], true);
					createCavalier(ranPositionsX[1], ranPositionsY[1], false);
					createTour(ranPositionsX[2], ranPositionsY[2], true);
					createTour(ranPositionsX[3], ranPositionsY[3], false);
					createRoi(ranPositionsX[4], ranPositionsY[4], true);
					createRoi(ranPositionsX[5], ranPositionsY[5], false);
					break;

				case Positions::Random_avec_3_rois:
					shuffle(ranPositionsX.begin(), ranPositionsX.end(), default_random_engine(rand()));
					shuffle(ranPositionsY.begin(), ranPositionsY.end(), default_random_engine(rand()));

					createCavalier(ranPositionsX[0], ranPositionsY[0], true);
					createCavalier(ranPositionsX[1], ranPositionsY[1], false);
					createTour(ranPositionsX[2], ranPositionsY[2], true);
					createTour(ranPositionsX[3], ranPositionsY[3], false);
					createRoi(ranPositionsX[4], ranPositionsY[4], true);
					createRoi(ranPositionsX[6], ranPositionsY[6], false);
					createRoi(ranPositionsX[5], ranPositionsY[5], true);
					break;

				case Positions::Pat:
					createTour(6, 0, true);
					createTour(0, 5, true);
					createRoi(0, 0, true);
					createRoi(7, 6, false);
					break;

				case Positions::Pat_Inverse:
					createTour(6, 0, false);
					createTour(0, 5, false);
					createRoi(0, 0, false);
					createRoi(7, 6, true);
					break;
			}
		}
		catch(PlusDe2Roi& msg)
		{
			cout << msg.what() << std::endl;
		}
	}

	// Crée le roi
	void Jeux::createRoi(int x, int y, bool couleur)
	{
		table_.getPiece(x, y) = make_unique<modele::Roi>(x, y, couleur);
		table_.getCoordonneesRois()[couleur] = {x, y};
	}

	// Crée le cavalier
	void Jeux::createCavalier(int x, int y, bool couleur)
	{
		table_.getPiece(x, y) = make_unique<modele::Cavalier>(x, y, couleur);
	}

	// Crée la tour
	void Jeux::createTour(int x, int y, bool couleur)
	{
		table_.getPiece(x, y) = make_unique<modele::Tour>(x, y, couleur);
	}

	// gère l'avancement d'un tour de jeu
	void Jeux::tourEnCours()
	{
		mouvementEnCours();

		if(tourFini_)
		{
			table_.setBlancAJouer(!table_.getBlancAJouer());
			emit sendReponse(tourFini_, QString("Deplacement fait!"),
							 carreOriginal_, carreFinal_);

			if(table_.getUpdate())
			{
				table_.setUpdate(false);
				emit updateImage(table_.getUpdatePosition());
			}
			tourFini_ = false;
		}
	}

	// Effectue le déplacement des pièces
	// ou retourne une exception si le déplacement est illégal
	void Jeux::mouvementEnCours()
	{
		try
		{
			table_.getPiece(carreOriginal_)->bouger(table_, carreFinal_);
			getTable().verificationEchec();
			getTable().verificationMat();
			settourFini(true);
		}
		catch(DeplacementInvalidException& msg)
		{
			emit sendReponse(tourFini_,
							 QString::fromStdString(msg.what()));
		}
	}

	// Traitement des clics éffectués par le joueur
	// Si les clics sont bons ou non
	// Par exemple cliquer sur une pièce ennemie
	void Jeux::recevoirCarreClique(pair<int, int> caseCliquee)
	{
		if(!carreOriginalDejaClique_)
		{
			if(table_.getPiece(caseCliquee) != nullptr)
			{
				if(table_.getPiece(caseCliquee)->getEstBlanc() != table_.getBlancAJouer())
					emit sendReponse(tourFini_,
									 QString::fromStdString("Changer d'équipe, je vois!"));

				else
				{
					carreOriginal_ = caseCliquee;
					carreOriginalDejaClique_ = true;
					table_.getPiece(caseCliquee)->trouverDeplacementsValides(table_);
					emit casesValides(table_.getPiece(caseCliquee)->getDeplacementsValides());
					emit sendReponse(tourFini_,
									 QString::fromStdString("Bien!"));
				}
			}
			else
				emit sendReponse(tourFini_,
								 QString::fromStdString("Vide!"));
		}
		else
		{
			if(carreOriginal_ == caseCliquee)
			{
				carreOriginalDejaClique_ = false;
				emit sendReponse(tourFini_,
								 QString::fromStdString("sélectionner une autre case!"));
				emit repeindre();
			}
			else
			{
				carreFinal_ = caseCliquee;
				carreOriginalDejaClique_ = false;

				tourEnCours();
				emit repeindre();

				if(table_.getMatBlanc() || table_.getMatNoir())
				{
					if(table_.getMatBlanc())
						emit mat(true);
					else if(table_.getMatNoir())
						emit mat(false);
				}
				else if(table_.getPatBlanc() || table_.getPatNoir())
				{
					if(table_.getPatBlanc())
						emit pat(true);
					else if(table_.getPatNoir())
						emit pat(false);
				}
			}
		}
	}

	// Traitement des déclics éffectués par le joueur
	void Jeux::recevoirCarreDeclique()
	{
		carreOriginalDejaClique_ = false;
		emit repeindre();

	}
}