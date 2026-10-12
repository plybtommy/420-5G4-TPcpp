#include <ctime>
#include <fstream>
#include <iostream>

#include "activitylog.h"

using namespace std;

// Constructor
ActivityLog::ActivityLog(const string& logFile) : logFileName(logFile) {}

// Current local date and time, e.g. 2026-10-23 10:23:57
string ActivityLog::currentTimestamp() const {
    time_t now = time(nullptr);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buffer;
}

// Append an entry to the log (the file is created if it does not exist)
bool ActivityLog::log(const string& action, const string& details) {
    ofstream file(logFileName, ios::app);
    if (!file.is_open()) {
        cout << "Erreur : Impossible d'écrire dans le journal " << logFileName << ".\n";
        return false;
    }

    file << currentTimestamp() << " - [" << action << "]";
    if (!details.empty()) {
        file << " " << details;
    }
    file << "\n";
    return true;
}
