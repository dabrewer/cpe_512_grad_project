#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <stdint.h>

#include <node.h>

class Geometry
{
public:
    Geometry( const char *cfg_file );
    Geometry( const double x_max, const double y_max, const double mesh_size );
    ~Geometry();
    void initPotentials( const double guess );
    void initBoundaries( const char *fname );
    uint32_t getNumNodes( void );
    double iterate( const float accel_factor );
    void save( const char *fname );
private:
    double _mesh_size;
    uint16_t _y_size;
    uint16_t _x_size;
    Node *potentials;

    Node sor( float accel_factor, uint16_t i );
    double sorResidual( uint16_t x, uint16_t y );
};

#endif // _GEOMETRY_H_
