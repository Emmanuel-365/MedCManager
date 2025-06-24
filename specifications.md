# Cahier des Charges - Système de Gestion Médicale

## Introduction
[Description introductive du projet visant à améliorer la gestion des patients, des médicaments et de la santé du personnel dans un contexte médical.]

## Objectifs
- Développer un système informatisé pour la gestion des consultations des patients.
- Gérer l'inventaire des médicaments avec suivi des stocks et dates de péremption.
- Surveiller l'état de santé du personnel avec mise à jour régulière des données.

## Utilisateurs
- Infirmières : Enregistrement des consultations, prescriptions.
- Responsables régionaux : Gestion des demandes et commandes.
- Responsables magasin : Suivi et restockage des médicaments.

## Fonctionnalités Requises
- **Gestion des Patients** : Ajout, recherche, modification des informations (nom, prénom, âge, maladie, etc.).
- **Gestion des Consultations** : Enregistrement avec date, heure d'arrivée/départ, motif.
- **Gestion des Médicaments** : Ajout, recherche, suppression, mise à jour des stocks et dates de péremption.
- **Alertes Automatiques** : Rappel en cas de rupture de stock ou date de péremption atteinte.
- **Gestion du Personnel** : Liste, informations personnelles, suivi médical.

## Contraintes Techniques
- Langage de programmation : C.
- Gestion manuelle de la mémoire (malloc/free).
- Pas d'accès à des bases de données externes ; stockage en mémoire ou fichiers texte.

## Modèle de Données Préliminaire
- **Structure Patient** : id_patient, nom, prénom, service, dossier_médical.
- **Structure Consultation** : id_consultation, date, heure_arrivee, heure_depart, motif, id_infirmiere, id_patient.
- **Structure Médicament** : id_medicament, nom, type, date_peremption, stock.

## Planning Prévisionnel
[À définir : étapes de développement, tests, déploiement.]

## Annexes
[Diagrammes MCD/MLD, exemples de structures C.]
