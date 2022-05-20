#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <set>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;
    bool placeChecker(Point tol, int id, Direction dir);
    bool unplaceChecker(Point tol, int id, Direction dir);
    bool attackChecker(Point p);
    bool isLastUndamaged(Point p, char shipSymbol);
    int findShipId(char shipSymbol);
    
//    void cheekyRemove(int r, int c);
//    void dump() const;
    
  private:
    const Game& m_game;
    char m_board[MAXROWS][MAXCOLS];
    bool m_placement[MAXROWS][MAXCOLS]; //each index represents if the grid can be placed by ships
    set<int> m_placedShips;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    for (int i = 0; i < g.rows(); i++)
    {
        for (int j = 0; j < g.cols(); j++)
        {
            m_board[i][j] = '.';
            m_placement[i][j] = true;
        }
    }
}

void BoardImpl::clear()
{
    for (int i = 0; i < m_game.rows(); i++)
        for (int j = 0; j < m_game.cols(); j++)
            m_board[i][j] = '.';
    unblock();
    m_placedShips.clear();
}

void BoardImpl::block()
{
    // TODO:  Replace this with code to block half of the cells on the board
    int nRows = m_game.rows(), nCols = m_game.cols();
    int numSpots = nRows * nCols, counter = 0;
    while (counter < numSpots / 2)
    {
        int targetR = randInt(nRows), targetC = randInt(nCols);
        if (m_placement[targetR][targetC])
        {
            m_placement[targetR][targetC] = false;
            counter++;
        }
    }
}

void BoardImpl::unblock()
{
    for (int i = 0; i < m_game.rows(); i++)
        for (int j = 0; j < m_game.cols(); j++)
            m_placement[i][j] = true;
}

bool BoardImpl::placeChecker(Point tol, int id, Direction dir)
{
    //1. The shipId is invalid
    int r = tol.r, c = tol.c, len = m_game.shipLength(id);
    int nRows = m_game.rows(), nCols = m_game.cols();
    
    if (id >= m_game.nShips() || id < 0)  return false;
    else if (r < 0 || r >= nRows || c < 0 || c >= nCols) return false;
    
    //2. The ship would be partly or fully outside the board.
    //3. The ship would overlap an already-placed ship.
    //    4. The ship would overlap one or more positions that were blocked by a previous
    //    call to the block function.
    if (dir == HORIZONTAL)
    {
        if (c + len - 1 >= nCols)
            return false;
        //check if there is any grid that has a value other than '.'
        for (int i = 0; i < len; i++)
        {
            if ((m_board[r][c + i] != 'o' && m_board[r][c + i] != '.') || (!m_placement[r][c + i]))
                return false;
        }
    }
    
    if (dir == VERTICAL)
    {
        if (r + len - 1 >= nRows)
            return false;
        for (int i = 0; i < len; i++)
            if ((m_board[r + i][c] != 'o' && m_board[r + i][c] != '.') || (!m_placement[r + i][c]))
                return false;
    }
    
    //    5. The ship with that ship ID has previously been placed on this Board and not
    //    yet been unplaced since its most recent placement.
    //search for the same shipId
    if (m_placedShips.find(id) != m_placedShips.end())  return false;
    
    return true;
}

