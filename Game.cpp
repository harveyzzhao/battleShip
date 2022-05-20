#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include "utility.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    vector<Ship>& getShips() {return m_ships;}
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    bool playTurn(Player* attacker, Board& defender);
    
private:
    vector<Ship> m_ships; //Created to contain a list of ships
    int m_rows;
    int m_cols;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols){
    if (nRows <= 0 || nRows > MAXROWS || nCols <= 0 || nCols > MAXCOLS)
        exit(1);
    m_rows = nRows;
    m_cols = nCols;
}

int GameImpl::rows() const
{
    return m_rows;
}

int GameImpl::cols() const
{
    return m_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
//    if (isLegitShip(this, length, symbol, name) == false)   return false;
    m_ships.push_back(Ship(length, symbol, name));
    return true;
}

int GameImpl::nShips() const
{
    return (int)(m_ships.size());
}

int GameImpl::shipLength(int shipId) const
{
    return m_ships.at(shipId).getLength();
}

char GameImpl::shipSymbol(int shipId) const
{
    return m_ships.at(shipId).getSymbol();
}

string GameImpl::shipName(int shipId) const
{
    return m_ships.at(shipId).getName();
}


bool GameImpl::playTurn(Player* attacker, Board& defender)
{
    //return false if defender is still alive, true if it is
    //PHASE I. PLAYER I ROUND//
    if (attacker->isHuman())  defender.display(true);
    else defender.display(false);
    
    //player 1 attack
    Point p1Attack = attacker->recommendAttack();
    
    bool validShot1, shotHit1, shipDestroyed1;
    int shipId;
    
    validShot1 = defender.attack(p1Attack, shotHit1, shipDestroyed1, shipId);
    
    //three cases: wasted a shot, missed, hit, destroyed
    //case 1: wasted a shot
    if (!validShot1)
    {
        cout << attacker->name() << " wasted a shot at "
             << "(" << p1Attack.r << ", " << p1Attack.c << ")." << endl;
    }
    
    else
    {
        cout << attacker->name() << " attacked " << "(" << p1Attack.r << ", " << p1Attack.c << ") and ";
        //case 2: missed
        if (!shotHit1)
            cout << "missed";
        //case 3: hit something
        else if (!shipDestroyed1)
            cout << "hit something";
        //case 4: destroyed a ship
        else
            cout << "destroyed the " << shipName(shipId);
        cout << ", resulting in:" << endl;
    }
    
    if (attacker->isHuman())  defender.display(true);
    else    defender.display(false);
    
    attacker->recordAttackResult(p1Attack, validShot1, shotHit1, shipDestroyed1, shipId);
    
    if (defender.allShipsDestroyed())
    {
        cout << attacker->name() << " wins!" << endl;
        return attacker;
    }
    
    return defender.allShipsDestroyed();
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    if (!p1->placeShips(b1) || !p2->placeShips(b2))  return nullptr;
    
    while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
    {
        //PHASE I. PLAYER I TURN
        cout << p1->name() << "'s turn. Board for "
             << p2->name() << ":" << endl;
        
        if (playTurn(p1, b2))
        {
            if (p2->isHuman())
            {
                cout << "Here is where " << p1->name() << "'s ships were:" << endl;
                b1.display(false);
            }
            return p1;
        }
        if (shouldPause)
            waitForEnter();
        
        //PHASE II. PLAYER II TURN
        cout << p2->name() << "'s turn. Board for "
             << p1->name() << ":" << endl;
        if (playTurn(p2, b1))
        {
            if (p1->isHuman())
            {
                cout << "Here is where " << p2->name() << "'s ships were:" << endl;
                b2.display(false);
            }
            return p2;
        }
        if (shouldPause)
            waitForEnter();
    }
    return nullptr;  // This compiles but may not be correct
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

//void Game::dump() const
//{
//    cout << "this is a game of " << rows() << " x " << cols() << " dimension." << endl;
//    cout << nShips() << " ships are listed below: " << endl
//         << "=================================" << endl;
//
//    for (int shipId = 0; shipId < nShips(); shipId++)
//    {
//        cout << "index " << shipId << ":" << endl
//                         << "   length " << shipLength(shipId) << endl
//                         << "   symbol " << shipSymbol(shipId) << endl
//                         << "   Name " << shipName(shipId) << endl;
//    }
//}
