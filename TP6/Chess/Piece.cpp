#include "Piece.h"
#include "Table.h"

namespace modele
{
	using namespace std;
	Piece::Piece(int x, int y)
	{
		x_ = x;
		y_ = y;
	}

	bool Piece::peutBouger(Table& table, pair<int, int> arrivee)
	{
		trouverDeplacementsValides(table);
		for(auto&& deplacement : deplacementsValides_)
		{
			if(arrivee == deplacement)
				return true;
		}
		return false;
	}
	void Piece::bouger(Table& table, pair<int, int> arrivee)
	{
		if(peutBouger(table, arrivee))
		{
			table.getPiece(arrivee) = move(table.getPiece(x_, y_));
			x_ = arrivee.first;
			y_ = arrivee.second;
			if(nom_ == "Roi")
			{
				table.getCoordonneesRois()[estBlanc_] = arrivee;
				return;
			}
		}
		else
			throw DeplacementInvalidException("What are you doing!");
	}

	void Piece::updateImage(Table& table, pair<int, int> arrivee)
	{
		if(estBlanc_ && y_ == 7)
		{
			table.setUpdatePosition(arrivee);
			table.getUpdate() = true;
		}
		else if(!estBlanc_ && y_ == 0)
		{
			table.setUpdatePosition(arrivee);
			table.getUpdate() = true;
		}
	}
	void Piece::trouverDeplacementsValides(Table& table)
	{
		auto [xRoi, yRoi] = table.getCoordonneesRois()[estBlanc_];
		deplacementsValides_.clear();
		trouverDeplacementsPossibles(table);
		auto deplacementsSemiValides = deplacementsPossible_;

		for(auto&& deplacementSemiValide : deplacementsSemiValides)
		{
			table.getPieceTampon() = move(table.getPiece(deplacementSemiValide));
			auto coordonneesAvant = getCoordonnees();
			table.getPiece(deplacementSemiValide) = move(table.getPiece(x_, y_));
			x_ = deplacementSemiValide.first;
			y_ = deplacementSemiValide.second;

			if(!dynamic_cast<Roi*>(table.getPiece(xRoi, yRoi).get())->estEnEchec(table))
				deplacementsValides_.push_back(deplacementSemiValide);

			table.getPiece(coordonneesAvant) = move(table.getPiece(x_, y_));
			table.getPiece(x_, y_) = move(table.getPieceTampon());
			x_ = coordonneesAvant.first;
			y_ = coordonneesAvant.second;
		}
	}

	bool Roi::estEnEchec(Table& table)
	{
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(table.getPiece(i, j) != nullptr
				   && table.getPiece(i, j) != table.getPiece(x_, y_)
				   && table.getPiece(i, j)->estBlanc() != estBlanc_)
				{
					if(dynamic_cast<Roi*>(table.getPiece(i, j).get()))
						if((abs(i - x_) == 1 && abs(j - y_) == 0)	   //deplacement horizontal
						   || (abs(i - x_) == 0 && abs(j - y_) == 1)  //deplacement vertical
						   || (abs(i - x_) == 1 && abs(j - y_) == 1)) //deplacemetn diagonal
							return true;
						else
							continue;
					else
					{
						table.getPiece(i, j)->trouverDeplacementsPossibles(table);
						if(table.getPiece(i, j)->getDeplacementsPossibles().size() > 0)
						{
							for(auto&& deplacement : table.getPiece(i, j)->getDeplacementsPossibles())
							{
								if(deplacement == this->getCoordonnees())
									return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	void Roi::trouverDeplacementsPossibles(Table& table)
	{
		deplacementsPossible_.clear();
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if((abs(i - x_) == 1 && abs(j - y_) == 0)
				   || (abs(i - x_) == 0 && abs(j - y_) == 1)
				   || (abs(i - x_) == 1 && abs(j - y_) == 1))
				{
					if(table.getPiece(i, j) == nullptr)
						deplacementsPossible_.push_back({i, j});
					else if(estBlanc_ != table.getPiece(i, j)->estBlanc())
						deplacementsPossible_.push_back({i, j});
				}
			}
		}
	}

	void Roi::trouverDeplacementsValides(Table& table)
	{
		deplacementsValides_.clear();
		trouverDeplacementsPossibles(table);
		auto deplacementsSemiValides = deplacementsPossible_;

		for(auto&& deplacementSemiValide : deplacementsSemiValides)
		{
			table.getPieceTamponRoi() = move(table.getPiece(deplacementSemiValide));
			auto coordonneesAvant = getCoordonnees();
			table.getPiece(deplacementSemiValide) = move(table.getPiece(x_, y_));
			x_ = deplacementSemiValide.first;
			y_ = deplacementSemiValide.second;
			table.getCoordonneesRois()[estBlanc_] = getCoordonnees();

			if(!estEnEchec(table))
				deplacementsValides_.push_back(deplacementSemiValide);

			table.getPiece(coordonneesAvant) = move(table.getPiece(deplacementSemiValide));
			table.getPiece(deplacementSemiValide) = move(table.getPieceTamponRoi());
			x_ = coordonneesAvant.first;
			y_ = coordonneesAvant.second;
			table.getCoordonneesRois()[estBlanc_] = getCoordonnees();
		}
	}

	void Tour::trouverDeplacementsPossibles(Table& table)
	{
		deplacementsPossible_.clear();
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				int xVariation = 0;
				int yVariation = 0;
				if(i - x_ != 0 || j - y_ != 0)
				{
					if(x_ != i)
					{
						if(i - x_ < 0)
							xVariation = -1;
						else
							xVariation = 1;
						int k = 1;
						for(; k <= abs(i - x_);)
						{
							if(table.getPiece(x_ + xVariation * k, y_ + yVariation * k) != nullptr)
							{
								k++;
								break;
							}
							k++;
						}
						if(x_ + xVariation * (k - 1) == i && y_ + yVariation * (k - 1) == j)
						{
							if(table.getPiece(i, j) == nullptr)
								deplacementsPossible_.push_back({i, j});
							else if(estBlanc_ != table.getPiece(i, j)->estBlanc())
								deplacementsPossible_.push_back({i, j});
						}
					}
					else if(y_ != j)
					{
						if(j - y_ < 0)
							yVariation = -1;
						else
							yVariation = 1;
						int k = 1;
						for(; k <= abs(j - y_);)
						{
							if(table.getPiece(x_ + xVariation * k, y_ + yVariation * k) != nullptr)
							{
								k++;
								break;
							}
							k++;
						}
						if(x_ + xVariation * (k - 1) == i && y_ + yVariation * (k - 1) == j)
						{
							if(table.getPiece(i, j) == nullptr)
								deplacementsPossible_.push_back({i, j});
							else if(estBlanc_ != table.getPiece(i, j)->estBlanc())
								deplacementsPossible_.push_back({i, j});
						}
					}
				}
			}
		}
	}

	void Cavalier::trouverDeplacementsPossibles(Table& table)
	{
		deplacementsPossible_.clear();
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(i != x_ && j != y_ &&
				   ((abs(x_ - i) == 2 && abs(y_ - j) == 1)	   //deplacement 2 cases horizontale et 1 verticale
				   || (abs(x_ - i) == 1 && abs(y_ - j) == 2))) //deplacement 1 case horizontale et 2 verticales
				{
					if(table.getPiece(i, j) == nullptr)
						deplacementsPossible_.push_back({i, j});
					else if(estBlanc_ != table.getPiece(i, j)->estBlanc())
						deplacementsPossible_.push_back({i, j});
				}
			}
		}
	}
}