#include "Commands.h"


///**********************************************************
///******************class BaseCommand***********************
///**********************************************************

BaseCommand::BaseCommand(string args):args(args){}

string BaseCommand::getArgs(){
    return args;
}

BaseCommand::~BaseCommand(){}


string BaseCommand::pathByPlace(string path, int n) {

    if(path[0] == '/')
        path = path.substr(1, path.size()-1);

    unsigned int i = 0;
    while(n > 1 && i < path.size()) {

        if(path[i] == '/')
            n = n - 1;

        i++;
    }

    if(i == path.size())
        return "";

    else {
        int j = i;
        while (path[i] != '/' && i < path.size())
            i++;

        return path.substr(j, i - j);
    }
}



///**********************************************************
///******************class PwdCommand************************
///**********************************************************

PwdCommand::PwdCommand(string args):BaseCommand(args){}

//print working directory path
void PwdCommand::execute(FileSystem & fs){

    cout << fs.getWorkingDirectory().getAbsolutePath() << endl;
}

string PwdCommand::toString(){
    return "pwd";
}



///**********************************************************
///******************class CdCommand*************************
///**********************************************************

CdCommand::CdCommand(string args):BaseCommand(args){}

// Change the current directory
void CdCommand::execute(FileSystem & fs){

    if(this->getArgs() != "") {

        Directory *directory = fs.getDirectoryByPath(this->getArgs());

        if (directory == nullptr)
            cout << "The system cannot find the path specified" << endl;
        else
            fs.setWorkingDirectory(directory);
    }
}

string CdCommand::toString(){
    return "cd";
}



///**********************************************************
///******************class LsCommand*************************
///**********************************************************

LsCommand::LsCommand(string args):BaseCommand(args){}

// Display the list of files and subdirectories of a directory
void LsCommand::execute(FileSystem & fs) {

    bool sortAlphabetically = true; // sortAlphabetically = true -> sort alphabetically / sortAlphabetically = false -> sort by size
    string path = this->getArgs();
    string typeOfBaseFile;
    Directory *directory;

    if (this->getArgs().size() > 1)
        if (this->getArgs().substr(0, 2) == "-s") {
            sortAlphabetically = false;

            if(getArgs() == "-s")
                path = "";
            else
                path = this->getArgs().substr(3);
        }

    if(path == "")
        directory = &fs.getWorkingDirectory();
    else
        directory = fs.getDirectoryByPath(path);


    if (directory == nullptr){
        cout << "The system cannot find the path specified" << endl;
    }
    else{

        if(sortAlphabetically)
            directory->sortByName();
        else
            directory->sortBySize();


        for(unsigned int i = 0 ; i < directory->getChildren().size() ; i++){

            if(directory -> getChildren()[i]-> myType() == "File" )
                typeOfBaseFile = "FILE";
            else
                typeOfBaseFile = "DIR";

            cout << typeOfBaseFile + "\t"  +directory->getChildren()[i]->getName() + "\t" + to_string(directory->getChildren()[i]->getSize()) << endl;
        }
    }
}

string LsCommand::toString(){
    return "ls";
}



///**********************************************************
///******************class MkdirCommand**********************
///**********************************************************

MkdirCommand::MkdirCommand(string args):BaseCommand(args){}

// Create a new directory
void MkdirCommand::execute(FileSystem & fs){

    if(getArgs() != ""){

        string path = this->getArgs();
        string nextDirectoryName;
        Directory *directory;
        bool isLegal = true;

        int index;

        if(fs.getDirectoryByPath(path) != nullptr)
            cout << "The directory already exists" << endl;

        else{

            if (path[0] == '/')
                directory = &fs.getRootDirectory();
            else
                directory = &fs.getWorkingDirectory();

            int i = 1;
            while (pathByPlace(path, i) != "" && isLegal) {

                nextDirectoryName = pathByPlace(path, i);
                index = directory->findChildIndexByName(nextDirectoryName);

                if (index == -1 && nextDirectoryName != "..") {
                    Directory *newDirectory = new Directory(nextDirectoryName, nullptr);
                    directory->addFile(newDirectory);
                    directory = newDirectory;

                } else if (nextDirectoryName == "..")
                    directory = directory->getParent();

                else if (directory->getChildren()[index]->myType() != "File") {
                    auto directoryCast = dynamic_cast<Directory *>(directory->getChildren()[index]);
                    directory = directoryCast;
                }

                else if (directory->getChildren()[index]->myType() == "File") {
                    cout << "The directory already exists" << endl;
                    isLegal = false;
                }
                i++;
            }
        }
    }
}

string MkdirCommand::toString(){
    return "mkdir";
}



