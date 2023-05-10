#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

class Board;

// this class displays the chess game components to the screen

class TextDisplay {
  int rows = 8, cols = 8;
  Board *theBoard;

 public:
  explicit TextDisplay(Board *theBoard): theBoard{theBoard} {}

  Board *&board() { return theBoard; } // updates the Board
  void displayBoard(); // displays the current chess board
  void printScore(float whitewins, float blackwins); //prints the current results - for testing purposes
  void printFinalScore(float whitewins, float blackwins); //prints the final results
  ~TextDisplay();
};

#endif
