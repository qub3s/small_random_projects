#pragma once

class gamestate{
    public:
        int combinations;
        int max_lvl;
        int num_players;
        int choices;       
    
    gamestate(int choices, int combinations, int max_lvl, int num_players){
        if(max_lvl%num_players != 0){
            throw "max_lvl needs to be dividable by num_players!";
        }
        
        this->choices = choices;
        this->combinations = combinations;
        this->max_lvl = max_lvl;
        this->num_players = num_players;
    }
};
