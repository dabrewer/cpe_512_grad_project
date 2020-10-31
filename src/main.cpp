//#include <omp.h>
#include <iostream>

#include <geometry.h>

using namespace std;

// ################################################################################
// THESE VALUES WILL BE PASSED IN AS ARGUMENTS LATER
// ################################################################################
#define MESH_SIZE   0.25f    // unit: cm
#define XMAX        65.0f   // unit: cm
#define YMAX        50.0f   // unit: cm

#define PRECISION   0.1f

#define FNAME_OUT   "mesh.out"

// ################################################################################
// MAIN
// ################################################################################
int main( void )
{
    // Initialize 2D voltage mesh representing physical geometry
    Geometry *geometry = new Geometry(XMAX, YMAX, MESH_SIZE);

    uint16_t iterations = 0;
    while(geometry->iterate() > PRECISION)
    {
        iterations++;
    }

    // geometry->iterate();
    // geometry->iterate();
    // geometry->iterate();

    cout << "Iterations: " << iterations << endl;

    geometry->save(FNAME_OUT);

    delete geometry;
}
