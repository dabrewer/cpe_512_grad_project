#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <mesh.h>

using namespace std;

// ################################################################################
// DEFINITIONS
// ################################################################################
// Define macro for easier 2d memory access
#define POTENTIALS(x,y) potentials[(y)*_x_size+x] 

#define FGET_BUF_SIZE   100

// ################################################################################
// MESH
// ################################################################################
Mesh::Mesh( const char *cfg_file )
{
    char buf[FGET_BUF_SIZE];

    _potentials_initialized = false;

    FILE *fp = fopen(cfg_file, "r");
    while(fgets(buf, FGET_BUF_SIZE, fp))
    {
        char key[FGET_BUF_SIZE];
        double value;

        sscanf(buf, "%s : %lf", key, &value);
        //printf("%s : %lf\n", key, value);

        if(strcmp("XMAX", key) == 0)
        {
            _x_size = (int)(value / _mesh_size) + 1;
            cout << "x_size: " << _x_size << endl;
        }
        if(strcmp("YMAX", key) == 0)
        {
            _y_size = (int)(value / _mesh_size) + 1;
            cout << "y_size: " << _y_size << endl;
        }
        if(strcmp("SIZE", key) == 0)
        {
            _mesh_size = value;
        }
        // NOTE: This assumes XMAX and YMAX preced INIT_GUESS in file
        if(strcmp("INIT_GUESS", key) == 0)
        {
            // Init all nodes to INIT_GUESS value
            initPotentials(value);
        }
    }
}

Mesh::Mesh( const double x_max, const double y_max, const double mesh_size )
{
    _potentials_initialized = false;

    _mesh_size = mesh_size;

    // Calculate array dimensions based upon input arguments
    _y_size = (int)(y_max / _mesh_size) + 1;
    _x_size = (int)(x_max / _mesh_size) + 1;

    cout << "x_size: " << _x_size << endl;
    cout << "y_size: " << _y_size << endl;
}

Mesh::~Mesh()
{
    delete potentials;
}

void Mesh::initPotentials( const double guess )
{
    // Only allocate memory once
    if( _potentials_initialized == false)
    {
        _potentials_initialized = true;
        // Dynamically allocate space for array
        potentials = new Node[getNumNodes()];
    }

    // Make initial guess for all nodes
    for(uint32_t i = 0; i < getNumNodes(); i++)
    {
        potentials[i] = Node(guess, false);
    }
}

void Mesh::initBoundaries( const char *fname )
{
    char buf[FGET_BUF_SIZE];
    char direction;
    double start;
    double end;
    double loc;
    double value;

    FILE *fp = fopen(fname, "r");
    while(fgets(buf, FGET_BUF_SIZE, fp))
    {
        sscanf(buf, "%c %lf %lf %lf %lf", &direction, &start, &end, &loc, &value);

        // printf("%d %d %d\n", (int)(start / _mesh_size), (int)(end / _mesh_size), (int)(loc / _mesh_size));
        // fflush(stdout);

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

uint32_t Mesh::getNumNodes( void )
{
    return _y_size * _x_size;
}

void Mesh::save( const char *fname )
{
    FILE *fp = fopen(fname, "w");
    for(uint16_t y = 0; y < _y_size; y++)
    {
        for(uint16_t x = 0; x < _x_size; x++)
        {
            fprintf(fp, "%lf\t%lf\t%lf\n", (double)(x * _mesh_size), (double)(y * _mesh_size), POTENTIALS(x,y).getValue());
            //printf("%lf %lf %lf %d\n", (double)(x * _mesh_size), (double)(y * _mesh_size), POTENTIALS(x,y).getValue(), POTENTIALS(x,y).isBoundary());
        }
    }
}

double Mesh::sorResidual(uint16_t x, uint16_t y)
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
Node Mesh::sor(float accel_factor, uint16_t i)
{
    Node node;
    uint16_t x;
    uint16_t y;
    double newValue;

    x = i % _x_size; // TODO:CONVERT i -> x
    y = i / _x_size; // TODO:CONVERT i -> y

    node = POTENTIALS(x,y);

    if(node.isBoundary())
        return node;

    newValue = node.getValue() + (accel_factor / 4.0) * sorResidual(x, y);

    return Node(newValue, node.isBoundary());
}

double Mesh::iterate( float accel_factor )
{
    double maxError = 0.0f;

    Node *potentials_shadow;
    potentials_shadow = new Node[getNumNodes()];   

    #pragma omp parallel
    {
        double error;
        #pragma omp for schedule (static) reduction (max:maxError)
        for(uint16_t i = 0; i < getNumNodes(); i++)
        {
            potentials_shadow[i] = sor(accel_factor, i);

            error = fabs(potentials_shadow[i].getValue() - potentials[i].getValue());

            if(error > maxError)
                maxError = error;
        }
    }

    delete potentials;
    potentials = potentials_shadow;

    cout << maxError << endl;

    return maxError;
}
