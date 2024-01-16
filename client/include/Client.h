#pragma once
#include <string>
#include <map>
#include <atomic>
#include "Game.h"

using namespace std;
class Event;

class Client
{
private:
    std::string username;
    std::string password;
    int subId;
    int recId;
    std::map<int, std::string> recpById; // receiptId, receiptMessage
    std::map<std::string, int> idBySub;  // subName = gameName, subId 
    std::map<std::string, std::map<std::string, Game&>> gameNameToUserAGame; 
    bool connected;

public:
    Client();
    std::string &getUsername();
    int &getSubId();
    int &getRecId();
    void addReceipt(int receiptId, std::string line);
    void removeReceipt(int receiptId);
    std::string getReceiptM(int id);
    void addEvent(std::string userMessage, Event &e); //
    void summary(std::string gameName, std::string user, std::string fileName);
    void setClient(std::string username, std::string password);
    int getIdBySub(std::string sub); //return subId
    void setIdBySub(std::string subName, int subId, bool toRemove);

};

