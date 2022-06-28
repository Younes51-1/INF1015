#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>
#include "cppitertools/range.hpp"
#include "gsl/span"
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

// Fonctions pour ajouter un Film à une ListeFilms.
//[
void ListeFilms::changeDimension(int nouvelleCapacite)
{
	Film** nouvelleListe = new Film * [nouvelleCapacite];

	if(elements != nullptr)
	{  // Noter que ce test n'est pas nécessaire puique nElements_ sera zéro si elements_ est nul, donc la boucle ne tentera pas de faire de copie, et on a le droit de faire delete sur un pointeur nul (ça ne fait rien).
		nElements = min(nouvelleCapacite, nElements);
		for(int i : range(nElements))
			nouvelleListe[i] = elements[i];
		delete[] elements;
	}

	elements = nouvelleListe;
	capacite = nouvelleCapacite;
}

void ListeFilms::ajouterFilm(Film* film)
{
	if(nElements == capacite)
		changeDimension(max(1, capacite * 2));
	elements[nElements++] = film;
}
//]

// Fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
//[
// On a juste fait une version const qui retourne un span non const.  C'est valide puisque c'est la struct qui est const et non ce qu'elle pointe.  Ça ne va peut-être pas bien dans l'idée qu'on ne devrait pas pouvoir modifier une liste const, mais il y aurais alors plusieurs fonctions à écrire en version const et non-const pour que ça fonctionne bien, et ce n'est pas le but du TD (il n'a pas encore vraiment de manière propre en C++ de définir les deux d'un coup).
span<Film*> ListeFilms::enSpan() const
{
	return span(elements, nElements);
}

// Fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.

//NOTE: Doit retourner un Acteur modifiable, sinon on ne peut pas l'utiliser pour modifier l'acteur tel que demandé dans le main, et on ne veut pas faire écrire deux versions.
shared_ptr<Acteur> ListeFilms::trouverActeur(const string& nomActeur) const
{
	for(const Film* film : enSpan())
	{
		for(const shared_ptr<Acteur>& acteur : film->acteurs.enSpan())
		{
			if(acteur->nom == nomActeur)
				return acteur;
		}
	}
	return nullptr;
}
//]

// Les fonctions pour lire le fichier et créer/allouer une ListeFilms.

shared_ptr<Acteur> lireActeur(istream& fichier, const ListeFilms& listeFilms)
{
	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);

	shared_ptr<Acteur> acteurExistant = listeFilms.trouverActeur(acteur.nom);
	if(acteurExistant != nullptr)
		return acteurExistant;
	else
	{
		cout << "Création Acteur " << acteur.nom << endl;
		return make_shared<Acteur>(move(acteur));  // Le move n'est pas nécessaire mais permet de transférer le texte du nom sans le copier.
	}
}

Film* lireFilm(istream& fichier, ListeFilms& listeFilms)
{
	Film* film = new Film;
	film->titre = lireString(fichier);
	film->realisateur = lireString(fichier);
	film->annee = lireUint16(fichier);
	film->recette = lireUint16(fichier);
	auto nActeurs = lireUint8(fichier);
	film->acteurs = ListeActeurs(nActeurs);  // On n'a pas fait de méthode pour changer la taille d'allocation, seulement un constructeur qui prend la capacité.  Pour que cette affectation fonctionne, il faut s'assurer qu'on a un operator= de move pour ListeActeurs.
	cout << "Création Film " << film->titre << endl;

	for([[maybe_unused]] auto i : range(nActeurs))
	{  // On peut aussi mettre nElements_ avant et faire un span, comme on le faisait au TD précédent.
		film->acteurs.ajouter(lireActeur(fichier, listeFilms));
	}

	return film;
}

ListeFilms creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);

	ListeFilms listeFilms;
	for([[maybe_unused]] int i : range(nElements))
	{ //NOTE: On ne peut pas faire un span simple avec ListeFilms::enSpan car la liste est vide et on ajoute des éléments à mesure.
		listeFilms.ajouterFilm(lireFilm(fichier, listeFilms));
	}

	return listeFilms;
}

// Fonction pour détruire une ListeFilms et tous les films qu'elle contient.
//[
//NOTE: La bonne manière serait que la liste sache si elle possède, plutôt qu'on le dise au moment de la destruction, et que ceci soit le destructeur.  Mais ça aurait complexifié le TD2 de demander une solution de ce genre, d'où le fait qu'on a dit de le mettre dans une méthode.
void ListeFilms::detruire(bool possedeLesFilms)
{
	delete[] elements;
}
//]

