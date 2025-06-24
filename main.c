#include <stdio.h>
#include <string.h> // Pour strlen, strcspn
#include <stdlib.h> // Pour atoi
#include "gestion_medicale.h" // Inclut patient.h etc.

// Prototypes des fonctions de patient.c (temporaire, jusqu'à ce qu'on ait un meilleur système de build ou des en-têtes plus complets pour les fonctions)
// Normalement, ces prototypes seraient dans patient.h si ce sont des fonctions publiques de ce module.
// Ou alors, on appelle des fonctions de plus haut niveau définies dans gestion_medicale.h qui elles-mêmes appellent celles de patient.c

// Fonctions de patient.c (pour test direct)
extern int ajouter_patient(const char* nom, const char* prenom, int age, const char* service, const char* dossier_medical);
extern void afficher_tous_les_patients();
extern Patient* rechercher_patient_par_id(int id_patient);
extern void rechercher_patient_par_nom(const char* nom_recherche);
extern int modifier_patient(int id_patient, const char* nouveau_nom, const char* nouveau_prenom, int nouvel_age, const char* nouveau_service, const char* nouveau_dossier);

// Prototypes similaires pour medicament.c, personnel.c, consultation.c quand ils seront créés.
extern int ajouter_medicament(const char* nom, const char* type, Date date_peremption, int stock, int seuil_alerte);
extern void afficher_tous_les_medicaments();
extern Medicament* rechercher_medicament_par_id(int id_medicament);
extern void rechercher_medicament_par_nom(const char* nom_recherche_med); // Renommé pour éviter conflit avec celui des patients
extern int mettre_a_jour_medicament(int id_medicament, int nouveau_stock, Date nouvelle_date_peremption, int nouveau_seuil_alerte, int modifier_stock, int modifier_date, int modifier_seuil);
extern void afficher_date_medicament(Date d); // Utile pour l'affichage dans le menu

// Prototypes pour personnel.c
extern int ajouter_membre_personnel(const char* nom, const char* prenom, const char* poste, const char* contact_info, const char* suivi_medical);
extern void afficher_tout_le_personnel();
extern Personnel* rechercher_personnel_par_id(int id_pers);
extern void rechercher_personnel_par_nom(const char* nom_recherche_pers); // Renommé pour éviter conflit
extern int modifier_personnel(int id_pers, const char* nouv_nom, const char* nouv_prenom, const char* nouv_poste, const char* nouv_contact, const char* nouv_suivi_medical);

// Prototypes pour consultation.c
extern int enregistrer_consultation(DateConsultation date, Heure heure_arrivee, Heure heure_depart, const char* motif, int id_patient, int id_personnel);
extern void afficher_toutes_les_consultations();
extern void rechercher_consultations_par_patient(int id_patient);
extern void rechercher_consultations_par_date(DateConsultation date_recherche);
// extern void afficher_date_consult(DateConsultation d); // Déjà dans consultation.c, pas besoin ici si non utilisé directement par main
// extern void afficher_heure_consult(Heure h); // Idem

// Fonctions de persistance (seront définies dans les fichiers .c respectifs)
extern void charger_patients_fichier();
extern void sauvegarder_patients_fichier();
extern void charger_medicaments_fichier();
extern void sauvegarder_medicaments_fichier();
extern void charger_personnel_fichier();
extern void sauvegarder_personnel_fichier();
extern void charger_consultations_fichier();
extern void sauvegarder_consultations_fichier();
extern void verifier_et_afficher_alertes_medicaments(); // Pour les alertes

#include "utilisateur.h" // Pour la gestion des utilisateurs et login
#include "user_roles.h"  // Pour ProfilUtilisateur et profil_actuel

// profil_actuel est défini dans user_roles.c et déclaré extern dans user_roles.h


