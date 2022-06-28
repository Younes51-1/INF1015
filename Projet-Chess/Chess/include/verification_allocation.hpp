#pragma once
/// Verification des fuites de mémoire.
/// Utilise un unordered_map pour conserver toutes les allocations, ceci a évidemment un impact sur la vitesse d'exécution, donc on ne l'utilise normalement pas sur un code final mais plutôt durant la vérification d'un programme.
/// \author Francois-R.Boyer@PolyMtl.ca
/// \version 2021-01
/// \since   2020-04

#include <unordered_map>

#define VERIFICATION_ALLOCATION_INCLUS
#define VERIFICATION_ALLOCATION_GLOBALE

namespace bibliotheque_cours {

using MarqueurVerificationAllocation = size_t;
static constexpr MarqueurVerificationAllocation depuisDebutVerificationAllocation = 0;

struct InfoBlocMemoire {
	size_t taille;
	bool est_tableau;
	const char* nom_fichier; int ligne_fichier;
	size_t numero_allocation;

	bool a_numero_ligne() const;
	bool est_depuis(MarqueurVerificationAllocation depuis) const;
	bool repond_aux_criteres(bool seulement_avec_numeros_ligne, MarqueurVerificationAllocation depuis) const;
};

enum class SorteErreurDelete {
	no_error, not_allocated, wrong_delete_array, wrong_delete_nonarray, corruption, last = corruption
};

class SansVerifierAllocations {
public:
	SansVerifierAllocations() : deja_actif(est_actif) { est_actif = true; }
	~SansVerifierAllocations() { if (!deja_actif) est_actif = false; }
	bool etait_deja_actif() { return deja_actif; }

	friend void activer_verification_allocation();
	friend void desactiver_verification_allocation();

private:
	bool deja_actif;

#ifdef VERIFICATION_ALLOCATION_GLOBALE
	inline static bool est_actif = false;
#else
	inline static bool est_actif = true;
#endif
};

const char* get_message_erreur_delete();
void remise_a_zero_compteurs_allocation();
void remise_a_zero_verification();
void activer_verification_allocation();
void desactiver_verification_allocation();
void afficher_fuites();

bool tous_les_new_ont_un_delete(bool seulement_avec_numeros_ligne = false, MarqueurVerificationAllocation depuis = depuisDebutVerificationAllocation);
void dump_blocs_alloues(bool seulement_avec_numeros_ligne = false, MarqueurVerificationAllocation depuis = depuisDebutVerificationAllocation);
bool tester_tous_blocs_alloues();

MarqueurVerificationAllocation get_marqueur_verification_allocation();


//void remplir_bloc_verification_corruption_a(void* ptr, size_t sz);
//bool tester_bloc_verification_corruption_a(void* ptr);
//char* pointeur_octets(void* p);
//const char* pointeur_octets(const void* p);
//void* notre_operator_new(size_t sz, bool est_tableau);
//void notre_operator_delete(void* ptr, bool est_tableau) noexcept;


/// Devrait être instaciée une seule fois au début du "main" pour vérifier les fuites du programme (excluant les allocations faites avant l'exécution du "main").
class VerifierFuitesAllocations {
public:
	VerifierFuitesAllocations(bool doitActiver = true) {
		static VerifierALaFin verifierALaFin;
		if (doitActiver)
			activer_verification_allocation();
	}
	// On désactive à la fin du main, pour ne pas qu'un delete hors main donne une erreur comme quoi il n'a pas de new associé, mais on affiche les fuites seulement plus tard.
	~VerifierFuitesAllocations() {
		est_phase_apres_main = true;
		desactiver_verification_allocation();
	}
	static bool est_dans_phase_apres_main() { return est_phase_apres_main; }

private:
	struct VerifierALaFin { ~VerifierALaFin() {
		afficher_fuites();
		VerifierFuitesAllocations::est_phase_apres_main = false;
	} };
	static inline bool est_phase_apres_main = false;
};

void set_breakpoint_sur_allocations(std::size_t* numeros, std::size_t nNumeros);

template <std::size_t N>
class BreakpointSurAllocations {
public:
	template <typename... Ts>
	BreakpointSurAllocations(Ts... numeros) : numeros_{numeros...} { set_breakpoint_sur_allocations(numeros_, N); }
	~BreakpointSurAllocations() { set_breakpoint_sur_allocations(nullptr, 0); }
private:
	std::size_t numeros_[N];
};
template <typename... Ts>
BreakpointSurAllocations(Ts...) -> BreakpointSurAllocations<sizeof...(Ts)>;

std::unordered_map<void*, InfoBlocMemoire>& get_blocs_alloues();
// Utiliser get_blocs_alloues au lieu de la référence dans les opérateurs new/delete qui pourraient être appelés avant l'initialisation de cette variable.
inline std::unordered_map<void*, InfoBlocMemoire>& blocs_alloues = get_blocs_alloues();
extern SorteErreurDelete derniere_erreur_delete;
extern size_t compteur_de_new, compteur_de_delete;
extern bool desactive_terminate_sur_erreur_delete;

}

// Les operator new et delete doivent être hors du namespace.

//void* operator new  (size_t sz);
//void* operator new[](size_t sz);
//void operator delete  (void* ptr) noexcept;
//void operator delete[](void* ptr) noexcept;
