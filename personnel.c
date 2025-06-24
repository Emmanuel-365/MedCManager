#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "personnel.h"
#include "gestion_medicale.h"
#include "csv_utils.h"

#define MAX_PERSONNEL_EN_MEMOIRE 50
Personnel liste_personnel[MAX_PERSONNEL_EN_MEMOIRE];
int nombre_personnel = 0;
int prochain_id_personnel = 1;

const char* FICHIER_PERSONNEL = "personnel.dat";

void actualiser_prochain_id_personnel() {
    int max_id = 0;
    for (int i = 0; i < nombre_personnel; i++) {
        if (liste_personnel[i].id_personnel > max_id) {
            max_id = liste_personnel[i].id_personnel;
        }
    }
    prochain_id_personnel = max_id + 1;
}

// Fonction pour ajouter un nouveau membre du personnel
// Retourne 1 si succès, 0 si échec
int ajouter_membre_personnel(const char* nom, const char* prenom, const char* poste, const char* contact_info, const char* suivi_medical) {
    if (nombre_personnel >= MAX_PERSONNEL_EN_MEMOIRE) {
        printf("Erreur : Nombre maximum de membres du personnel atteint.\n");
        return 0;
    }

    Personnel nouveau_membre;
    nouveau_membre.id_personnel = prochain_id_personnel++;
    strncpy(nouveau_membre.nom, nom, MAX_NOM_PERSONNEL - 1);
    nouveau_membre.nom[MAX_NOM_PERSONNEL - 1] = '\0';
    strncpy(nouveau_membre.prenom, prenom, MAX_NOM_PERSONNEL - 1);
    nouveau_membre.prenom[MAX_NOM_PERSONNEL - 1] = '\0';
    strncpy(nouveau_membre.poste, poste, MAX_POSTE - 1);
    nouveau_membre.poste[MAX_POSTE - 1] = '\0';
    strncpy(nouveau_membre.contact_info, contact_info, MAX_CONTACT_INFO - 1);
    nouveau_membre.contact_info[MAX_CONTACT_INFO - 1] = '\0';
    strncpy(nouveau_membre.suivi_medical_details, suivi_medical, MAX_SUIVI_MEDICAL_PERSONNEL - 1);
    nouveau_membre.suivi_medical_details[MAX_SUIVI_MEDICAL_PERSONNEL - 1] = '\0';

    liste_personnel[nombre_personnel] = nouveau_membre;
    nombre_personnel++;
    printf("Membre du personnel '%s %s' ajouté avec succès (ID: %d).\n", prenom, nom, nouveau_membre.id_personnel);
    return 1;
}

// Fonction pour afficher tous les membres du personnel
void afficher_tout_le_personnel() {
    if (nombre_personnel == 0) {
        printf("Aucun membre du personnel enregistré.\n");
        return;
    }
    printf("\n--- Liste du Personnel ---\n");
    for (int i = 0; i < nombre_personnel; i++) {
        printf("ID: %d\n", liste_personnel[i].id_personnel);
        printf("  Nom: %s\n", liste_personnel[i].nom);
        printf("  Prénom: %s\n", liste_personnel[i].prenom);
        printf("  Poste: %s\n", liste_personnel[i].poste);
        printf("  Contact: %s\n", liste_personnel[i].contact_info);
        printf("  Suivi Médical: %s\n", liste_personnel[i].suivi_medical_details);
        printf("--------------------------\n");
    }
}

// Fonction pour rechercher un membre du personnel par ID
Personnel* rechercher_personnel_par_id(int id_pers) {
    for (int i = 0; i < nombre_personnel; i++) {
        if (liste_personnel[i].id_personnel == id_pers) {
            return &liste_personnel[i];
        }
    }
    return NULL;
}

// Fonction pour rechercher des membres du personnel par nom (recherche partielle)
void rechercher_personnel_par_nom(const char* nom_recherche) {
    int trouves = 0;
    printf("\n--- Résultats de la recherche de personnel pour '%s' ---\n", nom_recherche);
    for (int i = 0; i < nombre_personnel; i++) {
        if (strstr(liste_personnel[i].nom, nom_recherche) != NULL ||
            strstr(liste_personnel[i].prenom, nom_recherche) != NULL) {
            printf("ID: %d, Nom: %s, Prénom: %s, Poste: %s\n",
                   liste_personnel[i].id_personnel,
                   liste_personnel[i].nom,
                   liste_personnel[i].prenom,
                   liste_personnel[i].poste);
            trouves++;
        }
    }
    if (trouves == 0) {
        printf("Aucun membre du personnel trouvé correspondant à '%s'.\n", nom_recherche);
    }
    printf("---------------------------------------------------\n");
}

