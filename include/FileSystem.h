#ifndef UNTITLED_FILESYSTEM_H
#define UNTITLED_FILESYSTEM_H

#include "Files.h"


class FileSystem {
private:
    Directory* rootDirectory;
    Directory* workingDirectory;

public:

    virtual ~FileSystem(); // Destructor
    FileSystem(const FileSystem &other); // Copy Constructor
    FileSystem(FileSystem &&other); // Move Constructor
    FileSystem& operator=(const FileSystem &other); // Copy Assignment
    FileSystem& operator=(FileSystem &&other); // Move Assignment

    FileSystem();
    Directory& getRootDirectory() const; // Return reference to the root directory
    Directory& getWorkingDirectory() const; // Return reference to the working directory
    void setWorkingDirectory(Directory *newWorkingDirectory); // Change the working directory of the file system

    Directory* getDirectoryByPath(string path); // Return a pointer to the last directory in path
    Directory* getDirectoryByPath(string path, Directory *directory);

    bool isPathValid(string path);
    bool isPathValid(string path, Directory& directory);

};

#endif //UNTITLED_FILESYSTEM_H
