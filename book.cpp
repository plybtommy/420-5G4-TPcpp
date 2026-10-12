#include <sstream>

#include "book.h"

using namespace std;

// Default constructor
Book::Book() : title(""), author(""), isbn(""), isAvailable(true), borrowerId("") {}

// Parameterized constructor
Book::Book(const string& title, const string& author, const string& isbn)
    : title(title), author(author), isbn(isbn), isAvailable(true), borrowerId("") {}

// Getters
string Book::getTitle() const { return title; }
string Book::getAuthor() const { return author; }
string Book::getISBN() const { return isbn; }
bool Book::getAvailability() const { return isAvailable; }
string Book::getBorrowerId() const { return borrowerId; }

// Setters
void Book::setTitle(const string& title) { this->title = title; }
void Book::setAuthor(const string& author) { this->author = author; }
void Book::setISBN(const string& isbn) { this->isbn = isbn; }
void Book::setAvailability(bool available) { this->isAvailable = available; }
void Book::setBorrowerId(const string& id) { this->borrowerId = id; }

// Check out the book to a borrower
void Book::checkOut(const string& borrowerId) {
    isAvailable = false;
    this->borrowerId = borrowerId;
}

// Return the book
void Book::returnBook() {
    isAvailable = true;
    borrowerId = "";
}

// Display book information
string Book::toString() const {
    return "Titre: " + title + "\nAuteur: " + author + "\nISBN: " + isbn +
           "\nStatut: " + (isAvailable ? "Disponible" : "Emprunté par : " + borrowerId);
}

// Format for file storage : titre|auteur|isbn|disponible(1/0)|idEmprunteur
string Book::toFileFormat() const {
    return title + "|" + author + "|" + isbn + "|" +
           (isAvailable ? "1" : "0") + "|" + borrowerId;
}

// Parse from file format
void Book::fromFileFormat(const string& line) {
    stringstream ss(line);
    string availableStr;

    getline(ss, title, '|');
    getline(ss, author, '|');
    getline(ss, isbn, '|');
    getline(ss, availableStr, '|');
    getline(ss, borrowerId, '|');

    isAvailable = (availableStr == "1");
    if (isAvailable) {
        borrowerId = "";
    }
}
