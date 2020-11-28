#ifndef _NODE_H_
#define _NODE_H_

#include <stdbool.h>

class Node
{
public:
    Node( void );
    Node( double value );
    Node( double value, bool isBoundary );
    bool isBoundary( void );
    void setValue( double value );
    double getValue( void );

private:
    double _value;
    bool _isBoundary;
};

#endif //_NODE_H_
