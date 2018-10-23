#include "Environment.h"


Environment::Environment():commandsHistory(), fs(){}

// Destructor
Environment::~Environment(){

    if(verbose == 1 || verbose == 3)
        cout << "Environment::~Environment()" << endl; // for verbose

    clear();
}

void Environment::clear() {

    for (unsigned int i = 0; i < commandsHistory.size(); i++)
        delete commandsHistory[i];
}

// Copy Constructor
Environment::Environment(const Environment &other): commandsHistory(), fs(other.fs){

    if(verbose == 1 || verbose == 3)
        cout << "Environment::Environment(const Environment &other)" << endl; // for verbose

    this -> copy(other);
}

// Move Constructor
Environment::Environment(Environment &&other): commandsHistory(other.commandsHistory), fs(other.fs){

    if(verbose == 1 || verbose == 3)
        cout << "Environment::Environment(Environment &&other)" << endl; // for verbose

    for(unsigned int i = 0 ; i < other.commandsHistory.size() ; i++)
        other.commandsHistory[i] = nullptr;
}

void Environment::copy(const Environment &other){

    BaseCommand* command;
    string commandName;
    string args;
	fs = other.fs;

    for(unsigned int i = 0 ; i < other.commandsHistory.size() ; i++) {

        commandName = other.commandsHistory[i]->toString();
        args = other.commandsHistory[i]->getArgs();

        if (commandName == "pwd")
            command = new PwdCommand(args);
        else if (commandName == "cd")
            command = new CdCommand(args);
        else if (commandName == "ls")
            command = new LsCommand(args);
        else if (commandName == "mkdir")
            command = new MkdirCommand(args);
        else if (commandName == "mkfile")
            command = new MkfileCommand(args);
        else if (commandName == "cp")
            command = new CpCommand(args);
        else if (commandName == "mv")
            command = new MvCommand(args);
        else if (commandName == "rename")
            command = new RenameCommand(args);
        else if (commandName == "rm")
            command = new RmCommand(args);
        else if (commandName == "history")
            command = new HistoryCommand(args, commandsHistory);
        else if (commandName == "verbose")
            command = new VerboseCommand(args);
        else if (commandName == "exec")
            command = new ExecCommand(args, commandsHistory);
        else
            command = new ErrorCommand(args);

        addToHistory(command);
    }
}

// Copy Assignment
Environment& Environment::operator=(const Environment &other){

    if(verbose == 1 || verbose == 3)
        cout << "Environment& Environment::operator=(const Environment &other)" << endl; // for verbose

    if(this == &other){
        return *this;
    }

    clear();
    this->copy(other);

    return *this;
}

// Move Assignment
Environment& Environment::operator=(Environment &&other){

    if(verbose == 1 || verbose == 3)
        cout << "Environment& Environment::operator=(Environment &&other)" << endl; // for verbose

    if(this != &other){

        clear();

        fs = other.fs;
        commandsHistory = other.commandsHistory;

        for(unsigned int i = 0 ; i <other.commandsHistory.size() ; i++)
            other.commandsHistory[i] = nullptr;
    }
    return *this;
}


void Environment::start(){

    cout << fs.getWorkingDirectory().getAbsolutePath() +">" ;
    string input;
    getline(cin, input);

    while (input != "exit"){

        string commandName;
        string args;
        BaseCommand *command;

        bool isSpace = false;
        for(unsigned int i = 0 ; i <input.size() && !isSpace ; i++)
            if(input[i] == ' ')
                isSpace = true;

        if(isSpace){
            commandName = input.substr(0,input.find_first_of(' '));
            args = input.substr(input.find_first_of(' ') + 1);
        }

        else{
            commandName = input;
            args = "";
        }

        if(commandName == "pwd")
            command = new PwdCommand(args);
        else if(commandName == "cd")
            command = new CdCommand(args);
        else if(commandName == "ls")
            command = new LsCommand(args);
        else if(commandName == "mkdir")
            command = new MkdirCommand(args);
        else if(commandName == "mkfile")
            command = new MkfileCommand(args);
        else if(commandName == "cp")
            command = new CpCommand(args);
        else if(commandName == "mv")
            command = new MvCommand(args);
        else if(commandName == "rename")
            command = new RenameCommand(args);
        else if(commandName == "rm")
            command = new RmCommand(args);
        else if(commandName == "history")
            command = new HistoryCommand(args, commandsHistory);
        else if(commandName == "verbose")
            command = new VerboseCommand(args);
        else if(commandName == "exec")
            command = new ExecCommand(args, commandsHistory);
        else
            command = new ErrorCommand(input);


        if(verbose == 2 || verbose == 3)
            cout << input << endl;

        command->execute(fs);

        addToHistory(command);

        cout << fs.getWorkingDirectory().getAbsolutePath() + ">" ;
        getline(cin, input);
    }


}

// Get a reference to the file system
FileSystem& Environment::getFileSystem(){
    return fs;
}

// Add a new command to the history
void Environment::addToHistory(BaseCommand *command){
    commandsHistory.push_back(command);
}

// Return a reference to the history of commands
const vector<BaseCommand*>& Environment::getHistory() const{
    return commandsHistory;
}
