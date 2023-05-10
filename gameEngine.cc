#include "board.h"
#include "gameEngine.h"
#include "textdisplay.h"
#include "scoreboard.h"
#include "turnControl.h"
#include "boardException.h"
#include "graphicsDisplay.h"
#include "computer.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>

using namespace std;

// see gameEngine.h for method explanations

GameEngine::GameEngine() {
    // initialize
    theScore = make_unique<Scoreboard>("White", "Black");
    theBoard = make_unique<Board>();
    textdisplay = make_unique<TextDisplay>(theBoard.get());
    graphicsdisplay = make_unique<GraphicsDisplay>(theBoard.get());
    turn = make_unique<TurnControl>("White", "Black", "White");
    gameInProcess = false;
    player1computer = nullptr;
    player2computer = nullptr;
    player1computerlevel = 0; // 1-4
    player2computerlevel = 0;
}

void GameEngine::runSession() {
    piecesSetUp = false; // if pieces have been pre-set in setup mode

    graphicsdisplay->displayBackground();

    string line; // to read commands until EOF - commands must be entered in one line, i.e. move e1 e3 instead of move \n e1 e3
    while (getline(cin, line)) {

        string command;
        stringstream ss {line};
        ss >> command;

        if (command == "game") {

            if (gameInProcess) {
                displayInfo("there is a game already in progress.");
                continue;
            }
            string player1, player2;
            ss >> player1 >> player2;
            // players can be any combination of human or computer[1-3]
            // put the level in player1computerlevel or player2computerlevel
            if (player1 == "computer[1]") {
                player1computerlevel = 1;
            } else if (player1 == "computer[2]") {
                player1computerlevel = 2;
            } else if (player1 == "computer[3]") {
                player1computerlevel = 3;
            } else if (player1 == "computer[4]") {
                player1computerlevel = 4;
            }
            if (player2 == "computer[1]") {
                player2computerlevel = 1;
            } else if (player2 == "computer[2]") {
                player2computerlevel = 2;
            } else if (player2 == "computer[3]") {
                player2computerlevel = 3;
            } else if (player2 == "computer[4]") {
                player1computerlevel = 4;
            }
            
            while ((player1 != "human" && player1 != "computer[1]" && player1 != "computer[2]" && player1 != "computer[3]" && player1 != "computer[4]")
            || (player2 != "human" && player2 != "computer[1]" && player2 != "computer[2]" && player2 != "computer[3]" && player2 != "computer[4]")) {
                cout << "please enter valid players - must be human or computer." << endl;
                cin >> player1 >> player2;
            }
            // start the game
            startGame(player1, player2, piecesSetUp);

        } else if (command == "setup") {
            // pieces cannot be set up if there is already a game in progress
            if (isGameInProgress()) {
                // cout << "there is a game already in progress." << endl;
                displayInfo("there is a game already in progress.");
            } else {
                // setup the pieces
                customSetup();
                getline(cin, line); // so the newline from customSetup() is not taken as an invalid command
            }

        } else if (command == "resign") {
            // end the game, clean up the board, and update score
            string msg = turn->whoseTurn() + " resigned.";
            displayInfo(msg);
            if (turn->whoseTurn() == turn->getPlayer1()) endRound(turn->getPlayer2());
            else endRound(turn->getPlayer1());
            piecesSetUp = false;

        } else if (command == "move") {
            // make a move then print the board
            if (isGameInProgress()) {
                string currturn = turn->whoseTurn();
                makeMove(currturn, line);
            } else {
                displayInfo("the game has not started yet.");
            }
        
        } else if (command == "print") {
            displayBoard(); // print command for testing
            
        } else {
            displayInfo("please enter a valid command.");
        }
    }
    // prints final score once all games are finished
    printFinalScore();
}

