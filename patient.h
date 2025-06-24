#ifndef PATIENT_H
#define PATIENT_H

#define MAX_NOM_PRENOM 50
#define MAX_SERVICE 50
#define MAX_DOSSIER 200 // Taille indicative pour un résumé du dossier

typedef struct {
    int id_patient;
    char nom[MAX_NOM_PRENOM];
    char prenom[MAX_NOM_PRENOM];
    int age; // Ajout du champ âge
    char service[MAX_SERVICE]; // Service où le patient est traité
    char dossier_medical[MAX_DOSSIER]; // Notes ou résumé du dossier
    // On pourrait ajouter d'autres champs comme sexe, date_naissance, etc.
} Patient;

#endif //PATIENT_H