///**********************************************************
///******************class MkfileCommand*********************
///**********************************************************

MkfileCommand::MkfileCommand(string args):BaseCommand(args){}

// Create a new file
void MkfileCommand::execute(FileSystem & fs){

    // check if <path/filename> <size> is valid
    if(getArgs() != ""){

        Directory* directory;
        string path;
        string filename_size;

        // separate between <path>, <filename> and <size>
        if(getArgs().find('/') == string::npos){
            directory = &fs.getWorkingDirectory();
            filename_size = getArgs();
        }
        else{
            path = getArgs().substr(0, getArgs().find_last_of('/'));
            directory = fs.getDirectoryByPath(path);
            filename_size = getArgs().substr(getArgs().find_last_of('/')+1);
        }

        string filename = filename_size.substr(0, filename_size.find_last_of(' '));
        string sizeInString = filename_size.substr(filename_size.find_last_of(' ')+1);

        int size = atoi(sizeInString.c_str());

        if(directory == nullptr)
            cout << "The system cannot find the path specified" << endl;

        else {
            // check if the new file already exists
            if((directory->isChildNameExist(filename)))
                cout << "File already exists" << endl;

            else {// creat and add the new file
                File* newfile = new File(filename, size);
                directory->addFile(newfile);
            }
        }
    }
}

string MkfileCommand::toString(){
    return "mkfile";
}



///**********************************************************
///******************class CpCommand*************************
///**********************************************************

CpCommand::CpCommand(string args):BaseCommand(args){}

//Copy a file or directory to a destination
void CpCommand::execute(FileSystem & fs){

    // check if  <source-path> <destination-path>  is valid
    if(getArgs() != ""){

        // separate between <source-path> and <destination-path>
        string sourcePath = getArgs().substr(0, getArgs().find_last_of(' '));
        string destinationPath = getArgs().substr(getArgs().find_last_of(' ')+1);

        // separate between <source> and <BaseFile-to-copy>
        if(!fs.isPathValid(sourcePath) || !fs.isPathValid(destinationPath))
            cout << "No such file or directory" << endl;
        else{
            if(sourcePath != "/"){ // sourcePath cant be "/" (we cant copy the root)

                Directory* sourceDirectory;
                string baseFileToCopyString;

                if(sourcePath.find('/') > sourcePath.size()) { // there is no '/' in sourcePath
                    sourceDirectory = &fs.getWorkingDirectory();
                    baseFileToCopyString = sourcePath;
                }
                else if(sourcePath.find_last_of('/') == 0) { // sourcePath = </name>
                    sourceDirectory = &fs.getRootDirectory();
                    baseFileToCopyString = sourcePath.substr(1);
                }
                else{
                    string source = sourcePath.substr(0, sourcePath.find_last_of('/'));
                    sourceDirectory = fs.getDirectoryByPath(source);
                    baseFileToCopyString = sourcePath.substr(sourcePath.find_last_of('/')+1);
                }
                Directory* destinationDirectory = fs.getDirectoryByPath(destinationPath);

                if(destinationDirectory == nullptr)
                    cout << "No such file or directory" << endl;
                else{

                    // if destination directory already has a child named 'baseFileToCopyString' -- do not copy!
                    if(!(destinationDirectory->isChildNameExist(baseFileToCopyString))){

                        int index = sourceDirectory->findChildIndexByName(baseFileToCopyString);
                        BaseFile* copiedBaseFile = sourceDirectory->getChildren()[index];

                        //check if the BaseFile to copy is a Directory or a File
                        // and copy it to the destination directory
                        if(copiedBaseFile->myType() == "Directory"){
                            auto directoryCast = dynamic_cast<Directory*>(copiedBaseFile);
                            Directory *copiedDir = new Directory(*directoryCast);
                            destinationDirectory->addFile(copiedDir);
                        }
                        else{
                            File* copiedFile = new File(copiedBaseFile->getName(), copiedBaseFile->getSize());
                            destinationDirectory->addFile(copiedFile);
                        }
                    }
                }
            }
        }
    }
}

string CpCommand::toString(){
    return "cp";
}



///**********************************************************
///******************class MvCommand*************************
///**********************************************************

MvCommand::MvCommand(string args):BaseCommand(args){}

