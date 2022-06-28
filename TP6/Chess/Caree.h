#pragma once

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

namespace modele
{
	class Carre : public QObject, public QGraphicsRectItem
	{
		Q_OBJECT;
		public:
		Carre() = default;
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
		void setPixmap(QString chemin);
		QString getCheminImage() const
		{
			return cheminImage_;;
		}
		QGraphicsPixmapItem& getPixmap()
		{
			return pixmap_;
		};
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
