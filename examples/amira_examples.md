# Cas de Test pour la Partie d'Amira (Opérations sur les Fichiers)

Ce document contient des cas de test conçus pour valider les fonctionnalités de la partie d'Amira dans le projet, qui concerne les opérations sur les fichiers. Chaque cas de test simule des scénarios spécifiques et vérifie que l'implémentation produit les résultats attendus.

## Cas de Test 1 : Recherche d'un Enregistrement

**Objectif :** Vérifier que la fonction `rechercherEnregistrement` trouve et affiche correctement un enregistrement par son ID.

- **Entrées :**
  - Nom du fichier : `test_file.dat`
  - ID de l'enregistrement : `1`
- **Préconditions :**
  - Le fichier `test_file.dat` contient un enregistrement avec l'ID `1`.
- **Étapes :**
  1. Appeler `rechercherEnregistrement("test_file.dat", 1)`.
- **Résultat Attendu :**
  - Sortie : `Enregistrement trouvé : ID: 1, Nom: Amira, Adresse: 16000 Alger`
  - La fonction se termine avec succès.

---

## Cas de Test 2 : Recherche d'un Enregistrement Inexistant

**Objectif :** Valider le comportement de la fonction `rechercherEnregistrement` lorsque l'ID de l'enregistrement n'existe pas.

- **Entrées :**
  - Nom du fichier : `test_file.dat`
  - ID de l'enregistrement : `0`
- **Préconditions :**
  - Le fichier `test_file.dat` ne contient pas d'enregistrement avec l'ID `0`.
- **Étapes :**
  1. Appeler `rechercherEnregistrement("test_file.dat", 0)`.
- **Résultat Attendu :**
  - Sortie : `Enregistrement avec l'ID 0 introuvable dans le fichier 'test_file.dat'.`
  - La fonction se termine sans erreurs.

---

## Cas de Test 3 : Suppression Logique d'un Enregistrement

**Objectif :** Vérifier que la fonction `supprimerEnregistrementLogique` marque correctement un enregistrement comme supprimé logiquement.

- **Entrées :**
  - Nom du fichier : `test_file.dat`
  - ID de l'enregistrement : `1`
- **Préconditions :**
  - Le fichier `test_file.dat` contient un enregistrement avec l'ID `1`.
- **Étapes :**
  1. Appeler `supprimerEnregistrementLogique("test_file.dat", 1)`.
  2. Vérifier que l'ID de l'enregistrement est mis à `-1` dans le fichier.
- **Résultat Attendu :**
  - Sortie : `Enregistrement avec l'ID 1 supprimé logiquement.`

---

## Cas de Test 4 : Suppression Logique d'un Enregistrement Inexistant

**Objectif :** Valider le comportement de `supprimerEnregistrementLogique` lorsque l'ID de l'enregistrement n'existe pas.

- **Entrées :**
  - Nom du fichier : `test_file.dat`
  - ID de l'enregistrement : `1`
- **Préconditions :**
  - Le fichier `test_file.dat` ne contient pas d'enregistrement avec l'ID `1`.
- **Étapes :**
  1. Appeler `supprimerEnregistrementLogique("test_file.dat", 1)`.
- **Résultat Attendu :**
  - Sortie : `Enregistrement avec l'ID 1 introuvable pour suppression logique.`

---

## Cas de Test 5 : Suppression Physique d'un Enregistrement

**Objectif :** Vérifier que la fonction `supprimerEnregistrementPhysique` supprime complètement un enregistrement du fichier.

- **Entrées :**
  - Nom du fichier : `test_file.dat`
  - ID de l'enregistrement : `1`
- **Préconditions :**
  - Le fichier `test_file.dat` contient un enregistrement avec l'ID `1`.
- **Étapes :**
  1. Appeler `supprimerEnregistrementPhysique("test_file.dat", 1)`.
  2. Vérifier que l'enregistrement n'est plus présent dans le fichier.
- **Résultat Attendu :**
  - Sortie : `Enregistrement avec l'ID 1 supprimé physiquement.`

---

## Cas de Test 6 : Suppression Physique d'un Enregistrement Inexistant

**Objectif :** Valider le comportement de `supprimerEnregistrementPhysique` lorsque l'ID de l'enregistrement n'existe pas.

- **Entrées :**
  - Nom du fichier : `test_file.dat`
  - ID de l'enregistrement : `3`
- **Préconditions :**
  - Le fichier `test_file.dat` ne contient pas d'enregistrement avec l'ID `3`.
- **Étapes :**
  1. Appeler `supprimerEnregistrementPhysique("test_file.dat", 3)`.
- **Résultat Attendu :**
  - Sortie : `Enregistrement avec l'ID 3 introuvable pour suppression physique.`

---


## Notes :
- Tous les cas de test supposent la disponibilité de données factices appropriées et une configuration préalable avant l'exécution.
- Veillez à effectuer un nettoyage des fichiers après chaque test pour maintenir un environnement isolé pour les tests suivants.
