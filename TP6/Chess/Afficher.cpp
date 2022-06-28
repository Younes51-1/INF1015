#include "Afficher.h"
#include "ui_Afficher.h"
namespace interface
{
	using namespace std;

	Afficher::Afficher(QWidget* parent)
		: QMainWindow(parent)
		, ui(new Ui::Afficher)
	{
		ui->setupUi(this);

		vue_ = new QGraphicsScene(this);
		ui->graphicsView->setScene(vue_);

		jeux_ = std::make_unique<modele::Jeux>();

		initialiser();
		setupPieces();

		QObject::connect(jeux_.get(), &modele::Jeux::sendReponse,
						 this, &Afficher::getReponseJeux);

		QObject::connect(jeux_.get(), &modele::Jeux::repeindre,
						 this, &Afficher::paint);

		QObject::connect(jeux_.get(), &modele::Jeux::updateImage,
						 this, &Afficher::getUpdate);
	}

	Afficher::~Afficher()
	{
		delete ui;
	}

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

				modele::Carre* carre = new modele::Carre(x, abs(y - 350), i, j);
				carre->setRect(x, abs(y - 350), 50, 50);

				if(carreNoir)
					carre->setBrush(QBrush(couleurGris));
				else
					carre->setBrush(QBrush(couleurBlanche));

				carres_[i][j] = carre;
				vue_->addItem(carre);
				vue_->addItem(&carre->getPixmap());

				QObject::connect(carre, &modele::Carre::caseCliquee,
								 jeux_.get(), &modele::Jeux::recevoirCarreClique);

				QObject::connect(carre, &modele::Carre::caseDecliquee,
								 jeux_.get(), &modele::Jeux::recevoirCarreDeclique);

				y += 50;
				if(y == 400)
				{
					x += 50;
					y = 0;
				}
			}
		}
		updateTour();
		updatePiece();

		ui->ValiditeDuMove->setText("");
	}

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
					if(jeux_->getTable().getPiece(i, j)->estBlanc())
						couleur = "Blanc";
					else if(!jeux_->getTable().getPiece(i, j)->estBlanc())
						couleur = "Noir";

					nom = QString::fromStdString(jeux_->getTable().getPiece(i, j)->getNom());

					carres_[i][j]->setPixmap(
						QString(":/images/") + nom + couleur + QString(".png"));
				}
			}
		}
	}

	void Afficher::updatePiece()
	{
		if(jeux_->getTable().EchecBlanc() || jeux_->getTable().EchecNoir())
		{
			if(jeux_->getTable().matBlanc())
				ui->Statuts->setText(QString("Echec et mat des Blancs"));
			else if(jeux_->getTable().matNoir())
				ui->Statuts->setText(QString("Echec et mat des Noirs"));
			else if(jeux_->getTable().EchecBlanc())
				ui->Statuts->setText(QString("Echec des Blancs"));
			else if(jeux_->getTable().EchecNoir())
				ui->Statuts->setText(QString("Echec des Noirs"));
		}
		else if(jeux_->getTable().patBlanc())
			ui->Statuts->setText(QString("Pat des Blancs"));
		else if(jeux_->getTable().patNoir())
			ui->Statuts->setText(QString("Pat des Noirs"));
		else
			ui->Statuts->setText(QString("Pas d'echec"));
	}

	void Afficher::updateTour()
	{
		if(jeux_->getTable().blancAJouer())
			ui->TourJoueur->setText(QString("Au tour des Blancs"));
		else
			ui->TourJoueur->setText(QString("Au tour des Noirs"));
	}

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

				if(jeux_->getTable().EchecBlanc() || jeux_->getTable().EchecBlanc())
				{
					if(jeux_->getTable().getPiece(i, j) != nullptr
					   && jeux_->getTable().getPiece(i, j)->getNom() == "Roi")
						continue;
					else
					{
						if(carreNoir)
							carres_[i][j]->setBrush(QBrush(couleurGris));
						else
							carres_[i][j]->setBrush(QBrush(couleurBlanche));
					}
				}
				else
				{
					if(carreNoir)
						carres_[i][j]->setBrush(QBrush(couleurGris));
					else
						carres_[i][j]->setBrush(QBrush(couleurBlanche));
				}
			}
		}
	}

	void Afficher::getUpdate(pair<int, int> position)
	{
		QString couleur;
		QString nom;
		carres_[position.first][position.second]->effacerPixmap();
		if(jeux_->getTable().getPiece(position)->estBlanc())
			couleur = "Blanc";
		else if(!jeux_->getTable().getPiece(position)->estBlanc())
			couleur = "Noir";

		nom = QString::fromStdString(
			jeux_->getTable().getPiece(position)->getNom());

		carres_[position.first][position.second]->setPixmap(
			QString(":/images/") + nom + couleur + QString(".png"));
	}
}