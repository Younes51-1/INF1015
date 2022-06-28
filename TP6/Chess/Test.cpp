#include <assert.h>

#include "Jeux.h"

#if __has_include("gtest/gtest.h")
#include "gtest/gtest.h"
#endif

#ifdef  TEST
TEST(DeplacementTest, TestValid)
{
	modele::Jeux jeux;

	std::pair<int, int> posRoiBlanc = jeux.getTable().getCoordonneesRois()[true];

	jeux.getTable().getPiece(posRoiBlanc)->bouger(jeux.getTable(), {posRoiBlanc.first, posRoiBlanc.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].first == posRoiBlanc.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[true].second == posRoiBlanc.second + 1);

	std::pair<int, int> posRoiNoire = jeux.getTable().getCoordonneesRois()[false];

	jeux.getTable().getPiece(posRoiNoire)->bouger(jeux.getTable(), {posRoiNoire.first, posRoiNoire.second + 1});

	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].first == posRoiNoire.first);
	EXPECT_TRUE(jeux.getTable().getCoordonneesRois()[false].second == posRoiNoire.second + 1);

	std::pair<int, int> posTourBlanche = {1, 7};

	jeux.getTable().getPiece(posTourBlanche)->bouger(jeux.getTable(), {posTourBlanche.first + 5, posTourBlanche.second});

	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first + 5, posTourBlanche.second})->getCoordonnees().first
				== posTourBlanche.first + 5);
	EXPECT_TRUE(jeux.getTable().getPiece({posTourBlanche.first + 5, posTourBlanche.second})->getCoordonnees().second
				== posTourBlanche.second);

	std::pair<int, int> posCavalierNoire = {0, 6};

	jeux.getTable().getPiece(posCavalierNoire)->bouger(jeux.getTable(), {posCavalierNoire.first + 2, posCavalierNoire.second + 1});

	EXPECT_TRUE(jeux.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second + 1})->getCoordonnees().first
				== posCavalierNoire.first + 2);
	EXPECT_TRUE(jeux.getTable().getPiece({posCavalierNoire.first + 2, posCavalierNoire.second + 1})->getCoordonnees().second
				== posCavalierNoire.second + 1);
};

#endif