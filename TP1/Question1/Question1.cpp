#include<iostream>
#include <cmath>

using namespace std;

int nombre;
bool estPremier = true;

// Cette fonction verifie si le nombre est premier suivant les derictives de l'énoncé du tp.
void verifierPrimalite(int nombre) 
{
	if(nombre % 2 == 0 && nombre != 2)
	{
		cout << "Ce nombre n'est pas premier car il est divisible par 2." << endl;
		estPremier = false;
	}
	else
	{
		for(int i = 3; i <= sqrt(nombre); i += 2)
		{
			if(nombre % i == 0)
			{
				cout << "Ce nombre n'est pas premier car il est divisible par " << i << "." << endl;
				estPremier = false;
			}
		}

	}
	if(estPremier)
	{
		cout << "Ce nombre est premier." << endl;
	}
}

int main()
{
	cout << "Entrer un nombre entier : ";
	cin >> nombre; 
// Aprés qu'on a demandé a l'utilisateur d'entrer un nombre, on vérifie sa primalité.
	verifierPrimalite(nombre); 
}