bool BoardImpl::unplaceChecker(Point tol, int id, Direction dir)
{
    //0. The r and c are invalid.
    if (tol.r < 0 || tol.r >= m_game.rows() || tol.c < 0 || tol.c >= m_game.cols()) return false;
    //1. The shipId is invalid
    else if (id >= m_game.nShips() || id < 0)  return false;
    //2. The board does not contain the entire ship at the indicated locations.
    char shipSymbol = m_game.shipSymbol(id);
    for (int i = 0; i < m_game.shipLength(id); i++)
    {
        if (dir == HORIZONTAL)
            if (m_board[tol.r][tol.c + i] != shipSymbol)    return false;
        if (dir == VERTICAL)
            if (m_board[tol.r + i][tol.c] != shipSymbol)    return false;
    }
    return true;
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if (!placeChecker(topOrLeft, shipId, dir))  return false;
    if (dir == HORIZONTAL)
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r][topOrLeft.c + i] = m_game.shipSymbol(shipId);
    
    else if (dir == VERTICAL)
    {
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r + i][topOrLeft.c] = m_game.shipSymbol(shipId);
    }
    
    m_placedShips.insert(shipId);   //TODO: check if this actually works
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if (!unplaceChecker(topOrLeft, shipId, dir))    return false;
    if (dir == HORIZONTAL)
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r][topOrLeft.c + i] = '.';
    
    else if (dir == VERTICAL)
    {
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r + i][topOrLeft.c] = '.';
    }
    
    m_placedShips.erase(shipId);
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    //First line: The function must print two spaces followed by the digits for each column, starting at 0, followed by a newline. You may assume there will be no more than 10 columns.
    cout << "  ";
    for (int i = 0; i < m_game.cols(); i++) cout << i;
    cout << endl;
        
    //Remaining lines: The function must print a digit specifying the row number, starting at 0, followed by a space, followed by the contents of the current row, followed by a newline. You may assume there will be no more than 10 rows. In each of the positions of the row, use the following characters to represent the playing field:
    for (int i = 0; i < m_game.rows(); i++)
    {
        cout << i << " ";
        for (int j = 0; j < m_game.cols(); j++)
        {
            char target = m_board[i][j];
            if (target != '.' && target != 'o' && target != 'X')
            {
                //  a. If the shotOnly parameter is false, use the ship's symbol to display an undamaged ship segment; if the shotsOnly parameter is true, show a period to display an undamaged ship segment.
                if (shotsOnly)
                {
                    cout << '.';
                    continue;
                }
            }
            cout << m_board[i][j];
        }
        cout << endl;
    }
    
//    cout << "  ";
//    for (int i = 0; i < m_game.cols(); i++) cout << i;
//    cout << endl;
//    for (int i = 0; i < m_game.rows(); i++)
//    {
//        cout << i << " ";
//        for (int j = 0; j < m_game.cols(); j++)
//            cout << m_placement[i][j];
//        cout << endl;
//    }
//    b. Use an X character to display any damaged ship segment.
//    c. Use a period to display water where no attack has been made.
//    d. Use a lower case letter o character to display water where an attack has
//    been made that missed a ship.
}

bool BoardImpl::attackChecker(Point p)
{
    //if (the attack point is outside of the board area) return false;
    if (p.r < 0 || p.r >= m_game.rows() || p.c < 0 || p.c >= m_game.cols()) return false;
    //if (an attack is made on a previously attacked location)  return false;
    else if (m_board[p.r][p.c] == 'o' || m_board[p.r][p.c] == 'X') return false;
    else return true;
}

bool BoardImpl::isLastUndamaged(Point p, char shipSymbol)
{
    //check if the same character exist in the same column and same row of the board
    //check row
    for (int i = 0; i < m_game.cols(); i++)
        if (i != p.c && m_board[p.r][i] == shipSymbol)
            return false;
    //check col
    for (int i = 0; i < m_game.rows(); i++)
        if (i != p.r && m_board[i][p.c] == shipSymbol)
            return false;
    
    return true;
}

