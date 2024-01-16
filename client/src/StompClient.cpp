#include <stdlib.h>
#include <thread>
#include "ConnectionHandler.h"
#include "Client.h"
#include "StompProtocol.h"
#include "ReadFromSocket.h"
#include "ReadFromKeyboard.h"

bool logout = false;
bool ended = false; // true if logout or disconnect
bool start = false;

int main()
{
    // get first command
    const short bufsize = 1024;
    char buf[bufsize];
    std::cin.getline(buf, bufsize);
	std::string line(buf);

    std::string host_port;
    std::string host;  
    std::string port;
    std::string answer;
   
    if(line.substr(0, line.find(" ")) != "login"){
        return 1;
    }
    // login Frame
        line = line.substr(line.find(" ") + 1);
        host_port = line.substr(0, line.find(" "));  
        host = host_port.substr(0, host_port.find(":"));
        port = host_port.substr(host_port.find(":") + 1);
        line = line.substr(line.find(" ") + 1);
        answer = "\nCONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\n"; 
        std::string username = line.substr(0, line.find(" "));
        std::string password = line.substr(line.find(" ") + 1);            
        answer += "login:" + username + "\npasscode:" + password + "\n\n" + '\0';    
    
        short p = stoi(port);
        ConnectionHandler connectionHandler(host, p);
        bool success = connectionHandler.connect();
        if (!success) {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            connectionHandler.close();
            return 1;
        }
        start = true; 
    
        Client *client = new Client();
        client->setClient(username, password);  
        StompProtocol protocol(connectionHandler, *client);
        ReadFromSocket sRead(connectionHandler, protocol);
        ReadFromKeyboard kRead(*client, protocol);
        connectionHandler.sendLine(answer); // first login
    
        while(!ended){
            std::thread keyboardRead(&ReadFromKeyboard::run, &kRead); // thread for keyboard reading
            std::thread socketRead(&ReadFromSocket::run, &sRead); // thread for socket reading

            socketRead.join(); // wait for socket reading to end
            keyboardRead.join(); // wait for keyboard reading to end
        }
        delete client; 
        return 0;
}
