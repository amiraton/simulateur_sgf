# Simulateur Simplifié d’un Système de Gestion de Fichiers (SGF)

## Introduction

Ce projet est un simulateur simplifié pour un **Système de Gestion de Fichiers (SGF)**. Le simulateur gère les principales opérations sur les fichiers, telles que la recherche d'enregistrements, la suppression logique, la suppression physique, la défragmentation des fichiers et le renommage de fichiers, etc. Le système est conçu pour imiter les systèmes de fichiers et la gestion de stockage dans le monde réel.

## Structure du Projet

Le projet est organisé comme suit :

```
Simulateur_SGF/
├── examples/        # Contient des fichiers d'exemple pour tester les fonctionnalités.
├── rapport/         # Inclut le rapport du projet détaillant l'implémentation.
├── src/             # Fichiers source pour le simulateur SGF.
├── LICENSE          # Licence du projet.
└── README.md        # Ce fichier README.
```

### Fichiers et Répertoires Clés

- **`examples/`** : Fournit des données et des scénarios d'exemple pour tester les différentes fonctions implémentées dans le projet.
- **`rapport/`** : Contient la documentation détaillée du projet, incluant les pseudocodes, des exemples et une analyse des fonctions.
- **`src/`** : Inclut le fichier source contenant les fonctionnalités du système.

## Comment Exécuter le Code

1. **Prérequis :**
   - Un compilateur C (par exemple, GCC).
   - Une interface en ligne de commande ou un terminal.

2. **Compilation :**
   Naviguez vers le répertoire `src` et compilez le code avec la commande suivante :
   ```bash
   gcc -o simulateur_sgf simulateur.c -lm
   ```

3. **Exécution :**
   Exécutez le programme compilé avec :
   ```bash
   ./simulateur_sgf
   ```

4. **Tests :**
   Utilisez les données d'exemple fournies dans le répertoire `examples/` pour tester les différentes fonctionnalités.

## Licence

Ce projet est sous licence MIT. Consultez le fichier `LICENSE` pour plus de détails.
