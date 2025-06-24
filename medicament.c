#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "medicament.h"
#include "gestion_medicale.h" // Pourrait contenir des utilitaires plus tard
#include "csv_utils.h"

#define MAX_MEDICAMENTS_EN_MEMOIRE 100
Medicament liste_medicaments[MAX_MEDICAMENTS_EN_MEMOIRE];
int nombre_medicaments = 0;
int prochain_id_medicament = 1;

const char* FICHIER_MEDICAMENTS = "medicaments.dat";

// Actualise prochain_id_medicament
void actualiser_prochain_id_medicament() {
    int max_id = 0;
    for (int i = 0; i < nombre_medicaments; i++) {
        if (liste_medicaments[i].id_medicament > max_id) {
            max_id = liste_medicaments[i].id_medicament;
        }
    }
    prochain_id_medicament = max_id + 1;
}


// Fonction utilitaire pour afficher une date
void afficher_date_medicament(Date d) {
    printf("%02d/%02d/%04d", d.jour, d.mois, d.annee);
}

// Fonction pour ajouter un nouveau médicament
// Retourne 1 si succès, 0 si échec
int ajouter_medicament(const char* nom, const char* type, Date date_peremption, int stock, int seuil_alerte) {
    if (nombre_medicaments >= MAX_MEDICAMENTS_EN_MEMOIRE) {
        printf("Erreur : Nombre maximum de médicaments atteint.\n");
        return 0;
    }

    Medicament nouveau_medicament;
    nouveau_medicament.id_medicament = prochain_id_medicament++;
    strncpy(nouveau_medicament.nom, nom, MAX_NOM_MEDICAMENT - 1);
    nouveau_medicament.nom[MAX_NOM_MEDICAMENT - 1] = '\0';
    strncpy(nouveau_medicament.type, type, MAX_TYPE_MEDICAMENT - 1);
    nouveau_medicament.type[MAX_TYPE_MEDICAMENT - 1] = '\0';
    nouveau_medicament.date_peremption = date_peremption;
    nouveau_medicament.stock = stock;
    nouveau_medicament.seuil_stock_alerte = (seuil_alerte > 0) ? seuil_alerte : SEUIL_STOCK_BAS_PAR_DEFAUT;
    // Si seuil_alerte est 0 ou négatif, on utilise le seuil par défaut.

    liste_medicaments[nombre_medicaments] = nouveau_medicament;
    nombre_medicaments++;
    printf("Médicament '%s' ajouté avec succès (ID: %d).\n", nom, nouveau_medicament.id_medicament);
    return 1;
}

// Fonction pour afficher tous les médicaments
void afficher_tous_les_medicaments() {
    if (nombre_medicaments == 0) {
        printf("Aucun médicament enregistré.\n");
        return;
    }
    printf("\n--- Liste des Médicaments ---\n");
    for (int i = 0; i < nombre_medicaments; i++) {
        printf("ID: %d\n", liste_medicaments[i].id_medicament);
        printf("  Nom: %s\n", liste_medicaments[i].nom);
        printf("  Type: %s\n", liste_medicaments[i].type);
        printf("  Date de Péremption: ");
        afficher_date_medicament(liste_medicaments[i].date_peremption);
        printf("\n");
        printf("  Stock: %d (Seuil Alerte: %d)\n", liste_medicaments[i].stock, liste_medicaments[i].seuil_stock_alerte);
        printf("--------------------------\n");
    }
}

// Fonction pour rechercher un médicament par ID
Medicament* rechercher_medicament_par_id(int id_medicament) {
    for (int i = 0; i < nombre_medicaments; i++) {
        if (liste_medicaments[i].id_medicament == id_medicament) {
            return &liste_medicaments[i];
        }
    }
    return NULL;
}

// Fonction pour rechercher des médicaments par nom (recherche partielle)
void rechercher_medicament_par_nom(const char* nom_recherche) {
    int trouves = 0;
    printf("\n--- Résultats de la recherche pour '%s' ---\n", nom_recherche);
    for (int i = 0; i < nombre_medicaments; i++) {
        if (strstr(liste_medicaments[i].nom, nom_recherche) != NULL) {
            printf("ID: %d, Nom: %s, Type: %s, Stock: %d, Péremption: ",
                   liste_medicaments[i].id_medicament,
                   liste_medicaments[i].nom,
                   liste_medicaments[i].type,
                   liste_medicaments[i].stock);
            afficher_date_medicament(liste_medicaments[i].date_peremption);
            printf("\n");
            trouves++;
        }
    }
    if (trouves == 0) {
        printf("Aucun médicament trouvé correspondant à '%s'.\n", nom_recherche);
    }
    printf("---------------------------------------\n");
}

