#pragma once

// Classe Table, c'est l'échiquier où on place nos pièces
// elle a 2 méthodes importantes qui sont verificationEchec() et verificationMat()
// Pour plus de details, consultez la description de chanque méthode dans Table.cpp.

#include <memory>
#include <utility>
#include <array>
#include <map>
#include <QObject>

#include "Piece.h"

namespace modele
{
	class Table : public QObject
	{
	public:
		Table() = default;
		~Table() = default;

		std::unique_ptr<Piece>& getPiece(int x, int y)
		{
			return pieces_[x][y];
		}
		std::unique_ptr<Piece>& getPiece(std::pair<int, int> position)
		{
			return pieces_[position.first][position.second];
		}
		std::map<bool, std::pair<int, int>>& getCoordonneesRois()
		{
			return coordonneesRois_;
		}
		std::unique_ptr<Piece>& getPieceTampon()
		{
			return pieceTampon_;
		}
		std::unique_ptr<Piece>& getPieceTamponRoi()
		{
			return pieceTamponRoi_;
		}

		bool getEchecBlanc()
		{
			return echecBlanc_;
		}
		bool getEchecNoir()
		{
			return echecNoir_;
		}
		bool getBlancAJouer()
		{
			return blancAJouer_;
		}
		bool getMatBlanc()
		{
			return matBlanc_;
		}
		bool getMatNoir()
		{
			return matNoir_;
		}
		bool getPatBlanc()
		{
			return patBlanc_;
		}
		bool getPatNoir()
		{
			return patNoir_;
		}
		bool getUpdate()
		{
			return update_;
		}

		std::pair<int, int> getUpdatePosition()
		{
			return updatePosition_;
		}
		void setUpdatePosition(std::pair<int, int> position)
		{
			updatePosition_ = position;
		}
		void setUpdate(bool u)
		{
			update_ = u;
		}
		void setEchecBlanc(bool b)
		{
			echecBlanc_ = b;
		}
		void setEchecNoir(bool n)
		{
			echecNoir_ = n;
		}
		void setBlancAJouer(bool j)
		{
			blancAJouer_ = j;
		}
		void verificationEchec();
		void verificationMat();

	private:
		std::unique_ptr<Piece> pieces_[8][8];
		std::unique_ptr<Piece> pieceTampon_;
		std::unique_ptr<Piece> pieceTamponRoi_;

		std::map<bool, std::pair<int, int>> coordonneesRois_;
		std::pair<int, int> updatePosition_;

		bool update_;

		bool echecBlanc_ = false;
		bool echecNoir_ = false;
		bool matBlanc_ = false;
		bool matNoir_ = false;
		bool patBlanc_ = false;
		bool patNoir_ = false;
		bool blancAJouer_ = true;
	};
}