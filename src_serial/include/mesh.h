#ifndef _MESH_H_
#define _MESH_H_

#include <stdint.h>

#include <node.h>

class Mesh
{
public:
    Mesh( const char *cfg_file );
    Mesh( const double x_max, const double y_max, const double mesh_size );
    ~Mesh();
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
    bool _potentials_initialized;

    Node sor( float accel_factor, uint16_t i );
    double sorResidual( uint16_t x, uint16_t y );
};

#endif // _MESH_H_
