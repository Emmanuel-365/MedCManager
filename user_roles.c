#include "user_roles.h"
#include <stdio.h>

ProfilUtilisateur profil_actuel = PROFIL_NON_DEFINI;

void choisir_profil_utilisateur() {
    int choix = 0;
    printf("\n--- Choix du Profil Utilisateur ---\n");
    printf("1. Infirmière / Infirmier\n");
    printf("2. Responsable Magasin\n");
    printf("3. Responsable Régional\n");
    printf("4. Administrateur\n");
    printf("Votre choix : ");

    while (scanf("%d", &choix) != 1 || choix < 1 || choix > 4) {
        printf("Choix invalide. Veuillez entrer un numéro entre 1 et 4.\n");
        while (getchar() != '\n'); // Vider buffer
        printf("Votre choix : ");
    }
    while (getchar() != '\n'); // Vider buffer

    switch (choix) {
        case 1: profil_actuel = PROFIL_INFIRMIERE; printf("Profil Infirmière sélectionné.\n"); break;
        case 2: profil_actuel = PROFIL_RESPONSABLE_MAGASIN; printf("Profil Responsable Magasin sélectionné.\n"); break;
        case 3: profil_actuel = PROFIL_RESPONSABLE_REGIONAL; printf("Profil Responsable Régional sélectionné.\n"); break;
        case 4: profil_actuel = PROFIL_ADMINISTRATEUR; printf("Profil Administrateur sélectionné.\n"); break;
        default: profil_actuel = PROFIL_NON_DEFINI; // Ne devrait pas arriver
    }
}

// --- Fonctions de Permissions ---
// Ces fonctions déterminent ce que chaque profil peut voir/faire via le menu.

int peut_acceder_gestion_patients(ProfilUtilisateur profil) {
    return (profil == PROFIL_INFIRMIERE || profil == PROFIL_ADMINISTRATEUR);
}

int peut_acceder_gestion_medicaments_complet(ProfilUtilisateur profil) {
    return (profil == PROFIL_RESPONSABLE_MAGASIN || profil == PROFIL_ADMINISTRATEUR);
}

int peut_visualiser_medicaments(ProfilUtilisateur profil) {
    // Infirmière peut visualiser (pour prescrire), Responsable Magasin et Admin aussi.
    return (profil == PROFIL_INFIRMIERE || profil == PROFIL_RESPONSABLE_MAGASIN || profil == PROFIL_ADMINISTRATEUR);
}

int peut_acceder_gestion_personnel(ProfilUtilisateur profil) {
    // Pour l'instant, seul l'admin. Pourrait être étendu.
    return (profil == PROFIL_ADMINISTRATEUR);
}

int peut_acceder_gestion_consultations(ProfilUtilisateur profil) {
    return (profil == PROFIL_INFIRMIERE || profil == PROFIL_ADMINISTRATEUR);
}

int peut_verifier_alertes(ProfilUtilisateur profil) {
    return (profil == PROFIL_INFIRMIERE || profil == PROFIL_RESPONSABLE_MAGASIN || profil == PROFIL_ADMINISTRATEUR);
}

int peut_sauvegarder_donnees(ProfilUtilisateur profil) {
    // Typiquement une tâche d'admin ou fait automatiquement à la sortie pour tous.
    // Pour la sauvegarde manuelle, on peut la restreindre.
    return (profil == PROFIL_ADMINISTRATEUR || profil == PROFIL_RESPONSABLE_MAGASIN); // Exemple
}

// Le responsable régional n'a pas de fonctionnalités spécifiques pour l'instant.
// On pourrait lui donner un accès en lecture seule à certaines choses ou un menu vide.
// Par exemple, il pourrait visualiser les stocks de médicaments.
int responsable_regional_peut_visualiser_medicaments(ProfilUtilisateur profil) {
    return (profil == PROFIL_RESPONSABLE_REGIONAL);
}
