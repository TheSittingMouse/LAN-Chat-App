#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <vector>
#include <mutex>

const int recvBufLen = 512;
const int sendBufLen = 512;

int clientAccepting(SOCKET& serverSocket, std::vector<SOCKET>& clients, bool& run){
    int error;

    while (run){
        for (int i = 0; i < clients.size(); ++i){
            if (clients[i] == INVALID_SOCKET && run){
                clients[i] = accept(serverSocket, NULL, NULL);

                if (clients[i] != INVALID_SOCKET){
                    std::cout << "New client recieved: " << clients[i] << "\n";
                }
                else{
                    continue;
                }
            }
        }
    }
    std::cout << "The server has closed accepting.\n";
    return 0;
}

int clientRecv(SOCKET& clientSocket){
    int iRecvRes;
    char recvBuf[recvBufLen];
    do{
        ZeroMemory(recvBuf, recvBufLen);
        iRecvRes = recv(clientSocket, recvBuf, recvBufLen, 0);
        if (clientSocket != INVALID_SOCKET){
            if (iRecvRes == 0){
                std::cout << "Connection has been closed.\n";
                break;
            }
            else if (iRecvRes > 0){
                std::cout << recvBuf << "\n";
            }
            else{
                std::cout << "Error at recieving: " << WSAGetLastError() << "\n";
            }
        }
    } while (iRecvRes > 0);
    return 0;
}


int clientSend(SOCKET& clientSocket, char* allias){
    int iSendRes;
    char sendBuf[sendBufLen];
    std::mutex mtxClSend;

    do{
        ZeroMemory(sendBuf, sendBufLen);
        std::cin.getline(sendBuf, sendBufLen);

        // check for terminating statement
        if (strcmp(sendBuf, "quit") == 0){
            mtxClSend.lock();
            iSendRes = shutdown(clientSocket, 1);
            if (iSendRes == 0){
                std::cout << "Disconnecting...\n";
            }
            else{
                std::cout << "Error while disconnecting: " << WSAGetLastError() << "\n";
            }
            break;
            mtxClSend.unlock();
        } 

        if (clientSocket != INVALID_SOCKET){

            iSendRes = send(clientSocket, sendBuf, sendBufLen, 0);
            if (iSendRes >= 0){
                continue;
            }
            else{
                std::cout << "Error at sending: " << WSAGetLastError() << "\n";
            }
        }
    } while(true);
    return 0;
}


int serverRecv(SOCKET& currentClient, std::vector<SOCKET>& clients, bool& run){
    int error; 
    const int serverBufLen = 512;
    char serverBuf[serverBufLen];

    while (run){
        if (currentClient != INVALID_SOCKET){
            ZeroMemory(serverBuf, serverBufLen);
            error = recv(currentClient, serverBuf, serverBufLen, 0);
            if (error == 0){
                std::cout << "Client disconnected \n";
                closesocket(currentClient);
                currentClient = INVALID_SOCKET;
            }
            else if (error == SOCKET_ERROR){
                //std::cout << "Error at recieving from " << currentClient << " : " << WSAGetLastError() << "\n";
                continue;
            }
            else{
                for (int j =0; j < clients.size(); ++j){
                    if (clients[j] != INVALID_SOCKET && currentClient != clients[j]){
                        error = send(clients[j], serverBuf, serverBufLen, 0);
                        if (error == SOCKET_ERROR){
                            std::cout << "Error at sending to " << clients[j] << " : " << WSAGetLastError() << "\n";
                        }
                    }
                }
            }
        }
    }
    return 0;
}

