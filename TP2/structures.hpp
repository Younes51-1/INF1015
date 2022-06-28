#pragma once
// Structures mémoires pour une collection de films.

#include <iostream>
#include <string>
#include "gsl/span"

using namespace gsl;
using namespace std;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.


//ListeFilms en classe.
class ListeFilms
{
	public:
	ListeFilms() = default;
	ListeFilms(string nomFichier);
	~ListeFilms();

	Film** accesElemnts() const;

	span<Film*> spanListeFilms();
	void ajouterFilm(Film* film);
	Acteur* trouverActeur(string nomActeur);
	Acteur* lireActeur(istream& fichier);
	Film* lireFilm(istream& fichier);

	void afficherListeFilms();
	void afficherFilmographieActeur(const string& nomActeur);

	void effacerFilm(const Film* film);
	void detruireFilm(Film* film);
	

	private:
	int capacite_ = 0;
	int nElements_ = 0;
	Film** elements_;
	
};

struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};

struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};