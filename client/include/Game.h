#pragma once
#include <map>
#include <string>
#include <iterator>

using namespace std;
class Event;

class Game{
    private:
    std::string gameName;
    std::string team_a_name; // name of team a
    std::string team_b_name; // name of team b

    std::map<std::string, std::string> timeEventToDesc; // timeEvent, description
    std::map<std::string, std::string> gameUpdateToValue; 
    std::map<std::string, std::string> teamAUpdates;
    std::map<std::string, std::string> teamBUpdates;


    public:
    Game(Event &e);
    const std::string &getGameName() const;
    std::string summary();
    void addEvent(Event &e);
    
};