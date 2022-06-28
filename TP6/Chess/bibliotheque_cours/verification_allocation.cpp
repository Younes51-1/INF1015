/// Verification des fuites de mémoire.
/// Utilise un unordered_map pour conserver toutes les allocations, ceci a évidemment un impact sur la vitesse d'exécution, donc on ne l'utilise normalement pas sur un code final mais plutôt durant la vérification d'un programme.
/// \author Francois-R.Boyer@PolyMtl.ca
/// \version 2021-01
/// \since   2020-04

#include "verification_allocation.hpp"

#if defined(_WIN32) && !defined(__CYGWIN__)
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <cassert>  // Peut-être utiliser gsl Expects à la place?
#include <limits>
#include <string>
#include <cstring>
#include <csignal>
#include "gsl/span"
using namespace gsl;
using namespace std;

// Configuration si on veut afficher les erreur à la console et si on veut terminer l'exécution sur une erreur de désallocation.  Définir ou non les macros suivantes.
#define AFFICHER_ERREUR_DELETE
#define TERMINATE_SUR_ERREUR_DELETE
//#define VERIFICATION_DELETE_STRICTE  // Cause une erreur si un delete est fait sans qu'il y ait un new correspondant connu.  Peut causer des problèmes si une bibliothèque a fait un new avant que le système de vérification soit actif, et fait son delete pendant que le système est actif.

namespace bibliotheque_cours
{
	bool desactive_terminate_sur_erreur_delete = false;

	SorteErreurDelete derniere_erreur_delete = SorteErreurDelete::no_error;
	const char* get_message_erreur_delete()
	{
		static const char* messages[] = {
			"Pas d'erreur de delete.",
			"Desallocation d'un bloc non alloue (ou deja desalloue).",
			"delete[] d'un pointeur alloue avec new sans [].",
			"delete sans [] d'un pointeur alloue avec new [].",
			"le programme a ecrit hors des bornes d'une allocation memoire",
			"Erreur inconnue, probablement une corruption de memoire."
		};
		if(unsigned(derniere_erreur_delete) > unsigned(SorteErreurDelete::last))
			return messages[unsigned(SorteErreurDelete::last) + 1];
		return messages[unsigned(derniere_erreur_delete)];
	}

	static const InfoBlocMemoire pas_de_bloc = {numeric_limits<size_t>::max(), false, nullptr, 0, 0};
	static void lancer_erreur_delete(SorteErreurDelete erreur, const InfoBlocMemoire& info = pas_de_bloc)
	{
		derniere_erreur_delete = erreur;
#if defined(TERMINATE_SUR_ERREUR_DELETE) || defined(AFFICHER_ERREUR_DELETE)
	// On ne peut pas "throw" dans l'operator delete, donc on "terminate" s'il y a une erreur.
		string message = get_message_erreur_delete();
		if(&info != &pas_de_bloc)
		{
			message += " Bloc de " + to_string(info.taille) + " octets, new" + (info.est_tableau ? "[] " : " ");
			if(info.nom_fichier)
				message += ", ligne '"s + info.nom_fichier + "':" + to_string(info.ligne_fichier);
		}
		cerr << "Erreur verification allocation: " << message << endl;
#endif
#ifdef TERMINATE_SUR_ERREUR_DELETE
	// Voir le message d'erreur affiché sur cerr ou le contenu de la variable 'message'.
	// Voir aussi la "Pile des appels" dans le débogueur pour trouver à partir du haut de la liste le premier nom qui correspond à une fonction de votre programme, pour savoir quelle ligne a causée cette erreur.
		if(!desactive_terminate_sur_erreur_delete) terminate();
#endif
	}

	using TypeValeurVerificationCorruption = unsigned;
	static constexpr TypeValeurVerificationCorruption valeur_verification_corruption = 0xDEADBEEF;
	static constexpr size_t n_mots_verification_corruption = 0;  //NOTE: Je n'arrive pas à faire fonctionner ma vérification de corruption avec GoogleTest, donc on la désactive.
	static constexpr size_t taille_verification_corruption = n_mots_verification_corruption * sizeof(valeur_verification_corruption);
	static constexpr bool a_verification_corruption = taille_verification_corruption != 0;
	static constexpr bool verification_contient_taille = taille_verification_corruption >= 2 * sizeof(valeur_verification_corruption);

	size_t compteur_de_new = 0, compteur_de_delete = 0;

	void remise_a_zero_compteurs_allocation()
	{
		compteur_de_new = 0;
		compteur_de_delete = 0;
		derniere_erreur_delete = SorteErreurDelete::no_error;
	}
	void remise_a_zero_verification()
	{
		SansVerifierAllocations sva;
		get_blocs_alloues().clear();
		remise_a_zero_compteurs_allocation();
	}

