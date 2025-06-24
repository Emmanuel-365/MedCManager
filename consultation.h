#ifndef CONSULTATION_H
#define CONSULTATION_H

#include "patient.h" // Pour id_patient
#include "personnel.h" // Pour id_infirmiere (ou id_personnel_soignant)

#define MAX_MOTIF_CONSULTATION 200

// Structure pour l'heure
typedef struct {
    int heure;
    int minute;
} Heure;

// Structure pour la date (peut être réutilisée ou définie globalement)
// Si non définie globalement, la voici à nouveau.
// Sinon, on pourrait l'inclure depuis medicament.h ou un fichier commun "utils.h"
#ifndef DATE_STRUCT_DEFINED // Pour éviter la redéfinition si on centralise Date plus tard
#define DATE_STRUCT_DEFINED
typedef struct {
    int jour;
    int mois;
    int annee;
} DateConsultation; // Renommée pour éviter conflit si Date de medicament.h n'est pas partagée
#endif

typedef struct {
    int id_consultation;
    DateConsultation date_consultation;
    Heure heure_arrivee;
    Heure heure_depart;
    char motif[MAX_MOTIF_CONSULTATION];
    int id_patient; // Référence à l'ID du patient
    int id_personnel_soignant; // Référence à l'ID du personnel (ex: infirmière)
    // On pourrait ajouter des champs comme diagnostic, prescription_ids, etc.
} Consultation;

#endif //CONSULTATION_H