int BoardImpl::findShipId(char shipSymbol)
{
    for (int i = 0; i < m_game.nShips(); i++)
    {
        if (shipSymbol == m_game.shipSymbol(i))
            return i;
    }
    return -1;
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    if (attackChecker(p) == false)  return false;
    //If any undamaged segment of a ship is at position p on the board, then the shotHit parameter must be set to true, and the segment must henceforth be considered a damaged segment. Otherwise the shotHit parameter must be set to false.
    char symbol = m_board[p.r][p.c];
    if (symbol != 'o' && symbol != 'X' && symbol != '.')
    {
        //If this specific attack destroyed the last undamaged segment of a ship, then the shipDestroyed parameter must be set to true and the shipId parameter must be set to the ship ID of the ship that was destroyed; otherwise the shipDestroyed parameter must be set to false and shipId must be left unchanged.
        if (isLastUndamaged(p, symbol))
        {
            shipDestroyed = true;
            shipId = findShipId(symbol);
        }
        else
            shipDestroyed = false;
        
        shotHit = true;
        m_board[p.r][p.c] = 'X';
    }
    else
    {
        if (symbol == '.')
            m_board[p.r][p.c] = 'o';
        shipDestroyed = false;
        shotHit = false;
    }
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
//This function returns true if all ships have been completely destroyed on the current board and false otherwise. (If this is true, it means that the player who was attacking that board has won the game.)
    for (int i = 0; i < m_game.rows(); i++)
    {
        for (int j = 0; j < m_game.cols(); j++)
        {
            if (m_board[i][j] != 'X' && m_board[i][j] != 'o' && m_board[i][j] != '.')
                return false;
        }
    }
    return true;
}

//void BoardImpl::cheekyRemove(int r, int c)
//{
//    m_board[r][c] = '*';
//}

