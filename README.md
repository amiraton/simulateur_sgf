# Projet: Simulateur Simplifié d’un Système de Gestion de Fichiers (SGF)

Bienvenue dans le projet **Simulateur Simplifié d'un Système de Gestion de Fichiers (SGF)**. Ce projet est la collaboration entre plusieurs étudiants, chacun ayant contribué à une partie spécifique. Ce document explique le fonctionnement, les objectifs, et les détails techniques du projet.

## Objectif du Projet

L'objectif de ce projet est de concevoir un simulateur de système de gestion de fichiers qui simule les principales fonctionnalités d'un système de fichiers, telles que la gestion des fichiers, des métadonnées, et des opérations sur les fichiers. Ce projet permet de comprendre les principes fondamentaux des systèmes de gestion de fichiers, tout en offrant une interface utilisateur claire et facile à utiliser.

---

## Structure du Projet

### Contributions des Étudiants

- **Amira** : Opérations sur les Fichiers
---

## Fonctionnalités Principales

### 3. **Opérations sur les Fichiers (Amira)**
- Recherche d'un enregistrement par ID.
- Suppression logique d'un enregistrement.
- Suppression physique d'un enregistrement avec réorganisation des fichiers.
- Défragmentation d'un fichier.
- Renommage de fichiers.

---

## Instructions d'Installation et d'Exécution

### Prérequis
- **Compilateur GCC** : Pour compiler le code C.
- **Git** : Pour cloner et gérer le dépôt GitHub.

### Étapes
1. **Cloner le dépôt** :
   ```bash
   git clone <URL-du-depot-GitHub>
   cd simulateur_sgf
   ```

2. **Compiler le programme** :
   ```bash
   gcc -o simulateur simulateur.c
   ```

3. **Exécuter le programme** :
   ```bash
   ./simulateur
   ```

---

## Structure des Répertoires

- **/src** : Contient le code source principal du projet.
- **/examples** : Contient des exemples d'utilisation des fonctionnalités.
- **/docs** : Documentation détaillée du projet.
- **/README.md** : Ce fichier, qui explique le projet.

---

## Exemples d'Utilisation

### Exemple : Recherche d'un Enregistrement
- **Commandes** :
  1. Créez un fichier avec des enregistrements.
  2. Sélectionnez l'option de recherche dans le menu principal.
  3. Entrez l'ID à rechercher.
- **Résultat attendu** : Affichage des détails de l'enregistrement ou un message indiquant qu'il n'existe pas.

### Exemple : Défragmentation d'un Fichier
- **Commandes** :
  1. Supprimez des enregistrements logiquement.
  2. Sélectionnez l'option de défragmentation.
  3. Le fichier est réorganisé avec uniquement les enregistrements valides.
- **Résultat attendu** : Affichage du nombre d'enregistrements restants après la défragmentation.

---

## Suivi Git et Collaboration

Le projet utilise **GitHub** pour le suivi des modifications, la collaboration entre les étudiants, et la gestion des versions. Toutes les contributions sont enregistrées et peuvent être revues pour assurer la qualité du code.

### Lien du Dépôt GitHub
[Accéder au projet sur GitHub](<URL-du-depot>)

---

## Remerciements

Merci à tous les étudiants qui ont contribué à ce projet. Ce simulateur est le résultat d'un effort collectif pour apprendre et appliquer les concepts des systèmes de gestion de fichiers.

---

## Contact

Pour toute question ou problème, veuillez contacter : **chiraz1azer@gmail.com**
