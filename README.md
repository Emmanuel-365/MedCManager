# Système de Gestion Médicale en Console

## Description
Ce projet est une application en console développée en C pour la gestion simplifiée des aspects d'un environnement médical. Elle permet de gérer les patients, les médicaments (avec stock et péremption), le personnel soignant, et les consultations. L'application intègre un système de profils utilisateurs (Infirmière, Responsable Magasin, Administrateur) avec des droits d'accès différenciés aux fonctionnalités, ainsi qu'un système de persistance des données via des fichiers CSV. Des alertes automatiques sont générées pour les stocks de médicaments bas et les dates de péremption proches ou dépassées.

## Prérequis
*   Un compilateur C (GCC est recommandé et utilisé dans le Makefile).
*   L'utilitaire `make` pour la compilation via le Makefile fourni.

## Compilation
Pour compiler l'application, naviguez jusqu'au répertoire racine du projet dans un terminal et exécutez la commande :
```bash
make
```
Cela générera un exécutable nommé `gestion_medicale` (ou `gestion_medicale.exe` sous Windows avec MinGW).

Pour nettoyer les fichiers compilés (fichiers objets et exécutable) :
```bash
make clean
```

## Exécution
Après compilation, lancez l'application avec :
```bash
./gestion_medicale
```
Sous Windows (si compilé avec MinGW) :
```bash
gestion_medicale.exe
```

## Fonctionnement Général

### Connexion et Utilisateurs
Au premier lancement, l'application ne trouvera pas de fichier d'utilisateurs (`users.dat`) et créera automatiquement un compte administrateur par défaut :
*   **Login :** `admin`
*   **Mot de passe :** `admin`

Il est fortement recommandé de changer ce mot de passe dès la première connexion via le menu de gestion des utilisateurs (accessible à l'admin).

L'administrateur peut ensuite créer d'autres utilisateurs avec des profils spécifiques. Le mot de passe est "haché" de manière simple (NON SÉCURISÉ POUR UN USAGE EN PRODUCTION) avant d'être stocké.

### Profils Utilisateurs
L'application supporte les profils suivants, avec des menus et des droits adaptés :
1.  **Administrateur :**
    *   Accès complet à toutes les fonctionnalités.
    *   Gestion des utilisateurs (création, suppression, modification de profil/mot de passe).
2.  **Infirmière / Infirmier :**
    *   Gestion des Patients (CRUD complet).
    *   Gestion des Consultations (CRUD complet).
    *   Visualisation des Médicaments et de leurs détails.
    *   Vérification des alertes médicaments.
3.  **Responsable Magasin :**
    *   Gestion complète des Médicaments (CRUD, y compris stock, date de péremption, seuil d'alerte).
    *   Vérification des alertes médicaments.
4.  **Responsable Régional :**
    *   Actuellement, ce profil a un accès limité. Il peut visualiser la liste des médicaments et les alertes.
    *   Les fonctionnalités spécifiques ("Gestion des demandes et commandes") ne sont pas implémentées.

### Fonctionnalités Principales
*   **Gestion des Patients :** Ajouter, rechercher, modifier, afficher les informations des patients.
*   **Gestion des Médicaments :** Ajouter, rechercher, modifier (stock, date péremption, seuil alerte), afficher les médicaments.
*   **Gestion du Personnel :** (Accessible à l'Admin) Ajouter, rechercher, modifier, afficher les membres du personnel.
*   **Gestion des Consultations :** Enregistrer, rechercher (par patient, par date), afficher les consultations.
*   **Alertes Médicaments :**
    *   Notification des stocks inférieurs à un seuil défini.
    *   Notification des médicaments périmés ou dont la date de péremption est proche (configurable, 30 jours par défaut).
    *   Les alertes sont affichées au démarrage (pour les profils concernés) et via une option de menu.
*   **Persistance des Données :** Toutes les données (patients, médicaments, personnel, consultations, utilisateurs) sont sauvegardées dans des fichiers `.dat` au format CSV dans le répertoire de l'exécutable. La sauvegarde est automatique à la fermeture de l'application et peut aussi être déclenchée manuellement.

## Format des Fichiers de Données (.dat)
Tous les fichiers de données utilisent un format CSV simple. Les chaînes de caractères contenant des virgules ou des guillemets sont entourées de guillemets doubles (`"`), et les guillemets doubles internes sont échappés en les doublant (`""`).

*   **`users.dat`**: `id_utilisateur,login,mot_de_passe_hash,profil_id`
*   **`patients.dat`**: `id_patient,nom,prenom,age,service,dossier_medical`
*   **`medicaments.dat`**: `id_medicament,nom,type,jour_peremption,mois_peremption,annee_peremption,stock,seuil_stock_alerte`
*   **`personnel.dat`**: `id_personnel,nom,prenom,poste,contact_info,suivi_medical_details`
*   **`consultations.dat`**: `id_consultation,jour_consult,mois_consult,annee_consult,heure_arrivee,minute_arrivee,heure_depart,minute_depart,motif,id_patient,id_personnel_soignant`

## Limitations Connues
*   **Hachage des Mots de Passe :** Le mécanisme de hachage des mots de passe est très simple et NON SÉCURISÉ pour une application en production. Il sert uniquement à éviter le stockage en clair dans le cadre de cet exercice.
*   **Gestion d'erreurs :** Bien que des vérifications de base soient en place, la gestion des erreurs de saisie et de fichiers pourrait être plus exhaustive.
*   **Interface Utilisateur :** L'interface est purement textuelle et basée sur des menus en console.
*   **Taille des Données :** L'application utilise des tableaux de taille fixe en mémoire pour stocker les données. Si le nombre d'enregistrements dépasse ces limites (ex: `MAX_PATIENTS_EN_MEMOIRE`), des problèmes surviendront. Une gestion dynamique de la mémoire (listes chaînées, réallocation) serait nécessaire pour une plus grande capacité.
*   **Pas de suppression pour Patients, Médicaments, Personnel, Consultations :** La fonctionnalité de suppression n'a pas été implémentée pour ces entités (sauf pour les Utilisateurs via l'admin).

Ce projet a été développé dans un but éducatif pour illustrer les concepts de base de la programmation en C, la manipulation de fichiers, et la structuration d'une application simple.
```
