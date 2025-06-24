#include "utilisateur.h"
#include "csv_utils.h" // Pour la persistance
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_UTILISATEURS 50 // Nombre maximum d'utilisateurs stockables
Utilisateur liste_utilisateurs[MAX_UTILISATEURS];
int nombre_utilisateurs = 0;
int prochain_id_utilisateur = 1;

const char* FICHIER_UTILISATEURS = "users.dat"; // Fichier de persistance

// Fonction de hachage simple (NON SÉCURISÉE POUR PRODUCTION)
// Transforme une chaîne en une autre chaîne hexadécimale de HASH_SIZE-1 caractères.
void hasher_mot_de_passe_simple(const char* mot_de_passe, char* hash_resultat) {
    if (!mot_de_passe || !hash_resultat) {
        if (hash_resultat) hash_resultat[0] = '\0';
        return;
    }
    if (strlen(mot_de_passe) == 0) { // Gérer mot de passe vide
        strncpy(hash_resultat, "empty_password_hash_placeholder", HASH_SIZE -1); // Placeholder pour mdp vide
        hash_resultat[HASH_SIZE-1] = '\0';
        return;
    }

    char temp_hash[HASH_SIZE] = {0};
    const char* key = "aBcDeFgHiJkLmNoPqRsTuVwXyZ1234567890!@#$%^&*()_+KeyForProject"; // Clé simple un peu plus longue
    size_t key_len = strlen(key);
    size_t mdp_len = strlen(mot_de_passe);
    size_t i = 0; // Changé en size_t

    for (i = 0; i < HASH_SIZE - 1; ++i) {
        if (i < mdp_len) {
            temp_hash[i] = mot_de_passe[i] ^ key[i % key_len];
        } else {
            // Remplir le reste du hash si le mot de passe est plus court
            temp_hash[i] = ('A' + (i % 26)) ^ key[i % key_len];
        }
    }
    temp_hash[HASH_SIZE - 1] = '\0';

    // S'assurer que les caractères du hash sont imprimables et utilisables en CSV
    for (i = 0; i < HASH_SIZE - 1; ++i) {
        if (temp_hash[i] < 33 || temp_hash[i] > 126 || temp_hash[i] == ',' || temp_hash[i] == '"' || temp_hash[i] == '\n' || temp_hash[i] == '\r' || temp_hash[i] == '\\') {
            // Remplacer par une lettre minuscule cyclique si non imprimable/problematique
            hash_resultat[i] = ((temp_hash[i] & 0x0F) % 26) + 'a';
        } else {
            hash_resultat[i] = temp_hash[i];
        }
    }
    hash_resultat[HASH_SIZE - 1] = '\0';
}


void actualiser_prochain_id_utilisateur() {
    int max_id = 0;
    for (int i = 0; i < nombre_utilisateurs; i++) {
        if (liste_utilisateurs[i].id_utilisateur > max_id) {
            max_id = liste_utilisateurs[i].id_utilisateur;
        }
    }
    prochain_id_utilisateur = max_id + 1;
}

// Ajoute un utilisateur. Si charger_depuis_fichier est vrai, mot_de_passe est déjà un hash.
int ajouter_utilisateur_interne(const char* login, const char* mot_de_passe_ou_hash, ProfilUtilisateur profil, int est_deja_hash) {
    if (nombre_utilisateurs >= MAX_UTILISATEURS) {
        printf("Erreur: Nombre maximum d'utilisateurs atteint.\n");
        return 0;
    }
    if (rechercher_utilisateur_par_login(login) != NULL) {
        printf("Erreur: Le login '%s' existe déjà.\n", login);
        return 0;
    }

    Utilisateur nouv_utilisateur;
    nouv_utilisateur.id_utilisateur = prochain_id_utilisateur++;
    strncpy(nouv_utilisateur.login, login, MAX_LOGIN_SIZE - 1);
    nouv_utilisateur.login[MAX_LOGIN_SIZE - 1] = '\0';

    if (est_deja_hash) {
        strncpy(nouv_utilisateur.mot_de_passe_hash, mot_de_passe_ou_hash, HASH_SIZE - 1);
    } else {
        hasher_mot_de_passe_simple(mot_de_passe_ou_hash, nouv_utilisateur.mot_de_passe_hash);
    }
    nouv_utilisateur.mot_de_passe_hash[HASH_SIZE - 1] = '\0';
    nouv_utilisateur.profil = profil;

    liste_utilisateurs[nombre_utilisateurs++] = nouv_utilisateur;
    return 1;
}

