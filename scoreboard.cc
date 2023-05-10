#include "scoreboard.h"

using namespace std;

// see scoreboard.h for method descriptions

void Scoreboard::updateScore(std::string winner) {
    if (winner == Player1) ++p1wins;
    else if (winner == Player2) ++p2wins;
    else if (winner == "tie" || winner == "Tie") {
        p1wins = p1wins + 0.5;
        p2wins = p2wins + 0.5;
    }
}

float Scoreboard::getP1Wins() {
    return p1wins;
}

float Scoreboard::getP2Wins() {
    return p2wins;
}

Scoreboard::~Scoreboard() {}
