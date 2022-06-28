#pragma once

// la classe Afficher initialise l'interface,
// capte les évènements crées par l utilisateur
// et met à jour l'interface s'il y a un changement.
// Pour plus de détails, consultez la description de chanque méthode dans Afficher.cpp.

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <array>
#include <thread>

#include "Jeux.h"
#include "Caree.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Afficher;
}
QT_END_NAMESPACE

namespace vue
{
	class Afficher : public QMainWindow
	{
		Q_OBJECT

	public:
		Afficher(QWidget* parent = nullptr);
		~Afficher();

		void resetList();
		void createGame(int game);
		void initialiser();
		void setupPieces();
		void initialiserCarres();
		void initialiserList();
		void updatePiece();
		void updateTour();

	private:
		Ui::Afficher* ui;
		std::unique_ptr<vue::Jeux> jeux_;
		QGraphicsScene* vue_;
		Carre* carres_[8][8];
		QPushButton* newGameButton_;
		QListWidget* selectGameList_;
		QListWidgetItem* gameSelected_;

		QColor couleurGris = QColor::fromRgb(208, 208, 208);
		QColor couleurBlanche = QColor::fromRgb(255, 255, 255);

	private slots:
		void getReponseJeux(bool tourFait, QString reponse,
							std::pair<int, int> positionOriginale,
							std::pair<int, int> positionFinale);

		void getUpdate(std::pair<int, int> position);
		void paint();
		void newGameButton();
		void currentGame();
	};
}