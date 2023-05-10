#include "graphicsDisplay.h"
#include "board.h"
#include "window.h"
#include <string>

using namespace std;

// see textdisplay.h for method descriptions

GraphicsDisplay::~GraphicsDisplay() {}

void GraphicsDisplay::displayBackground() {
    xw.fillRectangle(0, 0, 900, 700, Xwindow::Green);
    xw.fillRectangle(25, 120, 555, 555, Xwindow::White);
    xw.fillRectangle(74, 144, 482, 482, Xwindow::Black);

    int rownum = rows;
    for (int r = 0; r < rows; ++r) {
        xw.drawString(50, 180+(r*60), to_string(rownum));
        for (int c = 0; c < cols; ++c) {
            if ((r+c)%2 == 0) {
                xw.fillRectangle(75+(r*60), 145+(c*60), 60, 60, Xwindow::White);
            } else if ((r+c)%2 == 1) {
                xw.fillRectangle(75+(r*60), 145+(c*60), 60, 60, Xwindow::Blue);
            }
        }
        --rownum;
    }
    xw.drawString(103, 650, "a");
    xw.drawString(103+60, 650, "b");
    xw.drawString(103+120, 650, "c");
    xw.drawString(103+180, 650, "d");
    xw.drawString(103+240, 650, "e");
    xw.drawString(103+300, 650, "f");
    xw.drawString(103+360, 650, "g");
    xw.drawString(103+420, 650, "h");
}

void GraphicsDisplay::displayBoardState() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if ((r+c)%2 == 0) {
                xw.fillRectangle(75+(r*60), 145+(c*60), 60, 60, Xwindow::White);

            } else {
                xw.fillRectangle(75+(r*60), 145+(c*60), 60, 60, Xwindow::Blue);

            }
        }
    }
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
                char type[1] = {theBoard->getPieceType(r,c)};
                xw.drawString(103+(c*60), 180+(r*60), type);
        }
    }
}

void GraphicsDisplay::displayMessage(string msg) {
    xw.fillRectangle(25, 90, 875, 20, Xwindow::Green);
    xw.drawString(25, 105, msg);
}

void GraphicsDisplay::printScore(float whitewins, float blackwins) {
    xw.fillRectangle(590, 200, 200, 60, Xwindow::Green);
    string white = "White: " + to_string(whitewins).substr(0,3);
    string black = "Black: " + to_string(blackwins).substr(0,3);
    xw.drawString(600, 210, "Score:");
    xw.drawString(600, 225, white);
    xw.drawString(600, 240, black);
} 

