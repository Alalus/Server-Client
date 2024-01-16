#include "Game.h"
#include "event.h"
#include <string>
#include <map>
#include <iterator>
using namespace std;

Game::Game(Event &e): gameName(e.get_team_a_name() +"_"+ e.get_team_b_name()), team_a_name(e.get_team_a_name()), team_b_name(e.get_team_b_name()), timeEventToDesc(), gameUpdateToValue(), teamAUpdates(), teamBUpdates(){
    addEvent(e);
}

const std::string &Game::getGameName() const{
    return this->gameName;
}

std::string Game::summary(){

    std::string summary;
    summary = team_a_name + " vs " + team_b_name +"\nGame stats:\n";
    std::string generalStats = "General stats:\n";
    std::string teamAStats = team_a_name + " stats:\n";
    std::string teamBStats = team_b_name + " stats:\n";
    std::string gameEventsReports = "Game events reports:\n";
    for(const auto& gen_up : gameUpdateToValue){
        generalStats = generalStats + gen_up.first + " : " + gen_up.second + '\n';
    }
    for(const auto& a_up : teamAUpdates){ 
        teamAStats = teamAStats + a_up.first + " : " + a_up.second + '\n';
    }
    for(const auto& b_up : teamBUpdates){
        teamBStats = teamBStats + b_up.first + " : " + b_up.second + '\n';
    }
    for(const auto& timeE_d : timeEventToDesc){
        gameEventsReports = gameEventsReports + timeE_d.first + '\n' + timeE_d.second + '\n';
    }
    summary = summary + generalStats + teamAStats + teamBStats + gameEventsReports;
    return summary;
}

void Game::addEvent(Event &e){

    for(const auto& gameUp : e.get_game_updates()){
        gameUpdateToValue.insert({gameUp.first, gameUp.second});
    }
    for(const auto& a_up : e.get_team_a_updates()){
        teamAUpdates.insert({a_up.first, a_up.second});
    }
    for(const auto& b_up : e.get_team_b_updates()){
        teamBUpdates.insert({b_up.first, b_up.second});
    }
    timeEventToDesc.insert({std::to_string(e.get_time())+":"+ e.get_name(), e.get_discription()});

}