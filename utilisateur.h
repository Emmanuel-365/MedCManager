#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include "user_roles.h" // Pour ProfilUtilisateur

#define MAX_LOGIN_SIZE 50
#define HASH_SIZE 65 // 64 caractères + nul

typedef struct {
    int id_utilisateur;
    char login[MAX_LOGIN_SIZE];
    char mot_de_passe_hash[HASH_SIZE]; // Pour stocker le hash du mot de passe
    ProfilUtilisateur profil;
} Utilisateur;

// Fonction de hachage simple (NON SÉCURISÉE POUR PRODUCTION)
void hasher_mot_de_passe_simple(const char* mot_de_passe, char* hash_resultat);

// Gestion des utilisateurs
extern int prochain_id_utilisateur;
void initialiser_utilisateurs(); // Charge depuis fichier ou crée admin par défaut
int ajouter_utilisateur_interne(const char* login, const char* mot_de_passe, ProfilUtilisateur profil, int charger_depuis_fichier); // Interne pour gérer hash
int ajouter_utilisateur_par_admin(const char* login, const char* mot_de_passe, ProfilUtilisateur profil);
Utilisateur* rechercher_utilisateur_par_login(const char* login);
int verifier_mot_de_passe(const Utilisateur* utilisateur, const char* mot_de_passe_clair);
ProfilUtilisateur login_utilisateur(); // Retourne le profil de l'utilisateur connecté, ou PROFIL_NON_DEFINI

void sauvegarder_utilisateurs_fichier();
void charger_utilisateurs_fichier();

void afficher_tous_les_utilisateurs(); // Pour l'admin
int supprimer_utilisateur_par_login(const char* login); // Pour l'admin
int modifier_profil_utilisateur(const char* login, ProfilUtilisateur nouveau_profil); // Pour l'admin
int changer_mot_de_passe_par_admin(const char* login, const char* nouveau_mot_de_passe); // Pour l'admin

#endif // UTILISATEUR_H
