#include <node.h>

Node::Node( void )
{
    _value = 0;
    _isBoundary = false;
}

Node::Node( double value )
{
    _value = value;
    _isBoundary = false;
}

Node::Node( double value, bool isBoundary )
{
    _value = value;
    _isBoundary = isBoundary;
}

bool Node::isBoundary( void )
{
    return _isBoundary;
}

void Node::setValue( double value )
{
    _value = value;
}

double Node::getValue( void )
{
    return _value;
}
