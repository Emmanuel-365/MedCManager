#ifndef MEDICAMENT_H
#define MEDICAMENT_H

#define MAX_NOM_MEDICAMENT 50
#define MAX_TYPE_MEDICAMENT 50

// Structure pour la date (utilisée pour la date de péremption)
typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

typedef struct {
    int id_medicament;
    char nom[MAX_NOM_MEDICAMENT];
    char type[MAX_TYPE_MEDICAMENT]; // Comprimé, sirop, injection, etc.
    Date date_peremption;
    int stock;
    int seuil_stock_alerte; // Seuil pour l'alerte de stock bas
} Medicament;

// Constantes pour les alertes
#define SEUIL_STOCK_BAS_PAR_DEFAUT 10 // Si seuil_stock_alerte non spécifié ou <= 0
#define JOURS_ALERTE_PEREMPTION_PROCHE 30

#endif //MEDICAMENT_H
