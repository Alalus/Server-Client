#include "StompProtocol.h"
#include "event.h"
#include <string>
#include <map>

extern bool logout;
extern bool start;

StompProtocol::StompProtocol(ConnectionHandler &ch, Client &cl): connectionHandler(ch), client(cl), messagesToSend(){}

void StompProtocol::sInput(std::string message) // messages in socket from server
{
    std::string messageType = getMessageType(message);
    if(messageType == "CONNECTED"){
        start = true;
        logout = false;
        std::cout<< "Login successful" <<std::endl;
        message = "";
    }
    else if(messageType == "ERROR"){
        logout = true;
        connectionHandler.close();
    }
    else if(messageType == "RECEIPT"){ 
        std::string id = getValue(message,"id");
        int receiptId = std::stoi(id);
        std::string recpM = client.getReceiptM(receiptId); // return command + {...}
        std::string command = recpM.substr(0, recpM.find(" "));
    
        if (command == "join"){ // subscribe success
            std::string game_name = recpM.substr(recpM.find(" ") + 1);
            int subID = client.getSubId();
            client.setIdBySub(game_name, subID, false);
            std::cout<< "Joined channel " + game_name <<std::endl;
            client.removeReceipt(receiptId);
        }

        if (command == "exit"){ // unsubscribe success
            std::string game_name = recpM.substr(recpM.find(" ") + 1);
            int subID = client.getIdBySub(game_name);
            client.setIdBySub(game_name, subID, true); // want to remove existing sub, toRemove = true
            std::cout<< "Exited channel " + game_name <<std::endl;
            client.removeReceipt(receiptId);
        }

        if (command == "logout"){ // close the socket
            client.removeReceipt(receiptId); 
            client.setClient("","");
            logout = true;     
            std::cout << "Logout... \n" << std::endl;
            connectionHandler.close();
        }

    }

    else if(messageType == "MESSAGE"){

        std::string username = getValue(message, "user");
        std::string team_a = getValue(message, "team a");
        std::string team_b = getValue(message, "team b");
        std::string event_name = getValue(message, "event name");
        int time = stoi(getValue(message, "time"));

        std::string target0 = "general game updates:";
        int start_g = message.find(target0) + target0.size()+1;
        std::string sec_g = message.substr(start_g);
        std::string target1 = "team a updates";
        int end0 = sec_g.find(target1);
        std::string sec_general = sec_g.substr(0, end0);

        std::string target2 = "team a updates:";
        int start_a = message.find(target2) + target2.size()+1;
        std::string sec_a0 = message.substr(start_a);
        std::string target3 = "team b updates:";
        int end1 = sec_a0.find(target3);
        std::string sec_a = sec_a0.substr(0, end1);
        std::string target4 = "team b updates:";
        int start_b = message.find(target4) + target4.size()+1;
        std::string sec_b0 = message.substr(start_b);
        std::string target5 = "description:";
        int end2 = sec_b0.find(target5);
        std::string sec_b = sec_b0.substr(0, end2);
        
        std::string target6 = "description:";
        int start_d = message.find(target6) + target6.size()+1;
        std::string sec_des = message.substr(start_d);
        
        std::map<std::string ,std::string> generalUpdate;
        std::map<std::string ,std::string> team_a_updates;
        std::map<std::string ,std::string> team_b_updates;
        int size_g = std::count(sec_general.begin(), sec_general.end(), ':');
        int size_a = std::count(sec_a.begin(), sec_a.end(), ':');
        int size_b = std::count(sec_b.begin(), sec_b.end(), ':');
        
        for(int i = 0; i < size_g ; i++){ // general updates

            std::string line = sec_general.substr(0, sec_general.find('\n'));
            std::string t0 = ":";
            int s0 = line.find(t0);
            std::string left = line.substr(0, s0);
            std::string right = line.substr(s0 + 1);
            generalUpdate.insert({left, right});
            sec_general = sec_general.substr(line.size()+1);

        }
        for(int i = 0; i < size_a; i++){ // team a updates

            std::string line = sec_a.substr(0, sec_a.find('\n'));
            std::string t0 = ":";
            int s0 = line.find(t0);
            std::string left = line.substr(0, s0);
            std::string right = line.substr(s0 + 1);
            team_a_updates.insert({left, right});
            sec_a = sec_a.substr(line.size()+1);
            
        }
        for(int i = 0; i < size_b; i++){ // team b updates
            
            std::string line = sec_b.substr(0, sec_b.find('\n'));
            std::string t0 = ":";
            int s0 = line.find(t0);
            std::string left = line.substr(0, s0);
            std::string right = line.substr(s0 + 1);
            team_b_updates.insert({left, right});
            sec_b = sec_b.substr(line.size()+1);
        }
        
        Event e(team_a, team_b, event_name, time, generalUpdate, team_a_updates, team_b_updates, sec_des);
        client.addEvent(username, e);
    }

}

