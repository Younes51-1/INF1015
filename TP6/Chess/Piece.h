#pragma once
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>

#include "Deplacement.h"

class DeplacementInvalidException : public std::exception
{
	public:
	using std::exception::exception;
};

class PlusDe2Roi : public std::exception
{
	public:
	using std::exception::exception;
};

namespace modele
{
	class Piece : public Deplacement
	{
		public:
		Piece() = default;
		Piece(int x, int y);
		virtual ~Piece() = default;
		int& getX()
		{
			return x_;
		}
		int& getY()
		{
			return y_;
		}
		std::pair<int, int> getCoordonnees() const
		{
			return {x_, y_};
		};
		std::string& getNom()
		{
			return nom_;
		}
		bool estBlanc() const
		{
			return estBlanc_;
		}

		std::vector<std::pair<int, int>> getDeplacementsPossibles() const
		{
			return deplacementsPossible_;
		}
		std::vector<std::pair<int, int>> getDeplacementsValides() const
		{
			return deplacementsValides_;
		}
		bool peutBouger(Table& table, std::pair<int, int> arrivee);
		void bouger(Table& table, std::pair<int, int> arrivee);
		void updateImage(Table& table, std::pair<int, int> arrivee);
		virtual void trouverDeplacementsValides(Table& table);

		protected:
		int x_ = 0;
		int y_ = 0;
		bool estBlanc_ = false;
		std::string nom_;
		std::vector<std::pair<int, int>> deplacementsPossible_;
		std::vector<std::pair<int, int>> deplacementsValides_;
	};
}
namespace modele
{
	class Roi : public Piece
	{
		public:
		Roi(int x, int y, bool couleur) : Piece::Piece(x, y)
		{
			if(nbRoi_ < 2)
			{
				estBlanc_ = couleur;
				nom_ = "Roi";
				nbRoi_++;
			}
			else
				throw PlusDe2Roi("3 kings ???????!");
		}
		~Roi()
		{
			nbRoi_--;
		}
		bool estEnEchec(Table& table);
		virtual void trouverDeplacementsPossibles(Table& table) override;
		virtual void trouverDeplacementsValides(Table& table) override;

		private:
		static inline int nbRoi_ = 0;
	};
}
namespace modele
{
	class Cavalier : public Piece
	{
		public:
		Cavalier(int x, int y, bool couleur) : Piece::Piece(x, y)
		{
			estBlanc_ = couleur;
			nom_ = "Cavalier";
		}
		virtual void trouverDeplacementsPossibles(Table& table) override;
	};
}
namespace modele
{
	class Tour : public Piece
	{
		public:
		Tour() = default;
		Tour(int x, int y, bool couleur) : Piece::Piece(x, y)
		{
			estBlanc_ = couleur;
			nom_ = "Tour";
		}
		virtual void trouverDeplacementsPossibles(Table& table) override;
	};
}
