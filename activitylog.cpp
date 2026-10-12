#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "activitylog.h"

using namespace std;

// Constructor
ActivityLog::ActivityLog(const string& logFile) : logFileName(logFile) {}

// Current local date and time, e.g. 2026-10-23 10:23:57
string ActivityLog::currentTimestamp() {
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm localTime = *localtime(&now);

    ostringstream oss;
    oss << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
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

string ActivityLog::getFileName() const { return logFileName; }
