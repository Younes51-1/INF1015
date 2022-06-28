#include "Table.h"

#include <iostream>
namespace modele
{
	void Table::verificationEchec()
	{
		if(dynamic_cast<Roi*>(getPiece(getCoordonneesRois()[true]).get())->estEnEchec(*this))
			echecBlanc_ = true;
		else
			echecBlanc_ = false;

		if(dynamic_cast<Roi*>(getPiece(getCoordonneesRois()[false]).get())->estEnEchec(*this))
			echecNoir_ = true;
		else
			echecNoir_ = false;
	}

	void Table::verificationMat()
	{
		getPiece(getCoordonneesRois()[true])->trouverDeplacementsValides(*this);
		if(getPiece(getCoordonneesRois()[true])->getDeplacementsValides().size() == 0)
		{
			if(echecBlanc_)
			{
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 8; j++)
					{
						if(getPiece(i, j) != nullptr
						   && getPiece(i, j)->estBlanc() == getPiece(getCoordonneesRois()[true])->estBlanc())
						{
							getPiece(i, j)->trouverDeplacementsValides(*this);
							if(getPiece(i, j)->getDeplacementsValides().size() != 0)
							{
								matBlanc_ = false;
								return;
							}
						}
					}
				}
				matBlanc_ = true;
			}
			else if(!echecBlanc_)
			{
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 8; j++)
					{
						if(getPiece(i, j) != nullptr
						   && getPiece(i, j)->estBlanc() == getPiece(getCoordonneesRois()[true])->estBlanc())
						{
							getPiece(i, j)->trouverDeplacementsValides(*this);
							if(getPiece(i, j)->getDeplacementsValides().size() != 0)
							{
								patBlanc_ = false;
								return;
							}
						}
					}
				}
				patBlanc_ = true;
			}
		}

		getPiece(getCoordonneesRois()[false])->trouverDeplacementsValides(*this);
		if(getPiece(getCoordonneesRois()[false])->getDeplacementsValides().size() == 0)
		{
			if(echecNoir_)
			{
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 8; j++)
					{
						if(getPiece(i, j) != nullptr
						   && getPiece(i, j)->estBlanc() == getPiece(getCoordonneesRois()[false])->estBlanc())
						{
							getPiece(i, j)->trouverDeplacementsValides(*this);
							if(getPiece(i, j)->getDeplacementsValides().size() != 0)
							{
								matNoir_ = false;
								return;
							}
						}
					}
				}
				matNoir_ = true;
			}
			else if(!echecNoir_)
			{
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 8; j++)
					{
						if(getPiece(i, j) != nullptr
						   && getPiece(i, j)->estBlanc() == getPiece(getCoordonneesRois()[false])->estBlanc())
						{
							getPiece(i, j)->trouverDeplacementsValides(*this);
							if(getPiece(i, j)->getDeplacementsValides().size() != 0)
							{
								patNoir_ = false;
								return;
							}
						}
					}
				}
				patNoir_ = true;
			}
		}
	}
}