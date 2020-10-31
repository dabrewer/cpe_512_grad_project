#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <stdint.h>

#include <node.h>

class Geometry
{
public:
    Geometry( double x_max, double y_max, double mesh_size );
    ~Geometry();
    double iterate( void );
    void save( const char *fname );
private:
    double _mesh_size;
    uint16_t _y_size;
    uint16_t _x_size;
    Node *potentials;

    void initPotentials( void );
    void initBoundaries( void );
    uint32_t getNumNodes( void );
    Node sor( float accel_factor, uint16_t i );
    double sorResidual( uint16_t x, uint16_t y );
};

#endif // _GEOMETRY_H_
