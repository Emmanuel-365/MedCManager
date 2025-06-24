#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "consultation.h"
#include "gestion_medicale.h" // Pour patient.h, personnel.h
#include "csv_utils.h"      // Pour les fonctions CSV

// Fonctions externes pour vérifier l'existence d'un patient/personnel
extern Patient* rechercher_patient_par_id(int id_patient);
extern Personnel* rechercher_personnel_par_id(int id_personnel);


#define MAX_CONSULTATIONS_EN_MEMOIRE 200
Consultation liste_consultations[MAX_CONSULTATIONS_EN_MEMOIRE];
int nombre_consultations = 0;
int prochain_id_consultation = 1;

const char* FICHIER_CONSULTATIONS = "consultations.dat";

void actualiser_prochain_id_consultation() {
    int max_id = 0;
    for (int i = 0; i < nombre_consultations; i++) {
        if (liste_consultations[i].id_consultation > max_id) {
            max_id = liste_consultations[i].id_consultation;
        }
    }
    prochain_id_consultation = max_id + 1;
}

// Fonction utilitaire pour afficher une date de consultation
void afficher_date_consult(DateConsultation d) {
    printf("%02d/%02d/%04d", d.jour, d.mois, d.annee);
}

// Fonction utilitaire pour afficher une heure de consultation
void afficher_heure_consult(Heure h) {
    printf("%02d:%02d", h.heure, h.minute);
}

// Fonction pour enregistrer une nouvelle consultation
// Retourne 1 si succès, 0 si échec (ex: patient/personnel non trouvé, liste pleine)
int enregistrer_consultation(DateConsultation date, Heure heure_arrivee, Heure heure_depart, const char* motif, int id_patient, int id_personnel) {
    if (nombre_consultations >= MAX_CONSULTATIONS_EN_MEMOIRE) {
        printf("Erreur : Nombre maximum de consultations atteint.\n");
        return 0;
    }

    // Vérifier si le patient et le personnel existent
    if (rechercher_patient_par_id(id_patient) == NULL) {
        printf("Erreur : Patient avec ID %d non trouvé. Impossible d'enregistrer la consultation.\n", id_patient);
        return 0;
    }
    if (rechercher_personnel_par_id(id_personnel) == NULL) {
        printf("Erreur : Membre du personnel avec ID %d non trouvé. Impossible d'enregistrer la consultation.\n", id_personnel);
        return 0;
    }

    // Validation simple des heures (heure_depart >= heure_arrivee si même jour)
    // Une validation plus poussée serait nécessaire pour des cas complexes.
    if (date.jour > 0 && date.mois > 0 && date.annee > 0 && /* autres validations de date */
        heure_arrivee.heure >= 0 && heure_arrivee.heure < 24 && heure_arrivee.minute >= 0 && heure_arrivee.minute < 60 &&
        heure_depart.heure >= 0 && heure_depart.heure < 24 && heure_depart.minute >= 0 && heure_depart.minute < 60) {

        if (heure_depart.heure < heure_arrivee.heure || (heure_depart.heure == heure_arrivee.heure && heure_depart.minute < heure_arrivee.minute)) {
            printf("Avertissement : L'heure de départ est antérieure à l'heure d'arrivée.\n");
            // On pourrait choisir de bloquer ici ou juste avertir. Pour l'instant on avertit.
        }

        Consultation nouvelle_consultation;
        nouvelle_consultation.id_consultation = prochain_id_consultation++;
        nouvelle_consultation.date_consultation = date;
        nouvelle_consultation.heure_arrivee = heure_arrivee;
        nouvelle_consultation.heure_depart = heure_depart;
        strncpy(nouvelle_consultation.motif, motif, MAX_MOTIF_CONSULTATION - 1);
        nouvelle_consultation.motif[MAX_MOTIF_CONSULTATION - 1] = '\0';
        nouvelle_consultation.id_patient = id_patient;
        nouvelle_consultation.id_personnel_soignant = id_personnel;

        liste_consultations[nombre_consultations] = nouvelle_consultation;
        nombre_consultations++;
        printf("Consultation (ID: %d) pour patient ID %d avec personnel ID %d enregistrée.\n",
               nouvelle_consultation.id_consultation, id_patient, id_personnel);
        return 1;
    } else {
        printf("Erreur de validation des dates/heures pour la consultation.\n");
        return 0;
    }
}

