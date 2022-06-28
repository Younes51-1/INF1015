#include "Caree.h"

namespace vue
{
	Carre::Carre(int xPos, int yPos, int x, int y)
	{
		xPos_ = xPos;
		yPos_ = yPos;

		x_ = x;
		y_ = y;
	}

	// Crée l'image de la pièce
	void Carre::setPixmap(QString chemin)
	{
		cheminImage_ = chemin;
		image_ = QImage(chemin);

		pixmap_.setPixmap(chemin);
		pixmap_.setPos(xPos_, yPos_);
		pixmap_.show();
	}

	// Efface l'image d'une piece si elle est capturée
	// ou la supprime lors d'un reset
	void Carre::effacerPixmap()
	{
		pixmap_.hide();
		cheminImage_ = "";
	}

	// Capture les clics éffectués par l'utilisateur sur l'échiquier
	void Carre::mousePressEvent(QGraphicsSceneMouseEvent* event)
	{
		if(event->buttons() == Qt::LeftButton)
		{
			if(!dejaClique)
			{
				dejaClique = true;
				emit caseCliquee({x_, y_});
			}
			else
				emit caseCliquee({x_, y_});
		}
		else if(event->buttons() == Qt::RightButton)
		{
			dejaClique = false;
			emit caseDecliquee({x_, y_});
		}
	}
}