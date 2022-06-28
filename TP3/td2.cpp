#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>
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
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

// Methode pour réallocation du tableau en doublant sa capacité s’il
// ne reste pas de place
void ListeFilms::changeDimension(int nouvelleCapacite)
{
	Film** nouvelleListe = new Film*[nouvelleCapacite];
	
	if (elements != nullptr) { 
		for (int i : range(nElements))
			nouvelleListe[i] = elements[i];
		delete[] elements;
	}
	
	elements = nouvelleListe;
	capacite = nouvelleCapacite;
}


// Methode pour ajouter un Film dans ListeFilms
void ListeFilms::ajouterFilm(Film* film)
{
	if (nElements == capacite)
		changeDimension(max(1, capacite * 2));
	elements[nElements++] = film;
}


// Span pour ListeFilms
span<Film*> ListeFilms::enSpan() const { return span(elements, nElements); }


// Methode pour enlever un Film (PAS LE DETRUIRE)
void ListeFilms::enleverFilm(const Film* film)
{
	for (Film*& element : enSpan()) {
		if (element == film) {
			if (nElements > 1)
				element = elements[nElements - 1];
			nElements--;
			return;
		}
	}
}


// Span pour ListeActeurs
template <class T>
span<shared_ptr<T>> Liste<T>::spanListeActeurs() const { return span(elements_.get(), nElements_); }


// Methode pour trouver un acteur
shared_ptr<Acteur> ListeFilms::trouverActeur(const string& nomActeur) const
{
	for (const Film* film : enSpan())
		for (shared_ptr<Acteur> acteur : film->acteurs.spanListeActeurs())
			if (acteur->nom == nomActeur)
				return acteur;
	return nullptr;
}


// Fonction pour creer un Acteur et son pointeur
shared_ptr<Acteur> lireActeur(istream& fichier, ListeFilms& listeFilms)
{
	Acteur acteur = {};
	acteur.nom            = lireString(fichier);
	acteur.anneeNaissance = lireUint16 (fichier);
	acteur.sexe           = lireUint8  (fichier);

	shared_ptr<Acteur> acteurExistant = listeFilms.trouverActeur(acteur.nom);
	if (acteurExistant != nullptr)
		return acteurExistant;
	else 
	{
		cout << "Création Acteur " << acteur.nom << endl;
		return make_shared<Acteur>(acteur);
	}
}


// Fonction pour creer un Film et son pointeur
Film* lireFilm(istream& fichier, ListeFilms& listeFilms)
{
	Film film = {};
	Film* filmp = new Film;
	filmp->titre       = lireString(fichier);
	filmp->realisateur = lireString(fichier);
	filmp->anneeSortie = lireUint16 (fichier);
	filmp->recette     = lireUint16 (fichier);
	filmp->acteurs.modifierNElements(lireUint8 (fichier));
	filmp->acteurs.modifierCapacite(filmp->acteurs.accesNElemets());

	cout << "Création Film " << film.titre << endl;
	filmp->acteurs.modifierElements(make_unique<shared_ptr<Acteur>[]>(filmp->acteurs.accesNElemets()));
	
	for (shared_ptr<Acteur>& acteur : filmp->acteurs.spanListeActeurs())
		acteur =  lireActeur(fichier, listeFilms);
	return filmp;
}


// Constructeur de ListeFilms
ListeFilms::ListeFilms(const string& nomFichier) : possedeLesFilms_(true)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	
	int nElements = lireUint16(fichier);

	for ([[maybe_unused]] int i : range(nElements))
		ajouterFilm(lireFilm(fichier, *this));
}


// Fonction pour detruire un Film
void detruireFilm(Film* film)
{
	cout << "Destruction Film " << film->titre << endl;
	delete film;
}


// Destructeur de ListeFilms
ListeFilms::~ListeFilms()
{
	if (possedeLesFilms_)
		for (Film* film : enSpan())
			detruireFilm(film);
	delete[] elements;
}


// Methode pour chercher le film qui repond a une critere quelconque 
Film* ListeFilms::chercherFilm(const auto& critere)
{
	for(auto&& film : enSpan())
		if(critere(film))
			return film;
	return nullptr;
}


// Surcharge d'operateur [] pour listeFilms
Film* ListeFilms::operator [](int index)
{
	return enSpan()[index];
}


// Surcharge d'operateur [] pour Liste
template <class T>
shared_ptr<T> Liste<T>::operator[](int index)
{
	return spanListeActeurs()[index];
}