// Fonction pour modifier les informations d'un membre du personnel
// Retourne 1 si succès, 0 si non trouvé
int modifier_personnel(int id_pers, const char* nouv_nom, const char* nouv_prenom, const char* nouv_poste, const char* nouv_contact, const char* nouv_suivi_medical) {
    Personnel* pers = rechercher_personnel_par_id(id_pers);
    if (pers == NULL) {
        printf("Erreur : Membre du personnel avec ID %d non trouvé.\n", id_pers);
        return 0;
    }

    if (nouv_nom && strlen(nouv_nom) > 0) {
        strncpy(pers->nom, nouv_nom, MAX_NOM_PERSONNEL - 1);
        pers->nom[MAX_NOM_PERSONNEL - 1] = '\0';
    }
    if (nouv_prenom && strlen(nouv_prenom) > 0) {
        strncpy(pers->prenom, nouv_prenom, MAX_NOM_PERSONNEL - 1);
        pers->prenom[MAX_NOM_PERSONNEL - 1] = '\0';
    }
    if (nouv_poste && strlen(nouv_poste) > 0) {
        strncpy(pers->poste, nouv_poste, MAX_POSTE - 1);
        pers->poste[MAX_POSTE - 1] = '\0';
    }
    if (nouv_contact && strlen(nouv_contact) > 0) {
        strncpy(pers->contact_info, nouv_contact, MAX_CONTACT_INFO - 1);
        pers->contact_info[MAX_CONTACT_INFO - 1] = '\0';
    }
    if (nouv_suivi_medical && strlen(nouv_suivi_medical) > 0) {
        strncpy(pers->suivi_medical_details, nouv_suivi_medical, MAX_SUIVI_MEDICAL_PERSONNEL - 1);
        pers->suivi_medical_details[MAX_SUIVI_MEDICAL_PERSONNEL - 1] = '\0';
    }

    printf("Informations du membre du personnel ID %d modifiées.\n", id_pers);
    return 1;
}


// --- Gestion Fichiers CSV ---

// Sauvegarde le personnel dans un fichier CSV.
// Format: id_personnel,nom,prenom,poste,contact_info,suivi_medical_details
void sauvegarder_personnel_fichier() {
    FILE* fichier = fopen(FICHIER_PERSONNEL, "w");
    if (fichier == NULL) {
        perror("Erreur ouverture fichier personnel.dat pour sauvegarde");
        return;
    }

    for (int i = 0; i < nombre_personnel; i++) {
        ecrire_champ_csv_int(fichier, liste_personnel[i].id_personnel);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_personnel[i].nom);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_personnel[i].prenom);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_personnel[i].poste);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_personnel[i].contact_info);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_personnel[i].suivi_medical_details);
        fprintf(fichier, "\n");
    }
    fclose(fichier);
    printf("%d membres du personnel sauvegardés (CSV) dans %s\n", nombre_personnel, FICHIER_PERSONNEL);
}

// Charge le personnel depuis un fichier CSV.
void charger_personnel_fichier() {
    FILE* fichier = fopen(FICHIER_PERSONNEL, "r");
    if (fichier == NULL) {
        printf("Fichier %s non trouvé. Aucun personnel chargé.\n", FICHIER_PERSONNEL);
        return;
    }

    char ligne[1024]; // Buffer assez grand
    nombre_personnel = 0;

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (nombre_personnel >= MAX_PERSONNEL_EN_MEMOIRE) {
            fprintf(stderr, "Max personnel en mémoire atteint lors du chargement CSV.\n");
            break;
        }

        Personnel p;
        int index_ligne = 0;
        char temp_champ[MAX_SUIVI_MEDICAL_PERSONNEL]; // Pour le plus grand champ

        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        p.id_personnel = atoi(temp_champ);

        if (!lire_champ_csv(ligne, &index_ligne, p.nom, MAX_NOM_PERSONNEL)) continue;
        if (!lire_champ_csv(ligne, &index_ligne, p.prenom, MAX_NOM_PERSONNEL)) continue;
        if (!lire_champ_csv(ligne, &index_ligne, p.poste, MAX_POSTE)) continue;
        if (!lire_champ_csv(ligne, &index_ligne, p.contact_info, MAX_CONTACT_INFO)) continue;
        if (!lire_champ_csv(ligne, &index_ligne, p.suivi_medical_details, MAX_SUIVI_MEDICAL_PERSONNEL)) continue;

        if (p.id_personnel > 0) {
            liste_personnel[nombre_personnel++] = p;
        } else {
            fprintf(stderr, "Membre du personnel ID %d invalide ignoré (CSV).\n", p.id_personnel);
        }
    }
    fclose(fichier);
    actualiser_prochain_id_personnel();
    printf("%d membres du personnel chargés (CSV) depuis %s\n", nombre_personnel, FICHIER_PERSONNEL);
}

/*
// Exemple d'utilisation
void test_personnel() {
    printf("\n--- Test Gestion Personnel ---\n");
    ajouter_membre_personnel("Lefevre", "Alice", "Infirmière", "alice.lefevre@email.com", "Apte, vaccin à jour");
    ajouter_membre_personnel("Bernard", "Luc", "Responsable Magasin", "0612345678", "Apte");

    afficher_tout_le_personnel();

    rechercher_personnel_par_nom("Lef");

    Personnel* p = rechercher_personnel_par_id(1);
    if (p) {
        printf("Personnel ID 1: %s %s\n", p->prenom, p->nom);
        modifier_personnel(1, NULL, NULL, "Infirmière en chef", NULL, "Apte, vaccin à jour, responsable planning");
        printf("Après MAJ Poste: %s\n", p->poste);
    }
    afficher_tout_le_personnel();
    printf("--- Fin Test Gestion Personnel ---\n");
}
*/
