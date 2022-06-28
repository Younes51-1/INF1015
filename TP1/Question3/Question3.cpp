#include <iostream>
#include <cmath>
#include "../Question2/Question2.h"

using namespace std;

const double GRAVITÉ = 9.81;

// On fait les calculs nécessaire avec les fourmule fourni dans l'énoncé du tp.
double CalculerHauteurRebond(double hauteurInitiale, double nombreRebonds, double coefficientRebond)
{
	double hauteurActuelle = hauteurInitiale;
	double vitesseActuelle;
	double hauteurSuivante;
	double vitesseSuivante;

	for(int i = 0; i < nombreRebonds; ++i)
	{
		vitesseActuelle = sqrt(2 * GRAVITÉ * hauteurActuelle);
		vitesseSuivante = coefficientRebond * vitesseActuelle;
		hauteurSuivante = pow(vitesseSuivante, 2) / (2 * GRAVITÉ);
		hauteurActuelle = hauteurSuivante;
	}
	return hauteurActuelle;
}

int main()
{
// On demande a l'utilisateur les données nécessaires pour faire les calculs.
	cout << "Hauteur initiale: ";
	double hauteurInitialeUtilisateur = saisieUtilisateur(0);

	cout << "Nombre de rebonds: ";
	double nombreRebondsUtilisateur = saisieUtilisateur(0);

	cout << "Coefficient de rebond: ";
	double coefficientRebondUtilisateur = saisieUtilisateur(0, 1);

// On affiche notre résultat.
	cout << "Hauteur atteinte: " << CalculerHauteurRebond(hauteurInitialeUtilisateur, nombreRebondsUtilisateur, coefficientRebondUtilisateur);
}
