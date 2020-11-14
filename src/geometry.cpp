#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <geometry.h>

using namespace std;

// ################################################################################
// DEFINITIONS
// ################################################################################
// Define macro for easier 2d memory access
#define POTENTIALS(x,y) potentials[(y)*_x_size+x] 

#define FGET_BUF_SIZE   100

// ################################################################################
// GEOMETRY
// ################################################################################
Geometry::Geometry( const double x_max, const double y_max, const double mesh_size )
{
    _mesh_size = mesh_size;

    // Calculate array dimensions based upon input arguments
    _y_size = (int)(y_max / _mesh_size) + 1;
    _x_size = (int)(x_max / _mesh_size) + 1;

    cout << "x_size: " << _x_size << endl;
    cout << "y_size: " << _y_size << endl;

    // Dynamically allocate space for array
    potentials = new Node[getNumNodes()];
}

Geometry::~Geometry()
{
    delete potentials;
}

void Geometry::initPotentials( const double guess )
{
    uint32_t elements = (uint32_t)(_y_size * _x_size);
    for(uint32_t i = 0; i < elements; i++)
    {
        potentials[i] = Node(guess, false);
    }
}

void Geometry::initBoundaries( const char *fname )
{
    char buf[FGET_BUF_SIZE];
    char direction;
    double start;
    double end;
    double loc;
    double value;

    FILE *fp = fopen(fname, "r");
    while(fgets (buf, FGET_BUF_SIZE, fp))
    {
        sscanf(buf, "%c %lf %lf %lf %lf", &direction, &start, &end, &loc, &value);
        
        // TODO: maybe do error-checking to validate start, end, loc within window range

        //printf("%d %d %d\n", (int)(start / _mesh_size), (int)(end / _mesh_size), (int)(loc / _mesh_size));
        for(uint16_t i = (int)(start / _mesh_size); i <= (int)(end / _mesh_size); i++)
        {
            if(direction == 'h')
            {
                if(i >= _x_size || (int)(loc / _mesh_size) >= _y_size)
                {
                    cout << "BAD BOUNDARY VALUE" << direction << "\t" << start << "\t"  << end << "\t"  << loc << "\t"  << value << endl;
                    exit(-1);
                }
                POTENTIALS(i, (int)(loc / _mesh_size)) = Node(value, true);
            }
            else
            {
                if(i >= _y_size || (int)(loc / _mesh_size) >= _x_size)
                {
                    cout << "BAD BOUNDARY VALUE" << direction << "\t" << start << "\t"  << end << "\t"  << loc << "\t"  << value << endl;
                    exit(-1);
                }
                POTENTIALS((int)(loc / _mesh_size), i) = Node(value, true);
            }            
        }
    }
}

uint32_t Geometry::getNumNodes( void )
{
    return _y_size * _x_size;
}

void Geometry::save( const char *fname )
{
    double x;
    double y;

    FILE *fp = fopen(fname, "w");
    for(uint16_t y = 0; y < _y_size; y++)
    {
        for(uint16_t x = 0; x < _x_size; x++)
        {
            fprintf(fp, "%lf\t%lf\t%lf\n", (double)(x * _mesh_size), (double)(y * _mesh_size), POTENTIALS(x,y).getValue());
            //printf("%lf %lf %lf %d\n", (double)(x * _mesh_size), (double)(y * _mesh_size), POTENTIALS(x,y).getValue(), POTENTIALS(x,y).isBoundary());
        }
    }
    // for(uint32_t i = 0; i < getNumNodes(); i++)
    // {
    //     x = (i % _x_size) * _mesh_size;
    //     y = (i / _x_size) * _mesh_size;

    //     fprintf(fp, "%lf %lf %lf\n", x, y, potentials[i].getValue());
    // }
}

double Geometry::sorResidual(uint16_t x, uint16_t y)
{   
    double rv;

    // Calculate Residual Error in Each Direction
    // Must ensure not to reach outside mesh model
    rv = 0.0;

    // Right Node
    if((x+1) < _x_size)
        rv += POTENTIALS(x+1,y).getValue() - POTENTIALS(x,y).getValue();
    // Left Node
    if((x-1) >= 0)
        rv += POTENTIALS(x-1,y).getValue() - POTENTIALS(x,y).getValue();
    // Top Node
    if((y+1) < _y_size)
        rv += POTENTIALS(x,y+1).getValue() - POTENTIALS(x,y).getValue();
    // Bottom Node
    if((y-1) >= 0)
        rv += POTENTIALS(x,y-1).getValue() - POTENTIALS(x,y).getValue();

    return rv;
}

// TODO: SOR NEEDS TO RETURN NODE, NOT DOUBLE, SINCE IT IS FILLING A NODE ARRAY
// ALSO, WE NEED TO RETAIN IS BOUNDARY DATA WHEN WE RETURN BOUNDARY NODES TOO
Node Geometry::sor(float accel_factor, uint16_t i)
{
    Node node;
    uint16_t x;
    uint16_t y;
    double newValue;

    x = i % _x_size; // TODO:CONVERT i -> x
    y = i / _x_size; // TODO:CONVERT i -> y

    node = POTENTIALS(x,y);

    //cout << i << "\t" << x << "\t" << y << "\t" << (y)*_x_size+x << "\n";

    // return node;

    if(node.isBoundary())
        return node;

    newValue = node.getValue() + (accel_factor / 4.0) * sorResidual(x, y);

    return Node(newValue, node.isBoundary());
}

double Geometry::iterate( float accel_factor )
{
    double maxError = 0.0f;

    Node *potentials_shadow;
    potentials_shadow = new Node[getNumNodes()];   

    double error;
    for(uint16_t i = 0; i < getNumNodes(); i++)
    {
        potentials_shadow[i] = sor(accel_factor, i);

        error = fabs(potentials_shadow[i].getValue() - potentials[i].getValue());

        if(error > maxError)
            maxError = error;
    }

    delete potentials;
    potentials = potentials_shadow;

    cout << maxError << endl;

    return maxError;
}
