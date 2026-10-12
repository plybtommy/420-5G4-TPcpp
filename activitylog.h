#ifndef ACTIVITYLOG_H
#define ACTIVITYLOG_H

#include <string>

using namespace std;

// Append-only activity journal kept across sessions
class ActivityLog {
private:
    string logFileName;

    static string currentTimestamp();

public:
    explicit ActivityLog(const string& logFile);

    // Writes "YYYY-MM-DD HH:MM:SS - [ACTION] details" at the end of the file
    bool log(const string& action, const string& details = "");
    string getFileName() const;
};

#endif
