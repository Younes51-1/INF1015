#include "CodeCoverage_bugfix.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#if __has_include("entree_utf8.hpp")
#include "entree_utf8.hpp"
#define DOIT_CONFIGURER_CONSOLE_IN
#endif
#define DOIT_CONFIGURER_CONSOLE
#endif

#include "bibliotheque_cours.hpp"
#if __has_include("gtest/gtest.h")
#include "gtest/gtest.h"
#endif
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <array>
#include <locale>
#include "gsl/span"  // La version de VS dans les labs n'a pas <span> de C++20; on utilise gsl
#ifdef GSL_SPAN_H
namespace std
{
	using ::gsl::span;
}
#endif

#if __has_cpp_attribute(nodiscard) < 201907L
#define nodiscard(X) nodiscard  // Si le compilateur ne supporte pas le nouveau nodiscard de C++20
#endif

namespace bibliotheque_cours
{
	static const int valeur_succes = 0;

#pragma region "Debogage/Test" //{
// Passe les arguments de la ligne de commande à GoogleTest.
// Retourne vrai s'il y a au moins un argument de la ligne de commande qui a été utilisé par GoogleTest.
	bool initialiserGoogleTest(int& argc, [[maybe_unused]] char** argv)
	{
		int ancienArgc = argc;
#ifdef GTEST_TEST
		::testing::InitGoogleTest(&argc, argv);
#endif
		return argc != ancienArgc;
	}

	bool est_une_console_quon_a_ouverte = false;
#ifdef _MSC_VER
	static void __cdecl initialisation_juste_avant_variables_globales()
	{
		static struct S
		{
			~S()
			{ // cin et cout n'existent probablement pas ici si on ne les forcent pas.
				if(est_une_console_quon_a_ouverte)
				{
					static const char message[] = "\nAppuyez sur enter pour continuer...";
					WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, DWORD(std::size(message)), nullptr, nullptr);
					char lecture[2]; DWORD nLus = 0;
					ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), lecture, 1, &nLus, nullptr);
				}
			}
		} initialisee_juste_avant_variables_globales;
	}
#define INITIALISATION_JUSTE_AVANT_VARIABLES_GLOBALES ".CRT$XCT"
#pragma section(INITIALISATION_JUSTE_AVANT_VARIABLES_GLOBALES, long, read)
	__declspec(allocate(INITIALISATION_JUSTE_AVANT_VARIABLES_GLOBALES)) static auto initializer_juste_avant_variables_globales = &initialisation_juste_avant_variables_globales;

