#ifndef utility_h
#define utility_h

#include <iostream>
#include "globals.h"

enum Orientation {
    UP, RIGHT, DOWN, LEFT
};

class Point;

//Ship class, contains
    //coordinates
    //symbol
    //name
    //length
class Ship
{
public:
    Ship(int length, char symbol, std::string name);
    int getLength() const;
    char getSymbol() const;
    std::string getName() const;
    ~Ship();
    
private:
    int m_len;
    char m_symbol;
    std::string m_name;
};

#endif /* utility_h */
