#ifndef PERSONNEL_H
#define PERSONNEL_H

#define MAX_NOM_PERSONNEL 50
#define MAX_POSTE 50
#define MAX_CONTACT_INFO 100 // Pour email ou téléphone
#define MAX_SUIVI_MEDICAL_PERSONNEL 200

typedef struct {
    int id_personnel;
    char nom[MAX_NOM_PERSONNEL];
    char prenom[MAX_NOM_PERSONNEL];
    char poste[MAX_POSTE]; // Infirmière, Responsable magasin, etc.
    char contact_info[MAX_CONTACT_INFO]; // Ajout pour contact
    char suivi_medical_details[MAX_SUIVI_MEDICAL_PERSONNEL]; // Détails sur le suivi médical
    // On pourrait ajouter date d'embauche, etc.
} Personnel;

#endif //PERSONNEL_H