void StompProtocol::kbInput(std::string message) // messages in terminal from keyboard
{
    
    int i = message.find(" ");
    std::string command = message.substr(0,i);

    if(command == "join"){//Subscribe
        std::string gameName = message.substr(i+1);
        std::string answer="SUBSCRIBE\ndestination:/" + gameName;
        int recID = client.getRecId();
        int subID = client.getSubId();
        answer=answer+"\nid:" + std::to_string(subID) + "\nreceipt:" + std::to_string(recID) +"\n\n" +'\0';
        client.addReceipt(recID, message);
        connectionHandler.sendLine(answer);

    }
    else if(command == "exit"){//Unsubscribe
        std::string gameName = message.substr(i+1);
        int recID = client.getRecId();
        int subID = client.getIdBySub(gameName);
        std::string answer="UNSUBSCRIBE\nid:" + std::to_string(subID) + "\nreceipt:" + std::to_string(recID) +"\n\n"+'\0';
        client.addReceipt(recID, message);
        connectionHandler.sendLine(answer);

    } 
    else if(command == "report"){//send
        names_and_events newEvents = parseEventsFile(message.substr(i + 1, message.size()));
        
        for(unsigned int n = 0; n < newEvents.events.size(); n++)
        {   
            std::string topic = newEvents.team_a_name + "_" +newEvents.team_b_name;
            if(client.getIdBySub(topic) == 0){
                std::cout << "User is not subscribed to: "<< topic << std::endl;
                break;
            }
            std::string sendMessage="SEND\ndestination:/" + topic +"\n\nuser:" + client.getUsername()+ '\n';
            sendMessage = sendMessage + "team a: " + newEvents.team_a_name + "\nteam b: " + newEvents.team_b_name + "\n";
            sendMessage=sendMessage +"event name:" + newEvents.events[n].get_name()+"\n";
            sendMessage=sendMessage +"time:" + std::to_string(newEvents.events[n].get_time())+"\ngeneral game updates:\n";
            std::map<std::string, std::string> game_updates = newEvents.events[n].get_game_updates();
            std::map<std::string, std::string>::iterator itGStats = game_updates.begin();
            while (itGStats != game_updates.end())
            {
                sendMessage=sendMessage + itGStats->first + " : " + itGStats->second + '\n';
                itGStats++;
            }
            sendMessage = sendMessage + "team a updates:\n";
            // map of all team a updates the second type can be a string bool or int
            std::map<std::string, std::string> team_a_updates = newEvents.events[n].get_team_a_updates();
            std::map<std::string, std::string>::iterator itTAStats = team_a_updates.begin();
            while (itTAStats != team_a_updates.end())
            {
                sendMessage=sendMessage + itTAStats->first + " : " + itTAStats->second + '\n';
                itTAStats++;
            }
            sendMessage=sendMessage + "team b updates:\n";
            // map of all team b updates
            std::map<std::string, std::string> team_b_updates = newEvents.events[n].get_team_b_updates();
            std::map<std::string, std::string>::iterator itTBStats = team_b_updates.begin();
            while (itTBStats != team_b_updates.end())
            {
                sendMessage=sendMessage + itTBStats->first + " : " + itTBStats->second + '\n';
                itTBStats++;
            }
            sendMessage=sendMessage + "description:\n"+ newEvents.events[n].get_discription()+"\n\n"+'\0';
            messagesToSend.push_back(sendMessage);
        }
        for(std::string msg : messagesToSend){
            connectionHandler.sendLine(msg);
        }
        messagesToSend.clear();
    }

    else if(command == "logout"){//disconnect
        int recID = client.getRecId();
        std::string answer="DISCONNECT\nreceipt:"+std::to_string(recID)+"\n\n"+'\0';
        client.addReceipt(recID, message);
        connectionHandler.sendLine(answer);

    }

    else if(command == "summary"){
        std::string str = message.substr(i + 1);
        std::string gameName = str.substr(0, str.find(" "));
        str = str.substr(str.find(" ")+1);
        std::string userToGet = str.substr(0, str.find(" "));
        str = str.substr(str.find(" ")+1);
        std::string file =  str;
        client.summary(gameName, userToGet, file);
    }

    else if(command == "login"){
        if(logout){
            if(connectionHandler.connect()){
                std::string answer = "\nCONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\n";
                message = message.substr(i+1);
                i = message.find(" ");
                std::string user_pass = message.substr(i+1);
                std::string username = user_pass.substr(0, user_pass.find(" "));
                std::string password = user_pass.substr(user_pass.find(" ") + 1);    
                answer = answer + "login:" + username + "\npasscode:" + password + "\n\n" + '\0';
                client.setClient(username, password);
                client.addReceipt(client.getRecId(), message);
                connectionHandler.sendLine(answer); 
            }
            else{
                std::cerr << "Cannot connect to server" << std::endl;
                // connectionHandler.close();
                return;
            }   
        }
        else{ 
            std::cout<< "The client is already logged in, log out before trying again." <<std::endl;
        }
    }

}

std::string StompProtocol::getValue(std::string msg, std::string value)
{
    int start = msg.find(value);
    std::string ans=msg.substr(start);
    return ans.substr(ans.find(':') + 1, ans.find('\n'));
}

std::string StompProtocol::getMessageType(std::string msg)
{
    if(msg.find('\n')==0){ 
        msg = msg.substr(1);
    }
    int end = msg.find('\n');
    return msg.substr(0, end);
}

std::string StompProtocol::getProblem(string msg)
{
    int start1 = msg.find("----");
    std::string str1 = msg.substr(start1 + 4);
    int start2 = str1.find("----");
    std::string ans = str1.substr(start2 + 4, str1.size() - 1);
    return ans;
}

// connected
// receipt
// error
// send