#endif

	void ouvrirUneConsoleSiTestAvecDebogueur()
	{
#ifdef _MSC_VER
		if(IsDebuggerPresent() && GetConsoleWindow() == nullptr)
		{
			FreeConsole();
			AllocConsole();
			auto reouvrir = [](const char* nom_fichier, const char* mode, FILE* fStream, DWORD stdHandle, std::ios& cppStream) {
				DWORD access = GENERIC_READ | GENERIC_WRITE, share = FILE_SHARE_READ | FILE_SHARE_WRITE, creation = OPEN_EXISTING, attributs = FILE_ATTRIBUTE_NORMAL;
				FILE* f; freopen_s(&f, nom_fichier, mode, fStream);
				SetStdHandle(stdHandle, CreateFileA(nom_fichier, access, share, nullptr, creation, attributs, nullptr));
				cppStream.clear();
			};
			// Noter que ceci fait que l'Exlorateur de tests en mode débogage n'affichera pas la sortie standard "Message" du test, on doit la regarder dans la console.  Il affiche tout de même le résultat avec valeurs espérée.
			reouvrir("CONOUT$", "w", stdout, STD_OUTPUT_HANDLE, std::cout);
			reouvrir("CONOUT$", "w", stderr, STD_ERROR_HANDLE, std::cerr);
			reouvrir("CONIN$", "r", stdin, STD_INPUT_HANDLE, std::cin);
			est_une_console_quon_a_ouverte = true;
		}
#endif
	}

	// seulementSiArguments=true permet d'exécuter les tests dans l'Explorateur (ou si on passe nous-mêmes des arguments pour les tests sur la ligne de commande) sans les voir quand on fait une exécution normale.
	// arreterSiErreur=true arrête l'exécution du programme après les tests s'il y a un erreur.
	int executerGoogleTest([[maybe_unused]] int& argc, [[maybe_unused]] char** argv, [[maybe_unused]] bool seulementSiArguments, [[maybe_unused]] bool arreterSiErreur)
	{
#ifdef GTEST_TEST
		bool aArgumentsGoogleTest = initialiserGoogleTest(argc, argv);
		if(seulementSiArguments && !aArgumentsGoogleTest)
			return valeur_succes;
		ouvrirUneConsoleSiTestAvecDebogueur();
		int resultat = RUN_ALL_TESTS();
		if(aArgumentsGoogleTest || (arreterSiErreur && resultat != valeur_succes))
			exit(resultat);  // Termine immédiatement le programme.
		return resultat;
#else
		return valeur_succes;
#endif
	}

	// Returns true if message sent to debugger, false if sent to clog.
	static bool displayDebugString(const std::string& message)
	{
#ifdef _MSC_VER
		if constexpr(executionEstUtf8)
		{
// On doit convertir d'UTF-8 à wchar_t requis par la bibliohèque Windows.
			std::wstring resultat(message.size(), '\0');  // La taille résultante en caractères est toujours <= la taille en UTF-8.
			resultat.resize(MultiByteToWideChar(CP_UTF8, 0, &message.front(), int(message.size()), &resultat.front(), int(resultat.size())));
			OutputDebugStringW(resultat.c_str());
		}
		else
			OutputDebugStringA(message.c_str());
		return true;
#else
		std::clog << message << std::flush;
		return false;
#endif
	}

	void dbgbuf::flushMessageBuffer()
	{
		if(!logMessageBuffer.empty())
		{
			bool wasSentToDebugOutput = displayDebugString(logMessageBuffer);
			if(teeOutput && wasSentToDebugOutput)
				(*teeOutput) << logMessageBuffer << std::flush;

			logMessageBuffer.erase().shrink_to_fit();
		}
	}

	dbgstream cdbg;

#pragma endregion //}

