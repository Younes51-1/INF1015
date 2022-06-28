#include <iostream>

#include "Table.h"

namespace modele
{
	// Vérifie s'il y a echec
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

	// Vérifie s'il y a mat ou pat
	// en verifiant s'il y a des déplacement valides pour le Roi
	// si non, il vérifie si le roi en échec.
	//	  si c'est le cas il vérifie s'il y a un mat ou pas
	//			en vérfifiant s'il y a des déplacement valides.
	//	  si on a pas d'echec il vérifie s'il y a pat ou pas
	//			en vérifiant s'il y a des déplacement valides.
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
						   && getPiece(i, j)->getEstBlanc() == getPiece(getCoordonneesRois()[true])->getEstBlanc())
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
						   && getPiece(i, j)->getEstBlanc() == getPiece(getCoordonneesRois()[true])->getEstBlanc())
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
						   && getPiece(i, j)->getEstBlanc() == getPiece(getCoordonneesRois()[false])->getEstBlanc())
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
						   && getPiece(i, j)->getEstBlanc() == getPiece(getCoordonneesRois()[false])->getEstBlanc())
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