// Fonction pour afficher toutes les consultations
void afficher_toutes_les_consultations() {
    if (nombre_consultations == 0) {
        printf("Aucune consultation enregistrée.\n");
        return;
    }
    printf("\n--- Liste des Consultations ---\n");
    for (int i = 0; i < nombre_consultations; i++) {
        printf("ID Consultation: %d\n", liste_consultations[i].id_consultation);
        printf("  Date: "); afficher_date_consult(liste_consultations[i].date_consultation); printf("\n");
        printf("  Arrivée: "); afficher_heure_consult(liste_consultations[i].heure_arrivee);
        printf("  Départ: "); afficher_heure_consult(liste_consultations[i].heure_depart); printf("\n");
        printf("  Motif: %s\n", liste_consultations[i].motif);
        printf("  ID Patient: %d\n", liste_consultations[i].id_patient);
        // On pourrait afficher le nom du patient en le recherchant
        Patient* p = rechercher_patient_par_id(liste_consultations[i].id_patient);
        if (p) printf("    (Nom Patient: %s %s)\n", p->prenom, p->nom);

        printf("  ID Personnel Soignant: %d\n", liste_consultations[i].id_personnel_soignant);
        Personnel* pers = rechercher_personnel_par_id(liste_consultations[i].id_personnel_soignant);
        if (pers) printf("    (Nom Personnel: %s %s)\n", pers->prenom, pers->nom);
        printf("------------------------------\n");
    }
}

// Fonction pour rechercher des consultations par ID Patient
void rechercher_consultations_par_patient(int id_patient) {
    int trouves = 0;
    printf("\n--- Consultations pour Patient ID %d ---\n", id_patient);
    Patient* p_info = rechercher_patient_par_id(id_patient);
    if (p_info) {
         printf("Patient: %s %s\n", p_info->prenom, p_info->nom);
    } else {
        printf("Patient ID %d non trouvé.\n", id_patient);
        return;
    }

    for (int i = 0; i < nombre_consultations; i++) {
        if (liste_consultations[i].id_patient == id_patient) {
            printf("ID Cons: %d, Date: ", liste_consultations[i].id_consultation);
            afficher_date_consult(liste_consultations[i].date_consultation);
            printf(", Arrivée: "); afficher_heure_consult(liste_consultations[i].heure_arrivee);
            printf(", Départ: "); afficher_heure_consult(liste_consultations[i].heure_depart);
            printf(", Motif: %s, ID Pers: %d\n", liste_consultations[i].motif, liste_consultations[i].id_personnel_soignant);
            trouves++;
        }
    }
    if (trouves == 0) {
        printf("Aucune consultation trouvée pour ce patient.\n");
    }
    printf("-------------------------------------\n");
}

// Fonction pour rechercher des consultations par date (jour/mois/annee)
void rechercher_consultations_par_date(DateConsultation date_recherche) {
    int trouves = 0;
    printf("\n--- Consultations pour la date ");
    afficher_date_consult(date_recherche);
    printf(" ---\n");

    for (int i = 0; i < nombre_consultations; i++) {
        if (liste_consultations[i].date_consultation.jour == date_recherche.jour &&
            liste_consultations[i].date_consultation.mois == date_recherche.mois &&
            liste_consultations[i].date_consultation.annee == date_recherche.annee) {

            printf("ID Cons: %d, Patient ID: %d, Pers. ID: %d, Arrivée: ",
                   liste_consultations[i].id_consultation,
                   liste_consultations[i].id_patient,
                   liste_consultations[i].id_personnel_soignant);
            afficher_heure_consult(liste_consultations[i].heure_arrivee);
            printf(", Motif: %s\n", liste_consultations[i].motif);
            trouves++;
        }
    }
    if (trouves == 0) {
        printf("Aucune consultation trouvée pour cette date.\n");
    }
    printf("---------------------------------------\n");
}

// Modification de consultation n'est pas explicitement demandée mais pourrait être ajoutée.
// Idem pour la suppression.


// --- Gestion Fichiers CSV ---

