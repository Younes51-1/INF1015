#include <iostream>
#include <fstream>
#include <string>
#include<sstream>

using namespace std;

const int NBR_MOTS = 4;
// On a crée une structure qui inclut mot, genre et définition.
struct Dictionnaire
{
    string mot;
    string nomGenre;
    string definition;
};

int main()
{
// On ouvre notre fichier dictionnaire.txt.
    ifstream fichier("dictionnaire.txt", ios::in);
    string ligne;

    struct Dictionnaire tableauDictionnaire[NBR_MOTS];
	int indice = 0;
	int indiceDeMotPlusLong;
	string motPlusLong = "";

// On parcourt ligne par ligne et on extract mot, genre, et définition qui sont séparés par un caractère de tabulation 
// On les mets apres dans un tableau de structures.
	while(getline(fichier, ligne))
	{
		stringstream ss(ligne);
		getline(ss, tableauDictionnaire[indice].mot, '\t');
		getline(ss, tableauDictionnaire[indice].nomGenre, '\t');
		getline(ss, tableauDictionnaire[indice].definition, '\t');

// On verifie si le mot de la ligne actuelle est plus grands que le mot on a met dans la variable motPlusLong
// Si vrai on assigne ce mot a notre variable.
		if(tableauDictionnaire[indice].mot.size() > motPlusLong.size())
		{
			indiceDeMotPlusLong = indice;
			motPlusLong = tableauDictionnaire[indiceDeMotPlusLong].mot;
		}

		indice++;
	}

// On affiche le plus grand mot en terme de caractere de le format suivant => mot (nature/genre) : définition.
	cout << tableauDictionnaire[indiceDeMotPlusLong].mot <<" ("<< tableauDictionnaire[indiceDeMotPlusLong].nomGenre << ") : " << tableauDictionnaire[indiceDeMotPlusLong].definition << endl;
}
