#include "FileSystem.h"


FileSystem::FileSystem(): rootDirectory(), workingDirectory(){

    rootDirectory = new Directory("root", nullptr);
    workingDirectory = rootDirectory;
}

// Destructor
FileSystem::~FileSystem(){


    if(verbose == 1 || verbose == 3)
        cout << "FileSystem::~FileSystem()" << endl; // for verbose

    //delete workingDirectory;
    delete rootDirectory;
}

// Copy Constructor
FileSystem::FileSystem(const FileSystem &other): rootDirectory(), workingDirectory(){

    if(verbose == 1 || verbose == 3)
        cout << "FileSystem::FileSystem(const FileSystem &other)" << endl; // for verbose

    rootDirectory = new Directory(*other.rootDirectory);
    //find the working directory
    string pathToWorkingDirectory = other.workingDirectory->getAbsolutePath();
    workingDirectory = getDirectoryByPath(pathToWorkingDirectory);
}

// Move Constructor
FileSystem::FileSystem(FileSystem &&other):rootDirectory(other.rootDirectory), workingDirectory(other.workingDirectory){

    if(verbose == 1 || verbose == 3)
        cout << "FileSystem::FileSystem(FileSystem &&other)" << endl; // for verbose

    other.rootDirectory = nullptr;
    other.workingDirectory = nullptr;
}

// Copy Assignment
FileSystem& FileSystem::operator=(const FileSystem &other){

    if(verbose == 1 || verbose == 3)
        cout << "FileSystem& FileSystem::operator=(const FileSystem &other)" << endl; // for verbose

    // check for "self assignment" and do nothing in that case
    if (this == &other) {
        return *this;
    }
    // we need to write this function
    delete rootDirectory;
    //delete workingDirectory;
    workingDirectory = nullptr;

    rootDirectory = new Directory(*other.rootDirectory);
    //find the working directory
    string pathToWorkingDirectory = other.workingDirectory->getAbsolutePath();
    workingDirectory = getDirectoryByPath(pathToWorkingDirectory);

    // return this Directory
    return *this;
}

// Move Assignment
FileSystem& FileSystem::operator=(FileSystem &&other){

    if(verbose == 1 || verbose == 3)
        cout << "FileSystem& FileSystem::operator=(FileSystem &&other)" << endl; // for verbose

    if (this != &other)
    {
        // delete this
        delete workingDirectory;
        delete rootDirectory;
        // copy other to this
        rootDirectory = other.rootDirectory;
        workingDirectory = other.workingDirectory;

        other.rootDirectory = nullptr;
        other.workingDirectory = nullptr;
    }

    return *this;
}


// Return reference to the root directory
Directory& FileSystem::getRootDirectory() const{
    return *rootDirectory;
}

// Return reference to the working directory
Directory& FileSystem::getWorkingDirectory() const{
    return *workingDirectory;
}

// Change the working directory of the file system
void FileSystem::setWorkingDirectory(Directory *newWorkingDirectory){
    workingDirectory = newWorkingDirectory;
}

// Return a pointer to the last directory in path
Directory* FileSystem::getDirectoryByPath(string path) {

    if (path == "/") // <path> = /
        return rootDirectory;

    if (path[0] == '/') { // <path> = /dir1/dir2
        path = path.substr(1, path.size() - 1);
        return getDirectoryByPath(path, rootDirectory);
    }
    else {
        return getDirectoryByPath(path, workingDirectory);
    }
}

Directory* FileSystem::getDirectoryByPath(string path, Directory *directory) {

    if(directory == nullptr)
        return nullptr;

    unsigned int i = 0;
    while (i < path.size() && path[i] != '/')
        i++;

    string fileToFind = path.substr(0, i);
    int index = directory->findChildIndexByName(fileToFind);

    if (index == -1 && fileToFind != "..") // <path> is not valid
        return nullptr;

        // <path> is valid
    else if (i < path.size()) {

        path = path.substr(i+1);

        if (fileToFind == "..") // cheak if the directory toReturn is the parent of WorkingDirectory
            return getDirectoryByPath(path, directory->getParent());
        else {
            if (directory->getChildren()[index]->myType() == "Directory") {
                auto directoryCast = dynamic_cast<Directory *>(directory->getChildren()[index]);
                return getDirectoryByPath(path, directoryCast);
            }
            else
                return nullptr;
        }
    }
    else {
        if(fileToFind == "..")
            return directory->getParent();

        if (directory->getChildren()[index]->myType() == "Directory") {
            auto directoryCast = dynamic_cast<Directory *>(directory->getChildren()[index]);
            return directoryCast;
        }
        else
            return nullptr;
    }
}


bool FileSystem::isPathValid(string path){

    if(path != ""){

        if(path[0] == '/'){ // path starts from root directory
            path = path.substr(1, path.size() - 1);
            return isPathValid(path, getRootDirectory());
        }
        else // path starts from working directory
            return isPathValid(path, getWorkingDirectory());
    }

    return  false; // path = ""
}

bool FileSystem::isPathValid(string path, Directory& directory) {

    if(path == "")
        return true;

    unsigned int i = 0;
    while (i < path.size() && path[i] != '/')
        i++;

    string next = path.substr(0, i);

    if (directory.isChildNameExist(next)) { // next is a child of directory --- continue...

        if(i != path.size()){

            int indexOfNext = directory.findChildIndexByName(next);
            path = path.substr(i + 1, path.size() - i - 1);

            if (directory.getChildren()[indexOfNext]->myType() == "Directory") {
                auto directoryCast = dynamic_cast<Directory *>(directory.getChildren()[indexOfNext]);
                return isPathValid(path, *directoryCast);
            }
            else // path = /dir1/file1/file2 (illegal path)
                return false;
        }
        else // path = dir1 or file1
            return true;
    }

    else if (next == "..") { // next is the parent of directory

        if(i != path.size()){

            if (directory.getName() != getRootDirectory().getName()) {
                path = path.substr(i + 1, path.size() - i - 1);
                return isPathValid(path, *directory.getParent());
            }
            else
                return false;
        }

        else{ // path = ..
            if (directory.getName() != getRootDirectory().getName())
                return true;
            else
                return false;
        }

    }
    else // next is not a child of directory --- path is illegal
        return false;
}






