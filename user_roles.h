#ifndef USER_ROLES_H
#define USER_ROLES_H

typedef enum {
    PROFIL_NON_DEFINI = 0,
    PROFIL_INFIRMIERE,
    PROFIL_RESPONSABLE_MAGASIN,
    PROFIL_RESPONSABLE_REGIONAL, // Fonctionnalités limitées pour l'instant
    PROFIL_ADMINISTRATEUR // Accès à tout
} ProfilUtilisateur;

extern ProfilUtilisateur profil_actuel; // Déclaration, sera défini dans main.c ou un .c dédié

void choisir_profil_utilisateur();
int peut_acceder_gestion_patients(ProfilUtilisateur profil);
int peut_acceder_gestion_medicaments_complet(ProfilUtilisateur profil); // CRUD complet
int peut_visualiser_medicaments(ProfilUtilisateur profil);
int peut_acceder_gestion_personnel(ProfilUtilisateur profil);
int peut_acceder_gestion_consultations(ProfilUtilisateur profil);
int peut_verifier_alertes(ProfilUtilisateur profil);
int peut_sauvegarder_donnees(ProfilUtilisateur profil);
int responsable_regional_peut_visualiser_medicaments(ProfilUtilisateur profil);
// ... autres fonctions de permission si nécessaire

#endif // USER_ROLES_H