	void activer_verification_allocation()
	{
		remise_a_zero_verification(); SansVerifierAllocations::est_actif = false;
	}
	void desactiver_verification_allocation()
	{
		SansVerifierAllocations::est_actif = true;
	}

	void remplir_bloc_verification_corruption_a(void* ptr, size_t sz)
	{
		if constexpr(a_verification_corruption)
		{
			auto* p = reinterpret_cast<TypeValeurVerificationCorruption*>(ptr);
			if constexpr(verification_contient_taille)
			{
				p[0] = TypeValeurVerificationCorruption(sz);
				p[1] = p[0] ^ valeur_verification_corruption;
			}
			for(size_t i = verification_contient_taille ? 2 : 0; i < n_mots_verification_corruption; ++i)
				p[i] = valeur_verification_corruption;
		}
	}
	bool tester_bloc_verification_corruption_a(void* ptr)
	{
		if constexpr(a_verification_corruption)
		{
			auto* p = reinterpret_cast<TypeValeurVerificationCorruption*>(ptr);
			if constexpr(verification_contient_taille)
			{
				if((p[1] ^ p[0]) != valeur_verification_corruption)
					return false;
			}
			for(size_t i = verification_contient_taille ? 2 : 0; i < n_mots_verification_corruption; ++i)
				if(p[i] != valeur_verification_corruption)
					return false;
		}
		return true;
	}

	char* pointeur_octets(void* p)
	{
		return reinterpret_cast<char*>(p);
	}
//const char* pointeur_octets(const void* p) { return reinterpret_cast<const char*>(p); }

// Devrait être appelé uniquement lorsqu'on sait que le bloc n'est pas corrompu (l'assertion devrait se rendre compte d'un problème, mais ça n'est pas vérifiable dans un test).
	size_t taille_allocation_selon_verification_corruption(void* ptr)
	{
		assert(verification_contient_taille);
		auto* p = reinterpret_cast<TypeValeurVerificationCorruption*>(pointeur_octets(ptr) - taille_verification_corruption);
		assert((p[1] ^ p[0]) == valeur_verification_corruption);
		return p[0];
	}

	void mettre_verification_corruption_sur_allocation(void* ptr, size_t sz)
	{
		if constexpr(a_verification_corruption)
		{
			remplir_bloc_verification_corruption_a(pointeur_octets(ptr) - taille_verification_corruption, sz);
			remplir_bloc_verification_corruption_a(pointeur_octets(ptr) + sz, sz);
		}
	}
	bool tester_verification_corruption_sur_allocation(void* ptr, size_t sz = numeric_limits<size_t>::max())
	{
		if(!tester_bloc_verification_corruption_a(pointeur_octets(ptr) - taille_verification_corruption))
			return false;
		if(sz == numeric_limits<size_t>::max())
		{
			if constexpr(verification_contient_taille)
				sz = taille_allocation_selon_verification_corruption(ptr);
			else
				// On ne peut pas vérifier le bloc à la fin car on ne sait pas la longueur de l'allocation.
				return true;
		}
		return tester_bloc_verification_corruption_a(pointeur_octets(ptr) + sz);
	}

	void assurer_taille_allocation_possible(size_t sz)
	{
		// On ne supporte pas actuellement l'allocation de blocs plus gros de 4GB, ni que la taille plus la taille de la vérification de corruption dépasse un size_t.
		if(sz > std::min<size_t>(numeric_limits<TypeValeurVerificationCorruption>::max(), numeric_limits<size_t>::max() - 2 * taille_verification_corruption))
			throw std::runtime_error("Allocation trop grosse ou negative.");
	}

	bool debogage_allocation_est_actif()
	{
#ifdef _MSC_VER
	// GoogleTest utilise ce flag pour indiquer les allocations pour lesquelles il ne fera pas de désallocation (voir MemoryIsNotDeallocated dans gtest-port.cc).
		auto flags_presents = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		return (flags_presents & _CRTDBG_ALLOC_MEM_DF) != 0;
#else
	// Sur gcc/clang, GoogleTest ne fait rien... Il faudrait le "patcher" pour utiliser une technique similaire qui désactive notre vérification d'allocation.
		return true;
#endif
	}

	span<size_t> breakpoint_sur_allocations;

	void set_breakpoint_sur_allocations(size_t* numeros, size_t nNumeros)
	{
		breakpoint_sur_allocations = span(numeros, nNumeros);
	}

#if defined(_MSC_VER)
#define POINT_ARRET() __debugbreak()
#elif defined(_WINDOWS_)
#define POINT_ARRET() DebugBreak()
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#define POINT_ARRET() __asm__("int $3")
#elif defined(SIGTRAP)
#define POINT_ARRET() std::raise(SIGTRAP)
#else
#define POINT_ARRET() std::raise(SIGINT)
#endif