//void BoardImpl::dump() const
//{
//    //dump everything in m_placedShips
//    cout << "placed ships ID: " << endl;
//    for (auto it = m_placedShips.cbegin(); it != m_placedShips.end(); it++)
//        cout << (*it) << " ";
//    cout << endl;
//}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
//
//int main()
//{
//    Game a(8, 5);
//    a.dump();
//    assert(a.addShip(3, 'a', "Harry") == true);
//    assert(a.addShip(0, 'b', "ERROR") == false);
//    assert(a.addShip(9, 'c', "ERROR") == false);
//    assert(a.addShip(8, 'd', "Beta") == true);
//    assert(a.addShip(5, 'e', "Gamma") == true);
//    assert(a.addShip(3, 'e', "ADFA") == false);
//    assert(a.addShip(3, 'd', "dd") == false);
//    assert(a.addShip(1, EOF, "dd") == false);
//    assert(a.addShip(2, 'x', "test") == true);
//    assert(a.addShip(2, 'X', "test") == false);
//    assert(a.addShip(3, 'o', "FAIK") == false);
//    assert(a.addShip(3, '.', "dd") == false);
//    a.dump();
//    BoardImpl b(a);
//    b.block();
//    b.display(false);
//    b.clear();
//    b.display(false);
//
//    //check the implemnetation of the m_placedShips
//
//    assert(b.allShipsDestroyed() == true);
////    assert(b.placeChecker(Point(1,1), 4, HORIZONTAL) == false);
////    assert(b.placeChecker(Point(1,1), 5, HORIZONTAL) == false);
//    assert(b.placeChecker(Point(1, 0), 1, VERTICAL) == false);
//    assert(b.placeChecker(Point(0, 0), 1, VERTICAL) == true);
//    assert(b.placeChecker(Point(3, 1), 2, HORIZONTAL) == false);
//    assert(b.placeChecker(Point(3, 0), 2, HORIZONTAL) == true);  //placed
//    assert(b.placeChecker(Point(7, 7), 1, HORIZONTAL) == false);
//    assert(b.placeChecker(Point(8, 4), 1, HORIZONTAL) == false);
//    assert(b.placeChecker(Point(-1, -1), 1, HORIZONTAL) == false);
//    assert(b.placeChecker(Point(7, 7), 1, HORIZONTAL) == false);
//    assert(b.placeShip(Point(0, 0), 1, VERTICAL) == true);
//    assert(b.placeChecker(Point(3, 0), 2, HORIZONTAL) == false);
//    assert(b.placeShip(Point(4, 1), 3, HORIZONTAL) == true);
//    b.dump();
//    b.clear();
//
//    b.dump();
//    assert(b.placeChecker(Point(3,0), 2, HORIZONTAL) == true);
////    b.block();
////    b.display(true);
//    assert(b.placeShip(Point(5, 2), 3, VERTICAL) == true);
//    assert(b.placeShip(Point(1, 1), 2, VERTICAL) == true);
//
//    b.dump();
//
//    b.display(false);
//    assert(b.placeShip(Point(4, 4), 3, VERTICAL) == false);
//    assert(b.unplaceShip(Point(4, 0), 3, HORIZONTAL) == false);
//    assert(b.unplaceShip(Point(5, 2), 3, VERTICAL) == true);
//    b.dump();
//
//    assert(b.placeShip(Point(4, 4), 3, VERTICAL) == true);
//    assert(b.placeShip(Point(4, 0), 1, VERTICAL) == false);
//    assert(b.placeShip(Point(4, 0), 1, VERTICAL) == false);
//    assert(b.placeShip(Point(3, 2), 0, HORIZONTAL) == true);
//
//
//    //test attack()
//    bool shotHit;
//    bool shipDestroyed;
//    int shipId;
//    assert(b.attackChecker(Point(-1, -1)) == false);
//    assert(b.attackChecker(Point(8,4)) == false);
//    assert(b.attackChecker(Point(7, 5)) == false);
//    assert(b.attackChecker(Point(8, 5)) == false);
//    assert(b.attackChecker(Point(3, 2)) == true);
//    assert(b.attack(Point(3, 2), shotHit, shipDestroyed, shipId) == true && shotHit == true && shipDestroyed == false);
//    assert(b.attack(Point(3, 2), shotHit, shipDestroyed, shipId) == false && shotHit == true && shipDestroyed == false);
//    assert(b.attack(Point(3, 3), shotHit, shipDestroyed, shipId) == true && shotHit == true && shipDestroyed == false);
//    assert(b.attack(Point(1, 4), shotHit, shipDestroyed, shipId) && shotHit == false && shipDestroyed == false);
//    assert(b.isLastUndamaged(Point(3, 4), 'a'));
//    assert(b.attack(Point(3, 4), shotHit, shipDestroyed, shipId) == true && shotHit == true && shipDestroyed == true && shipId == 0);
//    assert(b.attack(Point(0, 4), shotHit, shipDestroyed, shipId) && shotHit == false && shipDestroyed == false);
//    assert(b.attackChecker(Point(3, 3)) == false);
//    assert(b.attack(Point(3, 1), shotHit, shipDestroyed, shipId) && shotHit == true && shipDestroyed == false);
//    assert(b.attack(Point(7, 0), shotHit, shipDestroyed, shipId) && shotHit == false && shipDestroyed == false);
//    assert(b.attack(Point(7, 0), shotHit, shipDestroyed, shipId) == false);
//
//    b.display(false);
//
//    //test unplaceChecker()
////    b.cheekyRemove(3, 3);
////    b.cheekyRemove(5, 1);
//    assert(b.unplaceChecker(Point(4,2), 0, HORIZONTAL) == false);
//    assert(b.unplaceShip(Point(3, 2), 0, HORIZONTAL) == false);
//    assert(b.unplaceShip(Point(-1, -1), 3, HORIZONTAL) == false);
//    assert(b.unplaceShip(Point(1, 1), 2, VERTICAL) == false);
//    assert(b.unplaceShip(Point(6, 2), 3, VERTICAL) == false);
//    assert(b.unplaceShip(Point(4, 2), 3, VERTICAL) == false);
//    assert(b.unplaceShip(Point(5, 2), 3, VERTICAL) == false);
//    assert(b.unplaceShip(Point(5, 2), 3, VERTICAL) == false);
//    b.display(false);
//    assert(b.allShipsDestroyed() == false);
//    for (int i = 1; i < 6; i++)
//        b.attack(Point(i, 1), shotHit, shipDestroyed, shipId);
//    b.display(false);
//    for (int i = 4; i < 6; i++)
//        b.attack(Point(i, 4), shotHit, shipDestroyed, shipId);
//    assert(b.allShipsDestroyed() == true);
//}
