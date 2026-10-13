#include <iostream>
#include <algorithm>

#include "library.h"

using namespace std;

// Constructor
Library::Library() {}

// Add book to library
void Library::addBook(const Book& book) {
    books.push_back(make_unique<Book>(book));
}

// Remove book from library
bool Library::removeBook(const string& isbn) {
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const unique_ptr<Book>& book) {
            return book->getISBN() == isbn;
        });
    
    if (it != books.end()) {
        books.erase(it);
        return true;
    }
    return false;
}

// Find book by ISBN
Book* Library::findBookByISBN(const string& isbn) {
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const unique_ptr<Book>& book) {
            return book->getISBN() == isbn;
        });
    
    return (it != books.end()) ? it->get() : nullptr;
}

// Search books by title (case-insensitive partial match)
vector<Book*> Library::searchBooksByTitle(const string& title) {
    vector<Book*> results;
    string lowerTitle = title;
    transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
    
    for (auto& book : books) {
        string bookTitle = book->getTitle();
        transform(bookTitle.begin(), bookTitle.end(), bookTitle.begin(), ::tolower);
        
        if (bookTitle.find(lowerTitle) != string::npos) {
            results.push_back(book.get());
        }
    }
    return results;
}

// Search books by author (case-insensitive partial match)
vector<Book*> Library::searchBooksByAuthor(const string& author) {
    vector<Book*> results;
    string lowerAuthor = author;
    transform(lowerAuthor.begin(), lowerAuthor.end(), lowerAuthor.begin(), ::tolower);
    
    for (auto& book : books) {
        string bookAuthor = book->getAuthor();
        transform(bookAuthor.begin(), bookAuthor.end(), bookAuthor.begin(), ::tolower);
        
        if (bookAuthor.find(lowerAuthor) != string::npos) {
            results.push_back(book.get());
        }
    }
    return results;
}

// Get all available books
vector<Book*> Library::getAvailableBooks() {
    vector<Book*> available;
    for (auto& book : books) {
        if (book->getAvailability()) {
            available.push_back(book.get());
        }
    }
    return available;
}

// Get all books
vector<Book*> Library::getAllBooks() {
    vector<Book*> allBooks;
    for (auto& book : books) {
        allBooks.push_back(book.get());
    }
    return allBooks;
}

// Add user to library
void Library::addUser(const User& user) {
    users.push_back(make_unique<User>(user));
}

// Find user by ID
User* Library::findUserById(const string& userId) {
    auto it = find_if(users.begin(), users.end(),
        [&userId](const unique_ptr<User>& user) {
            return user->getUserId() == userId;
        });
    
    return (it != users.end()) ? it->get() : nullptr;
}

// Get all users
vector<User*> Library::getAllUsers() {
    vector<User*> allUsers;
    for (auto& user : users) {
        allUsers.push_back(user.get());
    }
    return allUsers;
}

// Check out book
bool Library::checkOutBook(const string& isbn, const string& userId) {
    Book* book = findBookByISBN(isbn);
    User* user = findUserById(userId);
    
    if (book && user && book->getAvailability()) {
        book->checkOut(user->getUserId());
        user->borrowBook(isbn);
        return true;
    }
    return false;
}

// Return book
bool Library::returnBook(const string& isbn) {
    Book* book = findBookByISBN(isbn);
    
    if (book && !book->getAvailability()) {
        // Find the user who borrowed this book
        for (auto& user : users) {
            if (user->hasBorrowedBook(isbn)) {
                user->returnBook(isbn);
                break;
            }
        }
        book->returnBook();
        return true;
    }
    return false;
}

//NOUVELLE FONCTIONNALITE - ON GET LE NOM AU LIEU DU ID (interface et Expérience Utilisateur)

// Get le nom de la personne qui emprunte (borrower) - id si nom est inconnu
string Library::getBorrowerName(const Book& book) const {
    const string& borrowerId = book.getBorrowerId();//id de la personne
    auto it = find_if(users.begin(), users.end(), //parcourt la liste des utilisateurs
        [&borrowerId](const unique_ptr<User>& user) {//lambda recoit chaque users
            return user->getUserId() == borrowerId;//si id correspond
        });

    return (it != users.end()) ? (*it)->getName() : borrowerId; // retournee nom si valide sinon le id
}

