#ifndef GENGINE_H
#define GENGINE_H

#include <string>
#include <memory>

class Scoreboard;
class Board;
class TextDisplay;
class TurnControl;
class GraphicsDisplay;
class Computer;

// this class communicates with the user and the other classes to orchestrate a game of chess

class GameEngine {

    int level;
    std::unique_ptr<Scoreboard> theScore;
    std::unique_ptr<Board> theBoard;
    std::unique_ptr<TextDisplay> textdisplay;
    std::unique_ptr<GraphicsDisplay> graphicsdisplay;
    std::unique_ptr<TurnControl> turn;
    bool piecesSetUp;
    bool gameInProcess;
    std::string player1; // human or computer[1-4]
    std::string player2;
    std::unique_ptr<Computer>player1computer;
    std::unique_ptr<Computer> player2computer;
    int player1computerlevel; // 1-4
    int player2computerlevel;
   
public:
    GameEngine();
    std::string toUpper(std::string s); // converts a string to uppercase
    void runSession(); // starts a command-line game session
    void startGame(std::string firstplayer, std::string secondplayer, bool isSetup); // starts a new game
    void customSetup(); // sets up pieces in custom configuration
    void endRound(std::string winner); // end a game
    void makeMove(std::string player, std::string fullLine); // tells Board to make a move, whether human or computer
    void displayInfo(std::string msg); // displays information to text display and graphics display
    void displayBoard(); // displays the board to text display and graphics display
    void printFinalScore(); //prints the final results
    void printScore(); //prints the current results
    bool isGameInProgress(); // returns true if there is a current game in progress, false if otherwise
    void setLevel(); // sets computer's level of difficulty
    ~GameEngine();

};

#endif
