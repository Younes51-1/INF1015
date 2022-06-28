#include <assert.h>

#include "Jeux.h"

#if __has_include("gtest/gtest.h")
#include "gtest/gtest.h"
#endif

#ifdef  TEST

// TEST pour effectuer un échec et mat sur le roi noir
// pour la fin de partie de La Bourdonnais vs. McDonnell, 1834
TEST(TestPartie0, TestValid)
{
	vue::Jeux jeux(0);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_FALSE(jeux.getTable().getEchecBlanc());
	EXPECT_FALSE(jeux.getTable().getMatBlanc());

	EXPECT_FALSE(jeux.getTable().getEchecNoir());
	EXPECT_FALSE(jeux.getTable().getMatNoir());

	EXPECT_TRUE(jeux.getTable().getBlancAJouer());

	std::pair<int, int> posTourBlanche = {1, 7};

	jeux.getTable().getPiece(posTourBlanche)->bouger(jeux.getTable(), {posTourBlanche.first, posTourBlanche.second - 7});

	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first, posTourBlanche.second - 7})->getCoordonnees().first
				== posTourBlanche.first);
	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first, posTourBlanche.second - 7})->getCoordonnees().second
				== posTourBlanche.second - 7);

	std::pair<int, int> posCavalierNoire = {0, 6};
	jeux.getTable().setUpdatePosition(jeux.getTable().getUpdatePosition());
	jeux.getTable().getPiece(posCavalierNoire)->bouger(jeux.getTable(), {posCavalierNoire.first + 2, posCavalierNoire.second - 1});

	EXPECT_TRUE(jeux.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second - 1})->getCoordonnees().first
				== posCavalierNoire.first + 2);
	EXPECT_TRUE(jeux.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second - 1})->getCoordonnees().second
				== posCavalierNoire.second - 1);

	EXPECT_FALSE(jeux.getTable().getEchecBlanc());
	EXPECT_FALSE(jeux.getTable().getMatBlanc());

	EXPECT_FALSE(jeux.getTable().getEchecNoir());
	EXPECT_FALSE(jeux.getTable().getMatNoir());

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	std::pair<int, int> posRoiBlanc = jeux.getTable().getCoordonneesRois()[true];

	jeux.getTable().getPiece(posRoiBlanc)->bouger(jeux.getTable(), {posRoiBlanc.first, posRoiBlanc.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].first == posRoiBlanc.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].second == posRoiBlanc.second + 1);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	std::pair<int, int> posRoiNoire = jeux.getTable().getCoordonneesRois()[false];

	jeux.getTable().setBlancAJouer(false);
	jeux.getTable().getPiece(posRoiNoire)->bouger(jeux.getTable(), {posRoiNoire.first, posRoiNoire.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].first == posRoiNoire.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].second == posRoiNoire.second + 1);

	jeux.getTable().setUpdate(jeux.getTable().getUpdate());
	posTourBlanche = {1, 0};

	jeux.getTable().getPiece(posTourBlanche)->bouger(jeux.getTable(), {posTourBlanche.first - 1 , posTourBlanche.second});

	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first - 1, posTourBlanche.second})->getCoordonnees().first
				== posTourBlanche.first - 1);
	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first - 1, posTourBlanche.second})->getCoordonnees().second
				== posTourBlanche.second);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_FALSE(jeux.getTable().getEchecBlanc());
	EXPECT_FALSE(jeux.getTable().getMatBlanc());

	EXPECT_TRUE(jeux.getTable().getEchecNoir());
	EXPECT_TRUE(jeux.getTable().getMatNoir());
};

