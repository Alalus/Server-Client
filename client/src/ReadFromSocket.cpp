#include "ReadFromSocket.h"
#include "ConnectionHandler.h"
#include "StompProtocol.h"

extern bool logout;
extern bool start; // true if login

ReadFromSocket::ReadFromSocket(ConnectionHandler &connectionHandler_, StompProtocol &protocol_): connectionHandler(connectionHandler_), protocol(protocol_){}

void ReadFromSocket::run(){

    std::string message;
    while(!logout){   // read from socket just if didnt logged out
        if((start) && (connectionHandler.getLine(message))){ // if logged in and got message from socket
            protocol.sInput(message);
            message = "";
        } 
    }     
}