int ajouter_utilisateur_par_admin(const char* login, const char* mot_de_passe, ProfilUtilisateur profil) {
    if (profil_actuel != PROFIL_ADMINISTRATEUR) {
        printf("Accès refusé. Seul l'administrateur peut ajouter des utilisateurs.\n");
        return 0;
    }
    int resultat = ajouter_utilisateur_interne(login, mot_de_passe, profil, 0);
    if (resultat) {
        printf("Utilisateur '%s' ajouté avec le profil %d.\n", login, profil);
        sauvegarder_utilisateurs_fichier(); // Sauvegarder immédiatement
    }
    return resultat;
}


Utilisateur* rechercher_utilisateur_par_login(const char* login) {
    for (int i = 0; i < nombre_utilisateurs; i++) {
        if (strcmp(liste_utilisateurs[i].login, login) == 0) {
            return &liste_utilisateurs[i];
        }
    }
    return NULL;
}

int verifier_mot_de_passe(const Utilisateur* utilisateur, const char* mot_de_passe_clair) {
    if (!utilisateur) return 0;
    char hash_a_verifier[HASH_SIZE];
    hasher_mot_de_passe_simple(mot_de_passe_clair, hash_a_verifier);
    return strcmp(utilisateur->mot_de_passe_hash, hash_a_verifier) == 0;
}

ProfilUtilisateur login_utilisateur() {
    char login_saisi[MAX_LOGIN_SIZE];
    char mdp_saisi[100]; // Buffer pour mot de passe clair
    int tentatives = 0;
    const int MAX_TENTATIVES = 3;

    printf("\n--- Connexion ---\n");
    while (tentatives < MAX_TENTATIVES) {
        printf("Login : ");
        if (fgets(login_saisi, sizeof(login_saisi), stdin) == NULL) return PROFIL_NON_DEFINI; // Erreur de lecture
        login_saisi[strcspn(login_saisi, "\n")] = 0; // Enlever newline

        printf("Mot de passe : ");
        // Masquer le mot de passe n'est pas standard/simple en C console sans bibliothèques.
        // Pour cet exercice, on le lit en clair.
        if (fgets(mdp_saisi, sizeof(mdp_saisi), stdin) == NULL) return PROFIL_NON_DEFINI;
        mdp_saisi[strcspn(mdp_saisi, "\n")] = 0;

        Utilisateur* utilisateur = rechercher_utilisateur_par_login(login_saisi);
        if (utilisateur && verifier_mot_de_passe(utilisateur, mdp_saisi)) {
            printf("Connexion réussie. Profil : %d\n", utilisateur->profil);
            profil_actuel = utilisateur->profil; // Mettre à jour le profil global
            return utilisateur->profil;
        } else {
            printf("Login ou mot de passe incorrect.\n");
            tentatives++;
            printf("%d tentatives restantes.\n", MAX_TENTATIVES - tentatives);
        }
    }
    printf("Trop de tentatives échouées. Fermeture.\n");
    return PROFIL_NON_DEFINI; // Échec de la connexion
}

void sauvegarder_utilisateurs_fichier() {
    FILE* fichier = fopen(FICHIER_UTILISATEURS, "w");
    if (!fichier) {
        perror("Erreur sauvegarde utilisateurs.dat");
        return;
    }
    for (int i = 0; i < nombre_utilisateurs; i++) {
        ecrire_champ_csv_int(fichier, liste_utilisateurs[i].id_utilisateur);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_utilisateurs[i].login);
        fprintf(fichier, ",");
        ecrire_champ_csv_string(fichier, liste_utilisateurs[i].mot_de_passe_hash); // Le hash est déjà une chaîne
        fprintf(fichier, ",");
        ecrire_champ_csv_int(fichier, (int)liste_utilisateurs[i].profil);
        fprintf(fichier, "\n");
    }
    fclose(fichier);
    // printf("%d utilisateurs sauvegardés dans %s\n", nombre_utilisateurs, FICHIER_UTILISATEURS);
}

