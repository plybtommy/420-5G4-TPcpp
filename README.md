# TP C++ — Système de gestion de bibliothèque

**Auteur :** Tommy Rouleau

Reprise d'un logiciel de gestion des usagers et des livres d'une bibliothèque : complétion du code manquant, correction d'un bug et ajout de nouvelles fonctionnalités.

## Table des matières

- [Construire le projet](#construire-le-projet)
- [Utilisation](#utilisation)
- [Travail réalisé](#travail-réalisé)
- [Veille technologique](#veille-technologique)

## Construire le projet

Le projet utilise dev container C++ de VS Code (`.devcontainer/devcontainer.json`) qui inclut déjà `cmake` et `g++`. Ouvrir le dossier dans VS Code puis *Reopen in Container*.

```bash
mkdir build
cd build
cmake ..
make
```

## Utilisation

```bash
./bibliotheque --data-dir ../data
```

| Option | Description |
| --- | --- |
| `-d`, `--data-dir <répertoire>` | Répertoire contenant `books.txt` et `users.txt` (chargés au démarrage, sauvegardés à la sortie). |

Le répertoire `data` contient des fichiers d'exemple (`books.txt`, `users.txt`) pour tester l'application.

## Travail réalisé

### Code manquant

- `book.cpp` recréé à partir de `book.h` : constructeurs, accesseurs, emprunt/retour, affichage (`toString`) et lecture/écriture au format fichier `titre|auteur|isbn|disponible|idEmprunteur`.

### Fonctionnalités choisies

| Catégorie | Fonctionnalité |
| --- | --- |
| Interface et expérience utilisateur | Afficher le nom d'utilisateur plutôt que l'id dans l'affichage des livres (ex. `Emprunté par : Marie Dubois`). |
| Gestion des données | Tri par titre ou par auteur avec `std::sort` pour l'affichage des livres et les résultats de recherche. |
| Journal d'activités | Fichier `activites.log` créé s'il n'existe pas et conservé d'une session à l'autre. |

Exemple de journal :

```
2026-10-12 21:20:50 - [AJOUT LIVRE] L'Assommoir|Victor Hugo|9782253090275|1|
2026-10-12 21:20:50 - [EMPRUNT] L'Assommoir|Victor Hugo|9782253090275|0|Marie Dubois
2026-10-12 21:20:50 - [RETOUR] Les Misérables|Victor Hugo|9782253096337|1|Marie Dubois
2026-10-12 21:20:50 - [AJOUT UTILISATEUR] Thomas Piquet|USR420
2026-10-12 21:20:50 - [SUPPRESSION LIVRE] Candide|Voltaire|9782070380015|1|
```

### Correction de bug

- Sauvegarde automatique à la sortie sans répertoire de données : sans `--data-dir`, `saveLibraryData` essayait d'écrire dans `""` et échouait silencieusement. La sauvegarde est maintenant ignorée si les chemins sont vides et un message explicite est affiché.

## Veille technologique

### Question 1 : C++ — les pointeurs intelligents (`unique_ptr`)

Une notion qu'on a peu vue en cours et qui est beaucoup utilisé dansle projet, ce sont les pointeurs intelligents. Dans `library.h`, la bibliothèque ne garde pas ses livres directement, mais des pointeurs `unique_ptr` :

```cpp
vector<unique_ptr<Book>> books;
vector<unique_ptr<User>> users;
```

Normalement en C++, quand on crée un objet avec `new`, il faut penser à le supprimer soi-même avec `delete`. Si on oublie, la mémoire n'est jamais libérée et le programme utilise de plus en plus de RAM : c'est une fuite de mémoire. Un `unique_ptr` règle ce problème : c'est un pointeur qui fait le `delete` à notre place, automatiquement, quand on n'a plus besoin de l'objet. Dans le projet, un livre est créé avec `make_unique<Book>(book)` au lieu de `new`. Pour le supprimer, `removeBook` fait seulement `books.erase(it)` : le livre est retiré de la liste et sa mémoire est libérée toute seule, sans aucun `delete`. Le mot « unique » veut dire qu'un seul pointeur est responsable de chaque livre, ce qui évite de le supprimer deux fois par erreur.

### Question 2 : Options de développement possibles

Aujourd'hui, le programme garde ses livres dans un simple fichier texte (`books.txt`). Ça marche pour 40 livres, mais pas pour une bibliothèque avec des millions de livres : il faudrait charger tout le fichier en mémoire à chaque démarrage, une recherche devrait lire les livres un par un, et deux employés ne pourraient pas travailler en même temps sans interférer avec l'autre. Je proposerais plutôt d'utiliser une base de données MySQL. Une base de données est faite pour ça : elle range les livres dans des tables, retrouve un livre très vite grâce à des index (comme l'index à la fin d'un livre), et gère plusieurs utilisateurs en même temps sans perdre de données. On pourrait aussi ajouter un site web pour que les employés utilisent le système depuis n'importe quel ordinateur. Pour le lien avec le C++, on garderait nos classes `Library`, `Book` et `User`, et on remplacerait seulement `FileManager` (qui lit et écrit `books.txt`) par du code qui parle à MySQL grâce à la bibliothèque MySQL Connector/C++. MySQL pourrait tourner dans un conteneur Docker, comme on l'a vu en cours.
