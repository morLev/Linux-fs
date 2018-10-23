#include "GlobalVariables.h"
#include "Files.h"


///**********************************************************
///******************class BaseFile**************************
///**********************************************************

BaseFile::BaseFile(string name): name(name){}

string BaseFile::getName() const{
    return name;
}

void BaseFile::setName(string newName) {
    name = newName;
}

// Destructor
BaseFile::~BaseFile(){}



///**********************************************************
///******************class File******************************
///**********************************************************

File::File(string name, int size): BaseFile(name), size(size){}

int File::getSize(){
    return size;
}

string File::myType() {
    return "File";
}

// Destructor
File::~File(){}



///**********************************************************
///******************class Directory*************************
///**********************************************************

// Constructor
Directory::Directory(string name, Directory *parent): BaseFile(name), children(), parent(parent){

    if(parent != nullptr)
        parent->addFile(this);
}

// Destructor
Directory::~Directory(){

    if(verbose == 1 || verbose == 3)
        cout << "Directory::~Directory()" << endl; // for verbose

    clear();
}

void Directory::clear(){

    for (unsigned int i = 0; i < getChildren().size() ; i++)
        delete children[i];

    parent = nullptr;
}

// Copy Constructor
Directory::Directory(const Directory &other): BaseFile(other.getName()), children(), parent(){

    if(verbose == 1 || verbose == 3)
        cout << "Directory::Directory(const Directory &other)" << endl; // for verbose

    this->copy(other);

}

void Directory::copy(const Directory &other) {

    for (unsigned int i = 0; i < other.children.size(); i++) {

        if (other.children[i]->myType() == "Directory") {
            auto directoryCast = dynamic_cast<Directory*>(other.children[i]);
            Directory *newDirectory = new Directory(*directoryCast);
            addFile(newDirectory);
        }
        else {
            auto directoryCast = dynamic_cast<File *>(other.children[i]);
            File *newFile = new File(directoryCast->getName(), directoryCast->getSize());
            addFile(newFile);
        }
    }
}

// Move Constructor
Directory::Directory(Directory &&other):BaseFile(other.getName()), children(other.getChildren()), parent(){

    if(verbose == 1 || verbose == 3)
        cout << "Directory::Directory(Directory &&other)" << endl; // for verbose

    other.setName("");

    // changing the pointer of parent to this
    for(unsigned int i = 0 ; i < children.size() ; i++){
        if(children[i]->myType() == "Directory") {
            auto directoryCast = dynamic_cast<Directory *>(children[i]);
            directoryCast->setParent(this);
        }
    }

    for(unsigned int i = 0 ; i < other.getChildren().size() ; i++)
        other.children[i] = nullptr;

    other.parent = nullptr;

}

// Copy Assignment
Directory& Directory::operator=(const Directory &other){

    if(verbose == 1 || verbose == 3)
        cout << "Directory& Directory::operator=(const Directory &other)" << endl; // for verbose

    // check for "self assignment" and do nothing in that case
    if (this == &other) {
        return *this;
    }

    clear();

    this->copy(other);

    // return this Directory
    return *this;
}

// Move Assignment
Directory& Directory::operator=(Directory &&other){

    if(verbose == 1 || verbose == 3)
        cout << "Directory& Directory::operator=(Directory &&other)" << endl; // for verbose

        if (this != &other)
        {
            clear();

            setName(other.getName());
            children = other.children;

            // changing the pointer of parent to this
            for(unsigned int i = 0 ; i < children.size() ; i++){
                if(children[i]->myType() == "Directory") {
                    auto directoryCast = dynamic_cast<Directory *>(children[i]);
                    directoryCast->setParent(this);
                }
            }

            for(unsigned int i = 0 ; i < other.getChildren().size() ; i++)
                other.children[i] = nullptr;

            other.parent = nullptr;
        }
        return *this;
}

// Return a pointer to the parent of this directory
Directory* Directory::getParent() const{
    return parent;
}

// Change the parent of this directory
void Directory::setParent(Directory *newParent){
    parent = newParent;
}

// Add the file to children
void Directory::addFile(BaseFile* file){

    children.push_back(file);

    if(file->myType() == "Directory"){
        auto directoryCast = dynamic_cast<Directory*>(file);
        directoryCast->setParent(this);
    }
}

// Remove the file with the specified name from children
void Directory::removeFile(string name){

    int indexToRemove = this->findChildIndexByName(name);

    if(indexToRemove != -1)
        children.erase(children.begin() + indexToRemove);
}

// Remove the file from children
void Directory::removeFile(BaseFile* file) {

    for(unsigned int i = 0 ; i < children.size() ; i++){
        if(children[i] == file) {
            delete children[i];
            children.erase(children.begin() + i);
        }
    }
}

// Sort children by name alphabetically (not recursively)
void Directory::sortByName(){

    if (children.size() > 1){
        for(unsigned int i = 0 ; i < children.size()-1 ; i++) {
            for (unsigned int j = i + 1; j < children.size(); j++) {
                if (children[i]->getName() > children[j]->getName()) {
                    BaseFile *toSwap = children[i];
                    children[i] = children[j];
                    children[j] = toSwap;
                }
            }
        }
    }
}

// Sort children by size (not recursively)
void Directory::sortBySize(){

    if (children.size() > 1) {
        for (unsigned int i = 0; i < children.size() - 1; i++) {
            for (unsigned int j = i + 1; j < children.size(); j++) {
                if ((children[i]->getSize() > children[j]->getSize()) ||
                    (children[i]->getSize() == children[j]->getSize() &&
                     children[i]->getName() > children[j]->getName())) {
                    BaseFile *toSwap = children[i];
                    children[i] = children[j];
                    children[j] = toSwap;
                }
            }
        }
    }
}

// Return children
vector<BaseFile*> Directory::getChildren(){
    return children;
}

// Return the size of the directory (recursively)
int Directory::getSize(){
    int sum = 0;

    for(unsigned int i = 0 ; i < children.size() ; i++)
        sum = sum + children[i]->getSize();

    return sum;
}

//Return the path from the root to this
string Directory::getAbsolutePath(){

    if(parent == nullptr)
        return "/";

    if(parent->parent == nullptr)
        return "/" + getName();

    return  parent->getAbsolutePath() + "/" + getName();

}


// Return the index of a BaseFile named nameToFind, if exist
// else, return -1
int Directory::findChildIndexByName(string nameToFind){

    for(unsigned int i = 0 ; i < children.size() ; i++)
        if(children[i]->getName() == nameToFind)
            return i;

    return -1;
}

string Directory::myType() {
    return "Directory";
}

// check if a BaseFile named childName already exists in directory
bool Directory::isChildNameExist(string childName){

    for(unsigned int i = 0 ; i < children.size() ; i++){
        if(children[i]->getName() == childName)
            return true;
    }
    return false;
}