void GameEngine::startGame(string firstplayer, string secondplayer, bool isSetup) {
    player1 = firstplayer;
    player2 = secondplayer;
    // if firstplayer == computer[1-3], set player1computer to new computer pointer(level)
    // same thing for player 2
    // set player1computerlevel/player2computerlevel as necessary
    if (player1 == "computer[1]") {
        player1computer = unique_ptr<Computer>(new Computer(1));
        player1computerlevel = 1;
    } else if (player1 == "computer[2]") {
        player1computer = unique_ptr<Computer>(new Computer(2));
        player1computerlevel = 2;
    } else if (player1 == "computer[3]") {
        player1computer = unique_ptr<Computer>(new Computer(3));
        player1computerlevel = 3;
    } else if (player1 == "computer[4]") {
        player1computer = unique_ptr<Computer>(new Computer(4));
        player1computerlevel = 4;
    }
    if (player2 == "computer[1]") {
        player2computerlevel = 1;
        player2computer = unique_ptr<Computer>(new Computer(1));
    } else if (player2 == "computer[2]") {
        player2computerlevel = 2;
        player2computer = unique_ptr<Computer>(new Computer(2));
    } else if (player2 == "computer[3]") {
        player2computerlevel = 3;
        player2computer = unique_ptr<Computer>(new Computer(3));
    } else if (player2 == "computer[4]") {
        player2computerlevel = 4;
        player2computer = unique_ptr<Computer>(new Computer(4));
    }

    if (!isSetup) {
        // if piece's haven't already been set up, place the pieces in the default configuration
        displayInfo("setting up default pieces...");
        theBoard->setupDefault();
    }
    string msg = player1 + " vs. " + player2 + " game has started. " + turn->whoseTurn() + "'s move.";
    displayInfo(msg);
    displayBoard();
    gameInProcess = true;
}

string GameEngine::toUpper(string s) {
    string upper = s;
    for (int i = 0; i < s.length(); i++) {
        upper[i] = toupper(s[i]);
    }
    return upper;
}

void GameEngine::customSetup() {
    graphicsdisplay->displayBoardState();
    displayInfo("in setup mode.");
    theBoard->putInSetup();
    string command;
    while (cin >> command) {
        if (command == "+") { // add piece
            char type;
            string coords;
            cin >> type >> coords;
            try { 
                theBoard->addPiece(type, coords); 
                displayBoard();
            } catch (BoardException e) { // catch exceptions if they arise
                displayInfo(e.what());
            }

        } else if (command == "-") { // remove piece
            string coords;
            cin >> coords;
            try { 
                theBoard->removePiece(coords); 
                displayBoard();
            } catch (BoardException e) { // catch exceptions if they arise
                displayInfo(e.what());
            }

        } else if (command == "=") { // change turn
            string colour;
            cin >> colour;
            if (toUpper(colour) != toUpper(turn->getPlayer1()) && toUpper(colour) != toUpper(turn->getPlayer2())) { // input is not a player
                string msg = "invalid colour; must be " + turn->getPlayer1() + " or " + turn->getPlayer2() + ". please enter command again.";
                displayInfo(msg);
            } else { // set the turn to the given player
                if (toUpper(colour) == toUpper(turn->getPlayer1())) {
                    turn->setTurn(turn->getPlayer1());
                    string msg = turn->getPlayer1() + " moves first.";
                    displayInfo(msg);
                } else {
                    turn->setTurn(turn->getPlayer2());
                    string msg = turn->getPlayer2() + " moves first.";
                    displayInfo(msg);
                }
            }

        } else if (command == "print") { // print board 
            displayBoard();

        } else if (command == "done") {
            // check if setup is valid
            if (theBoard->canLeaveSetup()) {
                theBoard->LeaveSetup();
                displayInfo("exited setup mode.");
                break;
            } else { // otherwise must continue setting up
                displayInfo("setup is incomplete or invalid. please continue setup. check that there are two Kings and no player is in check or stalemate.");
            }
        } else {
            displayInfo("please enter a valid setup command.");
        }
    }
    piecesSetUp = true;
}

void GameEngine::endRound(string winner) {
    theBoard->clearBoard();
    theScore->updateScore(winner);
    printScore();
    gameInProcess = false;
    piecesSetUp = false;
    turn->setTurn(turn->getPlayer1());
}

