#include "turnControl.h"
#include <string>

using namespace std;

// see turnControl.h for method descriptions

void TurnControl::setTurn(std::string player) {
    if (player1 == player) {
        isPlayer1 = true;
    } else {
        isPlayer1 = false;
    }
}

void TurnControl::changeTurn() {
    isPlayer1 = !isPlayer1;
}

string TurnControl::whoseTurn() {
    if (isPlayer1) {
        return player1;
    } else {
        return player2;
    }    
}

string TurnControl::getPlayer1() {
    return player1;
}

string TurnControl::getPlayer2() {
    return player2;
}

TurnControl::~TurnControl() {}
