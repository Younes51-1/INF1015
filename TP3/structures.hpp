#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <iostream>
#include <cassert>
#include <memory>
#include "gsl/span"
#include <iostream>
using namespace std;
using gsl::span;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.


// Class ListeFilms
class ListeFilms {
public:
	ListeFilms() = default;
	ListeFilms(const std::string& nomFichier);
	ListeFilms(const ListeFilms& l) { assert(l.elements == nullptr); } // Pas demandé dans l'énoncé, mais on veut s'assurer qu'on ne fait jamais de copie de liste, car la copie par défaut ne fait pas ce qu'on veut.  Donc on ne permet pas de copier une liste non vide (la copie de liste vide est utilisée dans la création d'un acteur).
	~ListeFilms();

	void ajouterFilm(Film* film);
	void enleverFilm(const Film* film);
	shared_ptr<Acteur> trouverActeur(const std::string& nomActeur) const;
	span<Film*> enSpan() const;
	int size() const {return nElements;}
	Film* operator [](int index);
	Film* chercherFilm(const auto& critere);

private:
	void changeDimension(int nouvelleCapacite);

	int capacite = 0, nElements = 0;
	Film** elements = nullptr; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
	bool possedeLesFilms_ = false; // Les films seront détruits avec la liste si elle les possède.
};


// Class template
template <class T>
class Liste {
public:
    Liste() = default;
	~Liste() = default;

	void modifierCapacite(int newCapacite)
	{
		capacite_ = newCapacite;
	}
	void modifierNElements(int newNElements)
	{
		nElements_ = newNElements;
	}
	void modifierElements(unique_ptr<shared_ptr<T>[]> newElements)
	{
		elements_ = move(newElements);
	}

	int accesCapacite() const
	{
		return capacite_ ;
	}
	int accesNElemets() const
	{
		return nElements_;
	}
	auto accesElements() const
	{
		return elements_.get();
	}

	span<shared_ptr<T>> spanListeActeurs() const;
	Liste<T>(const Liste<T>& autre)
	{
		*this = autre;
	}
	Liste<T>& operator=(const Liste<T>& autre)
	{
		if(this != &autre)
		{
			capacite_ = autre.capacite_;
			nElements_ = autre.nElements_;
			elements_ = make_unique<shared_ptr<T>[]>(nElements_);
			for(int i = 0; i < nElements_; i++)
			{
				elements_[i] = autre.elements_[i];
			}
		}
		return *this;
	}
	shared_ptr<T> operator [](int index);

	
private:
	int capacite_ = 0, nElements_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_; 
};
using ListeActeurs = Liste<Acteur>; // On cree une class ListeActeurs a partir de Liste en passant Acteur comme type


struct Film
{
	std::string titre = "", realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie = 0, recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
	Film() = default;
	Film(const Film& autre)
	{
		titre = autre.titre;
		realisateur = autre.realisateur;
		anneeSortie = autre.anneeSortie;
		recette = autre.recette;
		acteurs = autre.acteurs;
	}
};


struct Acteur
{
	std::string nom =""; int anneeNaissance = 0; char sexe = ' ';
};