// Move a file or directory to a new destination
void MvCommand::execute(FileSystem & fs){

    // check if  <source-path/file-name> <destination-path>  is valid
    if(getArgs() != ""){

        // separate between <source-path/file-name> and <destination-path>
        string sourcePath_fileName = getArgs().substr(0, getArgs().find_last_of(' '));
        string destinationPath = getArgs().substr(getArgs().find_last_of(' ')+1);

        if(!fs.isPathValid(sourcePath_fileName) || !fs.isPathValid(destinationPath))
            cout << "No such file or directory" << endl;
        else{
            if(sourcePath_fileName == "/") // root directory can’t be moved
                cout << "Can’t move directory" << endl;

            else{
                Directory* sourceDirectory;
                string baseFileToMoveString;

                if(sourcePath_fileName.find('/') > sourcePath_fileName.size()) { // there is no '/' in sourcePath/fileName
                    sourceDirectory = &fs.getWorkingDirectory(); // we only got the file name --> we are in the working directory
                    baseFileToMoveString = sourcePath_fileName;
                }
                else if(sourcePath_fileName.find_last_of('/') == 0) { // sourcePath = </file-name>
                    sourceDirectory = &fs.getRootDirectory(); // we want to move a child of root directory
                    baseFileToMoveString = sourcePath_fileName.substr(1);
                }
                else{
                    string source = sourcePath_fileName.substr(0, sourcePath_fileName.find_last_of('/'));
                    sourceDirectory = fs.getDirectoryByPath(source);
                    baseFileToMoveString = sourcePath_fileName.substr(sourcePath_fileName.find_last_of('/')+1);
                }

                Directory* destinationDirectory = fs.getDirectoryByPath(destinationPath);

                if(destinationDirectory == nullptr)
                    cout << "No such file or directory" << endl;

                else{

                    BaseFile* baseFileToMove;
                    int index = sourceDirectory->findChildIndexByName(baseFileToMoveString);

                    if(index == -1){ // baseFileToMoveString = ".."
                        baseFileToMove = sourceDirectory;

                        if(baseFileToMove == &fs.getRootDirectory()
                           || baseFileToMove == &fs.getWorkingDirectory()
                           || baseFileToMove == fs.getWorkingDirectory().getParent())
                            cout << "Can’t move directory" << endl;

                        else
                            sourceDirectory = ((Directory*)baseFileToMove)->getParent();
                    }

                    else{
                        BaseFile* baseFileToMove = sourceDirectory->getChildren()[index];

                        // Working directory nor its parents can’t be moved
                        if(baseFileToMove == &fs.getWorkingDirectory() ||
                           baseFileToMove == fs.getWorkingDirectory().getParent())
                            cout << "Can’t move directory" << endl;

                        // if destination directory already has a child named 'baseFileToMoveString' -- do not move!
                        else if(!(destinationDirectory->isChildNameExist(baseFileToMoveString))){

                            // remove file from source directory
                            sourceDirectory->removeFile(baseFileToMoveString);
                            // add file to destination directory
                            destinationDirectory->addFile(baseFileToMove);
                        }
                    }
                }
            }
        }
    }
}

string MvCommand::toString(){
    return "mv";
}



///**********************************************************
///******************class RenameCommand*********************
///**********************************************************

RenameCommand::RenameCommand(string args):BaseCommand(args){}

//Rename a file or a directory
void RenameCommand::execute(FileSystem & fs){

    // check if <path/old-name> <new-name> is valid
    if(getArgs() != ""){// check if <path> is valid

        // separate between <path/old-name> and <new-name>
        string path_oldName = getArgs().substr(0, getArgs().find_last_of(' '));
        string newName = getArgs().substr(getArgs().find_last_of(' ')+1);

        if(!fs.isPathValid(path_oldName))
            cout << "No such file or directory" << endl;

        else{
            Directory* dirWithChildToRename;
            string oldName;

            if(path_oldName.find('/') > path_oldName.size()) { // there is no '/' in <path/old-name>
                dirWithChildToRename = &fs.getWorkingDirectory(); // we only got the old name --> we are in the working directory
                oldName = path_oldName;
            }
            else if(path_oldName.find_last_of('/') == 0) { // <path/old-name> = </old-name>
                dirWithChildToRename = &fs.getRootDirectory(); // we want to rename a child of root directory
                oldName = path_oldName.substr(1);
            }
            else{
                string path = path_oldName.substr(0, path_oldName.find_last_of('/'));
                dirWithChildToRename = fs.getDirectoryByPath(path);
                oldName = path_oldName.substr(path_oldName.find_last_of('/')+1);
            }

            int index = dirWithChildToRename->findChildIndexByName(oldName);
            BaseFile* baseFileToRename = dirWithChildToRename->getChildren()[index];

            // check if BaseFile to rename is the working-directory
            if(baseFileToRename == &fs.getWorkingDirectory())
                cout << "Can’t rename the working directory" << endl;

            // check if dirToChange already has a child named newName, if not rename
            else if(!(dirWithChildToRename->isChildNameExist(newName)))
                baseFileToRename->setName(newName);
        }
    }
}

