#pragma once
#include <iostream>
#include <cmath>

using namespace std;

// On verifie si le nombre saisie par l'utilisateur respect les conditions.
double saisieUtilisateur(double condition1, double condition2 = INFINITY)
{
	double nombre;
	cin >> nombre;

	while(nombre < condition1 || condition2 < nombre)
	{
		cout << "Saisie invalide, veuillez ressaisir : ";
		cin >> nombre;
	}
	return nombre;
}
