#ifndef __SCOREBOARD_H__
#define __SCOREBOARD_H__
#include <string>

// This class keeps track of the score of the current two-player game session

class Scoreboard{
    std::string Player1;
    std::string Player2;
    float p1wins = 0; 
    float p2wins = 0;
public:
    Scoreboard(std::string Player1, std::string Player2):
        Player1{Player1}, Player2{Player2} {}
    // updates the score with the most recent winner
    void updateScore(std::string winner);
    float getP1Wins(); // returns whitewins
    float getP2Wins(); // returns getblackwins
    ~Scoreboard();
};

#endif