string RenameCommand::toString(){
    return "rename";
}



///**********************************************************
///******************class RmCommand*************************
///**********************************************************

RmCommand::RmCommand(string args):BaseCommand(args){}

//Remove (delete) a file or a directory
void RmCommand::execute(FileSystem & fs){

    if(!fs.isPathValid(getArgs()))
        cout << "No such file or directory" << endl;

    else if(getArgs() == "/") // root cant be removed
        cout << "Can’t remove directory" << endl;

    else {
        string path;
        Directory *directoryToRemoveFrom;
        string baseFileToRemove;

        if (getArgs().find('/') > getArgs().size()) { // there is no / in args
            directoryToRemoveFrom = &fs.getWorkingDirectory();
            baseFileToRemove = getArgs();
        }

        else if (getArgs().find_last_of('/') == 0) { // args = </name>
                directoryToRemoveFrom = &fs.getRootDirectory();
                baseFileToRemove = getArgs().substr(1);
        }
        else {
            path = getArgs().substr(0, getArgs().find_last_of('/'));
            directoryToRemoveFrom = fs.getDirectoryByPath(path);
            baseFileToRemove = getArgs().substr(getArgs().find_last_of('/') + 1);
        }


        int index = directoryToRemoveFrom->findChildIndexByName(baseFileToRemove);
        BaseFile *toRemove = directoryToRemoveFrom->getChildren()[index];

        if (toRemove->myType() == "File")
            directoryToRemoveFrom->removeFile(toRemove);

        else { // toRemove is a directory

            if (toRemove == &fs.getWorkingDirectory() || toRemove == &fs.getRootDirectory())
                cout << "Can’t remove directory" << endl;

            else {
                string pathToRemove = directoryToRemoveFrom->getAbsolutePath();
                string pathOfWorkingDirectory = fs.getWorkingDirectory().getAbsolutePath();

                bool legal = false;
                if (pathToRemove.size() < pathOfWorkingDirectory.size())
                    for (unsigned int i = 0; i < pathToRemove.size() && !legal; i++)
                        if (pathToRemove[i] != pathOfWorkingDirectory[i])
                            legal = true;

                if (legal || pathToRemove.size() >= pathOfWorkingDirectory.size())
                    directoryToRemoveFrom->removeFile(toRemove);

                else
                    cout << "Can’t remove directory" << endl;
            }
        }
    }
}

string RmCommand::toString(){
    return "rm";
}



///**********************************************************
///******************class HistoryCommand********************
///**********************************************************

HistoryCommand::HistoryCommand(string args, const vector<BaseCommand *> & history):BaseCommand(args), history(history){}

void HistoryCommand::execute(FileSystem & fs){

    for(unsigned int i = 0 ; i < history.size() ; i++){

        if(history[i]->toString() == "error")
            cout << i << "\t" << history[i]->getArgs() << endl;

        else if(history[i]->getArgs() == "")
            cout << i << "\t" << history[i]->toString() << endl;

        else
            cout << i << "\t" << history[i]->toString() << " " << history[i]->getArgs() << endl;
    }
}

string HistoryCommand::toString(){
    return "history";
}



///**********************************************************
///******************class VerboseCommand********************
///**********************************************************

VerboseCommand::VerboseCommand(string args):BaseCommand(args){}

void VerboseCommand::execute(FileSystem & fs){

        if(getArgs() == "0")
            verbose = 0;
        else if(getArgs() == "1")
            verbose = 1;
        else if(getArgs() == "2")
            verbose = 2;
        else if(getArgs() == "3")
            verbose = 3;
        else
            cout << "Wrong verbose input" << endl;
}

string VerboseCommand::toString(){
    return "verbose";
}



///**********************************************************
///******************class ErrorCommand**********************
///**********************************************************

ErrorCommand::ErrorCommand(string args):BaseCommand(args){}

void ErrorCommand::execute(FileSystem & fs){

    cout << getArgs() + ": Unknown command" << endl;
}

string ErrorCommand::toString(){
    return "error";
}



///**********************************************************
///******************class ExecCommand***********************
///**********************************************************

ExecCommand::ExecCommand(string args, const vector<BaseCommand *> & history):BaseCommand(args), history(history){}

void ExecCommand::execute(FileSystem & fs){


    if(getArgs().find_first_not_of( "0123456789" ) == string::npos && getArgs() != "") {

        unsigned int commandNumber = atoi(getArgs().c_str());

        if (commandNumber < history.size())
            history[commandNumber]->execute(fs);
        else
            cout << "Command not found" << endl;
    }
}

string ExecCommand::toString(){
    return "exec";
}

















