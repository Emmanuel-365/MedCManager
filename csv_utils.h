#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stdio.h>

// Écrit une chaîne de caractères dans un fichier CSV.
// Gère l'échappement des guillemets doubles et entoure la chaîne de guillemets si elle contient des virgules,
// des guillemets, ou des sauts de ligne (bien que les sauts de ligne dans les champs CSV soient complexes et évités ici).
void ecrire_champ_csv_string(FILE* fichier, const char* chaine);

// Écrit un entier dans un fichier CSV.
void ecrire_champ_csv_int(FILE* fichier, int valeur);

// Lit un champ depuis une ligne CSV bufferisée.
// La fonction avance le pointeur `index_ligne` pour la prochaine lecture.
// Retourne 1 si un champ a été lu, 0 si fin de ligne ou erreur.
// `champ_dest` est le buffer où le champ est copié.
// `max_len` est la taille maximale de `champ_dest`.
// Gère les champs entre guillemets et l'échappement des guillemets.
int lire_champ_csv(const char* ligne_csv, int* index_ligne, char* champ_dest, int max_len);

#endif // CSV_UTILS_H