// Menu principal (très simple pour l'instant)
void afficher_menu_principal() {
    printf("\n--- Système de Gestion Médicale (Profil: %d) ---\n", profil_actuel);

    if (peut_acceder_gestion_patients(profil_actuel))
        printf("1. Gestion des Patients\n");
    if (peut_visualiser_medicaments(profil_actuel)) // Peut être affiné (visualiser vs gérer)
        printf("2. Gestion des Médicaments\n"); // Le sous-menu gérera les droits fins
    if (peut_acceder_gestion_personnel(profil_actuel))
        printf("3. Gestion du Personnel\n");
    if (peut_acceder_gestion_consultations(profil_actuel))
        printf("4. Gestion des Consultations\n");
    if (peut_verifier_alertes(profil_actuel))
        printf("5. Vérifier les Alertes Médicaments\n");

    if (profil_actuel == PROFIL_ADMINISTRATEUR) {
        printf("7. Gestion des Utilisateurs\n");
    }

    if (peut_sauvegarder_donnees(profil_actuel)) // Ou toujours visible et géré à l'appel
        printf("8. Sauvegarder Toutes les Données (Manuel)\n");

    // Option spécifique pour le responsable régional si on veut un menu dédié
    if (profil_actuel == PROFIL_RESPONSABLE_REGIONAL && !peut_visualiser_medicaments(profil_actuel)) {
         // Si on veut qu'il ait un menu très limité et différent
        printf("M. Voir les médicaments (Responsable Régional)\n");
    }


    printf("0. Quitter (et Sauvegarder si applicable)\n");
    printf("Votre choix : ");
}

