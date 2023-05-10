//
// Created by Victoria Li on 2022-11-24.
//

#ifndef CHESSPROJECT_COMPUTER_H
#define CHESSPROJECT_COMPUTER_H


#include <string>
#include <vector>

class Board;
class Piece;

class Computer {
    int level; //indicates the level of the computer ai player
    std::vector<std::string> allMovesW; // stores all possible moves of the all white pieces
    std::vector<std::string> allMovesB; // stores all possible moves of the all black pieces
    std::vector<std::string> safeMovesW; // stores all possible moves of the all safe white pieces (not used right now)
    std::vector<std::string> safeMovesB; // stores all possible moves of the all safe black pieces (not used right now)
    std::vector<std::string> capturingMoveW; // stores all possible capturing moves of the all white pieces
    std::vector<std::string> avoidingMoveW; // stores all possible avoid checking moves of the all white pieces
    std::vector<std::string> checkingMoveW; // stores all possible check moves of the all white pieces
    std::vector<std::string> capturingMoveB; // stores all possible capturing moves of the all black pieces
    std::vector<std::string> avoidingMoveB; // stores all possible avoid checking moves of the all black pieces
    std::vector<std::string> checkingMoveB; // stores all possible check moves of the all black pieces
    // getFirstCoord gets where the piece is moving from
    // returns a string coord (ex. "e4")
    static std::string getFirstCoord(std::string move);
    // getSecondCoord gets where the piece is moving to
    // returns a string coord (ex. "e4")
    static std::string getSecondCoord(std::string move);
    static std::string coordToString(std::pair<int, int> coord);// coordToString converts integer coordinates into string coordinates
    static std::pair<int,int> stringToCoord(std::string coord); // stringToCoord converts string coordinates into int coordinates
    int getLevel() const; // returns the level of computer
    void clearVectors(); // clear all vectors in the field
    void addMove(const Board *b, Piece* p, std::string colour); // add possible moves into the vectors
    int pieceToValue(Piece *p) const;// pieceToValue returns the number value of piece p
    // returns true if piece p can be captured on the board b right now
    // colour is the colour of the piece p
    bool pieceCanBeCaptured(const Board *b, const Piece *p, std::string colour);
    // dangerAfterMove checks if any piece of colour is in danger caused by p moving from {emptyRow, emptyCol} to {newRow, newCol}
    // colour is the colour of p
    bool dangerAfterMove(const Board *b, const Piece *p, int emptyRow, int emptyCol, int newRow, int newCol, std::string colour) const;
    std::string maxCapture(const Board *b, std::string colour) const; // maxCapture returns the capturingMove that captures the opponent's highest value piece
    //maxValueCheck returns a check move that is the most valuable
    // if all the check moves are moving the same piece, it returns a random check move
    std::string maxValueCheck (const Board *b, std::string colour) const;
    std::string maxAvoid (const Board *b, std::string colour) const; // maxAvoid returns the avoiding move that moves the maximum value piece
    std::string levelOne(const Board *b, std::string colour); // the levelOne computer player returns a random legal move
    std::string levelTwo(const Board* b, std::string colour); // the levelTwo computer player prioritizes capturing moves, checking moves
    std::string levelThree(const Board* b, std::string colour); // the levelThree computer player prioritizes avoiding moves, then checking moves, then capturing moves
    // levelfour computer player prioritizes avoiding moves, safe checking moves, safe capturing moves
    std::string levelFour(const Board *b, std::string colour);
    bool canCheck(const Piece *king, const Piece *p, std::pair<int,int> currPos, std::string colour);
    void addCheckMoves(const Board *b, std::string colour); // addCheckMoves adds moves to checkingMove lists where all the check moves are
    // addAvoid adds avoiding moves to colour's avoidingMoves vector
    // the avoiding moves are moves that avoids potential capture by the opponent
    // it won't put piece in danger after moving
    // If there is still piece in danger after the move, it will be a valid avoiding move only if the piece in danger has lower value than he piece that is moving
    // If the opponent's piece that is causing danger can be captured by colour's piece, and capturing this opponent's piece won't be a dangerous move
    // it is also a valid avoiding move
    void addAvoid(const Board *b, std::string colour);
    // addAvoidBetter is for level 4
    // it sorts out avoiding moves from all the moves, and adds to the colour's avoidingMove list
    // it also sort safe moves from allMoves and capturingMoves
    void addAvoidBetter(const Board *b, std::string colour);
public:
    explicit Computer(int level): level{level}{}
    ~Computer();
    std::string move(const Board *b, std::string colour); // call this function to return a string for the next computer move
};




#endif //CHESSPROJECT_COMPUTER_H
