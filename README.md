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

Une notion qu'on a peu vue en cours et qui est au coeur du projet, ce sont les pointeurs intelligents. Dans `library.h`, la bibliothèque ne garde pas ses livres directement, mais des `unique_ptr` :

```cpp
vector<unique_ptr<Book>> books;
vector<unique_ptr<User>> users;
```

Un `unique_ptr` est le seul propriétaire de l'objet vers lequel il pointe, et il libère la mémoire tout seul quand on n'en a plus besoin. Par exemple, quand on supprime un livre avec `books.erase(it)`, l'objet est détruit automatiquement, sans `delete` à écrire. Ça évite les fuites de mémoire, qui sont une erreur classique en C++. Les livres sont créés avec `make_unique<Book>(book)`, et le reste du programme les manipule avec de simples `Book*` obtenus par `.get()`, sans jamais en devenir propriétaire.

### Question 2 : Options de développement possibles

Avec des millions de livres, les fichiers texte montrent vite leurs limites : tout est chargé en mémoire, chaque sauvegarde réécrit le fichier au complet et deux agents ne peuvent pas travailler en même temps. Je proposerais plutôt une base de données PostgreSQL, avec des index sur l'ISBN, le titre et l'auteur pour garder des recherches rapides, et des transactions pour éviter qu'un livre soit emprunté deux fois. Une API REST et une interface web permettraient ensuite aux agents d'utiliser le système depuis n'importe quel poste. Côté C++, on pourrait garder les classes `Library`, `Book` et `User` et remplacer `FileManager` par une couche qui communique avec PostgreSQL grâce à la bibliothèque libpqxx. Le tout pourrait être déployé avec Docker, comme notre dev container, pour faciliter la mise à l'échelle.
