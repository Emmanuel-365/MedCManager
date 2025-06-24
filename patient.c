#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Pour strncpy, strcspn, atoi, etc.
#include "patient.h"
#include "gestion_medicale.h" // Pourrait contenir des utilitaires plus tard
#include "csv_utils.h"      // Pour les fonctions de manipulation CSV

// Pour l'instant, nous allons utiliser un tableau global pour stocker les patients.
// Ce sera amélioré avec une allocation dynamique plus robuste et une persistance fichier.
#define MAX_PATIENTS_EN_MEMOIRE 100 // Capacité initiale, à gérer dynamiquement plus tard
Patient liste_patients[MAX_PATIENTS_EN_MEMOIRE];
int nombre_patients = 0;
int prochain_id_patient = 1; // Simple compteur pour les IDs uniques

const char* FICHIER_PATIENTS = "patients.dat";

// Fonction utilitaire pour s'assurer que prochain_id_patient est toujours supérieur aux ID existants
void actualiser_prochain_id_patient() {
    int max_id = 0;
    for (int i = 0; i < nombre_patients; i++) {
        if (liste_patients[i].id_patient > max_id) {
            max_id = liste_patients[i].id_patient;
        }
    }
    prochain_id_patient = max_id + 1;
}

// Fonction pour ajouter un nouveau patient
// Retourne 1 si succès, 0 si échec (ex: liste pleine)
int ajouter_patient(const char* nom, const char* prenom, int age, const char* service, const char* dossier_medical) {
    if (nombre_patients >= MAX_PATIENTS_EN_MEMOIRE) {
        printf("Erreur : Nombre maximum de patients atteint.\n");
        return 0; // Échec
    }

    Patient nouveau_patient;
    nouveau_patient.id_patient = prochain_id_patient++;
    strncpy(nouveau_patient.nom, nom, MAX_NOM_PRENOM - 1);
    nouveau_patient.nom[MAX_NOM_PRENOM - 1] = '\0'; // Assurer la terminaison null
    strncpy(nouveau_patient.prenom, prenom, MAX_NOM_PRENOM - 1);
    nouveau_patient.prenom[MAX_NOM_PRENOM - 1] = '\0';
    nouveau_patient.age = age;
    strncpy(nouveau_patient.service, service, MAX_SERVICE - 1);
    nouveau_patient.service[MAX_SERVICE - 1] = '\0';
    strncpy(nouveau_patient.dossier_medical, dossier_medical, MAX_DOSSIER - 1);
    nouveau_patient.dossier_medical[MAX_DOSSIER - 1] = '\0';

    liste_patients[nombre_patients] = nouveau_patient;
    nombre_patients++;
    printf("Patient '%s %s' ajouté avec succès (ID: %d).\n", prenom, nom, nouveau_patient.id_patient);
    return 1; // Succès
}

// Fonction pour afficher tous les patients
void afficher_tous_les_patients() {
    if (nombre_patients == 0) {
        printf("Aucun patient enregistré.\n");
        return;
    }
    printf("\n--- Liste des Patients ---\n");
    for (int i = 0; i < nombre_patients; i++) {
        printf("ID: %d\n", liste_patients[i].id_patient);
        printf("  Nom: %s\n", liste_patients[i].nom);
        printf("  Prénom: %s\n", liste_patients[i].prenom);
        printf("  Âge: %d ans\n", liste_patients[i].age);
        printf("  Service: %s\n", liste_patients[i].service);
        printf("  Dossier Médical: %s\n", liste_patients[i].dossier_medical);
        printf("--------------------------\n");
    }
}

// Fonction pour rechercher un patient par ID
// Retourne un pointeur vers le patient si trouvé, NULL sinon
Patient* rechercher_patient_par_id(int id_patient) {
    for (int i = 0; i < nombre_patients; i++) {
        if (liste_patients[i].id_patient == id_patient) {
            return &liste_patients[i];
        }
    }
    return NULL; // Non trouvé
}