	size_t compte_allocations(bool donne_compte_sans_augmenter = false)
	{
		static size_t compteur_allocations = 0;
		if(donne_compte_sans_augmenter)
			return compteur_allocations;
		compteur_allocations++;
#ifdef _WINDOWS_
		if(IsDebuggerPresent())
#endif
		{
			for(auto&& break_sur_numero : breakpoint_sur_allocations)
			{
				if(compteur_allocations == break_sur_numero)
				{
					POINT_ARRET(); // Point d'arrêt sur allocation: regarder la "Pile des appels" pour voir où cette allocation a été faite.
				}
			}
		}
		return compteur_allocations;
	}

	MarqueurVerificationAllocation get_marqueur_verification_allocation()
	{
		return compte_allocations(false);
	}

	void* notre_operator_new(size_t sz, bool est_tableau, const char* nom_fichier = nullptr, int ligne_fichier = 0)
	{
		SansVerifierAllocations sva;
		assurer_taille_allocation_possible(sz);
		// On met la vérification de corruption partout, même pour les blocs "sans vérifier allocation", pour ne pas que les adresses soient différentes dans les deux cas.  "sans vérifier allocation" indique donc seulement de ne pas mettre les blocs dans le map des blocs_alloues.
		void* ptr = std::malloc(sz + 2 * taille_verification_corruption);
		if(ptr == nullptr)
			throw std::bad_alloc();
		ptr = pointeur_octets(ptr) + taille_verification_corruption;
		mettre_verification_corruption_sur_allocation(ptr, sz);
		memset(ptr, 0xCC, sz);  // Pour ne pas qu'on puisse supposer que l'allocation initialise à zéro.

		if(!sva.etait_deja_actif() && debogage_allocation_est_actif())
		{
// Attention que l'ajout aux blocs_alloues fait possiblement une allocation, et doit donc absolument être contrôlée pour ne pas faire une récursion infinie.
			get_blocs_alloues()[ptr] = {sz, est_tableau, nom_fichier, ligne_fichier, compte_allocations()};
			compteur_de_new++;
		}

		return ptr;
	}

	static void enlever_des_blocs_alloues(void* ptr, bool est_tableau) noexcept
	{
		SansVerifierAllocations sva;
		if(!sva.etait_deja_actif())
		{
			auto it = get_blocs_alloues().find(ptr);
			if(it == get_blocs_alloues().end())
			{
#ifdef VERIFICATION_DELETE_STRICTE
				return lancer_erreur_delete(SorteErreurDelete::not_allocated);
#else
				return;
#endif
			}

			if(it->second.est_tableau != est_tableau)
				return lancer_erreur_delete(est_tableau ? SorteErreurDelete::wrong_delete_array : SorteErreurDelete::wrong_delete_nonarray, it->second);

			if(!tester_verification_corruption_sur_allocation(ptr, it->second.taille))
				return lancer_erreur_delete(SorteErreurDelete::corruption, it->second);

			memset(ptr, 0xCC, it->second.taille);  // Pour ne pas qu'on puisse relire les données désallouées.
			get_blocs_alloues().erase(it);
			compteur_de_delete++;
		}
		else if(VerifierFuitesAllocations::est_dans_phase_apres_main())
		{
// Dans la phase après main, il peut y avoir des désallocations pour les "static". Le delete d'un bloc vérifié doit l'enlever même s'il est hors vérification, on ne donne juste pas d'erreur.
			if(auto it = get_blocs_alloues().find(ptr); it != get_blocs_alloues().end())
			{
				get_blocs_alloues().erase(it);
				compteur_de_delete++;
			}
		}
	}

	void notre_operator_delete(void* ptr, bool est_tableau) noexcept
	{
		if(ptr == nullptr)
			return;
		enlever_des_blocs_alloues(ptr, est_tableau);

		if(!tester_verification_corruption_sur_allocation(ptr))
			return lancer_erreur_delete(SorteErreurDelete::corruption);

		std::free(pointeur_octets(ptr) - taille_verification_corruption);
	}

	bool InfoBlocMemoire::a_numero_ligne()const
	{
		return ligne_fichier != 0;
	}
	bool InfoBlocMemoire::est_depuis(MarqueurVerificationAllocation depuis) const
	{
		return numero_allocation > depuis;
	}
	bool InfoBlocMemoire::repond_aux_criteres(bool seulement_avec_numeros_ligne, MarqueurVerificationAllocation depuis) const
	{
		return (!seulement_avec_numeros_ligne || a_numero_ligne()) && est_depuis(depuis);
	}