// Surcharge d'operateur cout pour Acteur
ostream& operator << (ostream& os, const Acteur& acteur)
{
	os << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
	return os;
}


// Surcharge d'operateur cout pour listeActeurs
ostream& operator << (ostream& os, const ListeActeurs& acteurs)
{
	for(const shared_ptr<Acteur> acteur : acteurs.spanListeActeurs())
		os << *acteur;
	return os;
}


// Surcharge d'operateur cout pour Film
ostream& operator << (ostream& os, const Film& film)
{
	os  << "Titre: "         << film.titre       << endl
		<< "Réalisateur: " << film.realisateur << "  Année: " << film.anneeSortie << endl
		<< "Recette: "     << film.recette     << "M$"        << endl
		<< "Acteurs:"        << endl
		<< film.acteurs;
	return os;
}


// Surcharge d'operateur cout pour listeFilms
ostream& operator << (ostream& os, const ListeFilms& listeFilms)
{
	static const string ligneDeSeparation = "\033[32m────────────────────────────────────────\033[0m\n";
	for(const Film* film : listeFilms.enSpan())
	{
		os << ligneDeSeparation;
		os << *film;
	}
	return os;
}


int main()
{
	#ifdef VERIFICATION_ALLOCATION_INCLUS
	bibliotheque_cours::VerifierFuitesAllocations verifierFuitesAllocations;
	#endif
	bibliotheque_cours::activerCouleursAnsi();

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	
	// Creation de Liste des Films
	ListeFilms listeFilms("films.bin");
	
	//Affichage du premier film de la liste
	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	cout <<*listeFilms[0];

	// Affichage des Films 
	cout << ligneDeSeparation << "Les films sont:" << endl;
	cout <<listeFilms;
	cout << ligneDeSeparation;
	
	// Modification de la date de naissance de "Benedict Cumberbatch"
	listeFilms.trouverActeur("Benedict Cumberbatch")->anneeNaissance = 1976;

	// Creation de Film "Skylien"
	Film skylien = *listeFilms[0];
	skylien.titre = "Skylien";
	skylien.acteurs.accesElements()[0] = listeFilms[1]->acteurs.accesElements()[0];
	skylien.acteurs[0]->nom = "Daniel Wroughton Craig";

	// Affichage des Films 
	cout <<skylien;
	cout << ligneDeSeparation;
	cout << *listeFilms[0];
	cout << ligneDeSeparation;
	cout << *listeFilms[1];
	cout << ligneDeSeparation;

	// Chercher le film qui verfier le criteres puis l'afficher
	Film* filmTemp = listeFilms.chercherFilm([](auto film) {return film->recette == 955;});
	if (filmTemp != nullptr)
		cout << *filmTemp;
	else
		cout << "Pas de Film trouve qui correspond au criteres specifie" << endl;
	cout << ligneDeSeparation;

	// Destruction du Film "Alien"
	detruireFilm(listeFilms.enSpan()[0]);
	listeFilms.enleverFilm(listeFilms[0]);

	//Affichage des Films
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	cout << listeFilms;
	cout << ligneDeSeparation;

	cout << "Chapitre 9: "<< endl;
	// Creation de Liste de type String
	string textes[2] = {"C++: An octopus made by nailing extra legs onto a dog.",
						"When your hammer is C++, everything begins to look like a thumb."};
	Liste<string> listeTextes;
	listeTextes.modifierCapacite(2);
	listeTextes.modifierNElements(2);
	listeTextes.modifierElements(make_unique<shared_ptr<string>[]>(listeTextes.accesNElemets()));
	

	for(int i = 0; i< listeTextes.accesNElemets(); i++)
		listeTextes.accesElements()[i] = make_shared<string>(textes[i]);

	// Modification des textes
	Liste<string> listeTextes2 = listeTextes;
	listeTextes.accesElements()[0] = make_shared<string>("Why do Java programmers have to wear glasses? Because they don’t C#.");
	*(listeTextes.accesElements()[1]) = "Two bytes meet. The first byte asks, ‘Are you ill?’ The second byte replies, ‘No, just feeling a bit off.’";
	// Affichage des 4 textes 
	cout << *listeTextes[0] << endl;
	cout << *listeTextes[1] << endl;
	cout << *listeTextes2[0] << endl;
	cout << *listeTextes2[1] << endl;
	cout << ligneDeSeparation;
}
