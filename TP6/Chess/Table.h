#pragma once

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

		bool& EchecBlanc()
		{
			return echecBlanc_;
		}
		bool& EchecNoir()
		{
			return echecNoir_;
		}
		bool& blancAJouer()
		{
			return blancAJouer_;
		}
		bool matBlanc()
		{
			return matBlanc_;
		}
		bool matNoir()
		{
			return matNoir_;
		}
		bool patBlanc()
		{
			return patBlanc_;
		}
		bool patNoir()
		{
			return patNoir_;
		}

		std::pair<int, int> getUpdatePosition()
		{
			return updatePosition_;
		}
		void setUpdatePosition(std::pair<int, int> position)
		{
			updatePosition_ = position;
		}
		bool& getUpdate()
		{
			return update_;
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
