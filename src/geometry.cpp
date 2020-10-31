#include <iostream>
#include<stdio.h>
#include <math.h>

#include "geometry.h"

using namespace std;

// ################################################################################
// DEFINITIONS
// ################################################################################
// Define initial state of array
#define INIT_GUESS  35.0f	// unit: V

// Define macro for easier 2d memory access
#define POTENTIALS(x,y) potentials[(y)*_x_size+x] 

#define FGET_BUF_SIZE   100

#define ACCEL_FACT  0.5

#define FNAME_MESH_CFG  "mesh.cfg"
#define FNAME_BOUNDARY  "boundary.cfg"

// ################################################################################
// GEOMETRY
// ################################################################################
Geometry::Geometry( double x_max, double y_max, double mesh_size )
{
    _mesh_size = mesh_size;

    // Calculate array dimensions based upon input arguments
    _y_size = (int)(y_max / _mesh_size) + 1;
    _x_size = (int)(x_max / _mesh_size) + 1;

    cout << "x_size: " << _x_size << endl;
    cout << "y_size: " << _y_size << endl;

    // Dynamically allocate space for array
    potentials = (Node *) malloc (getNumNodes() * sizeof(Node));

    // Initialize all mesh nodes to predefined guess
    initPotentials();

    // Initialize the boundary mesh nodes to static values
    initBoundaries();
}

Geometry::~Geometry()
{
    free(potentials);
}

void Geometry::initPotentials( void )
{
    uint32_t elements = (uint32_t)(_y_size * _x_size);
    for(uint32_t i = 0; i < elements; i++)
    {
        potentials[i] = Node(INIT_GUESS, false);
    }
}

void Geometry::initBoundaries( void )
{
    char buf[FGET_BUF_SIZE];
    char direction;
    double start;
    double end;
    double loc;
    double value;

    FILE *fp = fopen(FNAME_BOUNDARY, "r");
    while(fgets (buf, FGET_BUF_SIZE, fp))
    {
        sscanf(buf, "%c %lf %lf %lf %lf", &direction, &start, &end, &loc, &value);
        
        // TODO: maybe do error-checking to validate start, end, loc within window range

        //printf("%d %d %d\n", (int)(start / _mesh_size), (int)(end / _mesh_size), (int)(loc / _mesh_size));
        for(uint16_t i = (int)(start / _mesh_size); i <= (int)(end / _mesh_size); i++)
        {
            if(direction == 'h')
                POTENTIALS(i, (int)(loc / _mesh_size)) = Node(value, true);
            else
                POTENTIALS((int)(loc / _mesh_size), i) = Node(value, true);            
        }
    }
}

uint32_t Geometry::getNumNodes( void )
{
    return _y_size * _x_size;
}

void Geometry::save( char *fname )
{
    double x;
    double y;

    FILE *fp = fopen(fname, "w");
    for(uint16_t y = 0; y < _y_size; y++)
    {
        for(uint16_t x = 0; x < _x_size; x++)
        {
            fprintf(fp, "%lf %lf %lf %d\n", (double)(x * _mesh_size), (double)(y * _mesh_size), POTENTIALS(x,y).getValue(), POTENTIALS(x,y).isBoundary());
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
    Node current = POTENTIALS(x,y);
    Node top = POTENTIALS(x,y+1);
    Node right =  POTENTIALS(x+1,y);
    Node left = POTENTIALS(x-1,y);
    Node bottom = POTENTIALS(x,y-1);

    // cout << "top: " << top.getValue() << "\n";
    // cout << "right: " << right.getValue() << "\n";
    // cout << "left: " << left.getValue() << "\n";
    // cout << "bottom: " << bottom.getValue() << "\n";

    double rv;
    
    // // Left Edge
    // if(x == 0)
    //     rv = (top.getValue() + bottom.getValue() + right.getValue()) - 3.0 * current.getValue();
    // // Right Edge
    // else if(x == _x_size)
    //     rv = (top.getValue() + bottom.getValue() + left.getValue()) - 3.0 * current.getValue();
    // // Top Edge
    // else if(y == _y_size)
    //     rv = (bottom.getValue() + left.getValue() + right.getValue()) - 3.0 * current.getValue();
    // // Bottom Edge
    // else if(y == 0)
    //     rv = (top.getValue() + left.getValue() + right.getValue()) - 3.0 * current.getValue();

    if( (x > 0) && (x < _x_size) && (y > 0) && (y < _y_size))
        rv = (top.getValue() + bottom.getValue() + left.getValue() + right.getValue()) - 4.0 * current.getValue();
    else
        rv = current.getValue();

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

double Geometry::iterate( void )
{
    double maxError = 0.0f;

    Node *potentials_shadow;
    potentials_shadow = (Node *) malloc (getNumNodes() * sizeof(Node));   

    double error;
    for(uint16_t i = 0; i < getNumNodes(); i++)
    {
        potentials_shadow[i] = sor(ACCEL_FACT, i);

        error = fabs(potentials_shadow[i].getValue() - potentials[i].getValue());

        if(error > maxError)
            maxError = error;
    }

    free(potentials);
    potentials = potentials_shadow;

    cout << maxError << endl;

    return maxError;
}