// Sauvegarde les consultations dans un fichier CSV.
// Format: id_consultation,jour,mois,annee,heure_arr,min_arr,heure_dep,min_dep,motif,id_patient,id_personnel
void sauvegarder_consultations_fichier() {
    FILE* fichier = fopen(FICHIER_CONSULTATIONS, "w");
    if (fichier == NULL) {
        perror("Erreur ouverture consultations.dat pour sauvegarde");
        return;
    }

    for (int i = 0; i < nombre_consultations; i++) {
        ecrire_champ_csv_int(fichier, liste_consultations[i].id_consultation);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].date_consultation.jour);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].date_consultation.mois);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].date_consultation.annee);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].heure_arrivee.heure);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].heure_arrivee.minute);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].heure_depart.heure);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].heure_depart.minute);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_consultations[i].motif);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].id_patient);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_consultations[i].id_personnel_soignant);
        fprintf(fichier, "\n");
    }
    fclose(fichier);
    printf("%d consultations sauvegardées (CSV) dans %s\n", nombre_consultations, FICHIER_CONSULTATIONS);
}

// Charge les consultations depuis un fichier CSV.
void charger_consultations_fichier() {
    FILE* fichier = fopen(FICHIER_CONSULTATIONS, "r");
    if (fichier == NULL) {
        printf("Fichier %s non trouvé. Aucune consultation chargée.\n", FICHIER_CONSULTATIONS);
        return;
    }

    char ligne[1024];
    nombre_consultations = 0;

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (nombre_consultations >= MAX_CONSULTATIONS_EN_MEMOIRE) {
            fprintf(stderr, "Max consultations en mémoire atteint lors chargement CSV.\n");
            break;
        }

        Consultation c;
        int index_ligne = 0;
        char temp_champ[MAX_MOTIF_CONSULTATION];

        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.id_consultation = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.date_consultation.jour = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.date_consultation.mois = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.date_consultation.annee = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.heure_arrivee.heure = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.heure_arrivee.minute = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.heure_depart.heure = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.heure_depart.minute = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, c.motif, MAX_MOTIF_CONSULTATION)) continue;
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.id_patient = atoi(temp_champ);
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        c.id_personnel_soignant = atoi(temp_champ);

        // Validation (simple pour l'instant)
        if (c.id_consultation > 0 && rechercher_patient_par_id(c.id_patient) != NULL && rechercher_personnel_par_id(c.id_personnel_soignant) != NULL) {
            liste_consultations[nombre_consultations++] = c;
        } else {
            fprintf(stderr, "Consultation ID %d invalide ou patient/personnel lié non trouvé, ignorée (CSV).\n", c.id_consultation);
        }
    }
    fclose(fichier);
    actualiser_prochain_id_consultation();
    printf("%d consultations chargées (CSV) depuis %s\n", nombre_consultations, FICHIER_CONSULTATIONS);
}


/*
// Exemple d'utilisation
void test_consultations() {
    // Assurez-vous que des patients et du personnel existent avec les IDs utilisés
    // ex: Patient ID 1, Personnel ID 1
    printf("\n--- Test Gestion Consultations ---\n");

    DateConsultation dc1 = {10, 1, 2024};
    Heure ha1 = {10, 0};
    Heure hd1 = {10, 30};
    enregistrer_consultation(dc1, ha1, hd1, "Fièvre légère", 1, 1); // Supposant Patient 1 et Personnel 1 existent

    DateConsultation dc2 = {10, 1, 2024};
    Heure ha2 = {14, 15};
    Heure hd2 = {15, 0};
    enregistrer_consultation(dc2, ha2, hd2, "Douleur abdominale", 2, 1); // Supposant Patient 2 et Personnel 1 existent

    DateConsultation dc3 = {11, 1, 2024};
    Heure ha3 = {9, 0};
    Heure hd3 = {9,45};
    enregistrer_consultation(dc3, ha3, hd3, "Suivi tension", 1, 1);


    afficher_toutes_les_consultations();

    rechercher_consultations_par_patient(1);
    rechercher_consultations_par_patient(99); // Test patient inexistant

    rechercher_consultations_par_date(dc1);

    printf("--- Fin Test Gestion Consultations ---\n");
}
*/
