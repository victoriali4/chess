#include "textdisplay.h"
#include "board.h"
#include <iostream>

using namespace std;

// see textdisplay.h for method descriptions

TextDisplay::~TextDisplay() {}

void TextDisplay::displayBoard() {
    int rownum = rows;
    for (int r = 0; r < rows; ++r) {
        cout << rownum << ' ';
        for (int c = 0; c < cols; ++c) {
            if (theBoard->getPieceType(r,c) == ' ' && (r+c)%2 == 0) {
                cout << ' ';
            } else if (theBoard->getPieceType(r,c) == ' ' && (r+c)%2 == 1) {
                cout << '_';
            } else {
                cout << theBoard->getPieceType(r,c);
            }
        }
        cout << endl;
        --rownum;
    }
    cout << endl;
    cout << "  abcdefgh" << endl;
}

void TextDisplay::printScore(float whitewins, float blackwins) {
    cout << "Current Score:" << endl;
    cout << "White: " << whitewins << endl;
    cout << "Black: " << blackwins << endl;
} 
void TextDisplay::printFinalScore(float whitewins, float blackwins) {
    cout << "Final Score:" << endl;
    cout << "White: " << whitewins << endl;
    cout << "Black: " << blackwins << endl;    
}

