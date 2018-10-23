#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "Files.h"
#include "Commands.h"

#include <string>
#include <vector>


using namespace std;

class Environment {
private:
    vector<BaseCommand*> commandsHistory;
    FileSystem fs;

    void clear();

public:

    void copy(const Environment &other);

    virtual ~Environment(); // Destructor
    Environment(const Environment &other); // Copy Constructor
    Environment(Environment &&other); // Move Constructor
    Environment& operator=(const Environment &other); // Copy Assignment
    Environment& operator=(Environment &&other); // Move Assignment

    Environment();
    void start();
    FileSystem& getFileSystem(); // Get a reference to the file system
    void addToHistory(BaseCommand *command); // Add a new command to the history
    const vector<BaseCommand*>& getHistory() const; // Return a reference to the history of commands

};

#endif