// Fonction pour mettre à jour le stock, la date de péremption et/ou le seuil d'alerte d'un médicament
// Retourne 1 si succès, 0 si non trouvé
int mettre_a_jour_medicament(int id_medicament, int nouveau_stock, Date nouvelle_date_peremption, int nouveau_seuil_alerte, int modifier_stock, int modifier_date, int modifier_seuil) {
    Medicament* med = rechercher_medicament_par_id(id_medicament);
    if (med == NULL) {
        printf("Erreur : Médicament avec ID %d non trouvé.\n", id_medicament);
        return 0;
    }

    if (modifier_stock) {
        if (nouveau_stock >= 0) {
            med->stock = nouveau_stock;
        } else {
            printf("Stock invalide. Non modifié.\n");
        }
    }
    if (modifier_date) {
        // Idéalement, valider la date ici (déjà fait en partie à la saisie)
        med->date_peremption = nouvelle_date_peremption;
    }
    if (modifier_seuil) {
        med->seuil_stock_alerte = (nouveau_seuil_alerte > 0) ? nouveau_seuil_alerte : SEUIL_STOCK_BAS_PAR_DEFAUT;
    }
    printf("Médicament ID %d mis à jour.\n", id_medicament);
    return 1;
}

// La suppression de médicaments n'est pas explicitement demandée sauf "suppression" dans les fonctionnalités.
// On peut l'interpréter comme "retirer de la liste active" ou ne pas l'implémenter si non crucial pour le moment.
// Pour l'instant, on se concentre sur Ajout, Recherche, Mise à jour stock/date, Affichage.
// int supprimer_medicament(int id_medicament) { ... }


// --- Gestion Fichiers CSV ---

// Sauvegarde les médicaments dans un fichier CSV.
// Format: id_medicament,nom,type,jour_peremption,mois_peremption,annee_peremption,stock,seuil_stock_alerte
void sauvegarder_medicaments_fichier() {
    FILE* fichier = fopen(FICHIER_MEDICAMENTS, "w");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier medicaments.dat pour sauvegarde");
        return;
    }

    for (int i = 0; i < nombre_medicaments; i++) {
        ecrire_champ_csv_int(fichier, liste_medicaments[i].id_medicament);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_medicaments[i].nom);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_medicaments[i].type);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_medicaments[i].date_peremption.jour);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_medicaments[i].date_peremption.mois);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_medicaments[i].date_peremption.annee);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_medicaments[i].stock);
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, liste_medicaments[i].seuil_stock_alerte);
        fprintf(fichier, "\n");
    }

    fclose(fichier);
    printf("%d médicaments sauvegardés au format CSV dans %s\n", nombre_medicaments, FICHIER_MEDICAMENTS);
}

// Charge les médicaments depuis un fichier CSV.
void charger_medicaments_fichier() {
    FILE* fichier = fopen(FICHIER_MEDICAMENTS, "r");
    if (fichier == NULL) {
        printf("Fichier %s non trouvé. Aucun médicament chargé.\n", FICHIER_MEDICAMENTS);
        return;
    }

    char ligne[1024];
    nombre_medicaments = 0;

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        if (nombre_medicaments >= MAX_MEDICAMENTS_EN_MEMOIRE) {
            fprintf(stderr, "Attention : MAX_MEDICAMENTS_EN_MEMOIRE atteint lors du chargement CSV.\n");
            break;
        }

        Medicament m;
        int index_ligne = 0;
        char temp_champ[MAX_NOM_MEDICAMENT]; // Assez grand pour nom ou type

        // id_medicament
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        m.id_medicament = atoi(temp_champ);

        // nom
        if (!lire_champ_csv(ligne, &index_ligne, m.nom, MAX_NOM_MEDICAMENT)) continue;

        // type
        if (!lire_champ_csv(ligne, &index_ligne, m.type, MAX_TYPE_MEDICAMENT)) continue;

        // date_peremption.jour
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        m.date_peremption.jour = atoi(temp_champ);

        // date_peremption.mois
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        m.date_peremption.mois = atoi(temp_champ);

        // date_peremption.annee
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        m.date_peremption.annee = atoi(temp_champ);

        // stock
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        m.stock = atoi(temp_champ);

        // seuil_stock_alerte
        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) {
            // Si le champ n'existe pas (ancienne version du fichier), initialiser avec une valeur par défaut.
            m.seuil_stock_alerte = SEUIL_STOCK_BAS_PAR_DEFAUT;
        } else {
            m.seuil_stock_alerte = atoi(temp_champ);
            if (m.seuil_stock_alerte <= 0) m.seuil_stock_alerte = SEUIL_STOCK_BAS_PAR_DEFAUT;
        }


        // Validation simple
        if (m.id_medicament > 0 && m.stock >= 0 &&
            m.date_peremption.jour > 0 && m.date_peremption.jour <= 31 &&
            m.date_peremption.mois > 0 && m.date_peremption.mois <= 12 &&
            m.date_peremption.annee >= 1900 && m.date_peremption.annee <= 2200) { // Plage large pour l'année
            liste_medicaments[nombre_medicaments++] = m;
        } else {
            fprintf(stderr, "Médicament avec données invalides (ID %d) ignoré lors du chargement CSV.\n", m.id_medicament);
        }
    }

    fclose(fichier);
    actualiser_prochain_id_medicament();
    printf("%d médicaments chargés depuis le CSV %s\n", nombre_medicaments, FICHIER_MEDICAMENTS);
}


