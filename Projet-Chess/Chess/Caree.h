#pragma once

// Cree des cases pour les connecte a l echiquier
// chaque case s occupe de mettre l image de la piece associe
// et capte les evenements sur cette souris
// Pour plus de detail consulter la description de chanque methode dans Carre.cpp.

#include <memory>
#include <string>
#include <cmath>

#include <QDebug>
#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QBrush>
#include <QImage>

#include "Table.h"

namespace vue
{
	class Carre : public QObject, public QGraphicsRectItem
	{
		Q_OBJECT;

	public:
		Carre(int xPos, int yPos, int x, int y);
		~Carre() = default;

		int& getX()
		{
			return x_;
		}
		int& getY()
		{
			return y_;
		}
		int& getXPos_()
		{
			return xPos_;
		}
		int& getYPos_()
		{
			return yPos_;
		}
		QString getCheminImage() const
		{
			return cheminImage_;;
		}
		QGraphicsPixmapItem& getPixmap()
		{
			return pixmap_;
		};

		void setPixmap(QString chemin);
		void effacerPixmap();
		void mousePressEvent(QGraphicsSceneMouseEvent* event);

	private:
		int x_, y_;
		int xPos_, yPos_;
		QString cheminImage_;
		QGraphicsPixmapItem pixmap_;
		QImage image_;
		bool dejaClique = false;

	signals:
		void caseCliquee(std::pair<int, int>);
		void caseDecliquee(std::pair<int, int>);
		void repeindre();
	};
}