// Return affichage du livre avec le nom de l'emprunteur au lieu de son id
string Library::formatBook(const Book& book) const {
    if (book.getAvailability()) {// si le livre disponible, 0 borrower à afficher
        return book.toString();//return affichage normal du livre
    }

    Book displayed(book);// copie le livre pour pas modifier l'original
    
    displayed.setBorrowerId(getBorrowerName(book));//remplace l'id du borrower par son nom
    
    return displayed.toString();// return qui affiche le nom et non le id
}

//NOUVELLE FONCTIONNALITE - JOURNAL D'ACTIVITES

// ligne du journal : titre auteur isbn disponible nom de l'emprunteur
string Library::toLogFormat(const Book& book) const {
    string borrowerName = book.getAvailability() ? "" : getBorrowerName(book);// si le livre est dispo, pas d'emprunteur ; sinon on va chercher son nom
    return book.getTitle() + "|" + book.getAuthor() + "|" + book.getISBN() + "|" +// on colle les infos du livre séparées par des | (1 = disponible, 0 = emprunté)
           (book.getAvailability() ? "1" : "0") + "|" + borrowerName;
}

// copy en lowercase
string toLower(string text) {
    transform(text.begin(), text.end(), text.begin(), ::tolower);
    return text;
}

//NOUVELLE FONCTIONNALITE - ON TRIE LES LIVRES (Gestion de données)

// trier liste de livres par titre ou auteur (std::sort)
void Library::sortBooks(vector<Book*>& list, SortOrder order) {
    if (order == SortOrder::Title) {//par titre
        sort(list.begin(), list.end(), [](const Book* a, const Book* b) {
            return toLower(a->getTitle()) < toLower(b->getTitle());// 'a' devant si son titre vient avant alphabetique
        });
    } else if (order == SortOrder::Author) {//par autheur
        sort(list.begin(), list.end(), [](const Book* a, const Book* b) {
            string authorA = toLower(a->getAuthor());
            string authorB = toLower(b->getAuthor());
            if (authorA != authorB) {
                return authorA < authorB;
            }
            return toLower(a->getTitle()) < toLower(b->getTitle()); //meme principe mais return autheur
        });
    }
}

// Display all books
void Library::displayAllBooks(SortOrder order) {
    if (books.empty()) {
        cout << "Aucun livre dans la bibliothèque.\n";
        return;
    }
    
    auto allBooks = getAllBooks();//copie de la liste des livres
    sortBooks(allBooks, order);//on trie la liste

    cout << "\n=== TOUS LES LIVRES ===\n";
    for (size_t i = 0; i < allBooks.size(); ++i) {
        cout << "\nLivre " << (i + 1) << " :\n";
        cout << formatBook(*allBooks[i]) << "\n";
        cout << "-------------------------\n";
    }
}

// Display available books
void Library::displayAvailableBooks(SortOrder order) {
    auto available = getAvailableBooks();
    
    if (available.empty()) {
        cout << "Aucun livre disponible pour emprunt.\n";
        return;
    }
    
    sortBooks(available, order);//trie livres

    cout << "\n=== LIVRES DISPONIBLES ===\n";
    for (size_t i = 0; i < available.size(); ++i) {
        cout << "\nLivre " << (i + 1) << " :\n";
        cout << formatBook(*available[i]) << "\n";
        cout << "---------------------------\n";
    }
}

// Display all users
void Library::displayAllUsers() {
    if (users.empty()) {
        cout << "Aucun utilisateur enregistré.\n";
        return;
    }
    
    cout << "\n=== TOUS LES UTILISATEURS ===\n";
    for (size_t i = 0; i < users.size(); ++i) {
        cout << "\nUtilisateur " << (i + 1) << " :\n";
        cout << users[i]->toString() << "\n";
        cout << "------------------------------\n";
    }
}

// Statistics
int Library::getTotalBooks() const { return books.size(); }
int Library::getAvailableBookCount() const {
    return count_if(books.begin(), books.end(),
        [](const unique_ptr<Book>& book) {
            return book->getAvailability();
        });
}
int Library::getCheckedOutBookCount() const { return getTotalBooks() - getAvailableBookCount(); }