#ifndef TURN_H
#define TURN_H

#include <string>

// this class provides the player turn control for a two-person alternating game

class TurnControl {
    std::string player1;
    std::string player2;
    std::string startingPlayer;
    bool isPlayer1;

    public:
    TurnControl(std::string player1, std::string player2, std::string startPlayer):
        player1{player1}, player2{player2}, startingPlayer{startPlayer} {
            if (startingPlayer == player1) isPlayer1 = true;
            else isPlayer1 = false;
        }
    void setTurn(std::string player); // set the next turn to player
    void changeTurn(); // change the turn to the other player
    std::string whoseTurn(); // returns the player that must a move next
    std::string getPlayer1(); // returns player1
    std::string getPlayer2(); // returns player2
    ~TurnControl();
    
};

#endif
