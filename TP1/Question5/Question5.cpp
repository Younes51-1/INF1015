#include<iostream>
using namespace std;

int indice = 0;

/**
 * ajoute dans l'ordre les nombres pairs du tableau1 au tableauTrie
 *
 * \param [in] tableau1
 *        tableau des notes entrees par l'utilisateur
 * 
 * \param [in] tableauTrie
 *        tableau qui recoit les notes triees
 * 
 * \param [in] dimTableau
 *        dimension du tableau1 et du tableauTrie
 */
void ajouterPairAuNouveauTableau(const int tableau1[], int tableauTrie[], int dimTableau)
{
    for(int i = 0; i < dimTableau; i++)
    {
        if(tableau1[i] % 2 == 0)
        {
            tableauTrie[indice] = tableau1[i];
            indice++;
        }
    }
}


/**
 * ajoute dans l'ordre les nombres impairs du tableau1 au tableauTrie
 *
 * \param [in] tableau1
 *        tableau des notes entrees par l'utilisateur
 *
 * \param [in] tableauTrie
 *        tableau qui recoit les notes triees
 *
 * \param [in] dimTableau
 *        dimension du tableau1 et du tableauTrie
 */
void ajouterImpairAuNouveauTableau(const int tableau1[], int tableauTrie[], int dimTableau)
{
    for(int i = 0; i < dimTableau; i++)
    {
        if(tableau1[i] % 2 != 0)
        {
            tableauTrie[indice] = tableau1[i];
            indice++;
        }
    }
}


int main()
{   
    static const int dimensionTableau = 10;
    int tableau1[dimensionTableau];
    int tableauTrie[dimensionTableau];

// On demande a l'utilisateur les données nécessaires pour faire les calculs.
    cout << "Saisissez " << dimensionTableau << " notes a mettre dans le tableau : ";

    for(int& note : tableau1)
    {
        cin >> note;
    }

    ajouterPairAuNouveauTableau(tableau1, tableauTrie, dimensionTableau);
    ajouterImpairAuNouveauTableau(tableau1, tableauTrie, dimensionTableau);

    // On affiche notre résultat.
    cout << "Nouveau tableau trie : ";

    for(int note : tableauTrie)
    {
        cout << note << " ";
    }
}