// Fonction pour rechercher des patients par nom (recherche partielle)
// Affiche les patients correspondants
void rechercher_patient_par_nom(const char* nom_recherche) {
    int trouves = 0;
    printf("\n--- Résultats de la recherche pour '%s' ---\n", nom_recherche);
    for (int i = 0; i < nombre_patients; i++) {
        // strstr recherche la première occurrence d'une sous-chaîne
        if (strstr(liste_patients[i].nom, nom_recherche) != NULL ||
            strstr(liste_patients[i].prenom, nom_recherche) != NULL) {
            printf("ID: %d, Nom: %s, Prénom: %s, Âge: %d, Service: %s\n",
                   liste_patients[i].id_patient,
                   liste_patients[i].nom,
                   liste_patients[i].prenom,
                   liste_patients[i].age,
                   liste_patients[i].service);
            trouves++;
        }
    }
    if (trouves == 0) {
        printf("Aucun patient trouvé correspondant à '%s'.\n", nom_recherche);
    }
    printf("---------------------------------------\n");
}


// Fonction pour modifier les informations d'un patient
// Retourne 1 si succès, 0 si patient non trouvé
int modifier_patient(int id_patient, const char* nouveau_nom, const char* nouveau_prenom, int nouvel_age, const char* nouveau_service, const char* nouveau_dossier) {
    Patient* patient_a_modifier = rechercher_patient_par_id(id_patient);
    if (patient_a_modifier == NULL) {
        printf("Erreur : Patient avec ID %d non trouvé.\n", id_patient);
        return 0; // Non trouvé
    }

    if (nouveau_nom) {
      strncpy(patient_a_modifier->nom, nouveau_nom, MAX_NOM_PRENOM - 1);
      patient_a_modifier->nom[MAX_NOM_PRENOM - 1] = '\0';
    }
    if (nouveau_prenom) {
      strncpy(patient_a_modifier->prenom, nouveau_prenom, MAX_NOM_PRENOM - 1);
      patient_a_modifier->prenom[MAX_NOM_PRENOM - 1] = '\0';
    }
    if (nouvel_age > 0) { // On suppose qu'un âge valide est positif
        patient_a_modifier->age = nouvel_age;
    }
    if (nouveau_service) {
      strncpy(patient_a_modifier->service, nouveau_service, MAX_SERVICE - 1);
      patient_a_modifier->service[MAX_SERVICE - 1] = '\0';
    }
    if (nouveau_dossier) {
      strncpy(patient_a_modifier->dossier_medical, nouveau_dossier, MAX_DOSSIER - 1);
      patient_a_modifier->dossier_medical[MAX_DOSSIER - 1] = '\0';
    }

    printf("Informations du patient ID %d modifiées avec succès.\n", id_patient);
    return 1; // Succès
}

// Une fonction de suppression pourrait être ajoutée ici.
// Pour l'instant, on va se concentrer sur Ajout, Recherche, Modification, Affichage.
// La suppression impliquerait de décaler les éléments dans le tableau ou de marquer comme "supprimé".
// int supprimer_patient(int id_patient) { ... }


// --- Gestion Fichiers ---

// Sauvegarde les patients dans un fichier CSV.
// Format: id_patient,nom,prenom,age,service,dossier_medical
void sauvegarder_patients_fichier() {
    FILE* fichier = fopen(FICHIER_PATIENTS, "w");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier patients.dat pour sauvegarde");
        return;
    }

    // Optionnel: écrire un en-tête CSV (peut aider pour la lisibilité ou l'import)
    // fprintf(fichier, "id_patient,nom,prenom,age,service,dossier_medical\n");

    for (int i = 0; i < nombre_patients; i++) {
        ecrire_champ_csv_int(fichier, liste_patients[i].id_patient);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_patients[i].nom);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_patients[i].prenom);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_patients[i].age);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_patients[i].service);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_patients[i].dossier_medical);
        fprintf(fichier, "\n");
    }

    fclose(fichier);
    printf("%d patients sauvegardés au format CSV dans %s\n", nombre_patients, FICHIER_PATIENTS);
}

