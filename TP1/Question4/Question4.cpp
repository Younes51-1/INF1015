#include<iostream>
#include <cstdlib> 
#include <ctime>
#include<cmath>

using namespace std;

int iterations;
int nombrePointsDansLeCercle;
const double precision = 0.000001;
const double piArrondi = 3.141593;
bool positif;
double x;
double y;
double ecartRelatif;
double estimationPi;


double genererNombreAleatoire()
{
    return rand() / 16383.5 - 1;
}


/**
 * genere un nombre aleatoire compris entre -1 et 1
 * 
 * \return nombre genere aleatoirement
 */
int demanderNombreIterations()
{
    positif = false;
    while(!positif)
    {
        cout << "Entrez le nombre d'iterations souhaite : ";
        cin >> iterations;

        if(iterations > 0)
        {
            positif = true;
        }
    }
    return iterations;
}


/**
 * estime la valeur de pi en generant des points aleatoires entre -1 et 1 en 
 * abscisses et en ordonnees puis verifie si ce point appartient au cercle 
 * de rayon 1 et de centre 0.
 *
 * \param [in] iterations
 *        determine le nombre de points aleatoires qui seront generes pour 
          estimer pi
 *
 * \return estimation de pi
 */
double estimerPi(int iterations)
{
    nombrePointsDansLeCercle = 0;
    for(int i = 0; i <= iterations; i++)
    {
        x = genererNombreAleatoire();
        y = genererNombreAleatoire();

        if(pow(x, 2.0) + pow(y, 2.0) < 1)
        {
            nombrePointsDansLeCercle++;
        }
    }
    return nombrePointsDansLeCercle * 4.0 / double(iterations);
}


/**
 * arrondi la valeur d'entree selon la precision d'entree
 *
 * \param [in] valeur
 *        valeur a arrondir
 * 
 * \param [in] precision
 *        determine la precision a laquelle la valeur doit etre arrondie
 *
 * \return valeur arrondi a la precision d'entree
 */
double arrondir(double valeur, double precision)
{
    return round(valeur / precision) * precision;
}

int main()
{
// On demande a l'utilisateur les données nécessaires pour faire les calculs.
    srand(unsigned(time(nullptr)));
    iterations = demanderNombreIterations();
    estimationPi = estimerPi(iterations);
    ecartRelatif = abs(estimationPi - piArrondi);

// On affiche notre résultat.
    cout.precision(7);
    cout << "Estimation de pi : " << arrondir(estimationPi, precision) << endl;
    cout << "Ecart relatif : " << ecartRelatif;
}
