#ifndef UNTITLED_FILES_H
#define UNTITLED_FILES_H

#include <string>
#include <vector>
#include <iostream>

#include "GlobalVariables.h"

using namespace std;

class BaseFile {
private:
    string name;

public:
    BaseFile(string name);
    BaseFile();
    string getName() const;
    void setName(string newName);
    virtual int getSize() = 0;

    virtual string myType() = 0;
    virtual ~BaseFile(); // Destructor

};

class Directory : public BaseFile {
private:
    vector<BaseFile*> children;
    Directory *parent;

    void clear();

public:

    void copy(const Directory &other);
    virtual ~Directory(); // Destructor
    Directory(const Directory &other); // Copy Constructor
    Directory(Directory &&other); // Move Constructor
    Directory& operator=(const Directory &other); // Copy Assignment
    Directory& operator=(Directory &&other); // Move Assignment

    Directory(string name, Directory *parent); // Constructor
    Directory *getParent() const; // Return a pointer to the parent of this directory
    void setParent(Directory *newParent); // Change the parent of this directory
    void addFile(BaseFile* file); // Add the file to children
    void removeFile(string name); // Remove the file with the specified name from children
    void removeFile(BaseFile* file); // Remove the file from children
    void sortByName(); // Sort children by name alphabetically (not recursively)
    void sortBySize(); // Sort children by size (not recursively)
    vector<BaseFile*> getChildren(); // Return children
    int getSize(); // Return the size of the directory (recursively)
    string getAbsolutePath();  //Return the path from the root to this
    string getAbsolutePathRec();


    int findChildIndexByName(string nameToFind); //Return the index of a BaseFile named nameToFind, if exist
                                                 // else, return -1
    string myType();
    bool isChildNameExist(string childName);// check if a BaseFile named childName already exists in directory

};

class File : public BaseFile {
private:
    int size;

public:
    File(string name, int size); // Constructor
     int getSize(); // Return the size of the file

    string myType();
    virtual ~File(); // Destructor

};



#endif //UNTITLED_FILES_H
