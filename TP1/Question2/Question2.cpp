#include <iostream>
#include "Question2.h"

using namespace std;

// On a crée une structure qui contient la durée pour rembourser le pret et aussi la somme des intérêts.
struct DureeInteret 
{
	double dureePret = 0.0;
	double interetTotal = 0.0;
};


// On calcule dans cette fonctions  la durée pour rembourser et la somme des intérêts en utilisant une boucle while.
DureeInteret calculateLoanTerm(double montantPret, double paiementMensuel, double tauxInteret, DureeInteret dureeInteret)
{
	double interetMensuel = tauxInteret / 1200;

	while(montantPret >= 0)
	{
		dureeInteret.interetTotal += montantPret * interetMensuel;

		montantPret -= (paiementMensuel - montantPret * interetMensuel);

		dureeInteret.dureePret += 1;
	}
	return dureeInteret;
};



int main()
{
// On demande a l'utilisateur d'entrer les informations necessaires.
	cout << "Somme d'argent prete: ";
	double montantPretUtilisateur = saisieUtilisateur(0);

	cout << "Montant rembourse chaque mois: ";
	double paimentMensuelUtilisateur = saisieUtilisateur(0);

	cout << "Taux d'interet annuel en pourcentage: ";
	double tauxInteretUtilisateur = saisieUtilisateur(0, 100);

	DureeInteret dureeInteretUtilisateur;
	dureeInteretUtilisateur = calculateLoanTerm(montantPretUtilisateur, paimentMensuelUtilisateur, tauxInteretUtilisateur, dureeInteretUtilisateur);

// On affiche les résultats de notre fonctions.
	cout << "Le nombre de mois necessaires pour rembourser la dette: " << dureeInteretUtilisateur.dureePret << endl;
	cout << "La somme des interets: " << dureeInteretUtilisateur.interetTotal << endl;
}
