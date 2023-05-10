#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "window.h"
class Board;


class GraphicsDisplay {
    int rows = 8, cols = 8;
    Board *theBoard;
    Xwindow xw{900, 700};

    public:

    explicit GraphicsDisplay(Board *theBoard): theBoard{theBoard} {}

    Board *&board() { return theBoard; } // updates the Board
    void displayBackground(); // displays the empty chess board and surroundings
    void displayBoardState(); // displays the current chess board
    void displayMessage(std::string msg); // displays a message above the chess board
    void printScore(float whitewins, float blackwins); // prints the score beside the chess board
    ~GraphicsDisplay();

};

#endif
