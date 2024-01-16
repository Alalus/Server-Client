#include "ReadFromKeyboard.h"
#include "Client.h"
#include "StompProtocol.h"

extern bool logout; 
extern bool ended; 
extern bool start; // true if login

ReadFromKeyboard::ReadFromKeyboard(Client &client_, StompProtocol &protocol_): client(client_), protocol(protocol_){}

void ReadFromKeyboard::run(){
        
    while (!ended) {   
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        if((!logout) & (line!="")){ // still logged in and wrote in kb   
            protocol.kbInput(line);
            line="";
        }
        else if((logout) & (line!="") && (line.find("login") == std::string::npos) ){// //try to send messages after logout that are not login messages
            ended = true;
        }
        else if ((logout) & (line.find("login") != std::string::npos)){ // user logged out, and new user try to login
            start = false;
            protocol.kbInput(line);
            line="";
            break; // join keyboard thread
        }                
    }
        
    
}
		

