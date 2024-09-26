#include <iostream>
#include <windows.h>

// declaring the functions to be used
int server(char** argv);
int client(char** argv, char* allias);

// user allias variables
const int alliasLen = 10;
char allias[alliasLen];

int main(int argc, char* argv[]){

    // planned usage: ./talkApp <option> <node_name> <service_name>
    // option should be either 'server' or 'client'

    // check the argument count:
    if (argc != 4){
        std::cout << "Correct usage: " << argv[0] << " <option> <node_name> <service_name>.\n";
        return 1;
    }

    // configure talkApp as either server or client
    if (strcmp(argv[1], "server") == 0){
        std::cout << "Configuring as server...\n\n";
        server(argv);
    }
    else if (strcmp(argv[1], "client") == 0){
        std::cout << "Configuring as client...\n";
        ZeroMemory(allias, alliasLen);
        std::cout << "Enter allias (max 10 characters): ";
        std::cin.getline(allias, alliasLen);
        client(argv, allias);
    }
    else{
        std::cout << "Invalid option: First argument can be either 'server' or 'client'.\n";
        return 1;
    }

    return 0;
}