// Tests de la partie J. Polgar vs. Kasparov, 1996
// Test des déplacments possibles
TEST(TestPartie1, TestValid)
{
	vue::Jeux jeuxPartie1(1);

	std::pair<int, int> posTourBlanche = {4, 7};

	jeuxPartie1.getTable().getPiece(posTourBlanche)->bouger(jeuxPartie1.getTable(), {posTourBlanche.first - 3, posTourBlanche.second});

	EXPECT_TRUE(jeuxPartie1.getTable().getPiece({posTourBlanche.first - 3, posTourBlanche.second})->getCoordonnees().first
				== posTourBlanche.first - 3);
	EXPECT_TRUE(jeuxPartie1.getTable().getPiece({posTourBlanche.first - 3, posTourBlanche.second})->getCoordonnees().second
				== posTourBlanche.second);

	std::pair<int, int> posCavalierNoire = {4, 1};

	jeuxPartie1.getTable().getPiece(posCavalierNoire)->bouger(jeuxPartie1.getTable(), {posCavalierNoire.first + 2, posCavalierNoire.second + 1});

	EXPECT_TRUE(jeuxPartie1.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second + 1})->getCoordonnees().first
				== posCavalierNoire.first + 2);
	EXPECT_TRUE(jeuxPartie1.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second + 1})->getCoordonnees().second
				== posCavalierNoire.second + 1);

	std::pair<int, int> posRoiBlanc = jeuxPartie1.getTable().getCoordonneesRois()[true];

	jeuxPartie1.getTable().getPiece(posRoiBlanc)->bouger(jeuxPartie1.getTable(), {posRoiBlanc.first, posRoiBlanc.second - 1});

	EXPECT_TRUE(jeuxPartie1.getTable().getCoordonneesRois()[true].first == posRoiBlanc.first);
	EXPECT_TRUE(jeuxPartie1.getTable().getCoordonneesRois()[true].second == posRoiBlanc.second - 1);

	std::pair<int, int> posRoiNoire = jeuxPartie1.getTable().getCoordonneesRois()[false];

	jeuxPartie1.getTable().getPiece(posRoiNoire)->bouger(jeuxPartie1.getTable(), {posRoiNoire.first - 1, posRoiNoire.second - 1});

	EXPECT_TRUE(jeuxPartie1.getTable().getCoordonneesRois()[false].first == posRoiNoire.first - 1);
	EXPECT_TRUE(jeuxPartie1.getTable().getCoordonneesRois()[false].second == posRoiNoire.second - 1);
};

// Test de la partie Karpov vs. Ftáčnik, 1988
// Test d'un mouvement impossible
TEST(TestPartie2, TestValid)
{
	vue::Jeux jeux(2);

	std::pair<int, int> posTourBlanche = {5, 6};

	EXPECT_TRUE(jeux.getTable().getPiece(posTourBlanche)->getNom() == "Tour");

	jeux.getTable().getPiece(posTourBlanche)->bouger(jeux.getTable(), {posTourBlanche.first, posTourBlanche.second - 5});

	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first, posTourBlanche.second - 5})->getCoordonnees().first
				== posTourBlanche.first);
	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first, posTourBlanche.second - 5})->getCoordonnees().second
				== posTourBlanche.second - 5);

	std::pair<int, int> posCavalierNoire = {1, 5};

	jeux.getTable().getPiece(posCavalierNoire)->bouger(jeux.getTable(), {posCavalierNoire.first + 2, posCavalierNoire.second + 1});

	EXPECT_TRUE(jeux.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second + 1})->getCoordonnees().first
				== posCavalierNoire.first + 2);
	EXPECT_TRUE(jeux.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second + 1})->getCoordonnees().second
				== posCavalierNoire.second + 1);

	std::pair<int, int> posRoiBlanc = jeux.getTable().getCoordonneesRois()[true];

	jeux.getTable().getPiece(posRoiBlanc)->bouger(jeux.getTable(), {posRoiBlanc.first, posRoiBlanc.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].first == posRoiBlanc.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].second == posRoiBlanc.second + 1);

	std::pair<int, int> posRoiNoire = jeux.getTable().getCoordonneesRois()[false];

	jeux.getTable().getPiece(posRoiNoire)->bouger(jeux.getTable(), {posRoiNoire.first, posRoiNoire.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].first == posRoiNoire.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].second == posRoiNoire.second + 1);
	try
	{
		jeux.getTable().getPiece(jeux.getTable().getCoordonneesRois()[true])->bouger(jeux.getTable(), {posRoiBlanc.first + 1, posRoiBlanc.second - 1});
	}
	catch(DeplacementInvalidException& msg)
	{
		std::cout << msg.what() << std::endl;
	}
};

// Créer une partie aléatoire avec 3 rois
// pour tester l'exception et déplacement en X du roi
TEST(CreationGame, TestValid)
{
	vue::Jeux jeux(3);
	vue::Jeux jeuxException(4);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_TRUE(jeux.getTable().getEchecBlanc());
	EXPECT_FALSE(jeux.getTable().getMatBlanc());

	EXPECT_TRUE(jeux.getTable().getEchecNoir());
	EXPECT_FALSE(jeux.getTable().getMatNoir());

	std::pair<int, int> posRoiBlanc = jeux.getTable().getCoordonneesRois()[true];

	jeux.getTable().getPiece(posRoiBlanc)->bouger(jeux.getTable(), {posRoiBlanc.first + 1, posRoiBlanc.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].first == posRoiBlanc.first + 1);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].second == posRoiBlanc.second + 1);
};

