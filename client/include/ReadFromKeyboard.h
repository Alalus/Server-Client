#pragma once
#include "Client.h"
#include "StompProtocol.h"
#include <thread>
#include <iostream>

using namespace std;


class ReadFromKeyboard
{
    private:
        Client &client;
        StompProtocol &protocol;
    public:
        ReadFromKeyboard(Client &client_, StompProtocol &protocol_);
        void run();
                
};