// Pour que l'affichage de Film fonctionne avec <<, il faut aussi modifier l'affichage de l'acteur pour avoir un ostream; l'énoncé ne demande pas que ce soit un opérateur, mais tant qu'à y être...
ostream& operator<< (ostream& os, const Acteur& acteur)
{
	return os << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

// Fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).
//[
ostream& operator<< (ostream& os, const Film& film)
{
	return film.output(os);
}

ostream& Film::output(ostream& os) const
{
	os << "Titre: " << titre << endl;
	os << "  Réalisateur: " << realisateur << "  Année : " << annee << endl;
	os << "  Recette: " << recette << " M$" << endl;

	os << "Acteurs:" << endl;
	for(const shared_ptr<Acteur>& acteur : acteurs.enSpan())
		os << *acteur;
	return os;
}
//]

// Fonction pour afficher un livre.
//[
ostream& operator<< (ostream& os, const Livre& livre)
{
	return livre.output(os);
}

ostream& Livre::output(ostream& os) const
{
	os << "Titre: " << titre << endl;
	os << "  Auteur: " << auteur << "  Année : " << annee << endl;
	os << "  Millions de copies vendues: " << millionsCopiesVendues << " M copies" << endl;
	os << "  nombre de pages: " << nombrePages << " pages" << endl;
	return os;
}
//]

// Fonction pour afficher un FilmLivre.
//[
ostream& FilmLivre::output(ostream& os) const
{
	os << "Titre: " << Item::titre << endl;
	os << "  Réalisateur: " << realisateur << "  Année : " << Affichable::annee << endl;
	os << "  Recette: " << recette << " M$" << endl;

	os << "  Acteurs:" << endl;
	for(const shared_ptr<Acteur>& acteur : acteurs.enSpan())
		os << *acteur;
	os << endl << "Base sur le livre de l'auteur: " << auteur << endl;
	os << "  Millions de copies vendues: " << millionsCopiesVendues << " M copies" << endl;
	os << "  nombre de pages: " << nombrePages << " pages" << endl;
	return os;
}

ostream& operator<< (ostream& os, const FilmLivre& filmLivre)
{
	return filmLivre.output(os);
}
//]

// Afficher un item.
//[
ostream& Item::output(ostream& os) const
{
	os << "Titre: " << titre << "  Année : " << annee << endl;

	return os;
}

ostream& operator<< (ostream& os, const Item& item)
{
	return item.output(os);
}
//]

// Afficher la bibliotheque.
ostream& operator<< (ostream& os, const	vector<shared_ptr<Item>>& biblio)
{
	static const string ligneDeSeparation = //[
		"\033[32m────────────────────────────────────────\033[0m\n";
	os << ligneDeSeparation;
	for(int i : range(biblio.size()))
	{
		os << *biblio[i] << ligneDeSeparation;
	}
	return os;
}

// Fonction pour ajouter un Item dans la bibliotheque.
void ajouterElement(vector<shared_ptr<Item>>& biblio, shared_ptr<Item> item)
{
	if(biblio.size() == biblio.capacity())
		biblio.reserve(max(1, static_cast<int>(biblio.capacity() * 2)));
	biblio.push_back(item);
}

// Fonction pour ajouter la listeFilms dans la bibliotheque.
void ajouterListeFilms(vector<shared_ptr<Item>>& biblio, const ListeFilms& listeFilms)
{
	for(Film* film : listeFilms.enSpan())
		ajouterElement(biblio, shared_ptr<Film>(film));
}

// Fonction pour ajouter Les livres dans la bibliotheque.
void ajouterListeLivre(vector<shared_ptr<Item>>& biblio, string fichiername)
{
	fstream fichier;
	fichier.open(fichiername);

	while(!fichier.eof())
	{
		Livre* livre = new Livre;

		fichier >> quoted(livre->titre) >>
			livre->annee >> quoted(livre->auteur) >>
			livre->millionsCopiesVendues >> livre->nombrePages;

		ajouterElement(biblio, shared_ptr<Livre>(livre));
	}
	fichier.close();
}

// Function pour ajouter un FilmLivre dans la bibliotheque.
void ajouterFilmLivre(vector<shared_ptr<Item>>& biblio, const Film& film, const Livre& livre)
{
	FilmLivre* filmLivre = new FilmLivre;

	filmLivre->Film::titre = film.titre;
	filmLivre->Film::annee = film.annee;
	filmLivre->Film::realisateur = film.realisateur;
	filmLivre->Film::recette = film.recette;
	filmLivre->Film::acteurs = ListeActeurs(3);
	for(shared_ptr<Acteur>& acteur : film.acteurs.enSpan())
		filmLivre->Film::acteurs.ajouter(acteur);

	filmLivre->Livre::auteur = livre.auteur;
	filmLivre->Livre::nombrePages = livre.nombrePages;
	filmLivre->Livre::millionsCopiesVendues = livre.millionsCopiesVendues;

	ajouterElement(biblio, shared_ptr<Livre>(filmLivre));
}

// Fonction pour trouver un item dans la bibliotheque selon le critere.
Item* trouver(const vector<shared_ptr<Item>>& biblio, const auto& critere)
{
	for(auto& item : biblio)
		if(critere(item))
			return item.get();
	return nullptr;
}

int main()
{
#ifdef VERIFICATION_ALLOCATION_INCLUS
	bibliotheque_cours::VerifierFuitesAllocations verifierFuitesAllocations;
#endif
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	ListeFilms listeFilms = creerListe("films.bin");

	vector<shared_ptr<Item>> biblio;
	ajouterListeFilms(biblio, listeFilms);
	ajouterListeLivre(biblio, "livres.txt");

	Film* filmrechercher = dynamic_cast<Film*>(trouver(biblio, [](auto film) { return film->titre == "Le Hobbit : La Bataille des Cinq Armées"; }));
	Livre* livrerechercher = dynamic_cast<Livre*>(trouver(biblio, [](auto livre) { return livre->titre == "The Hobbit"; }));

	ajouterFilmLivre(biblio, *filmrechercher, *livrerechercher);
	cout << biblio;

	listeFilms.detruire();
}