#pragma region "Affichage/Console" //{
// Active les code de couleurs ANSI pour l'affichage sur la console.
// Des codes tels que "\x1b[1m" (bold) et "\x1b[0m" (normal), "\x1b" étant le caractère "escape", voir https://en.wikipedia.org/wiki/ANSI_escape_code .
	void activerCouleursAnsi()
	{
#ifdef WIN32_LEAN_AND_MEAN
// Depuis Windows 10.0.10586 les couleurs sont activées par défaut dans la console "cmd" mais sont désctivées quand on exécute des programmes depuis 10.0.14393 car ça a créé des problèmes de compatibilité (https://github.com/microsoft/WSL/issues/1173#issuecomment-254250445).  Noter que Visual Studio exécute notre programme avec les couleurs activées et la version actuelle de "Windows Terminal Preview" les laisse activés aussi.  On s'assure de l'activer au cas.
		HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD modeActuel = 0;
		GetConsoleMode(hConOut, &modeActuel);
		SetConsoleMode(hConOut, modeActuel | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
// Dans la console Linux ou MacOS, c'est normalement toujours activé par défaut.
#endif
	}

	bool activerEncodageLocale(std::span<const char*> locales_a_essayer)
	{
		// Noter que sur Windows, setlocale change la traduction de sortie entre le programme et la console sans change le codepage; si on veut pouvoir faire tous les caractères, il faut aussi changer le codepage avant setlocale.
		for(const char* nom_locale : locales_a_essayer)
			if(setlocale(LC_CTYPE, nom_locale) != nullptr)
				return true;
		return false;
	}

	// Classe générique pour "remplacer" une valeur en conservant l'ancienne et on "remet" l'ancienne valeur automatiquement quand l'objet est détruit.
	template <typename FonctionRemplaceT, typename ParamT>
	class RemplaceRemet
	{
		public:
		template <typename ParamT_>
		RemplaceRemet(FonctionRemplaceT fonctionRemplace_, ParamT_&& nouvelleValeur)
			: fonctionRemplace(fonctionRemplace_)
			, ancienneValeur(fonctionRemplace(std::forward<ParamT_>(nouvelleValeur)))
		{
			static_assert(std::is_same_v<ParamT, ParamT_>, "Le guide a mal déduit le type ou la construction force le mauvais type.");
		}
		void remet()
		{
			if(!estDejaRemis)
			{
// On move si la construction originale utilisait un rvalue.  On ne peut pas std::forward car il pourait y avoir une conversion implicite à faire sur l'ancienne valeur.
				if constexpr(std::is_rvalue_reference_v<ParamT>)
					(void) fonctionRemplace(std::move(ancienneValeur));
				else
					(void) fonctionRemplace(ancienneValeur);
				estDejaRemis = true;
			}
		}
		~RemplaceRemet()
		{
			remet();
		}

		private:
		bool estDejaRemis = false;
		FonctionRemplaceT fonctionRemplace;
		std::invoke_result_t<FonctionRemplaceT, ParamT> ancienneValeur;
	};
	// Guide de déduction de type à cause du paramètre "forwarding reference".
	template <typename FonctionRemplaceT, typename ParamT>
	RemplaceRemet(FonctionRemplaceT, ParamT&&)->RemplaceRemet<FonctionRemplaceT, ParamT>;

#ifdef WIN32_LEAN_AND_MEAN

	[[nodiscard("Remet l'ancienne valeur immediatément si on ne conserve pas l'objet retourné.")]]
	auto remplaceRdbuf(std::ios& leStream, std::streambuf* nouveauStreambuf)
	{
		return RemplaceRemet([&leStream](std::streambuf* sb) {
			return leStream.rdbuf(sb);
							 }, nouveauStreambuf);
	}

	[[nodiscard("Remet l'ancienne valeur immediatément si on ne conserve pas l'objet retourné.")]]
	auto remplaceConsoleInputCP(UINT codePageId)
	{
		return RemplaceRemet([](UINT cp) {
			auto ancien = GetConsoleCP();
			SetConsoleCP(cp);
			return ancien;
							 }, codePageId);
	}

	[[nodiscard("Remet l'ancienne valeur immediatément si on ne conserve pas l'objet retourné.")]]
	auto remplaceConsoleOutputCP(UINT codePageId)
	{
		return RemplaceRemet([](UINT cp) {
			auto ancien = GetConsoleOutputCP();
			SetConsoleOutputCP(cp);
			return ancien;
							 }, codePageId);
	}

#endif

	static const char* to_const_char(const std::string& s)
	{
		return s.c_str();
	}
	static const char* to_const_char(const char* s)
	{
		return s;
	}

	[[nodiscard]]//("Remet l'ancienne valeur immediatément si on ne conserve pas l'objet retourné.")]] // bug de gcc 9.3: error: wrong number of arguments specified for ‘nodiscard’ attribute
	auto remplaceCLocale(int category, std::span<const char*> locales_a_essayer)
	{
		return RemplaceRemet([category](auto locales) {
			const char* ancien = setlocale(category, nullptr);
			std::string ancienStr = ancien ? ancien : "C";  // Attention, le standard dit que le pointeur retourné par setlocale peut être invalidé par un prochain appel à setlocale.  On doit donc prendre une copie du texte.
			for(auto&& nom_locale : locales)
				if(setlocale(category, to_const_char(nom_locale)) != nullptr)
					break;
			return std::array<std::string, 1>{ancienStr};
							 }, locales_a_essayer);
	}

	// Retourne la liste des encodages à essayer dans setlocale pour l'encodage d'exécution actuel.
	static std::span<const char*> locales_a_essayer()
	{
		if constexpr(executionEstUtf8)
		{
			static const char* locales_utf8[] = {".UTF-8", "C.UTF-8"}; return locales_utf8;
		}
		else
		{
			static const char* locales_defaut[] = {""}; return locales_defaut;
		}
	}

	// Tout est static dans le constructeur (similaire au getInstance du patron "singleton") pour que l'initialisation se fasse une seule fois lors de la première instanciation, similairement à ios_base::Init.
	InitConsole::InitConsole()
	{
#ifdef DOIT_CONFIGURER_CONSOLE
		static const UINT latin1 = 1252;  // L'unique différence entre Windows-1252 et Latin-1 est le bloc de caractères de contrôles de Latin-1 qui contient des glyphes en 1252.
		static const UINT codepageConsole = executionEstUtf8 ? CP_UTF8 : latin1;
#ifdef DOIT_CONFIGURER_CONSOLE_IN
		if constexpr(executionEstUtf8)
		{
			static std::ios_base::Init ioinit;  // On s'assure que cin est initialisé.
			static auto gardeCin = remplaceRdbuf(std::cin, ifilebuf_console_utf8_instance::get());
		}
#endif
		static auto gardeInputCP = remplaceConsoleInputCP(codepageConsole);
		static auto gardeOutputCP = remplaceConsoleOutputCP(codepageConsole);
		// Il faut changer le codepage avant le locale, car la fonction de traduction des caractères est choisie selon le codepage au moment du setlocale.
#endif
		static auto gardeCLocale = remplaceCLocale(LC_CTYPE, locales_a_essayer());
	}

#pragma endregion //}
}

