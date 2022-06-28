#pragma once
/// Ajout des numéros de ligne au rapport de fuites.
/// Pour aller avec verification_allocation.cpp .  Doit être inclus dans tous les fichiers qui font des "new".
/// Malheureusement incompatible avec le "placement new".
/// \author Francois-R.Boyer@PolyMtl.ca
/// \since   2021-01

void* operator new  (size_t sz, const char* nom_fichier, int ligne_fichier);
void* operator new[](size_t sz, const char* nom_fichier, int ligne_fichier);
void operator delete  (void* ptr, const char* nom_fichier, int ligne_fichier) noexcept;
void operator delete[](void* ptr, const char* nom_fichier, int ligne_fichier) noexcept;
#define new new( __FILE__ , __LINE__ )
