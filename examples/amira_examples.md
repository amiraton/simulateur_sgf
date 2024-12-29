# Exemples pour la Partie d'Amira : Opérations sur les Fichiers

Ce document explique comment tester les fonctionnalités implémentées dans la partie d'Amira du projet.

---

## Fichiers d'Entrée

1. **`amira_file.dat`**
   - Un fichier binaire contenant 5 enregistrements. Chaque enregistrement est structuré comme suit :
     ```
     ID   Nom         Adresse
     -------------------------
     1    Alice       123 Rue Principale
     2    Bob         456 Rue des Ormes
     3    Charlie     789 Avenue des Érables
     4    Diana       101 Rue des Chênes
     5    Eve         202 Rue des Pins
     ```
   - Utilisez ce fichier pour tester les fonctionnalités de **recherche**, **suppression logique**, **suppression physique**, et **défragmentation**.

2. **`logically_deleted.dat`**
   - Identique à `amira_file.dat`, sauf que l'enregistrement avec l'ID `3` est supprimé logiquement (marqué avec l'ID `-1`).

3. **`physically_deleted.dat`**
   - Identique à `amira_file.dat`, sauf que l'enregistrement avec l'ID `3` est supprimé physiquement, laissant un espace dans le fichier.

4. **`defragmented.dat`**
   - Identique à `physically_deleted.dat`, mais les espaces sont supprimés, ne laissant que les enregistrements valides.

---

## Exemples de Tests

### 1. Recherche d'un Enregistrement

**Fichier d'Entrée** : `amira_file.dat`

**Commande dans le programme** :
```
Entrez le nom du fichier : amira_file.dat
Entrez l'ID de l'enregistrement à rechercher : 3
```

**Résultat Attendu** :
```
Enregistrement trouvé : ID : 3, Nom : Charlie, Adresse : 789 Avenue des Érables, Position : 2
```

**Fichier de Sortie** : `output/search_output.txt`

---

### 2. Suppression Logique

**Fichier d'Entrée** : `amira_file.dat`

**Commande dans le programme** :
```
Entrez le nom du fichier : amira_file.dat
Entrez l'ID de l'enregistrement à supprimer logiquement : 3
```

**Résultat Attendu** :
```
Enregistrement avec l'ID 3 supprimé logiquement.
```

**Fichier de Sortie** : `output/logical_delete_output.txt`

**Fichier Binaire Modifié** : `input/logically_deleted.dat`

---

### 3. Suppression Physique

**Fichier d'Entrée** : `logically_deleted.dat`

**Commande dans le programme** :
```
Entrez le nom du fichier : logically_deleted.dat
Entrez l'ID de l'enregistrement à supprimer physiquement : 3
```

**Résultat Attendu** :
```
Enregistrement avec l'ID 3 supprimé physiquement.
```

**Fichier de Sortie** : `output/physical_delete_output.txt`

**Fichier Binaire Modifié** : `input/physically_deleted.dat`

---

### 4. Défragmentation

**Fichier d'Entrée** : `physically_deleted.dat`

**Commande dans le programme** :
```
Entrez le nom du fichier à défragmenter : physically_deleted.dat
```

**Résultat Attendu** :
```
Défragmentation terminée. 4 enregistrements restent dans le fichier 'physically_deleted.dat'.
```

**Fichier de Sortie** : `output/defragmentation_output.txt`

**Fichier Binaire Modifié** : `input/defragmented.dat`

---

### 5. Renommage d'un Fichier

**Fichier d'Entrée** : `amira_file.dat`

**Commande dans le programme** :
```
Entrez le nom actuel du fichier : amira_file.dat
Entrez le nouveau nom du fichier : renamed_file.dat
```

**Résultat Attendu** :
```
Fichier 'amira_file.dat' renommé avec succès en 'renamed_file.dat'.
Métadonnées mises à jour pour le fichier renommé 'renamed_file.dat'.
```

**Fichier de Sortie** : `output/rename_output.txt`

**Fichier Renommé** : `input/renamed_file.dat`

---

## Instructions pour Exécuter les Tests

1. Placez les fichiers `.dat` dans le répertoire `input/`.
2. Compilez et exécutez le programme principal avec `simulateur.exe`.
3. Suivez les commandes mentionnées ci-dessus pour chaque test.
4. Vérifiez les résultats dans le répertoire `output/`.

## Création des Fichiers d'Exemple

Pour générer les fichiers `.dat`, utilisez le script suivant :

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    char address[100];
} Record;

void createExampleFile(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    Record records[] = {
        {1, "Amira", "123 Rue Principale"}
        {2, "Nadia", "456 Rue des Ormes"},
        {3, "Souriya", "789 Avenue des Érables"},
        {4, "Lamis", "101 Rue des Chênes"},
        {5, "Rodayna", "202 Rue des Pins"}
    };

    for (int i = 0; i < 5; i++) {
        fwrite(&records[i], sizeof(Record), 1, file);
    }

    fclose(file);
}

int main() {
    createExampleFile("amira_file.dat");
    return 0;
}
```

Compilez et exécutez ce script pour générer `amira_file.dat`. Vous pouvez le modifier pour produire d'autres fichiers comme `logically_deleted.dat` ou `physically_deleted.dat`.

---