#ifdef ExcludeSourceFromCodeCoverage
// Exclusions de la couverture de code:
ExcludeSourceFromCodeCoverage(ProgramFilesFilter1, L"*\\Program Files (x86)\\*")
ExcludeSourceFromCodeCoverage(ProgramFilesFilter2, L"*\\Program Files\\*")
ExcludeSourceFromCodeCoverage(SdkFilesFilter1, L"sdk\\*")
ExcludeSourceFromCodeCoverage(SharedFilesFilter1, L"shared\\*")
ExcludeSourceFromCodeCoverage(BoostFilesFilter1, L"*\\boost\\*")
ExcludeSourceFromCodeCoverage(GtestFilesFilter1, L"*\\googletest\\*")
ExcludeFromCodeCoverage(GtestFilter1, L"testing::*")
ExcludeSourceFromCodeCoverage(GslFilesFilter1, L"*\\gsl\\*")
ExcludeSourceFromCodeCoverage(ItertoolsFilesFilter1, L"*\\cppitertools\\*")
ExcludeSourceFromCodeCoverage(QtMocFilesFilter1, L"*\\moc\\moc_*.cpp")
ExcludeSourceFromCodeCoverage(QtFilesFilter1, L"*\\include\\Qt*\\*")
ExcludeSourceFromCodeCoverage(VcpkgFilesFilter1, L"*\\vcpkg*\\*")
#ifndef TESTING_BIBLIOTHEQUE_COURS
ExcludeFromCodeCoverage(BibliothequeCoursFilter1, L"*bibliotheque_cours::*")  // Il y a des "* bib..." et des "*'bib...", on les filtres tous d'un coup avec "*bib..." même si ça filtre aussi je_suis_etudiant_et_jai_ma_propre_bibliotheque_cours.
//ExcludeSourceFromCodeCoverage(BibliothequeCoursFilter2, L"*\\bibliotheque_cours.*")  // L'exclusion du fichier ne semble pas nécessaire avec l'exlusion ci-dessus.
#endif
// On ne veut pas la couveture pour les fonctions des tests.
ExcludeFromCodeCoverage(FonctionsDeTestGtestFilter1, L"*_Test::*")
ExcludeSourceFromCodeCoverage(TestsFilesFilter1, L"*\\tests\\*")
ExcludeFromCodeCoverage(FonctionsDebogageAllocation1, L"operator new*")
ExcludeFromCodeCoverage(FonctionsDebogageAllocation2, L"operator delete*")
#endif