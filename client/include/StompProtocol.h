#pragma once
#include <string>
#include "ConnectionHandler.h"
#include "Client.h"
#include <vector>
using namespace std;
// TODO: implement the STOMP protocol


class StompProtocol
{
    private: 
    ConnectionHandler &connectionHandler; // client connection handler
    Client &client;
    std::vector<std::string> messagesToSend;

    public:
    StompProtocol(ConnectionHandler &ch, Client &cl);
    void sInput(std::string message);
    void kbInput(std::string message);
    std::string getValue(std::string msg, std::string value);
    std::string getMessageType(std::string msg);
    std::string getProblem(std::string msg);

};
