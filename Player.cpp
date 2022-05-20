////========================================================================
//// Timer t;                 // create a timer and start it
//// t.start();               // start the timer
//// double d = t.elapsed();  // milliseconds since timer was last started
////========================================================================
//
//#include <chrono>
//
//class Timer
//{
//  public:
//    Timer()
//    {
//        start();
//    }
//    void start()
//    {
//        m_time = std::chrono::high_resolution_clock::now();
//    }
//    double elapsed() const
//    {
//        std::chrono::duration<double,std::milli> diff =
//                          std::chrono::high_resolution_clock::now() - m_time;
//        return diff.count();
//    }
//  private:
//    std::chrono::high_resolution_clock::time_point m_time;
//};

#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include "utility.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include <string>
#include <cassert>
#include <set>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}


//This is a concrete class derived from Player. It places ships and recommends attacks based on user input. The isHuman function must return true, so that Game's play function can ensure that a human player doesn't get to see where the opponent's ships are during play. The placeShips and recommendAttack functions must prompt the user for where to place the ships and where to attack, respectively, in the manner of the posted sample program. The recordAttackResult and recordAttackByOpponent presumably need not do anything.

class HumanPlayer: public Player
{
public:
    HumanPlayer(string nm, const Game& g)
        : Player(nm, g){}
    virtual ~HumanPlayer(){}
    virtual bool isHuman() const {return true;}
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool validateDirection(char dirInput);
    bool validateCoordinate(Board& b, int rowInput, int colInput, int shipId, Direction dir);
private:
};

bool HumanPlayer::validateCoordinate(Board& b, int rowInput, int colInput, int shipId, Direction dir)
{
    bool isValid = false;
    //bad cases
    if (rowInput < 0 || rowInput >= game().rows()){}
    else if (colInput < 0 || colInput >= game().cols()){}
    else if (!b.placeShip(Point(rowInput, colInput), shipId, dir)){}
    //good cases
    else {isValid = true;}
    
    if (!isValid)   cout << "The ship can not be placed there." << endl;
    
    return isValid;
}

bool HumanPlayer::validateDirection(char dirInput)
{
    if (dirInput != 'h' && dirInput != 'v')
    {
        cout << "Direction must be h or v." << endl;
        return false;
    }
    return true;
}

bool HumanPlayer::placeShips(Board& b)
{
    //It returns true if all ships could be placed, and false otherwise (e.g., because the game parameters are such that there is no configuration of ships that will fit, or because a MediocrePlayer is unable to place all of the ships after 50 tries).
    cout << name() << " must place "
         << game().nShips() << " ships." << endl;
    b.display(false);
    
    for (int shipId = 0; shipId < game().nShips(); shipId++)
    {
        Direction dir;
        char dirInput;
        do {
            //ask for direction
            cout << "Enter h or v for direction of "
                 << game().shipName(shipId)
                 << " (length " << game().shipLength(shipId) << "): ";
            cin >> dirInput;
            cin.ignore(10000, '\n');
        } while (!validateDirection(dirInput));
        if (dirInput == 'h')    dir = HORIZONTAL;
        if (dirInput == 'v')    dir = VERTICAL;
        
        //ask for TOL position
        int rowInput;
        int colInput;
        LOOP:do {
            cout << "Enter row and column of ";
            if (dir == HORIZONTAL)  cout << "leftmost cell (e.g., 3 5): ";
            if (dir == VERTICAL)    cout << "topmost cell (e.g., 3 5): ";
//            cin.ignore(1000000, '\n');
            cin >> rowInput;
            if (!cin)
            {
                cin.clear();  // clear the error so that input can be re-processed
                cout << "You must enter two integers." << endl;
                cin.ignore(10000, '\n');
                goto LOOP;
            }
            
            cin >> colInput;
            if (!cin)
            {
                cin.clear();
                cout << "You must enter two integers." << endl;
                cin.ignore(10000, '\n');
                goto LOOP;
            }
            cin.ignore(10000, '\n');
        } while (!validateCoordinate(b, rowInput, colInput, shipId, dir));
        if (shipId < game().nShips() - 1)  b.display(false);
    }
    return true;
}