	/// Si seulement_avec_numeros_ligne, vérifie uniquement les blocs qui ont un numéro de ligne, donc qui ont été alloués avec le "new" spécial défini dans debogage_memoire.hpp .
	bool tous_les_new_ont_un_delete(bool seulement_avec_numeros_ligne, MarqueurVerificationAllocation depuis)
	{
		if(!seulement_avec_numeros_ligne && depuis == depuisDebutVerificationAllocation)
			return get_blocs_alloues().empty();

		for(const auto& [ptr, info] : get_blocs_alloues())
		{
			if(info.repond_aux_criteres(seulement_avec_numeros_ligne, depuis))
				return false;
		}
		return true;
	}

	ostream& operator<< (ostream& os, const InfoBlocMemoire& info)
	{
		os << "new" << (info.est_tableau ? "[] " : "   ") << info.taille << " octets";
		if(info.nom_fichier)
			os << ", ligne '" << info.nom_fichier << "':" << info.ligne_fichier;
		os << "  (#" << info.numero_allocation << ")";
		return os;
	}

	/// Si seulement_avec_numeros_ligne, affiche uniquement les blocs qui ont un numéro de ligne, donc qui ont été alloués avec le "new" spécial défini dans debogage_memoire.hpp .
	void dump_blocs_alloues(bool seulement_avec_numeros_ligne, MarqueurVerificationAllocation depuis)
	{
		SansVerifierAllocations sva;
		for(const auto& [ptr, info] : get_blocs_alloues())
			if(info.repond_aux_criteres(seulement_avec_numeros_ligne, depuis))
				cout << ptr << " " << info << endl;
	}

	void afficher_fuites()
	{
		if(tous_les_new_ont_un_delete())
			cout << endl << "Aucune fuite detectee." << endl;
		else
		{
			cout << endl << "Fuite detectee:" << endl;
			dump_blocs_alloues();
			cout << endl << "Pour arreter sur ces # allocations la prochaine execution, ajouter la variable globale suivante (si le programme fait toujours les memes allocations; attention que l'execution dans le debogueur vs sans debogueur peut changer les allocations):" << endl
				<< "bibliotheque_cours::BreakpointSurAllocations breakpointSurAllocations = { ";
			for(const auto& [ptr, info] : get_blocs_alloues())
				cout << info.numero_allocation << "U, ";
			cout << "};" << endl;
		}
	}
	bool tester_tous_blocs_alloues()
	{
		for(const auto& [ptr, info] : get_blocs_alloues())
			if(!tester_verification_corruption_sur_allocation(ptr, info.taille))
				return false;
		return true;
	}

	// Fait comme un singleton, pour s'assurer qu'il existe pour les premiers new globaux avant le main.
	std::unordered_map<void*, InfoBlocMemoire>& get_blocs_alloues()
	{
		static std::unordered_map<void*, InfoBlocMemoire> static_blocs_alloues;
#ifdef VERIFICATION_ALLOCATION_GLOBALE
		static VerifierFuitesAllocations verifierFuitesAllocations(false);
#endif
		return static_blocs_alloues;
	}
}

// Les operator new et delete doivent être hors du namespace.
#ifdef _MSC_VER
__pragma(warning(push)) __pragma(warning(disable:28251 28252 28253))
#endif
void* operator new  (size_t sz)
{
	return bibliotheque_cours::notre_operator_new(sz, false);
}
void* operator new[](size_t sz) { return bibliotheque_cours::notre_operator_new(sz, true);  }
void* operator new  (size_t sz, const char* nom_fichier, int ligne_fichier)
{
	return bibliotheque_cours::notre_operator_new(sz, false, nom_fichier, ligne_fichier);
}
void* operator new[](size_t sz, const char* nom_fichier, int ligne_fichier) { return bibliotheque_cours::notre_operator_new(sz, true, nom_fichier, ligne_fichier);  }

#ifdef _MSC_VER
__pragma(warning(pop))
#endif
void operator delete  (void* ptr) noexcept
{
	bibliotheque_cours::notre_operator_delete(ptr, false);
}
void operator delete[](void* ptr) noexcept { bibliotheque_cours::notre_operator_delete(ptr, true);  }
void operator delete  (void* ptr, size_t) noexcept
{
	bibliotheque_cours::notre_operator_delete(ptr, false);
}  // On pourrait passer le size à notre opérateur pour ajouter un test sur cette taille.
void operator delete[](void* ptr, size_t) noexcept { bibliotheque_cours::notre_operator_delete(ptr, true);  }
void operator delete  (void* ptr, const char*, int) noexcept
{
	bibliotheque_cours::notre_operator_delete(ptr, false);
}
void operator delete[](void* ptr, const char*, int) noexcept { bibliotheque_cours::notre_operator_delete(ptr, true);  }