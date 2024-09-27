# TalkApp
* This is basic project of a chatting app that works on LAN for Windows. 
* Use cmake with the CMakeLists.txt in the repository for compilation. 

## Compilation
* Make sure Windows SDK and cmake are installed before attempting compilation.
1. Clone the repository.
2. Open up a terminal window and go to the directory where you cloned the repository.
3. run "cmake --build ." 

## Usage
1. You need to initialize a server by running "./talkApp server [local-ip-address] [port-number]" on a terminal window.
2. Now, other computers on LAN can connect and start chatting by running "./talkApp client [server-ip-address] [port-no]" on their terminals.

## Future Work
* A better organization for server and client functions must be implemented.
* Will implement pausing times in the infinite loops to reduce cpu usage.
* Alliasing curently does not work. Need to work on that.