Point HumanPlayer::recommendAttack()
{
//    It must return a point that indicates the position on the opponent's board that the player would like to attack next.
    int rowInput, colInput;
    while(true)
    {
        cout << "Enter the row and column to attack (e.g., 3 5): ";
//
//        cin.ignore(100000, '\n');
        
        cin >> rowInput;
        if (!cin)
        {
            cin.clear();  // clear the error so that input can be re-processed
            cout << "You must enter two integers." << endl;
            cin.ignore(10000, '\n');
            continue;
        }
        
        cin >> colInput;
        if (!cin)
        {
            cin.clear();
            cout << "You must enter two integers." << endl;
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');
        break;
    }
    return Point(rowInput, colInput);
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    //DO NOTHING
}

void HumanPlayer::recordAttackByOpponent(Point p)
{
    //DO NOTHING
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer: public Player
{
public:
    MediocrePlayer(string nm, const Game& g)
        : Player(nm, g), m_lastHit(-999, -999), m_state(1){}
    virtual ~MediocrePlayer(){}
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool recursivePlace(Board& b, int shipId);
    bool isValid(Point target);
    bool crossHasSpace(Point target);
    
//    void dump() const;
private:
    int m_state;
    Point m_lastHit;    //the last hit point on any ship on the board.
    set<pair<int, int>> m_pointHistory; //points on the board that the AI attempted to attack.
};

//Helper Functions//
//returns a Random point in the CROSS
Point randPoint(int targetr, int targetc)
{
    int randr, randc;
    int rc = randInt(2);    //0 means row random, 1 means col random
    if (rc == 0)
    {
        randr = -4 + targetr + randInt(9);
        randc = targetc;
    }
    else
    {
        randr = targetr;
        randc = -4 + targetc + randInt(9);
    }
    return Point(randr, randc);
}

bool MediocrePlayer::placeShips(Board& b)
{
    for (int i = 0; i < 50; i++)
    {
        b.block();
        if (recursivePlace(b, 0))
        {
            b.unblock();
            return true;
        }
        b.unblock();
    }
    return false;
}

bool MediocrePlayer::recursivePlace(Board& b, int shipId)
{
    if (shipId >= game().nShips())  return true;
    
    for (int row = 0; row < game().rows(); row++)
    {
        for (int col = 0; col < game().cols(); col++)
        {
            if (b.placeShip(Point(row, col), shipId, HORIZONTAL))
            {
                if (recursivePlace(b, shipId + 1))  return true;
                else b.unplaceShip(Point(row, col), shipId, HORIZONTAL);
            }

            if (b.placeShip(Point(row, col), shipId, VERTICAL))
            {
                if (recursivePlace(b, shipId + 1))  return true;
                else b.unplaceShip(Point(row, col), shipId, VERTICAL);
            }
        }
    }
    return false;
}

bool MediocrePlayer::crossHasSpace(Point target)
{
    for (int i = -4; i < 5; i++)
    {
        if (target.r + i >= 0 && target.r + i < game().rows() && m_pointHistory.find(make_pair(target.r + i, target.c)) == m_pointHistory.end())
            return true;
        if (target.c + i >= 0 && target.c + i < game().cols() && m_pointHistory.find(make_pair(target.r, target.c + i)) == m_pointHistory.end())
            return true;
    }
    return false;
}

bool MediocrePlayer::isValid(Point target)
{
    if (!game().isValid(target))    return false;
    if (m_pointHistory.find(make_pair(target.r, target.c)) != m_pointHistory.end()) return false;
    else return true;
}

Point MediocrePlayer::recommendAttack()
{
    Point target = game().randomPoint();
    int numCoords = game().rows() * game().cols();
    if (m_pointHistory.size() >= numCoords)
        return Point(-1, -1);
    
    if (m_state == 2)
    {
        //select a legit point
        bool isDoable = true;
        
        Point target = randPoint(m_lastHit.r, m_lastHit.c); //FIXME: I am removing the Point declaration. lmk if this is wrong
        
        //try generating all coordinates in the vicinity of m_lastHit
        while (!isValid(target) || (target.r == m_lastHit.r && target.c == m_lastHit.c))
        {
            if (!crossHasSpace(target))
            {
                isDoable = false;
                break;
            }
            target = randPoint(m_lastHit.r, m_lastHit.c);
        }
        
        if (!isDoable)
            m_state = 1;
        else
        {
            m_pointHistory.insert(make_pair(target.r, target.c));
            return target;
        }
    }
    
    if (m_state == 1)
    {
        while (m_pointHistory.find(make_pair(target.r, target.c)) != m_pointHistory.end())
            target = game().randomPoint();
        m_pointHistory.insert(make_pair(target.r, target.c));
        return target;
    }
    
    //return an invalid point (all of the condition outlined above has to be met)
    return Point(-1, -1);
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (!validShot) return;
    
    if (m_state == 1)
    {
        if (!shotHit || shipDestroyed)  return;
        else
        {
            m_lastHit = p;
            m_state = 2;
        }
    }
    if (m_state == 2)
    {
        if (!shotHit)   return;
        else if (!shipDestroyed){}
        else    m_state = 1;
    }
}

void MediocrePlayer::recordAttackByOpponent(Point p)
{
    //do nothing
}
//
//void MediocrePlayer::dump() const
//{
//    cout << "displaying vital information." << endl;
//    cout << "m_state = " << m_state << endl;
//    cout << "m_lastHit: r = " << m_lastHit.r << ", c = " << m_lastHit.c << endl;
//
//    set<pair<int, int>>::iterator it = m_pointHistory.begin();
//    cout << "displaying coordinates that the AI attempted to attack." << endl;
//    for (; it != m_pointHistory.end(); it++)
//        cout << "r: " << it->first << ", c: " << it->second << endl;
//}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer: public Player
{
public:
    GoodPlayer(string nm, const Game& g)
        : Player(nm, g), m_state(1), m_firstBlindTry(Point(1, UP)),
          m_lastHit(Point(-999, -999)), m_lastAttack(make_pair(0, UP))
        {}
    virtual ~GoodPlayer(){}
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool recursivePlace(Board& b, int shipId);
    bool isValid(Point target);
    bool diagonalHasSpace(Point target);
    Point betterRandomPoint(char color);
    Point state2Attack();
//    void dump() const;
    bool crossHasSpace(Point target);
    bool isSwitchColor(char color);
    
private:
    int m_state;
    Point m_firstBlindTry;
    Point m_lastHit;    //the last hit point on any ship on the board.
    pair<int, Orientation> m_lastAttack;
    set<pair<int, int>> m_pointHistory; //points on the board that the AI attempted to attack.
};

//Helper Functions//
//returns a Random point in the diagonals
Point randPointOnDiagonal(int targetr, int targetc)
{
    int randr = -1, randc = -1;
    int randomDiff = -MAXROWS + randInt(MAXCOLS * 2);
    
    int num = randInt(5);
    switch(num)
    {
        case 0:
            randr = targetr - randomDiff;
            randc = targetc - randomDiff;
            break;
        case 1:
            randr = targetr - randomDiff;
            randc = targetc + randomDiff;
            break;
        case 2:
            randr = targetr + randomDiff;
            randc = targetc - randomDiff;
            break;
        case 3:
            randr = targetr + randomDiff;
            randc = targetc + randomDiff;
            break;
        default:
            break;
    }
    return Point(randr, randc);
}

bool GoodPlayer::isSwitchColor(char color)
{
    for (int i = 0; i < game().rows(); i++)
    {
        for (int j = 0; j < game().cols(); j++)
        {
            if (color == 'b')
            {
                if ((i + j) % 2 == 0 && m_pointHistory.find(make_pair(i, j)) == m_pointHistory.end())
                    return false;
            }
            else
            {
                if ((i + j) % 2 == 1 && m_pointHistory.find(make_pair(i, j)) == m_pointHistory.end())
                    return false;
            }
        }
    }
    return true;
}

bool GoodPlayer::recursivePlace(Board& b, int shipId)
{
    if (shipId >= game().nShips())  return true;
    
    for (int row = 0; row < game().rows(); row++)
    {
        for (int col = 0; col < game().cols(); col++)
        {
            if (b.placeShip(Point(row, col), shipId, HORIZONTAL))
            {
                if (recursivePlace(b, shipId + 1))  return true;
                else b.unplaceShip(Point(row, col), shipId, HORIZONTAL);
            }

            if (b.placeShip(Point(row, col), shipId, VERTICAL))
            {
                if (recursivePlace(b, shipId + 1))  return true;
                else b.unplaceShip(Point(row, col), shipId, VERTICAL);
            }
        }
    }
    return false;
}

bool GoodPlayer::placeShips(Board& b)
{
    for (int i = 0; i < 50; i++)
    {
        b.block();
        if (recursivePlace(b, 0))
        {
            b.unblock();
            return true;
        }
        b.unblock();
    }
    return false;
}

//assumes that target is in Board!
bool GoodPlayer::diagonalHasSpace(Point target)
{
    int numIterations = max(game().rows(), game().cols());
    
    for (int i = 1; i < numIterations; i++)
    {
        //top left
        if (target.r - i >= 0 && target.c - i >= 0 && m_pointHistory.find(make_pair(target.r - i, target.c - i)) == m_pointHistory.end())
            return true;
        
        //top right
        if (target.r - i >= 0 && target.c + i < game().cols() && m_pointHistory.find(make_pair(target.r - i, target.c + i)) == m_pointHistory.end())
            return true;
        
        //bottom left
        if (target.r + i < game().rows() && target.c - i >= 0 && m_pointHistory.find(make_pair(target.r + i, target.c - i)) == m_pointHistory.end())
            return true;
        
        if (target.r + i < game().rows() && target.c + i < game().cols() && m_pointHistory.find(make_pair(target.r + i, target.c + i)) == m_pointHistory.end())
            return true;
    }
    return false;
}

bool GoodPlayer::isValid(Point target)
{
    if (!game().isValid(target))    return false;
    if (m_pointHistory.find(make_pair(target.r, target.c)) != m_pointHistory.end()) return false;
    else return true;
}

Point GoodPlayer::recommendAttack()
{
    //choose the black ones first
    //only check those to the diagonal of the current target
    int numCoords = game().rows() * game().cols();
    if (m_pointHistory.size() >= numCoords)
        return Point(-1, -1);
    
    //if a ship is hit
    if (m_state == 2)
    {
        bool isDoable = true;
        
        Point target = state2Attack();
        //try generating all coordinates in the vicinity of m_lastHit
        while (!isValid(target) || (target.r == m_lastHit.r && target.c == m_lastHit.c))
        {
            if (!crossHasSpace(target))
            {
                isDoable = false;
                break;
            }
            target = state2Attack();
        }
        
        if (!isDoable)  m_state = 1;
        else
        {
            m_pointHistory.insert(make_pair(target.r, target.c));
            return target;
        }
    }
    
    //state 3. randomly finding a point on the diagonal
    if (m_state == 3)
    {
        //randomly select a point based off of the diagonal space of m_firstBlindTry.
        Point target = randPointOnDiagonal(m_firstBlindTry.r, m_firstBlindTry.c);
        if (!diagonalHasSpace(m_firstBlindTry)) m_state = 1;
        else
        {
            while(!isValid(target))
                target = randPointOnDiagonal(m_firstBlindTry.r, m_firstBlindTry.c);
            m_pointHistory.insert(make_pair(target.r, target.c));
            return target;
        }
    }
    
    //state 1. randomly finding a point(when the current diagonal is full)
    if (m_state == 1)
    {
        Point target = betterRandomPoint('w');
        
        while (m_pointHistory.find(make_pair(target.r, target.c)) != m_pointHistory.end())
        {
            if (isSwitchColor('w'))
                target = betterRandomPoint('b');
            else
                target = betterRandomPoint('w');
        }
        m_pointHistory.insert(make_pair(target.r, target.c));
        m_firstBlindTry = target;
        return target;
    }

    //return an invalid point (all of the condition outlined above has to be met)
    return Point(-1, -1);
}

Point GoodPlayer::betterRandomPoint(char color)
{
    int randr = randInt(game().rows()), randc = randInt(game().cols());
    if (color == 'b')
    {
        while ((randr + randc) % 2 != 0)
        {
            randr = randInt(game().rows());
            randc = randInt(game().cols());
        }
        return Point(randr, randc);
    }
    else if (color == 'w')
    {
        while ((randr + randc) % 2 != 1)
        {
            randr = randInt(game().rows());
            randc = randInt(game().cols());
        }
        return Point(randr, randc);
    }
    else    return Point(-1, -1);
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (!validShot)
        return;
    
    if (m_state == 1)
    {
        if (!shotHit || shipDestroyed)
        {
            m_state = 3;
            return;
        }
        else
        {
            m_lastHit = p;
            m_state = 2;
        }
    }
    else if (m_state == 2)
    {
        if (!shotHit)
        {
            //reset m_lastAttack
            m_lastAttack.first = 0;
            //switch up direction
            switch(m_lastAttack.second)
            {
                case UP:
                    m_lastAttack.second = RIGHT;
                    break;
                case RIGHT:
                    m_lastAttack.second = DOWN;
                    break;
                case DOWN:
                    m_lastAttack.second = LEFT;
                    break;
                case LEFT:
                    m_lastAttack.second = UP;
                    break;
                default:
                    break;
            }
        }
        else if (!shipDestroyed){}
        else
        {
            m_lastAttack = make_pair(0, UP);    //reset m_lastAttack;
            m_state = 1;
        }
    }
    
    else
    {
        if (shotHit && !shipDestroyed)
        {
            m_lastHit = p;
            m_state = 2;
        }
    }
}

void GoodPlayer::recordAttackByOpponent(Point p)
{
    
}

//void GoodPlayer::dump() const
//{
//    cout << "displaying vital information." << endl;
//    cout << "m_state = " << m_state << endl;
//    cout << "m_lastHit: r = " << m_lastHit.r << ", c = " << m_lastHit.c << endl;
//
//    set<pair<int, int>>::iterator it = m_pointHistory.begin();
//    cout << "displaying coordinates that the AI attempted to attack." << endl;
//    for (; it != m_pointHistory.end(); it++)
//        cout << "r: " << it->first << ", c: " << it->second << endl;
//}



//functions that are not tested yet.
//check if the entire cross has space
bool GoodPlayer::crossHasSpace(Point target)
{
    int maxCheck = max(game().rows(), game().cols());
    
    for (int i = 0; i < maxCheck; i++)
    {
        if (i < game().rows() && m_pointHistory.find(make_pair(i, target.c)) == m_pointHistory.end())
            return true;
        if (i < game().cols() && m_pointHistory.find(make_pair(target.r, i)) == m_pointHistory.end())
            return true;
    }
    return false;
}

//IF STATE2 fails to attack the indicated location, reset m_lastAttack's parameters to 0 and new orientation
//returns a point in the CROSS
Point GoodPlayer::state2Attack()
{
    int lastHitr = m_lastHit.r;
    int lastHitc = m_lastHit.c;
    
    switch(m_lastAttack.second)
    {
        case UP:
        {
            int difference = m_lastAttack.first;
            Point target = Point(lastHitr - difference - 1, lastHitc);
            do
            {
                if (target.r < 0)
                {
                    m_lastAttack.first = 0;
                    m_lastAttack.second = RIGHT;
                    break;
                }
                else if (m_pointHistory.find(make_pair(target.r, target.c)) == m_pointHistory.end())
                {
                    m_lastAttack.first = difference + 1;
                    m_lastAttack.second = UP;
                    return target;
                }
                else
                {
                    target.r--;
                    m_lastAttack.first++;
                }
            } while(game().isValid(target));
            
            break;
        }
        case RIGHT:
        {
            int difference = m_lastAttack.first;
            Point target = Point(lastHitr, lastHitc + difference + 1);
            do
            {
                if (target.c >= game().cols())
                {
                    m_lastAttack.first = 0;
                    m_lastAttack.second = DOWN;
                    break;
                }
                else if (m_pointHistory.find(make_pair(target.r, target.c)) == m_pointHistory.end())
                {
                    m_lastAttack.first = difference + 1;
                    m_lastAttack.second = RIGHT;
                    return target;
                }
                else
                {
                    target.c++;
                    m_lastAttack.first++;
                }
            } while(game().isValid(target));
            break;
        }
        case DOWN:
        {
            int difference = m_lastAttack.first;
            Point target = Point(lastHitr + difference + 1, lastHitc);
            do
            {
                if (target.r >= game().rows())
                {
                    m_lastAttack.first = 0;
                    m_lastAttack.second = LEFT;
                    break;
                }
                else if (m_pointHistory.find(make_pair(target.r, target.c)) == m_pointHistory.end())
                {
                    m_lastAttack.first = difference + 1;
                    m_lastAttack.second = DOWN;
                    return target;
                }
                else
                {
                    target.r++;
                    m_lastAttack.first++;
                }
            } while(game().isValid(target));
            break;
        }
        case LEFT:
        {
            int difference = m_lastAttack.first;
            Point target = Point(lastHitr, lastHitc - difference - 1);
            do
            {
                if (target.c < 0)
                {
                    m_lastAttack.first = 0;
                    m_lastAttack.second = UP;
                    break;
                }
                else if (m_pointHistory.find(make_pair(target.r, target.c)) == m_pointHistory.end())
                {
                    m_lastAttack.first = difference + 1;
                    m_lastAttack.second = LEFT;
                    return target;
                }
                else
                {
                    target.c--;
                    m_lastAttack.first++;
                }
            } while(game().isValid(target));
            break;
        }
        default:
            return Point(-1, -1);
    }
    return Point(-1, -1);
}

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}


//int main()
//{
//    Game demo(10, 10);
//    Board a(demo);
//    assert(demo.addShip(3, 'a', "destroyer"));
//    assert(demo.addShip(7, 'b', "nuclear Bomber"));
//    assert(demo.addShip(2, '*', "patrol vessel"));
//    HumanPlayer b("harvey", demo);
//    b.placeShips(a);
//
//    MediocrePlayer alpha("smartypants", demo);
//    alpha.placeShips(a);
//    a.display(false);
//    
//    Game AI(4, 4);
//    Board b(AI);
//    assert(AI.addShip(5, 'a', "Ass Cheeks"));
//    assert(AI.addShip(3, 'b', "Beta Males"));
//    assert(AI.addShip(2, 'c', "Cuties"));
//    assert(AI.addShip(4, 'd', "dildos"));
//    assert(AI.addShip(3, 'd', "Dickheads"));
//    for (int i = 0; i < 10)
//    b.block();
//    b.display(false);
//    MediocrePlayer kong("lilPump", AI);
//    GoodPlayer c("King of the hill", AI);
//        for (int i = 0; i < 20; i++)
//            c.recommendAttack();
//    assert(c.placeShips(b));
//    b.display(false);
//    c.dump();
//    assert(kong.placeShips(b));
//    b.display(false);
//
//    //test recommendAttack()
//    for (int i = 0; i < 10; i++)
//        kong.recommendAttack();
//
//    //test recordAttackResult()
//    kong.recordAttackResult(Point(1, 1), false, true, true, 1);
//    kong.recordAttackResult(Point(1, 2), true, false, false, 0);
//    Point a1 = kong.recommendAttack();
//    cout << "!!!!attempted to attack " << a1.r << " " << a1.c << endl;
//    kong.recordAttackResult(Point(1, 3), true, true, false, 2);
//    for (int i = 0; i < 9; i++)
//        kong.recommendAttack(); //should attempt to attack around (1, 3)
//    kong.recordAttackResult(Point(2, 0), true, true, true, 2);
//    kong.recordAttackResult(Point(2, 2), true, true, false, 1);
//    for (int i = 0; i < 10; i++)
//        kong.recommendAttack();
//    kong.dump();
//    cout << "passed all tests." << endl;
//}
//
//Timer timer;
//
//void f(bool& startWrappingItUp)
//{
////    if (!startWrappingItUp)
////    {
////        for (int k = 0; k < 1000000; k++)
////        {
////            if (timer.elapsed() > 3900)  // have 3.9 seconds elapsed?
////            {
////                startWrappingItUp = true;
////                return;
////            }
////            // ... do some computation here ...
////        }
////    }
//    if (!startWrappingItUp)
//       {
//           int callcount = 0;
//           for (int k = 0; k < 1000000; k++)
//           {
//               if (++callcount == 1000)
//               {
//                   if (timer.elapsed() > 3900)  // have 3.9 seconds elapsed?
//                   {
//                       startWrappingItUp = true;
//                       return;
//                   }
//                   callcount = 0;
//               }
//               // ... do some computation here ...
//           }
//       }
//}

//int main()
//{
//    cout << "You should see something in close to 4 seconds...\n";
//    bool shouldWrapUp = false;
//    timer.start();
//    int ncalls;
//    for (ncalls = 0; !shouldWrapUp; ncalls++)
//        f(shouldWrapUp);
//    cout << "f was called " << ncalls << " times\n";
//}
