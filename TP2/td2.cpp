#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

#include "cppitertools/range.hpp"
#include "gsl/span"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*) &valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*) &valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*) &texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}


// Fonction pour acceder aux films de ListeFilms
Film** ListeFilms::accesElemnts() const
{
	return elements_;
};


// Fonction span pour ListeFilms
span<Film*> ListeFilms::spanListeFilms()
{
	return span(elements_, nElements_);
}


// Fonction span pour ListeActeurs
span<Acteur*> spanListeActeurs(const ListeActeurs& listeActeurs)
{
	return span(listeActeurs.elements, listeActeurs.nElements);
}


//Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; 
//on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit 
//doubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la 
//capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau 
//tableau plus grand, copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  
//Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ListeFilms::ajouterFilm(Film* film)
{
	if(capacite_ < nElements_ + 1)
	{
		if(capacite_ == 0)
			capacite_ = 1;
		Film** nouvelleListeFilms = new Film * [capacite_ * 2];
		elements_ = nouvelleListeFilms;
		capacite_ *= 2;
	}
	elements_[nElements_++] = film;
}


//Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film;
//la fonction prenant en paramètre un pointeur vers le film à enlever. L'ordre des films dans la liste n'a pas conservé.
void ListeFilms::effacerFilm(const Film* film)
{
	for(Film*& filmListe : spanListeFilms())
	{
		if(filmListe->titre == film->titre)
		{
			if(nElements_ > 1)
			{
				filmListe = elements_[nElements_ - 1];
			}
			nElements_--;
		}
	}
}

//Une fonction pour trouver un Acteur par son nom dans une ListeFilms,
//qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.
Acteur* ListeFilms::trouverActeur(string nomActeur)
{
	for(Film* film : spanListeFilms())
	{
		for(Acteur* acteur : spanListeActeurs((*film).acteurs))
		{
			if((*acteur).nom == nomActeur) return acteur;
		}
	}
	return nullptr;
}


//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms. La ListeFilms devra être passée entre les fonctions,
//pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).
Acteur* ListeFilms::lireActeur(istream& fichier)
{
	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);
	if(trouverActeur(acteur.nom) != nullptr)
	{
		return trouverActeur(acteur.nom);
	}
	else
	{
		cout << "Création de " << acteur.nom << endl; //Pour des raisons de débogage
		return new Acteur(acteur);
	}
}


//TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations,
//selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées;
//vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
Film* ListeFilms::lireFilm(istream& fichier)
{
	Film film = {};
	film.titre = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = lireUint16(fichier);
	film.recette = lireUint16(fichier);
	film.acteurs.nElements = lireUint8(fichier);
	film.acteurs.capacite = film.acteurs.nElements;
	film.acteurs.elements = new Acteur * [film.acteurs.nElements];
	Film* film_ptr = new Film(film);
	cout << "Création de " << film_ptr->titre << endl; //Pour des raisons de débogage
	for(int i = 0; i < film.acteurs.nElements; i++)
	{
		film_ptr->acteurs.elements[i] = lireActeur(fichier); //Placer l'acteur au bon endroit dans les acteurs du film.
		film_ptr->acteurs.elements[i]->joueDans.ajouterFilm(film_ptr);
	}
	return film_ptr; //Retourner le pointeur vers le nouveau film.
}


//Constructeur de la classe ListeFilms
ListeFilms::ListeFilms(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	//fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);
	//Créer une liste de films vide.
	elements_ = new Film * [nElements];
	capacite_ = nElements;

	for(int i = 0; i < nElements; i++)
	{
		ajouterFilm(lireFilm(fichier)); //Ajouter le film à la liste.
	}
}


//Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film,
// et les acteurs qui ne jouent plus dans aucun films de la collection).
//On enleve egalement le film détruit des films dans lesquels jouent les acteurs. 
//Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.
void ListeFilms::detruireFilm(Film* film)
{
	for(Acteur* acteur : spanListeActeurs(film->acteurs))
	{
		acteur->joueDans.effacerFilm(film);
		if(acteur->joueDans.nElements_ == 0)
		{
			cout << "Déstruction de " << acteur->nom << endl; //Pour des raisons de débogage
			acteur->joueDans.~ListeFilms();
			delete[] acteur->joueDans.elements_;
			acteur->joueDans.elements_ = nullptr;
			delete acteur;
			acteur = nullptr;
			film->acteurs.nElements--;
		}
	}
	cout << "Déstruction de " << film->titre << endl; //Pour des raisons de débogage
	effacerFilm(film);
	delete[] film->acteurs.elements;
	film->acteurs.elements = nullptr;
	delete film;
	film = nullptr;
}


//Déstructeur de ListeFilms et tous les films qu'elle contient.
ListeFilms::~ListeFilms()
{
	for(Film* film : spanListeFilms())
	{
		detruireFilm(film);
	}
	delete[] elements_;
	elements_ = nullptr;
}


//Fonction pour afficher l'acteur.
void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}


//Une fonction pour afficher un film avec tous ces acteurs.
void afficherFilm(Film film)
{
	cout << "Titre: " << film.titre << endl;
	cout << "Realisateur: " << film.realisateur << endl;
	cout << "Annee de sortie: " << film.anneeSortie << endl;
	cout << "Recette en M$: " << film.recette << endl;
	cout << "Acteurs: " << endl;
	for(Acteur* acteur : spanListeActeurs(film.acteurs))
	{
		afficherActeur(*acteur);
	}
}


//Fonction qui affiche les films dans ListeFilms.
void ListeFilms::afficherListeFilms()
{
	static const string ligneDeSeparation = "\n\033[40m═════════════════════════════════\033[0m\n";
	for(Film* film : spanListeFilms())
	{
		//Affiche le film.
		afficherFilm(*film);
		cout << ligneDeSeparation;
	}
}


//Fonction qui affiche la Filmographie d'acteur.
void ListeFilms::afficherFilmographieActeur(const string& nomActeur)
{
	Acteur* acteur = trouverActeur(nomActeur);
	if(acteur == nullptr)
	{
		cout << "Aucun acteur de ce nom" << endl;
	}
	else
	{
		acteur->joueDans.afficherListeFilms();
	}
}


int main()
{
	// Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code; 
	//les consoles Linux / Mac les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi();
	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.
	//Afficher les noms de 20 acteurs sans doublons pour raisons de débogage.
	ListeFilms listeFilms("films.bin");

	cout << ligneDeSeparation << "Le premier film de la liste est: ";
	//Affiche le premier film de la liste.
	afficherFilm(*listeFilms.accesElemnts()[0]);

	cout << ligneDeSeparation << "Les films sont:" << endl;
	//Afficher la liste des films.
	listeFilms.afficherListeFilms();

	//Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).
	listeFilms.trouverActeur("Benedict Cumberbatch")->anneeNaissance = 1976;

	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl << endl;
	//Afficher la liste des films où Benedict Cumberbatch joue.
	listeFilms.afficherFilmographieActeur("Benedict Cumberbatch");

	//Détruire et enlever le premier film de la liste (Alien).
	//Également on détruit les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	listeFilms.detruireFilm(listeFilms.accesElemnts()[0]);

	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//Affiche la liste des films.
	listeFilms.afficherListeFilms();
	//Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme
	//(aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).
	cout << "Ces lignes sont pour des raisons de coverture de code  et débogage vous pouvez les ignorer" <<endl;
	listeFilms.afficherFilmographieActeur("Younes Benabbou");

	//Déstruction de tout avant de terminer le programme.
}
