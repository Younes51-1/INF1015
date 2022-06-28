#include "Afficher.h"
#include "ui_Afficher.h"

namespace vue
{
	using namespace std;

	Afficher::Afficher(QWidget* parent)
		: QMainWindow(parent)
		, ui(new Ui::Afficher)
	{
		ui->setupUi(this);

		vue_ = new QGraphicsScene(this);
		ui->graphicsView->setScene(vue_);

		newGameButton_ = ui->NewGame;
		selectGameList_ = ui->GamesList;

		selectGameList_->setVisible(true);

		initialiserList();
		initialiser();

		QObject::connect(newGameButton_, &QPushButton::clicked,
						 this, &Afficher::newGameButton);
		QObject::connect(selectGameList_, &QListWidget::doubleClicked,
						 this, &Afficher::currentGame);
	}

	Afficher::~Afficher()
	{
		delete ui;
	}

	// Initialise l'échiquier
	void Afficher::initialiser()
	{
		int x = 0, y = 0;
		bool carreNoir;
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(i % 2 == 0 && j % 2 == 0)
					carreNoir = true;
				else if(i % 2 != 0 && j % 2 == 0)
					carreNoir = false;
				else if(i % 2 != 0 && j % 2 != 0)
					carreNoir = true;
				else
					carreNoir = false;

				Carre* carre = new Carre(x, abs(y - 350), i, j);
				carre->setRect(x, abs(y - 350), 50, 50);

				if(carreNoir)
					carre->setBrush(QBrush(couleurGris));
				else
					carre->setBrush(QBrush(couleurBlanche));

				carres_[i][j] = carre;
				vue_->addItem(carre);
				vue_->addItem(&carre->getPixmap());

				y += 50;
				if(y == 400)
					x += 50, y = 0;
			}
		}
		ui->TourJoueur->setText("Prêt à jouer?");
		ui->ValiditeDuMove->setText("Double cliquez sur une position dans la liste");
		ui->Statuts->setText("pour démarrer une nouvelle partie cliquez  sur 'nouvelle partie'");
	}

	// Place les pièces selon la position choisie
	// et verifie s'il y a échec au départ (utile pour le Random)
	void Afficher::createGame(int game)
	{
		resetList();
		selectGameList_->setVisible(false);
		jeux_ = std::make_unique<vue::Jeux>(game);

		ui->TourJoueur->setText("");
		ui->ValiditeDuMove->setText("");
		ui->Statuts->setText("");

		jeux_->getTable().verificationEchec();
		jeux_->getTable().verificationMat();

		initialiserCarres();
		updateTour();
		updatePiece();
		setupPieces();

		QObject::connect(jeux_.get(), &vue::Jeux::sendReponse,
						 this, &Afficher::getReponseJeux);

		QObject::connect(jeux_.get(), &vue::Jeux::repeindre,
						 this, &Afficher::paint);

		QObject::connect(jeux_.get(), &vue::Jeux::updateImage,
						 this, &Afficher::getUpdate);
	}

	// Connecte les cases avec l'échiquier
	void Afficher::initialiserCarres()
	{
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				QObject::connect(carres_[i][j], &Carre::caseCliquee,
								 jeux_.get(), &vue::Jeux::recevoirCarreClique);

				QObject::connect(carres_[i][j], &Carre::caseDecliquee,
								 jeux_.get(), &vue::Jeux::recevoirCarreDeclique);
			}
		}
	}

	// Initialse la liste des fins de partie
	void Afficher::initialiserList()
	{
		selectGameList_->setSelectionBehavior(QAbstractItemView::SelectItems);
		selectGameList_->setSelectionMode(QAbstractItemView::SingleSelection);

		selectGameList_->addItem("La Bourdonnais vs. McDonnell, 1834");
		selectGameList_->addItem("J. Polgar vs. Kasparov, 1996");
		selectGameList_->addItem("Karpov vs. Ftáčnik, 1988");
		selectGameList_->addItem("Position aléatoire");
		selectGameList_->addItem("Position aléatoire avec 3 rois");
		selectGameList_->addItem("Pat");
		selectGameList_->addItem("Pat inversé");
	}

	// Place les pièces dans l'échiquier
	void Afficher::setupPieces()
	{
		QString couleur;
		QString nom;
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(jeux_->getTable().getPiece(i, j) != nullptr)
				{
					if(jeux_->getTable().getPiece(i, j)->getEstBlanc())
						couleur = "Blanc";
					else if(!jeux_->getTable().getPiece(i, j)->getEstBlanc())
						couleur = "Noir";

					nom = QString::fromStdString(jeux_->getTable().getPiece(i, j)->getNom());

					carres_[i][j]->setPixmap(
						QString(":/images/") + nom + couleur + QString(".png"));
				}
			}
		}
	}

	// Met à jour le statut du jeux
	void Afficher::updatePiece()
	{
		if(jeux_->getTable().getEchecBlanc() || jeux_->getTable().getEchecNoir())
		{
			if(jeux_->getTable().getMatBlanc())
				ui->Statuts->setText(QString("Échec et mat des Blancs"));
			else if(jeux_->getTable().getMatNoir())
				ui->Statuts->setText(QString("Échec et mat des Noirs"));
			else if(jeux_->getTable().getEchecBlanc())
				ui->Statuts->setText(QString("Échec des Blancs"));
			else if(jeux_->getTable().getEchecNoir())
				ui->Statuts->setText(QString("Échec des Noirs"));
		}
		else if(jeux_->getTable().getPatBlanc())
			ui->Statuts->setText(QString("Pat des Blancs"));
		else if(jeux_->getTable().getPatNoir())
			ui->Statuts->setText(QString("Pat des Noirs"));
		else
			ui->Statuts->setText(QString("Pas d'échec"));
	}

	// Met à jour le tour du joueur
	void Afficher::updateTour()
	{
		if(jeux_->getTable().getBlancAJouer())
			ui->TourJoueur->setText(QString("Au tour des Blancs"));
		else
			ui->TourJoueur->setText(QString("Au tour des Noirs"));
	}

	// Met à jour l'échiquier s'il y a un changement
	// (déplacement des pièces)
	void Afficher::getReponseJeux(bool tourFait, QString reponse,
								  pair<int, int> positionOriginale,
								  pair<int, int> positionFinale)
	{
		if(tourFait)
		{
			QString ancienneImage = carres_[positionOriginale.first][positionOriginale.second]->getCheminImage();
			carres_[positionOriginale.first][positionOriginale.second]->effacerPixmap();
			carres_[positionFinale.first][positionFinale.second]->setPixmap(ancienneImage);

			ui->ValiditeDuMove->setText(reponse);

			updatePiece();
			updateTour();
		}
		else
			ui->ValiditeDuMove->setText(reponse);
	}

	// Peint l'échiquier en gris et blanc
	void Afficher::paint()
	{
		bool carreNoir;
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(i % 2 == 0 && j % 2 == 0)
					carreNoir = true;
				else if(i % 2 != 0 && j % 2 == 0)
					carreNoir = false;
				else if(i % 2 != 0 && j % 2 != 0)
					carreNoir = true;
				else
					carreNoir = false;

				if(carreNoir)
					carres_[i][j]->setBrush(QBrush(couleurGris));
				else
					carres_[i][j]->setBrush(QBrush(couleurBlanche));
			}
		}
	}

	// Met à jour la position des images correspondantes aux pièces
	void Afficher::getUpdate(pair<int, int> position)
	{
		QString couleur;
		QString nom;
		carres_[position.first][position.second]->effacerPixmap();

		if(jeux_->getTable().getPiece(position)->getEstBlanc())
			couleur = "Blanc";
		else if(!jeux_->getTable().getPiece(position)->getEstBlanc())
			couleur = "Noir";

		nom = QString::fromStdString(
			jeux_->getTable().getPiece(position)->getNom());

		carres_[position.first][position.second]->setPixmap(
			QString(":/images/") + nom + couleur + QString(".png"));
	}

	// Pour réinitialiser l'interface
	// On supprime les pièces existantes
	// et on rend la liste des positions visible
	void Afficher::newGameButton()
	{
		if(jeux_.get() != nullptr)
		{
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					if(jeux_->getTable().getPiece(i, j) != nullptr)
						jeux_->getTable().getPiece(i, j) = nullptr;
				}
			}
			jeux_.reset();
			initialiser();
		}

		selectGameList_->setVisible(true);
	}

	// Pour sélectionner la fin de partie
	void Afficher::currentGame()
	{
		resetList();
		gameSelected_ = selectGameList_->currentItem();
		gameSelected_->setForeground(Qt::white);
		gameSelected_->setBackground(Qt::blue);

		createGame(selectGameList_->currentRow());
	}

	// Pour réinitialiser la liste
	void Afficher::resetList()
	{
		if(gameSelected_ != nullptr)
		{
			gameSelected_->setForeground(Qt::black);
			gameSelected_->setBackground(Qt::white);
		}
	}
}