#include "utility.h"

Ship::Ship(int length, char symbol, std::string name)
    : m_len(length), m_symbol(symbol), m_name(name)
{
    
}

int Ship::getLength() const
{
    return m_len;
}

char Ship::getSymbol() const
{
    return m_symbol;
}

std::string Ship::getName() const
{
    return m_name;
}

Ship::~Ship()
{
}