void GameEngine::makeMove(string player, string fullLine) {
    string playertype; // human or computer
    Computer *computerPlayer;
    string colour; // = turn->getPlayer1() or turn->getPlayer2()
    if (turn->whoseTurn() == turn->getPlayer1()) {
        playertype = player1;
        if (playertype == "computer[1]" || playertype == "computer[2]" || playertype == "computer[3]" || playertype == "computer[4]"){
            playertype = "computer";
            computerPlayer = player1computer.get();
            colour = turn->getPlayer1();
        }
    } else {
        playertype = player2;
        if (playertype == "computer[1]" || playertype == "computer[2]" || playertype == "computer[3]" || playertype == "computer[4]"){
            playertype = "computer";
            computerPlayer = player2computer.get();
            colour = turn->getPlayer2();
        }
    }

    if (colour == "White") colour = "white";
    else if (colour == "Black") colour = "black";

    string next;
    stringstream ss {fullLine};
    ss >> next; // read "move"
    string start, dest;
    if (!(ss >> next)) { // no other commands in line - indicates computer
        if (playertype != "computer") {
            displayInfo("please enter a human move command.");
            return;
        }
        string comp = computerPlayer->move(theBoard.get(), colour);
        stringstream ss {comp};
        string start, dest, piece;
        ss >> next; // read move
        ss >> start >> dest;
        if (ss >> piece) {
            if (piece.size() == 1) { // if third argument indicates pawn promotion
                try { 
                    theBoard->pawnPromotion(player, start, dest, piece[0]); 
                    string msg = player + " made a move.";
                    displayInfo(msg);
                } catch (BoardException e) { // catch exceptions if they arise
                    displayInfo(e.what());
                    return;
                }
            } else {
                displayInfo("Not a valid move command. Try again.");
            }
        } else {
                try { 
                    theBoard->makeMove(player, start, dest);
                    string msg = player + " made a move.";
                    displayInfo(msg);
                } catch (BoardException e) { // catch exceptions if they arise
                    displayInfo(e.what());
                    return;
                }
        }
    } else {
        if (playertype != "human") {
            displayInfo("Please enter a computer move command.");
            return;
        }
        start = next;
        ss  >> dest;
        if (ss >> next) {
            if (next.size() == 1) { // if third argument indicates pawn promotion
                try { 
                    theBoard->pawnPromotion(player, start, dest, next[0]); 
                    string msg = player + " made a move.";
                    displayInfo(msg);
                } catch (BoardException e) { // catch exceptions if they arise
                    displayInfo(e.what());
                    return;
                }
            } else {
                displayInfo("Not a valid move command. Try again.");
            }
        } else { // no pawn promotion
                try { 
                    theBoard->makeMove(player, start, dest);
                    string msg = player + " made a move.";
                    displayInfo(msg);
                } catch (BoardException e) { // catch exceptions if they arise
                    displayInfo(e.what());
                    return;
                }
        }
    }

    try {
        if (turn->whoseTurn() == turn->getPlayer1() && theBoard->isCheckmate(turn->getPlayer2())) { // check if opponent is in checkmate
            displayBoard();
            string msg = "Checkmate! " + turn->getPlayer1() + " wins.";
            displayInfo(msg);
            endRound(turn->getPlayer1());
            return;
        } else if (turn->whoseTurn() == turn->getPlayer2() && theBoard->isCheckmate(turn->getPlayer1())) {
            displayBoard();
            string msg = "Checkmate! " + turn->getPlayer2() + " wins.";
            displayInfo(msg);
            endRound(turn->getPlayer2());
            return;
        } else if (turn->whoseTurn() == turn->getPlayer2() && theBoard->isCheck(turn->getPlayer1())) { // check if opponent is in check
            string msg = turn->getPlayer1() + " is in check.";
            displayInfo(msg);
        } else if (turn->whoseTurn() == turn->getPlayer1() && theBoard->isCheck(turn->getPlayer2())) {
            string msg = turn->getPlayer2() + " is in check.";
            displayInfo(msg);
        } else if ((turn->whoseTurn() == turn->getPlayer1() && theBoard->isStalemate(turn->getPlayer2())) || 
                   (turn->whoseTurn() == turn->getPlayer2() && theBoard->isStalemate(turn->getPlayer1()))) { // check if opponent is in stalemate
            displayBoard();
            displayInfo("Stalemate!");
            endRound("tie");
            return;
        } 
        displayBoard();
        turn->changeTurn();
        string msg = turn->whoseTurn() + "'s move.";
        displayInfo(msg);

    } catch (BoardException e) {
        cout << e.what() << endl;
    }
}

bool GameEngine::isGameInProgress() {
    return gameInProcess;
}

void GameEngine::displayBoard() {
    textdisplay->board() = theBoard.get();
    textdisplay->displayBoard();
    graphicsdisplay->board() = theBoard.get();
    graphicsdisplay->displayBoardState();
}

void GameEngine::displayInfo(string msg) {
    cout << msg << endl;
    graphicsdisplay->displayMessage(msg);
}

void GameEngine::printFinalScore() {
    float p1wins = theScore->getP1Wins();
    float p2wins = theScore->getP2Wins();
    textdisplay->printFinalScore(p1wins, p2wins);
}

void GameEngine::printScore() {
    float p1wins = theScore->getP1Wins();
    float p2wins = theScore->getP2Wins();
    textdisplay->printScore(p1wins, p2wins);
    graphicsdisplay->printScore(p1wins, p2wins);
}

GameEngine::~GameEngine() {}