// Charge les patients depuis un fichier CSV.
void charger_patients_fichier() {
    FILE* fichier = fopen(FICHIER_PATIENTS, "r");
    if (fichier == NULL) {
        printf("Fichier %s non trouvé. Aucun patient chargé (normal au premier lancement).\n", FICHIER_PATIENTS);
        return;
    }

    char ligne[1024]; // Buffer pour lire une ligne entière du CSV
    nombre_patients = 0; // Réinitialiser avant de charger

    // Optionnel: si on a écrit un en-tête, il faut le sauter
    // if (fgets(ligne, sizeof(ligne), fichier) == NULL && nombre_patients == 0) { /* gérer erreur ou fichier vide */ }


    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (nombre_patients >= MAX_PATIENTS_EN_MEMOIRE) {
            fprintf(stderr, "Attention : MAX_PATIENTS_EN_MEMOIRE atteint lors du chargement CSV.\n");
            break;
        }

        Patient p;
        int index_ligne = 0;
        char temp_champ[MAX_DOSSIER]; // Assez grand pour le plus grand champ texte, ou MAX_NOM_PRENOM etc.

        // 1. id_patient (int)
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue; // Ligne vide ou malformée
        p.id_patient = atoi(temp_champ);

        // 2. nom (string)
        if (!lire_champ_csv(ligne, &index_ligne, p.nom, MAX_NOM_PRENOM)) continue;

        // 3. prenom (string)
        if (!lire_champ_csv(ligne, &index_ligne, p.prenom, MAX_NOM_PRENOM)) continue;

        // 4. age (int)
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        p.age = atoi(temp_champ);
        if (p.age < 0) p.age = 0; // Validation simple

        // 5. service (string)
        if (!lire_champ_csv(ligne, &index_ligne, p.service, MAX_SERVICE)) continue;

        // 6. dossier_medical (string)
        // Pour le dernier champ, lire_champ_csv doit gérer correctement la fin de ligne
        if (!lire_champ_csv(ligne, &index_ligne, p.dossier_medical, MAX_DOSSIER)) {
            // Si le dernier champ est vide et lire_champ_csv ne le gère pas comme "lu", il faut ajuster
            // Mais si la ligne est juste "1,Nom,Prenom,30,Service," (dossier vide), lire_champ_csv devrait retourner 1 avec champ vide.
            // Si le dossier est le dernier champ et n'est pas entre guillemets et vide, il faut un ajustement.
            // La version actuelle de lire_champ_csv devrait retourner un champ vide.
        }


        // Vérifier si l'ID est valide (non nul, etc., si nécessaire)
        if (p.id_patient > 0) { // Simple vérification, un ID 0 pourrait être problématique
            liste_patients[nombre_patients++] = p;
        } else {
            fprintf(stderr, "Patient avec ID invalide (%d) ignoré lors du chargement CSV.\n", p.id_patient);
        }
    }

    fclose(fichier);
    actualiser_prochain_id_patient(); // Mettre à jour le compteur d'ID
    printf("%d patients chargés depuis le CSV %s\n", nombre_patients, FICHIER_PATIENTS);
}


/*
// Exemple d'utilisation (à mettre dans un main.c pour tester)
int main() {
    printf("Début du test de gestion des patients.\n");

    ajouter_patient("Dupont", "Jean", 35, "Cardiologie", "Angine de poitrine stable.");
    ajouter_patient("Martin", "Sophie", 28, "Pédiatrie", "Contrôle annuel, RAS.");
    ajouter_patient("Durand", "Pierre", 50, "Cardiologie", "Post-opératoire pontage.");

    afficher_tous_les_patients();

    printf("\nRecherche du patient ID 2:\n");
    Patient* p = rechercher_patient_par_id(2);
    if (p) {
        printf("Trouvé: %s %s, Service: %s\n", p->prenom, p->nom, p->service);
    } else {
        printf("Patient ID 2 non trouvé.\n");
    }

    printf("\nRecherche du patient ID 99 (inexistant):\n");
    p = rechercher_patient_par_id(99);
    if (p) {
        printf("Trouvé: %s %s\n", p->prenom, p->nom);
    } else {
        printf("Patient ID 99 non trouvé.\n");
    }

    printf("\nRecherche de patients par nom 'Du':\n");
    rechercher_patient_par_nom("Du");

    printf("\nModification du patient ID 1:\n");
    modifier_patient(1, "Dupond", "Jean-Claude", 36, "Cardiologie Avancée", "Angine de poitrine stable, suivi traitement X.");

    p = rechercher_patient_par_id(1);
    if (p) {
        printf("Après modif: ID: %d, Nom: %s, Prénom: %s, Âge: %d, Service: %s, Dossier: %s\n",
               p->id_patient, p->nom, p->prenom, p->age, p->service, p->dossier_medical);
    }

    afficher_tous_les_patients(); // Pour voir toutes les modifications

    printf("\nFin du test de gestion des patients.\n");
    return 0;
}
*/
