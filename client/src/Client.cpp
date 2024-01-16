#include "Client.h"
#include <fstream>
#include "event.h"
using namespace std;

Client::Client(): username(""), password(""), subId(0), recId(0), recpById(), idBySub(), gameNameToUserAGame(), connected(false){}

std::string &Client::getUsername(){
    return this->username;
}
int &Client::getSubId(){
    subId++;
    return this->subId;
}
int &Client::getRecId(){
    recId++;
    return this->recId;
}
void Client::addReceipt(int receiptId, std::string line){
    recpById.insert(std::pair<int, std::string>(receiptId, line));
}
void Client::removeReceipt(int receiptId){
    recpById.erase(receiptId);
}

std::string Client::getReceiptM(int id){
    return recpById.at(id);
}

void Client::addEvent(std::string userMessage, Event &e) { 
    std::string game = e.get_team_a_name() + "_" + e.get_team_b_name();
    if(gameNameToUserAGame.count(game) > 0){ // game exist 
        if(gameNameToUserAGame.at(game).count(userMessage) > 0){ // user exist
            (gameNameToUserAGame.at(game).at(userMessage)).addEvent(e);
        }
        else{ 
            Game g(e);
            gameNameToUserAGame.at(game).insert({userMessage, g}); 
        }
    }
    else{
        Game g(e);
        std::map<std::string, Game&> temp; 
        temp.insert({userMessage, g});
        gameNameToUserAGame.insert({game, temp}); 
    }

}
void Client::summary(std::string gameName, std::string user, std::string fileName){
   
    if(gameNameToUserAGame.count(gameName) > 0)
    {
        if(gameNameToUserAGame.at(gameName).count(user) > 0)
        {
            std::string summ = gameNameToUserAGame.at(gameName).at(user).summary();
            std::fstream file;
            file.open(fileName, std::ios::out);
            if(file.is_open())
            {
                //std::cout << "file is open" << std::endl;
                file << summ;
                file.close();

            }
        }
    }
    
}

void Client::setClient(std::string username, std::string password){
    this->username = username;
    this->password = password;
}

int Client::getIdBySub(std::string sub)
{
    if(idBySub.count(sub) > 0)
    {
        return idBySub.at(sub);
    }
    return 0;
}
void Client::setIdBySub(std::string subName, int subId, bool toRemove)
{
    if(toRemove && idBySub.count(subName) > 0){
        idBySub.erase(subName);
    }
    else{
        idBySub.insert({subName, subId});
    }
}

 