void menu_gestion_patients() {
    int choix_patient;
    char nom[MAX_NOM_PRENOM], prenom[MAX_NOM_PRENOM], service[MAX_SERVICE], dossier[MAX_DOSSIER];
    int age, id_patient;

    if (!peut_acceder_gestion_patients(profil_actuel)) {
        printf("Accès refusé à la gestion des patients.\n");
        return;
    }

    do {
        printf("\n--- Gestion des Patients ---\n");
        // Les infirmières et admins peuvent tout faire ici selon la config actuelle de peut_acceder_gestion_patients
        printf("1. Ajouter un patient\n");
        printf("2. Afficher tous les patients\n");
        printf("3. Rechercher un patient par ID\n");
        printf("4. Rechercher un patient par nom\n");
        printf("5. Modifier un patient\n");
        printf("0. Retour au menu principal\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix_patient) != 1) {
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n'); // Vider le buffer d'entrée
            choix_patient = -1; // Mettre une valeur invalide pour continuer la boucle
            continue;
        }
        while (getchar() != '\n'); // Vider le buffer après un scanf numérique réussi

        switch (choix_patient) {
            case 1:
                printf("Nom du patient : ");
                fgets(nom, MAX_NOM_PRENOM, stdin); nom[strcspn(nom, "\n")] = 0;
                printf("Prénom du patient : ");
                fgets(prenom, MAX_NOM_PRENOM, stdin); prenom[strcspn(prenom, "\n")] = 0;
                printf("Âge du patient : ");
                while (scanf("%d", &age) != 1 || age <=0) {
                     printf("Âge invalide. Veuillez entrer un nombre positif.\nÂge du patient : ");
                     while (getchar() != '\n');
                }
                while (getchar() != '\n');
                printf("Service : ");
                fgets(service, MAX_SERVICE, stdin); service[strcspn(service, "\n")] = 0;
                printf("Dossier médical (résumé) : ");
                fgets(dossier, MAX_DOSSIER, stdin); dossier[strcspn(dossier, "\n")] = 0;
                ajouter_patient(nom, prenom, age, service, dossier);
                break;
            case 2:
                afficher_tous_les_patients();
                break;
            case 3:
                printf("ID du patient à rechercher : ");
                if (scanf("%d", &id_patient) == 1) {
                    while (getchar() != '\n');
                    Patient* p = rechercher_patient_par_id(id_patient);
                    if (p) {
                        printf("Patient trouvé: ID %d, %s %s, %d ans, Service: %s, Dossier: %s\n",
                               p->id_patient, p->prenom, p->nom, p->age, p->service, p->dossier_medical);
                    } else {
                        printf("Patient avec ID %d non trouvé.\n", id_patient);
                    }
                } else {
                    printf("Entrée ID invalide.\n");
                    while (getchar() != '\n');
                }
                break;
            case 4:
                printf("Nom ou prénom à rechercher : ");
                fgets(nom, MAX_NOM_PRENOM, stdin); nom[strcspn(nom, "\n")] = 0;
                rechercher_patient_par_nom(nom);
                break;
            case 5:
                printf("ID du patient à modifier : ");
                 if (scanf("%d", &id_patient) == 1) {
                    while (getchar() != '\n');
                    // Demander les nouvelles informations (laisser vide pour ne pas changer)
                    printf("Nouveau nom (laisser vide pour ne pas changer) : ");
                    fgets(nom, MAX_NOM_PRENOM, stdin); nom[strcspn(nom, "\n")] = 0;
                    printf("Nouveau prénom (laisser vide pour ne pas changer) : ");
                    fgets(prenom, MAX_NOM_PRENOM, stdin); prenom[strcspn(prenom, "\n")] = 0;
                    char age_str[10];
                    printf("Nouvel âge (laisser vide ou 0 pour ne pas changer) : ");
                    fgets(age_str, 10, stdin); age_str[strcspn(age_str, "\n")] = 0;
                    int nouvel_age_val = (strlen(age_str) > 0) ? atoi(age_str) : 0;
                    if (nouvel_age_val < 0) nouvel_age_val = 0; // Sécurité

                    printf("Nouveau service (laisser vide pour ne pas changer) : ");
                    fgets(service, MAX_SERVICE, stdin); service[strcspn(service, "\n")] = 0;
                    printf("Nouveau dossier (laisser vide pour ne pas changer) : ");
                    fgets(dossier, MAX_DOSSIER, stdin); dossier[strcspn(dossier, "\n")] = 0;

                    modifier_patient(id_patient,
                                     strlen(nom) > 0 ? nom : NULL,
                                     strlen(prenom) > 0 ? prenom : NULL,
                                     nouvel_age_val, // fonction modifier_patient gère si 0
                                     strlen(service) > 0 ? service : NULL,
                                     strlen(dossier) > 0 ? dossier : NULL);
                } else {
                    printf("Entrée ID invalide.\n");
                    while (getchar() != '\n');
                }
                break;
            case 0:
                printf("Retour au menu principal...\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix_patient != 0);
}

void menu_gestion_medicaments() {
    int choix_medicament;
    char nom_med[MAX_NOM_MEDICAMENT], type_med[MAX_TYPE_MEDICAMENT];
    Date date_peremption_med;
    int stock_med, id_med, seuil_alerte_med; // Ajout de seuil_alerte_med
    char temp_date_str[12]; // "dd/mm/yyyy\0"

    do {
        printf("\n--- Gestion des Médicaments ---\n");
        // Afficher les options en fonction des droits
        if (peut_acceder_gestion_medicaments_complet(profil_actuel)) {
            printf("1. Ajouter un médicament\n");
        }
        printf("2. Afficher tous les médicaments\n"); // Tous les profils concernés peuvent afficher
        printf("3. Rechercher un médicament par ID\n");
        printf("4. Rechercher un médicament par nom\n");
        if (peut_acceder_gestion_medicaments_complet(profil_actuel)) {
            printf("5. Mettre à jour un médicament (stock/date/seuil)\n");
        }
        printf("0. Retour au menu principal\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix_medicament) != 1) {
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n');
            choix_medicament = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (choix_medicament) {
            case 1:
                if (!peut_acceder_gestion_medicaments_complet(profil_actuel)) {
                    printf("Accès refusé.\n");
                    break;
                }
                printf("Nom du médicament : ");
                fgets(nom_med, MAX_NOM_MEDICAMENT, stdin); nom_med[strcspn(nom_med, "\n")] = 0;
                printf("Type du médicament : ");
                fgets(type_med, MAX_TYPE_MEDICAMENT, stdin); type_med[strcspn(type_med, "\n")] = 0;
                printf("Date de péremption (JJ/MM/AAAA) : ");
                // Boucle pour s'assurer que le format de la date est correct
                while (1) {
                    fgets(temp_date_str, sizeof(temp_date_str), stdin); temp_date_str[strcspn(temp_date_str, "\n")] = 0;
                    if (sscanf(temp_date_str, "%d/%d/%d", &date_peremption_med.jour, &date_peremption_med.mois, &date_peremption_med.annee) == 3) {
                        // Validation simple de la date (peut être améliorée)
                        if (date_peremption_med.jour > 0 && date_peremption_med.jour <= 31 &&
                            date_peremption_med.mois > 0 && date_peremption_med.mois <= 12 &&
                            date_peremption_med.annee >= 2000 && date_peremption_med.annee <= 2100) {
                            break; // Date valide
                        }
                    }
                    printf("Format de date invalide ou date hors limites. Réessayez (JJ/MM/AAAA) : ");
                }
                printf("Stock initial : ");
                while (scanf("%d", &stock_med) != 1 || stock_med < 0) {
                    printf("Stock invalide. Veuillez entrer un nombre positif ou nul.\nStock initial : ");
                    while (getchar() != '\n');
                }
                while (getchar() != '\n');
                printf("Seuil d'alerte de stock (ex: 10, mettre 0 pour défaut %d) : ", SEUIL_STOCK_BAS_PAR_DEFAUT);
                if (scanf("%d", &seuil_alerte_med) != 1) {
                    printf("Entrée invalide pour le seuil, utilisation du défaut.\n");
                    seuil_alerte_med = 0; // Sera géré par ajouter_medicament
                }
                while (getchar() != '\n');

                ajouter_medicament(nom_med, type_med, date_peremption_med, stock_med, seuil_alerte_med);
                break;
            case 2:
                afficher_tous_les_medicaments();
                break;
            case 3:
                printf("ID du médicament à rechercher : ");
                if (scanf("%d", &id_med) == 1) {
                    while (getchar() != '\n');
                    Medicament* m = rechercher_medicament_par_id(id_med);
                    if (m) {
                        printf("Médicament trouvé: ID %d, %s (%s), Stock: %d, Péremption: ",
                               m->id_medicament, m->nom, m->type, m->stock);
                        afficher_date_medicament(m->date_peremption);
                        printf("\n");
                    } else {
                        printf("Médicament avec ID %d non trouvé.\n", id_med);
                    }
                } else {
                    printf("Entrée ID invalide.\n");
                    while (getchar() != '\n');
                }
                break;
            case 4:
                printf("Nom du médicament à rechercher : ");
                fgets(nom_med, MAX_NOM_MEDICAMENT, stdin); nom_med[strcspn(nom_med, "\n")] = 0;
                rechercher_medicament_par_nom(nom_med); // Utilise la version renommée
                break;
            case 5:
                if (!peut_acceder_gestion_medicaments_complet(profil_actuel)) {
                    printf("Accès refusé.\n");
                    break;
                }
                printf("ID du médicament à mettre à jour : ");
                if (scanf("%d", &id_med) == 1) {
                    while (getchar() != '\n');
                    Medicament* med_a_maj = rechercher_medicament_par_id(id_med);
                    if (!med_a_maj) {
                        printf("Médicament ID %d non trouvé.\n", id_med);
                        break;
                    }

                    int modifier_stock_flag = 0;
                    int nouveau_stock_val = med_a_maj->stock;
                    char saisie_str[20]; // Buffer pour les saisies de stock et seuil

                    printf("Nouveau stock (laisser vide pour ne pas changer [%d]) : ", med_a_maj->stock);
                    fgets(saisie_str, sizeof(saisie_str), stdin); saisie_str[strcspn(saisie_str, "\n")] = 0;
                    if (strlen(saisie_str) > 0) {
                        if (sscanf(saisie_str, "%d", &nouveau_stock_val) == 1 && nouveau_stock_val >= 0) {
                            modifier_stock_flag = 1;
                        } else {
                             printf("Entrée de stock invalide. Le stock ne sera pas modifié.\n");
                             nouveau_stock_val = med_a_maj->stock;
                        }
                    }

                    int modifier_date_flag = 0;
                    Date nouvelle_date_val = med_a_maj->date_peremption;
                    // temp_date_str déjà déclaré plus haut
                    printf("Nouvelle date de péremption (JJ/MM/AAAA) (laisser vide pour ne pas changer [%02d/%02d/%04d]) : ",
                           med_a_maj->date_peremption.jour, med_a_maj->date_peremption.mois, med_a_maj->date_peremption.annee);
                    fgets(temp_date_str, sizeof(temp_date_str), stdin); temp_date_str[strcspn(temp_date_str, "\n")] = 0;
                    if (strlen(temp_date_str) > 0) {
                         if (sscanf(temp_date_str, "%d/%d/%d", &nouvelle_date_val.jour, &nouvelle_date_val.mois, &nouvelle_date_val.annee) == 3 &&
                             nouvelle_date_val.jour > 0 && nouvelle_date_val.jour <= 31 &&
                             nouvelle_date_val.mois > 0 && nouvelle_date_val.mois <= 12 &&
                             nouvelle_date_val.annee >= 2000 && nouvelle_date_val.annee <= 2100) {
                            modifier_date_flag = 1;
                        } else {
                            printf("Date invalide ou hors limites. La date ne sera pas modifiée.\n");
                            nouvelle_date_val = med_a_maj->date_peremption;
                        }
                    }

                    int modifier_seuil_flag = 0;
                    int nouveau_seuil_val = med_a_maj->seuil_stock_alerte;
                    printf("Nouveau seuil d'alerte (laisser vide pour ne pas changer [%d], 0 pour défaut %d) : ", med_a_maj->seuil_stock_alerte, SEUIL_STOCK_BAS_PAR_DEFAUT);
                    fgets(saisie_str, sizeof(saisie_str), stdin); saisie_str[strcspn(saisie_str, "\n")] = 0;
                    if (strlen(saisie_str) > 0) {
                        if (sscanf(saisie_str, "%d", &nouveau_seuil_val) == 1) {
                             modifier_seuil_flag = 1;
                             // la fonction mettre_a_jour_medicament gérera la mise au défaut si <=0
                        } else {
                            printf("Entrée de seuil invalide. Le seuil ne sera pas modifié.\n");
                            nouveau_seuil_val = med_a_maj->seuil_stock_alerte;
                        }
                    }

                    if(modifier_stock_flag || modifier_date_flag || modifier_seuil_flag) {
                        mettre_a_jour_medicament(id_med, nouveau_stock_val, nouvelle_date_val, nouveau_seuil_val, modifier_stock_flag, modifier_date_flag, modifier_seuil_flag);
                    } else {
                        printf("Aucune modification apportée.\n");
                    }

                } else {
                    printf("Entrée ID invalide.\n");
                    while (getchar() != '\n');
                }
                break;
            case 0:
                printf("Retour au menu principal...\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix_medicament != 0);
}

void menu_gestion_personnel() {
    int choix_personnel;
    char nom_pers[MAX_NOM_PERSONNEL], prenom_pers[MAX_NOM_PERSONNEL], poste_pers[MAX_POSTE];
    char contact_pers[MAX_CONTACT_INFO], suivi_med_pers[MAX_SUIVI_MEDICAL_PERSONNEL];
    int id_pers;

    if (!peut_acceder_gestion_personnel(profil_actuel)) {
        printf("Accès refusé à la gestion du personnel.\n");
        return;
    }

    do {
        printf("\n--- Gestion du Personnel ---\n");
        printf("1. Ajouter un membre du personnel\n");
        printf("2. Afficher tout le personnel\n");
        printf("3. Rechercher un membre par ID\n");
        printf("4. Rechercher un membre par nom\n");
        printf("5. Modifier un membre du personnel\n");
        printf("0. Retour au menu principal\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix_personnel) != 1) {
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n');
            choix_personnel = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (choix_personnel) {
            case 1:
                printf("Nom du membre : "); fgets(nom_pers, sizeof(nom_pers), stdin); nom_pers[strcspn(nom_pers, "\n")] = 0;
                printf("Prénom du membre : "); fgets(prenom_pers, sizeof(prenom_pers), stdin); prenom_pers[strcspn(prenom_pers, "\n")] = 0;
                printf("Poste : "); fgets(poste_pers, sizeof(poste_pers), stdin); poste_pers[strcspn(poste_pers, "\n")] = 0;
                printf("Informations de contact : "); fgets(contact_pers, sizeof(contact_pers), stdin); contact_pers[strcspn(contact_pers, "\n")] = 0;
                printf("Détails du suivi médical : "); fgets(suivi_med_pers, sizeof(suivi_med_pers), stdin); suivi_med_pers[strcspn(suivi_med_pers, "\n")] = 0;
                ajouter_membre_personnel(nom_pers, prenom_pers, poste_pers, contact_pers, suivi_med_pers);
                break;
            case 2:
                afficher_tout_le_personnel();
                break;
            case 3:
                printf("ID du membre à rechercher : ");
                if (scanf("%d", &id_pers) == 1) {
                    while (getchar() != '\n');
                    Personnel* p = rechercher_personnel_par_id(id_pers);
                    if (p) {
                        printf("Membre trouvé: ID %d, %s %s, Poste: %s, Contact: %s, Suivi: %s\n",
                               p->id_personnel, p->prenom, p->nom, p->poste, p->contact_info, p->suivi_medical_details);
                    } else {
                        printf("Membre du personnel avec ID %d non trouvé.\n", id_pers);
                    }
                } else {
                    printf("Entrée ID invalide.\n");
                    while (getchar() != '\n');
                }
                break;
            case 4:
                printf("Nom ou prénom du membre à rechercher : ");
                fgets(nom_pers, sizeof(nom_pers), stdin); nom_pers[strcspn(nom_pers, "\n")] = 0;
                rechercher_personnel_par_nom(nom_pers); // Utilise la version renommée pour personnel
                break;
            case 5:
                printf("ID du membre à modifier : ");
                if (scanf("%d", &id_pers) == 1) {
                    while (getchar() != '\n'); // Vider le buffer

                    if (!rechercher_personnel_par_id(id_pers)) {
                        printf("Membre du personnel avec ID %d non trouvé.\n", id_pers);
                        break;
                    }
                    printf("Nouveau nom (laisser vide pour ne pas changer) : ");
                    fgets(nom_pers, sizeof(nom_pers), stdin); nom_pers[strcspn(nom_pers, "\n")] = 0;
                    printf("Nouveau prénom (laisser vide pour ne pas changer) : ");
                    fgets(prenom_pers, sizeof(prenom_pers), stdin); prenom_pers[strcspn(prenom_pers, "\n")] = 0;
                    printf("Nouveau poste (laisser vide pour ne pas changer) : ");
                    fgets(poste_pers, sizeof(poste_pers), stdin); poste_pers[strcspn(poste_pers, "\n")] = 0;
                    printf("Nouvelles infos de contact (laisser vide pour ne pas changer) : ");
                    fgets(contact_pers, sizeof(contact_pers), stdin); contact_pers[strcspn(contact_pers, "\n")] = 0;
                    printf("Nouveau suivi médical (laisser vide pour ne pas changer) : ");
                    fgets(suivi_med_pers, sizeof(suivi_med_pers), stdin); suivi_med_pers[strcspn(suivi_med_pers, "\n")] = 0;

                    modifier_personnel(id_pers, nom_pers, prenom_pers, poste_pers, contact_pers, suivi_med_pers);
                } else {
                    printf("Entrée ID invalide.\n");
                    while (getchar() != '\n');
                }
                break;
            case 0:
                printf("Retour au menu principal...\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix_personnel != 0);
}

void menu_gestion_consultations() {
    int choix_consult;
    DateConsultation date_c;
    Heure heure_a, heure_d;
    char motif_c[MAX_MOTIF_CONSULTATION];
    int id_pat_c, id_pers_c;
    char temp_str[20]; // Pour saisie date/heure

    if(!peut_acceder_gestion_consultations(profil_actuel)) {
        printf("Accès refusé à la gestion des consultations.\n");
        return;
    }

    do {
        printf("\n--- Gestion des Consultations ---\n");
        printf("1. Enregistrer une nouvelle consultation\n");
        printf("2. Afficher toutes les consultations\n");
        printf("3. Rechercher des consultations par ID Patient\n");
        printf("4. Rechercher des consultations par date\n");
        printf("0. Retour au menu principal\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix_consult) != 1) {
            printf("Entrée invalide.\n");
            while (getchar() != '\n');
            choix_consult = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (choix_consult) {
            case 1:
                printf("ID du Patient : ");
                if (scanf("%d", &id_pat_c) != 1) { printf("ID Patient invalide.\n"); while (getchar() != '\n'); break; }
                while (getchar() != '\n');

                printf("ID du Personnel soignant : ");
                if (scanf("%d", &id_pers_c) != 1) { printf("ID Personnel invalide.\n"); while (getchar() != '\n'); break; }
                while (getchar() != '\n');

                printf("Date de la consultation (JJ/MM/AAAA) : ");
                while (1) {
                    fgets(temp_str, sizeof(temp_str), stdin); temp_str[strcspn(temp_str, "\n")] = 0;
                    if (sscanf(temp_str, "%d/%d/%d", &date_c.jour, &date_c.mois, &date_c.annee) == 3 &&
                        date_c.jour > 0 && date_c.jour <= 31 && date_c.mois > 0 && date_c.mois <= 12 && date_c.annee >= 2000 && date_c.annee <= 2100) break;
                    printf("Date invalide. Réessayez (JJ/MM/AAAA) : ");
                }

                printf("Heure d'arrivée (HH:MM) : ");
                while (1) {
                    fgets(temp_str, sizeof(temp_str), stdin); temp_str[strcspn(temp_str, "\n")] = 0;
                    if (sscanf(temp_str, "%d:%d", &heure_a.heure, &heure_a.minute) == 2 &&
                        heure_a.heure >= 0 && heure_a.heure < 24 && heure_a.minute >= 0 && heure_a.minute < 60) break;
                    printf("Heure invalide. Réessayez (HH:MM) : ");
                }

                printf("Heure de départ (HH:MM) : ");
                 while (1) {
                    fgets(temp_str, sizeof(temp_str), stdin); temp_str[strcspn(temp_str, "\n")] = 0;
                    if (sscanf(temp_str, "%d:%d", &heure_d.heure, &heure_d.minute) == 2 &&
                        heure_d.heure >= 0 && heure_d.heure < 24 && heure_d.minute >= 0 && heure_d.minute < 60) break;
                    printf("Heure invalide. Réessayez (HH:MM) : ");
                }

                printf("Motif de la consultation : ");
                fgets(motif_c, MAX_MOTIF_CONSULTATION, stdin); motif_c[strcspn(motif_c, "\n")] = 0;

                enregistrer_consultation(date_c, heure_a, heure_d, motif_c, id_pat_c, id_pers_c);
                break;
            case 2:
                afficher_toutes_les_consultations();
                break;
            case 3:
                printf("ID du Patient pour rechercher ses consultations : ");
                if (scanf("%d", &id_pat_c) == 1) {
                    while (getchar() != '\n');
                    rechercher_consultations_par_patient(id_pat_c);
                } else {
                    printf("Entrée ID invalide.\n"); while (getchar() != '\n');
                }
                break;
            case 4:
                printf("Date à rechercher (JJ/MM/AAAA) : ");
                 while (1) {
                    fgets(temp_str, sizeof(temp_str), stdin); temp_str[strcspn(temp_str, "\n")] = 0;
                    if (sscanf(temp_str, "%d/%d/%d", &date_c.jour, &date_c.mois, &date_c.annee) == 3 &&
                        date_c.jour > 0 && date_c.jour <= 31 && date_c.mois > 0 && date_c.mois <= 12 && date_c.annee >= 2000 && date_c.annee <= 2100) break;
                    printf("Date invalide. Réessayez (JJ/MM/AAAA) : ");
                }
                rechercher_consultations_par_date(date_c);
                break;
            case 0:
                printf("Retour au menu principal...\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix_consult != 0);
}

void menu_gestion_utilisateurs() {
    if (profil_actuel != PROFIL_ADMINISTRATEUR) {
        printf("Accès non autorisé à la gestion des utilisateurs.\n");
        return;
    }

    int choix;
    char login[MAX_LOGIN_SIZE];
    char mdp[100];
    int profil_num;
    ProfilUtilisateur profil_u;

    do {
        printf("\n--- Gestion des Utilisateurs (Admin) ---\n");
        printf("1. Ajouter un utilisateur\n");
        printf("2. Afficher tous les utilisateurs\n");
        printf("3. Supprimer un utilisateur\n");
        printf("4. Modifier le profil d'un utilisateur\n");
        printf("5. Changer le mot de passe d'un utilisateur\n");
        printf("0. Retour au menu principal\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1) {
            printf("Entrée invalide.\n");
            while(getchar()!='\n');
            choix = -1;
            continue;
        }
        while(getchar()!='\n');

        switch (choix) {
            case 1:
                printf("Login du nouvel utilisateur : "); fgets(login, sizeof(login), stdin); login[strcspn(login, "\n")] = 0;
                printf("Mot de passe temporaire : "); fgets(mdp, sizeof(mdp), stdin); mdp[strcspn(mdp, "\n")] = 0;
                printf("Profil (1:Infirmière, 2:RespMagasin, 3:RespRegional, 4:Admin): ");
                if (scanf("%d", &profil_num) != 1 || profil_num < 1 || profil_num > 4) {
                    printf("Numéro de profil invalide.\n");
                    while(getchar()!='\n');
                    break;
                }
                while(getchar()!='\n');
                profil_u = (ProfilUtilisateur)profil_num;
                ajouter_utilisateur_par_admin(login, mdp, profil_u);
                break;
            case 2:
                afficher_tous_les_utilisateurs();
                break;
            case 3:
                printf("Login de l'utilisateur à supprimer : "); fgets(login, sizeof(login), stdin); login[strcspn(login, "\n")] = 0;
                supprimer_utilisateur_par_login(login);
                break;
            case 4:
                printf("Login de l'utilisateur dont le profil est à modifier : "); fgets(login, sizeof(login), stdin); login[strcspn(login, "\n")] = 0;
                printf("Nouveau Profil (1:Infirmière, 2:RespMagasin, 3:RespRegional, 4:Admin): ");
                 if (scanf("%d", &profil_num) != 1 || profil_num < 1 || profil_num > 4) {
                    printf("Numéro de profil invalide.\n");
                    while(getchar()!='\n');
                    break;
                }
                while(getchar()!='\n');
                profil_u = (ProfilUtilisateur)profil_num;
                modifier_profil_utilisateur(login, profil_u);
                break;
            case 5:
                 printf("Login de l'utilisateur dont le mot de passe est à changer : "); fgets(login, sizeof(login), stdin); login[strcspn(login, "\n")] = 0;
                 printf("Nouveau mot de passe : "); fgets(mdp, sizeof(mdp), stdin); mdp[strcspn(mdp, "\n")] = 0;
                 changer_mot_de_passe_par_admin(login, mdp);
                 break;
            case 0:
                printf("Retour au menu principal...\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 0);
}


int main() {
    int choix_principal;

    // Initialiser et charger les utilisateurs (crée admin par défaut si besoin)
    initialiser_utilisateurs();

    // Demander le login
    profil_actuel = login_utilisateur(); // login_utilisateur met à jour profil_actuel globalement

    if (profil_actuel == PROFIL_NON_DEFINI) {
        printf("Échec de la connexion. L'application va se fermer.\n");
        return 1; // Quitter si login échoué
    }

    // Charger les autres données depuis les fichiers au démarrage
    printf("Chargement des données de l'application...\n");
    charger_patients_fichier();
    charger_medicaments_fichier();
    charger_personnel_fichier();
    charger_consultations_fichier();
    printf("Données de l'application chargées.\n");

    // Afficher les alertes au démarrage (si l'utilisateur y a accès)
    if(peut_verifier_alertes(profil_actuel)){
        verifier_et_afficher_alertes_medicaments();
    }

    /*
    // Les données initiales simulées ne sont plus nécessaires car chargées depuis les fichiers.
    // Laisser commenté si besoin de réinsérer des données de test rapidement.
    // Patient ID 1 & 2, Medicament ID 1 & 2, Personnel ID 1 & 2
    // ajouter_patient("Dupont", "Jean", 35, "Cardiologie", "Angine de poitrine stable.");
    // ajouter_patient("Martin", "Sophie", 28, "Pédiatrie", "Contrôle annuel, RAS.");
    // Date d1_med = {15, 12, 2025}; Date d2_med = {01, 06, 2024};
    // ajouter_medicament("Paracétamol 500mg", "Comprimé", d1_med, 150);
    // ajouter_medicament("Amoxicilline 250mg", "Sirop", d2_med, 75);
    // ajouter_membre_personnel("Lefevre", "Alice", "Infirmière", "alice.lefevre@email.com", "Apte, vaccin à jour");
    // ajouter_membre_personnel("Bernard", "Luc", "Responsable Magasin", "0612345678", "Apte");
    // DateConsultation dc_init_main = {10, 1, 2024}; Heure ha_init_main = {10,0}; Heure hd_init_main = {10,30};
    // enregistrer_consultation(dc_init_main, ha_init_main, hd_init_main, "Fièvre test initial", 1, 1);
    */

    do {
        afficher_menu_principal();
        if (scanf("%d", &choix_principal) != 1) {
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n'); // Vider le buffer d'entrée
            choix_principal = -1; // Mettre une valeur invalide pour continuer la boucle
            continue;
        }
        while (getchar() != '\n'); // Vider le buffer après un scanf numérique réussi

        switch (choix_principal) {
            case 1:
                menu_gestion_patients();
                break;
            case 2:
                menu_gestion_medicaments();
                break;
            case 3:
                menu_gestion_personnel();
                break;
            case 4:
                if (peut_acceder_gestion_consultations(profil_actuel)) menu_gestion_consultations();
                else printf("Accès refusé.\n");
                break;
            case 5: // Vérifier les alertes
                if (peut_verifier_alertes(profil_actuel)) verifier_et_afficher_alertes_medicaments();
                else printf("Accès refusé.\n");
                break;
            case 7: // Gestion des Utilisateurs
                if (profil_actuel == PROFIL_ADMINISTRATEUR) menu_gestion_utilisateurs();
                else printf("Accès refusé.\n");
                break;
            case 8: // Sauvegarde manuelle
                if (peut_sauvegarder_donnees(profil_actuel)) {
                    printf("Sauvegarde manuelle des données...\n");
                    sauvegarder_patients_fichier();
                    // ... (autres sauvegardes)
                } else {
                    printf("Accès refusé.\n");
                }
                break;
            // Le cas 'M' a été retiré pour éviter le conflit de 'case 0' et simplifier.
            // Les droits du responsable régional sont gérés par les fonctions de permission standard.
            // Si des vues spécifiques sont nécessaires, elles devront être assignées à des numéros de menu uniques.

            case 0:
                printf("Sauvegarde des données avant de quitter...\n");
                sauvegarder_patients_fichier();
                sauvegarder_medicaments_fichier();
                sauvegarder_personnel_fichier();
                sauvegarder_consultations_fichier();
                printf("Données sauvegardées. Fermeture de l'application.\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix_principal != 0);

    return 0;
}
