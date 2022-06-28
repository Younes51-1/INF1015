#include <iostream>
#include <utility>

#include "Jeux.h"

namespace modele
{
	using namespace std;
	Jeux::Jeux(std::string game)
	{
		try
		{
			createRoi(3, 4, true);
			createRoi(0, 4, false);
			createRoi(5, 4, false);
		}
		catch(PlusDe2Roi& e)
		{
			cout << e.what() << std::endl;
		}

		createCavalier(0, 6, false);
		createTour(1, 7, true);

		PieceTemporaire p = PieceTemporaire(*this, table_, "Tour", 0, 0, true);
	}

	void Jeux::createRoi(int x, int y, bool couleur)
	{
		table_.getPiece(x, y) = make_unique<Roi>(x, y, couleur);
		table_.getCoordonneesRois()[couleur] = {x, y};
	}
	void Jeux::createCavalier(int x, int y, bool couleur)
	{
		table_.getPiece(x, y) = make_unique<Cavalier>(x, y, couleur);
	}
	void Jeux::createTour(int x, int y, bool couleur)
	{
		table_.getPiece(x, y) = make_unique<Tour>(x, y, couleur);
	}

	void Jeux::createPiece(std::string type, int x, int y, bool couleur)
	{
		if(type == "Roi")
			createRoi(x, y, couleur);
		else if(type == "Cavalier")
			createCavalier(x, y, couleur);
		else if(type == "Tour")
			createTour(x, y, couleur);
	}

	void Jeux::tourEnCours()
	{
		mouvementEnCours();
		if(tourFini_)
		{
			table_.blancAJouer() = !table_.blancAJouer();
			emit sendReponse(tourFini_, QString("Deplacement fait!"),
							 carreOriginal_, carreFinal_);

			if(table_.getUpdate())
			{
				table_.getUpdate() = false;
				emit updateImage(table_.getUpdatePosition());
			}
			tourFini_ = false;
		}
	}

	void Jeux::mouvementEnCours()
	{
		try
		{
			table_.getPiece(carreOriginal_)->bouger(table_, carreFinal_);
			getTable().verificationEchec();
			getTable().verificationMat();
			tourFini() = true;
		}
		catch(DeplacementInvalidException& e)
		{
			emit sendReponse(tourFini_,
							 QString::fromStdString(e.what()));
		}
	}

	void Jeux::recevoirCarreClique(pair<int, int> caseCliquee)
	{
		if(!carreOriginalDejaClique_)
		{
			if(table_.getPiece(caseCliquee) != nullptr)
			{
				if(table_.getPiece(caseCliquee)->estBlanc() != table_.blancAJouer())
					emit sendReponse(tourFini_,
									 QString::fromStdString("Switching team I see!"));

				else
				{
					carreOriginal_ = caseCliquee;
					carreOriginalDejaClique_ = true;
					table_.getPiece(caseCliquee)->trouverDeplacementsValides(table_);
					emit casesValides(table_.getPiece(caseCliquee)->getDeplacementsValides());
					emit sendReponse(tourFini_,
									 QString::fromStdString("Good!"));
				}
			}
			else
				emit sendReponse(tourFini_,
								 QString::fromStdString("Empty like my brain!"));
		}
		else
		{
			if(carreOriginal_ == caseCliquee)
			{
				carreOriginalDejaClique_ = false;
				emit sendReponse(tourFini_,
								 QString::fromStdString("Wait a minute!"));
				emit repeindre();
			}
			else
			{
				carreFinal_ = caseCliquee;
				carreOriginalDejaClique_ = false;

				tourEnCours();
				emit repeindre();

				if(table_.matBlanc() || table_.matNoir())
				{
					if(table_.matBlanc())
						emit mat(true);
					else if(table_.matNoir())
						emit mat(false);
				}
				else if(table_.patBlanc() || table_.patNoir())
				{
					if(table_.patBlanc())
						emit pat(true);
					else if(table_.patNoir())
						emit pat(false);
				}
			}
		}
	}
	void Jeux::recevoirCarreDeclique()
	{
		carreOriginalDejaClique_ = false;
		emit repeindre();
	}

	PieceTemporaire::PieceTemporaire(Jeux& jeu, Table& table, std::string type, int x, int y, bool estBlanc)
	{
		table_ = &table;
		x_ = x;
		y_ = y;
		jeu.createPiece(type, x_, y_, estBlanc);
		cout << "Creation de la Piece Temp \n";
	}

	PieceTemporaire::~PieceTemporaire()
	{
		table_->getPiece(x_, y_) = nullptr;
		cout << "Destruction de la Piece Temp \n";
	}
}