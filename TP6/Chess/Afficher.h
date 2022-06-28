#pragma once

#include <QMainWindow>
#include <QGraphicsScene>
#include <array>

#include "Jeux.h"
#include "Caree.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Afficher;
}
QT_END_NAMESPACE
namespace interface
{
	class Afficher : public QMainWindow
	{
		Q_OBJECT

		public:
		Afficher(QWidget* parent = nullptr);
		~Afficher();

		void initialiser();
		void setupPieces();
		void updatePiece();
		void updateTour();

		private:
		Ui::Afficher* ui;
		std::unique_ptr<modele::Jeux> jeux_;
		QGraphicsScene* vue_;
		modele::Carre* carres_[8][8];

		QColor couleurGris = QColor::fromRgb(208, 208, 208);
		QColor couleurBlanche = QColor::fromRgb(255, 255, 255);

		private slots:
		void getReponseJeux(bool tourFait, QString reponse,
							std::pair<int, int> positionOriginale,
							std::pair<int, int> positionFinale);

		void getUpdate(std::pair<int, int> position);
		void paint();
	};
}