// Test d'un pat sur roi noir
TEST(PatTest, TestValid)
{
	vue::Jeux jeux(5);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_FALSE(jeux.getTable().getPatBlanc());
	EXPECT_FALSE(jeux.getTable().getPatNoir());

	std::pair<int, int> posTourBlanche = {0, 5};

	jeux.getTable().getPiece(posTourBlanche)->bouger(jeux.getTable(), {posTourBlanche.first, posTourBlanche.second + 1});

	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first, posTourBlanche.second + 1})->getCoordonnees().first
				== posTourBlanche.first);
	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first, posTourBlanche.second + 1})->getCoordonnees().second
				== posTourBlanche.second + 1);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_TRUE(jeux.getTable().getEchecNoir());
	EXPECT_FALSE(jeux.getTable().getMatNoir());

	std::pair<int, int> posRoiNoire = jeux.getTable().getCoordonneesRois()[false];

	jeux.getTable().getPiece(posRoiNoire)->bouger(jeux.getTable(), {posRoiNoire.first, posRoiNoire.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].first == posRoiNoire.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].second == posRoiNoire.second + 1);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_FALSE(jeux.getTable().getPatBlanc());
	EXPECT_TRUE(jeux.getTable().getPatNoir());
};

// Test d'un pat sur roi blanc
TEST(PatInverseTest, TestValid)
{
	vue::Jeux jeux(6);

	EXPECT_FALSE(jeux.getTable().getPatBlanc());
	EXPECT_FALSE(jeux.getTable().getPatNoir());

	std::pair<int, int> posRoiBlanc = jeux.getTable().getCoordonneesRois()[true];

	jeux.getTable().getPiece(posRoiBlanc)->bouger(jeux.getTable(), {posRoiBlanc.first, posRoiBlanc.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].first == posRoiBlanc.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].second == posRoiBlanc.second + 1);

	std::pair<int, int> posTourNoir = {0, 5};

	jeux.getTable().getPiece(posTourNoir)->bouger(jeux.getTable(), {posTourNoir.first, posTourNoir.second + 1});

	EXPECT_TRUE(jeux.getTable().getPiece({posTourNoir.first, posTourNoir.second + 1})->getCoordonnees().first
				== posTourNoir.first);
	EXPECT_TRUE(jeux.getTable().getPiece({posTourNoir.first, posTourNoir.second + 1})->getCoordonnees().second
				== posTourNoir.second + 1);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_FALSE(jeux.getTable().getPatNoir());
	EXPECT_TRUE(jeux.getTable().getPatBlanc());
};

// Test d'un échec et mat sur roi blanc
TEST(PatInverseTestCheckMate, TestValid)
{
	vue::Jeux jeux(6);

	EXPECT_FALSE(jeux.getTable().getPatBlanc());
	EXPECT_FALSE(jeux.getTable().getPatNoir());

	std::pair<int, int> posRoiBlanc = jeux.getTable().getCoordonneesRois()[true];

	jeux.getTable().getPiece(posRoiBlanc)->bouger(jeux.getTable(), {posRoiBlanc.first, posRoiBlanc.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].first == posRoiBlanc.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].second == posRoiBlanc.second + 1);

	std::pair<int, int> posTourNoir = {0, 5};

	jeux.getTable().getPiece(posTourNoir)->bouger(jeux.getTable(), {posTourNoir.first + 7, posTourNoir.second});

	EXPECT_TRUE(jeux.getTable().getPiece({posTourNoir.first + 7, posTourNoir.second})->getCoordonnees().first
				== posTourNoir.first + 7);
	EXPECT_TRUE(jeux.getTable().getPiece({posTourNoir.first + 7, posTourNoir.second})->getCoordonnees().second
				== posTourNoir.second);

	jeux.getTable().verificationEchec();
	jeux.getTable().verificationMat();

	EXPECT_FALSE(jeux.getTable().getPatNoir());
	EXPECT_FALSE(jeux.getTable().getPatBlanc());

	EXPECT_TRUE(jeux.getTable().getEchecBlanc());
	EXPECT_TRUE(jeux.getTable().getMatBlanc());

	EXPECT_FALSE(jeux.getTable().getEchecNoir());
	EXPECT_FALSE(jeux.getTable().getMatNoir());
};

#endif