void charger_utilisateurs_fichier() {
    FILE* fichier = fopen(FICHIER_UTILISATEURS, "r");
    if (!fichier) {
        printf("Fichier %s non trouvé. Création de l'admin par défaut.\n", FICHIER_UTILISATEURS);
        // Ajouter admin par défaut si le fichier n'existe pas
        ajouter_utilisateur_interne("admin", "admin", PROFIL_ADMINISTRATEUR, 0); // 0 car "admin" n'est pas un hash
        sauvegarder_utilisateurs_fichier(); // Sauvegarder l'admin par défaut
        return;
    }

    char ligne[512];
    nombre_utilisateurs = 0;
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (nombre_utilisateurs >= MAX_UTILISATEURS) break;

        Utilisateur u;
        int index_ligne = 0;
        char temp_champ[HASH_SIZE]; // Assez pour le hash ou login

        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        u.id_utilisateur = atoi(temp_champ);

        if (!lire_champ_csv(ligne, &index_ligne, u.login, MAX_LOGIN_SIZE)) continue;
        if (!lire_champ_csv(ligne, &index_ligne, u.mot_de_passe_hash, HASH_SIZE)) continue;

        if (!lire_champ_csv(ligne, &index_ligne, temp_champ, sizeof(temp_champ))) continue;
        u.profil = (ProfilUtilisateur)atoi(temp_champ);

        if (u.id_utilisateur > 0 && u.profil > PROFIL_NON_DEFINI && u.profil <= PROFIL_ADMINISTRATEUR) {
             // On ne re-hashe pas, car il est déjà hashé dans le fichier
            liste_utilisateurs[nombre_utilisateurs++] = u;
        }
    }
    fclose(fichier);
    actualiser_prochain_id_utilisateur();
    if (nombre_utilisateurs == 0) { // Si le fichier était vide ou malformé
        printf("Aucun utilisateur valide trouvé dans %s. Création de l'admin par défaut.\n", FICHIER_UTILISATEURS);
        ajouter_utilisateur_interne("admin", "admin", PROFIL_ADMINISTRATEUR, 0);
        sauvegarder_utilisateurs_fichier();
    }
    // printf("%d utilisateurs chargés depuis %s\n", nombre_utilisateurs, FICHIER_UTILISATEURS);
}

void initialiser_utilisateurs() {
    charger_utilisateurs_fichier();
}

void afficher_tous_les_utilisateurs() {
    if (profil_actuel != PROFIL_ADMINISTRATEUR) {
        printf("Accès refusé.\n");
        return;
    }
    printf("\n--- Liste des Utilisateurs ---\n");
    for (int i = 0; i < nombre_utilisateurs; i++) {
        printf("ID: %d, Login: %s, Profil: %d (Hash: ...)\n", // Ne pas afficher le hash
               liste_utilisateurs[i].id_utilisateur,
               liste_utilisateurs[i].login,
               liste_utilisateurs[i].profil);
    }
    printf("------------------------------\n");
}

int supprimer_utilisateur_par_login(const char* login) {
    if (profil_actuel != PROFIL_ADMINISTRATEUR) {
        printf("Accès refusé.\n"); return 0;
    }
    if (strcmp(login, "admin") == 0) {
        printf("L'utilisateur 'admin' ne peut pas être supprimé.\n"); return 0;
    }
    for (int i = 0; i < nombre_utilisateurs; i++) {
        if (strcmp(liste_utilisateurs[i].login, login) == 0) {
            for (int j = i; j < nombre_utilisateurs - 1; j++) {
                liste_utilisateurs[j] = liste_utilisateurs[j+1];
            }
            nombre_utilisateurs--;
            printf("Utilisateur '%s' supprimé.\n", login);
            sauvegarder_utilisateurs_fichier();
            return 1;
        }
    }
    printf("Utilisateur '%s' non trouvé.\n", login);
    return 0;
}

int modifier_profil_utilisateur(const char* login, ProfilUtilisateur nouveau_profil) {
     if (profil_actuel != PROFIL_ADMINISTRATEUR) {
        printf("Accès refusé.\n"); return 0;
    }
    if (nouveau_profil <= PROFIL_NON_DEFINI || nouveau_profil > PROFIL_ADMINISTRATEUR) {
        printf("Profil invalide.\n"); return 0;
    }
    if (strcmp(login, "admin") == 0 && nouveau_profil != PROFIL_ADMINISTRATEUR) {
        printf("Le profil de 'admin' ne peut pas être changé.\n"); return 0;
    }
    Utilisateur* u = rechercher_utilisateur_par_login(login);
    if (u) {
        u->profil = nouveau_profil;
        printf("Profil de '%s' mis à jour.\n", login);
        sauvegarder_utilisateurs_fichier();
        return 1;
    }
    printf("Utilisateur '%s' non trouvé.\n", login);
    return 0;
}

int changer_mot_de_passe_par_admin(const char* login_cible, const char* nouveau_mot_de_passe) {
    if (profil_actuel != PROFIL_ADMINISTRATEUR) {
        printf("Accès refusé.\n"); return 0;
    }
     Utilisateur* u = rechercher_utilisateur_par_login(login_cible);
    if (u) {
        hasher_mot_de_passe_simple(nouveau_mot_de_passe, u->mot_de_passe_hash);
        printf("Mot de passe pour '%s' changé par l'admin.\n", login_cible);
        sauvegarder_utilisateurs_fichier();
        return 1;
    }
    printf("Utilisateur '%s' non trouvé.\n", login_cible);
    return 0;
}
