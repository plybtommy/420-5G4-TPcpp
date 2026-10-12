# TP C++ — Système de gestion de bibliothèque

**Auteur :** Tommy Rouleau

Reprise d'un logiciel de gestion des usagers et des livres d'une bibliothèque : complétion du code manquant, correction d'un bug et ajout de nouvelles fonctionnalités.

## Table des matières

- [Construire le projet](#construire-le-projet)
- [Utilisation](#utilisation)
- [Travail réalisé](#travail-réalisé)
- [Veille technologique](#veille-technologique)

## Construire le projet

Le projet utilise le **dev container C++** de VS Code (`.devcontainer/devcontainer.json`) qui inclut déjà `cmake` et `g++`. Ouvrir le dossier dans VS Code puis *Reopen in Container*.

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

- **`book.cpp`** recréé à partir de `book.h` : constructeurs, accesseurs, emprunt/retour, affichage (`toString`) et lecture/écriture au format fichier `titre|auteur|isbn|disponible|idEmprunteur`.

### Fonctionnalités choisies

| Catégorie | Fonctionnalité |
| --- | --- |
| Interface et expérience utilisateur | **Afficher le nom d'utilisateur plutôt que l'id** dans l'affichage des livres (ex. `Emprunté par : Marie Dubois`). |
| Gestion des données | **Tri par titre ou par auteur** avec `std::sort` pour l'affichage des livres et les résultats de recherche. |
| Journal d'activités | Fichier **`activites.log`** créé s'il n'existe pas et conservé d'une session à l'autre. |

Exemple de journal :

```
2026-10-12 21:20:50 - [AJOUT LIVRE] L'Assommoir|Victor Hugo|9782253090275|1|
2026-10-12 21:20:50 - [EMPRUNT] L'Assommoir|Victor Hugo|9782253090275|0|Marie Dubois
2026-10-12 21:20:50 - [RETOUR] Les Misérables|Victor Hugo|9782253096337|1|Marie Dubois
2026-10-12 21:20:50 - [AJOUT UTILISATEUR] Thomas Piquet|USR420
2026-10-12 21:20:50 - [SUPPRESSION LIVRE] Candide|Voltaire|9782070380015|1|
```

### Correction de bug

- **Sauvegarde automatique à la sortie sans répertoire de données** : sans `--data-dir`, `saveLibraryData` essayait d'écrire dans `""` et échouait silencieusement. La sauvegarde est maintenant ignorée si les chemins sont vides et un message explicite est affiché.

## Veille technologique

### Question 1 : C++ — les pointeurs intelligents (`std::unique_ptr`) et les lambdas avec les algorithmes de la STL

La classe `Library` ne stocke pas directement des `Book`, mais des `std::unique_ptr<Book>` :

```cpp
class Library {
private:
    vector<unique_ptr<Book>> books;
    vector<unique_ptr<User>> users;
```

Un `unique_ptr` est un *pointeur intelligent* : il possède l'objet alloué sur le tas et le détruit automatiquement (`delete`) quand il sort de portée. C'est l'idiome **RAII** (*Resource Acquisition Is Initialization*) : la durée de vie de la ressource est liée à celle d'un objet. Ici, quand un livre est retiré du vecteur avec `books.erase(it)`, ou quand la `Library` est détruite, la mémoire est libérée sans aucun `delete` manuel, ce qui évite les fuites de mémoire. Comme son nom l'indique, la possession est *unique* : un `unique_ptr` ne peut pas être copié, seulement déplacé (`std::move`). On le crée avec `make_unique<Book>(book)` dans `addBook`. Le reste du programme reçoit seulement des pointeurs bruts *non propriétaires* (`Book*`, obtenus par `.get()`) qui permettent de lire ou modifier un livre sans en prendre la responsabilité. Ce code combine aussi les pointeurs intelligents avec des **lambdas** passées aux algorithmes de la STL :

```cpp
auto it = find_if(books.begin(), books.end(),
    [&isbn](const unique_ptr<Book>& book) {
        return book->getISBN() == isbn;
    });
```

La lambda `[&isbn](...) { ... }` est une fonction anonyme définie sur place. Ce qui est entre crochets est la *liste de capture* : `&isbn` donne à la lambda accès par référence à la variable locale `isbn`. `find_if` appelle cette lambda pour chaque élément et renvoie un itérateur sur le premier qui répond `true`. J'ai réutilisé le même principe pour le tri (`std::sort` avec une lambda qui compare les titres ou les auteurs) dans `Library::sortBooks`.

### Question 2 : Options de développement possibles

Pour une bibliothèque comme celle d'Alexandrie, avec des millions de livres, des milliers d'usagers et plusieurs agents qui travaillent en même temps, des fichiers texte chargés entièrement en mémoire ne suffisent plus : chaque sauvegarde réécrit tout le fichier, deux agents ne peuvent pas travailler en même temps sans écraser les données de l'autre, et une recherche parcourt tous les livres un par un. Je proposerais une architecture client-serveur avec une **base de données relationnelle PostgreSQL** pour les livres, les usagers et les emprunts. Les tables seraient indexées (ISBN, titre, auteur), les transactions garantiraient qu'un livre ne peut pas être emprunté deux fois, et la recherche plein texte de PostgreSQL (ou un moteur comme **Elasticsearch/OpenSearch** pour des recherches tolérantes aux fautes sur des millions de titres) remplacerait la recherche linéaire. Par-dessus, une **API REST** et une **interface web** (par exemple React) permettraient aux agents et aux usagers d'utiliser le système depuis n'importe quel poste, sans installer de binaire. Le C++ resterait utile de deux façons. Premièrement, le cœur métier existant (`Library`, `Book`, `User`) pourrait devenir un service web avec un framework C++ comme **Drogon** ou **Crow**, et la classe `FileManager` serait remplacée par une couche d'accès aux données qui utilise **libpqxx** (la bibliothèque C++ officielle de PostgreSQL) au lieu de lire `books.txt`. Deuxièmement, si l'API est écrite dans un autre langage (Python/FastAPI, Node.js), on peut exposer les parties performantes en C++ (import en lot, calculs de statistiques) avec **pybind11** ou les appeler en **gRPC**, avec un contrat défini dans un fichier `.proto`. Le tout serait déployé dans des conteneurs **Docker**, comme notre dev container, ce qui faciliterait la mise à l'échelle quand le catalogue grandit.