// --- Alertes ---
#include <time.h> // Pour obtenir la date actuelle

// Fonction pour vérifier et afficher les alertes de stock bas et de péremption
void verifier_et_afficher_alertes_medicaments() {
    time_t t = time(NULL);
    struct tm tm_actuelle = *localtime(&t);

    int annee_actuelle = tm_actuelle.tm_year + 1900;
    int mois_actuel = tm_actuelle.tm_mon + 1;
    int jour_actuel = tm_actuelle.tm_mday;

    printf("\n--- ALERTES MÉDICAMENTS ---\n");
    int alertes_trouvees = 0;

    // Alerte Stock Bas
    for (int i = 0; i < nombre_medicaments; i++) {
        int seuil = (liste_medicaments[i].seuil_stock_alerte > 0) ? liste_medicaments[i].seuil_stock_alerte : SEUIL_STOCK_BAS_PAR_DEFAUT;
        if (liste_medicaments[i].stock < seuil) {
            if (!alertes_trouvees) printf("--- Alertes de Stock Bas ---\n");
            printf("ALERTE STOCK BAS: Médicament ID %d '%s' - Stock: %d (Seuil: %d)\n",
                   liste_medicaments[i].id_medicament,
                   liste_medicaments[i].nom,
                   liste_medicaments[i].stock,
                   seuil);
            alertes_trouvees = 1;
        }
    }
    if(alertes_trouvees && nombre_medicaments > 0) printf("----------------------------\n");


    // Alerte Péremption
    int alertes_peremption_trouvees = 0;
    for (int i = 0; i < nombre_medicaments; i++) {
        Date peremption = liste_medicaments[i].date_peremption;

        // Conversion des dates en "nombre de jours depuis une époque" pour comparaison facile
        // Approximation simple: (annee * 365 + mois * 30 + jour)
        // Une bibliothèque de date serait plus précise pour gérer les mois de différentes longueurs et années bissextiles
        long jours_peremption = peremption.annee * 365L + peremption.mois * 30L + peremption.jour;
        long jours_actuels = annee_actuelle * 365L + mois_actuel * 30L + jour_actuel;

        if (jours_peremption < jours_actuels) { // Déjà périmé
            if (!alertes_peremption_trouvees) {
                 if (alertes_trouvees) printf("\n"); // Saut de ligne si alertes de stock avant
                 printf("--- Alertes de Péremption ---\n");
                 alertes_peremption_trouvees = 1;
                 alertes_trouvees = 1;
            }
            printf("ALERTE PÉRIMÉ: Médicament ID %d '%s' - Date péremption: %02d/%02d/%04d\n",
                   liste_medicaments[i].id_medicament,
                   liste_medicaments[i].nom,
                   peremption.jour, peremption.mois, peremption.annee);
        } else if (jours_peremption < (jours_actuels + JOURS_ALERTE_PEREMPTION_PROCHE)) { // Péremption proche
             if (!alertes_peremption_trouvees) {
                 if (alertes_trouvees) printf("\n");
                 printf("--- Alertes de Péremption ---\n");
                 alertes_peremption_trouvees = 1;
                 alertes_trouvees = 1;
            }
            printf("ALERTE PÉREMPTION PROCHE: Médicament ID %d '%s' - Expire le: %02d/%02d/%04d (dans %ld jours approx.)\n",
                   liste_medicaments[i].id_medicament,
                   liste_medicaments[i].nom,
                   peremption.jour, peremption.mois, peremption.annee,
                   (jours_peremption - jours_actuels));
        }
    }


    if (!alertes_trouvees) {
        printf("Aucune alerte pour les médicaments.\n");
    }
    printf("---------------------------\n");
}


/*
// Exemple d'utilisation (à intégrer dans main.c ou une fonction de test dédiée)
void test_medicaments() {
    printf("\n--- Test Gestion Médicaments ---\n");
    Date d1 = {15, 12, 2025};
    Date d2 = {01, 06, 2024};
    Date d3 = {20, 8, 2026};


    ajouter_medicament("Paracétamol 500mg", "Comprimé", d1, 150);
    ajouter_medicament("Amoxicilline 250mg", "Sirop", d2, 75);
    ajouter_medicament("Ibuprofène 400mg", "Comprimé", d1, 200);


    afficher_tous_les_medicaments();

    rechercher_medicament_par_nom("mol");

    Medicament* med = rechercher_medicament_par_id(2);
    if (med) {
        printf("Médicament ID 2 trouvé: %s, stock %d\n", med->nom, med->stock);
        mettre_a_jour_medicament(2, 60, d3, 1, 1); // Modifie stock et date
        printf("Après MAJ: %s, stock %d, péremption: ", med->nom, med->stock);
        afficher_date_medicament(med->date_peremption);
        printf("\n");
    }

    afficher_tous_les_medicaments();
    printf("--- Fin Test Gestion Médicaments ---\n");
}
*/
