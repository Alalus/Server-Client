#pragma once
#include "ConnectionHandler.h"
#include "StompProtocol.h"
#include <thread>
#include <string>
#include <iostream>

using namespace std;


class ReadFromSocket
{
    private:
        ConnectionHandler &connectionHandler;
        StompProtocol &protocol;
    public:
        ReadFromSocket(ConnectionHandler &connectionHandler_, StompProtocol &protocol_);
        void run();
};