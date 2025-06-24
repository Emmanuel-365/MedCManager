#include "csv_utils.h"
#include <string.h>
#include <stdlib.h> // Pour atoi lors de la lecture d'entiers si on l'ajoute ici

// Écrit une chaîne de caractères dans un fichier CSV.
// Entoure la chaîne de guillemets si elle contient une virgule, un guillemet ou un saut de ligne.
// Échappe les guillemets internes en les doublant.
void ecrire_champ_csv_string(FILE* fichier, const char* chaine) {
    if (chaine == NULL) {
        fprintf(fichier, "\"\""); // Champ vide
        return;
    }

    int contient_car_special = 0;
    const char* ptr = chaine;
    while (*ptr) {
        if (*ptr == ',' || *ptr == '"' || *ptr == '\n' || *ptr == '\r') {
            contient_car_special = 1;
            break;
        }
        ptr++;
    }

    if (contient_car_special) {
        fprintf(fichier, "\"");
        ptr = chaine;
        while (*ptr) {
            if (*ptr == '"') {
                fprintf(fichier, "\"\""); // Double guillemet pour échapper
            } else {
                fputc(*ptr, fichier);
            }
            ptr++;
        }
        fprintf(fichier, "\"");
    } else {
        fprintf(fichier, "%s", chaine);
    }
}

// Écrit un entier dans un fichier CSV.
void ecrire_champ_csv_int(FILE* fichier, int valeur) {
    fprintf(fichier, "%d", valeur);
}


// Lit un champ depuis une ligne CSV bufferisée.
// Retourne 1 si un champ a été lu, 0 si fin de ligne ou erreur.
// `index_ligne` est mis à jour pour pointer vers le début du prochain champ ou la fin de la ligne.
int lire_champ_csv(const char* ligne_csv, int* index_ligne, char* champ_dest, int max_len) {
    if (ligne_csv == NULL || champ_dest == NULL || max_len <= 0) {
        return 0; // Arguments invalides
    }

    int i_dest = 0;
    int i_src = *index_ligne;
    int entre_guillemets = 0;

    // Passer les espaces de début (non standard, mais peut arriver)
    // while (ligne_csv[i_src] == ' ') i_src++;

    if (ligne_csv[i_src] == '\0' || ligne_csv[i_src] == '\n' || ligne_csv[i_src] == '\r') {
        return 0; // Fin de ligne ou ligne vide
    }

    if (ligne_csv[i_src] == '"') {
        entre_guillemets = 1;
        i_src++; // Sauter le guillemet initial
    }

    while (ligne_csv[i_src] != '\0' && ligne_csv[i_src] != '\n' && ligne_csv[i_src] != '\r') {
        if (i_dest >= max_len - 1) {
            // Buffer de destination trop petit
            champ_dest[max_len - 1] = '\0';
            // Continuer à avancer i_src pour trouver la fin du champ
            while(ligne_csv[i_src] != '\0' && ligne_csv[i_src] != '\n' && ligne_csv[i_src] != '\r' &&
                  (entre_guillemets || ligne_csv[i_src] != ',') &&
                  (!entre_guillemets || ligne_csv[i_src] != '"' || (ligne_csv[i_src] == '"' && ligne_csv[i_src+1] == '"'))) {
                if (ligne_csv[i_src] == '"' && ligne_csv[i_src+1] == '"') i_src++; // sauter le double guillemet
                i_src++;
            }
             if (entre_guillemets && ligne_csv[i_src] == '"') i_src++; // guillemet fermant
             if (ligne_csv[i_src] == ',') i_src++; // séparateur
            *index_ligne = i_src;
            //fprintf(stderr, "Avertissement: champ CSV tronqué.\n");
            return 1; // Champ lu, mais potentiellement tronqué
        }

        if (entre_guillemets) {
            if (ligne_csv[i_src] == '"') {
                if (ligne_csv[i_src + 1] == '"') { // C'est un double guillemet ""
                    champ_dest[i_dest++] = '"';
                    i_src += 2; // Avancer de deux caractères
                } else { // C'est le guillemet de fin de champ
                    i_src++; // Sauter le guillemet fermant
                    // S'attendre à une virgule ou fin de ligne après
                    // while (ligne_csv[i_src] == ' ') i_src++; // ignorer espaces après "
                    if (ligne_csv[i_src] == ',') i_src++; else if (ligne_csv[i_src] != '\0' && ligne_csv[i_src] != '\n' && ligne_csv[i_src] != '\r') {
                         // Caractères inattendus après le guillemet fermant et avant la virgule -> malformé
                         // Pour être robuste, on pourrait considérer le champ comme terminé.
                    }
                    goto fin_champ;
                }
            } else {
                champ_dest[i_dest++] = ligne_csv[i_src++];
            }
        } else { // Pas entre guillemets
            if (ligne_csv[i_src] == ',') {
                i_src++; // Sauter la virgule, fin du champ
                goto fin_champ;
            } else {
                // Les guillemets dans un champ non-entre-guillemets sont généralement une erreur de format
                // mais on peut les copier tels quels pour une certaine robustesse.
                 if (ligne_csv[i_src] == '"') {
                    // Ceci est problématique si non échappé et non au début.
                    // Pour l'instant, on le copie. Un parseur strict pourrait lever une erreur.
                 }
                champ_dest[i_dest++] = ligne_csv[i_src++];
            }
        }
    }

fin_champ:
    champ_dest[i_dest] = '\0';
    *index_ligne = i_src;
    return 1;
}
