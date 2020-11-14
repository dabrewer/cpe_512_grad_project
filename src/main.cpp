//#include <omp.h>
#include <iostream>

#include <geometry.h>

using namespace std;

// g++ -Iinclude/ main.cpp geometry.cpp node.cpp -o ../bin/main; ../bin/main

// set pm3d map
// set view map
// splot "mesh.out" u 1:2:3
// replot

// ################################################################################
// THESE VALUES WILL BE PASSED IN AS ARGUMENTS LATER
// ################################################################################
#define MESH_SIZE   1.0f    // unit: cm
#define XMAX        65.0f   // unit: cm
#define YMAX        50.0f   // unit: cm

// Define initial state of array
#define INIT_GUESS  35.0f	// unit: V
#define PRECISION   0.01f
#define ACCEL_FACT  1.0

#define FNAME_MESH_CFG  "config/mesh.cfg"
#define FNAME_BOUNDARY  "config/boundary.cfg"
#define FNAME_OUT       "output/mesh.out"

#define NUM_ARGS    6
#define USAGE       "./bin/main [NUM_THREADS] [MESH_CFG_PATH] [BOUNDARY_CFG_PATH] [SOR_CFG_PATH] [MESH_OUTPUT_PATH]"
// #define FNAME_MESH_CFG  argv[1]
// #define FNAME_BOUNDARY  argv[2]
// #define FNAME_SOR_CFG   argv[3]
// #define FNAME_OUT       argv[4]

// ################################################################################
// MAIN
// ################################################################################
int main( int argc, char *argv[] )
{
    Geometry *geometry;

    // if(argc != NUM_ARGS)
    //     cout << USAGE << endl;
    
    // Initialize 2D voltage mesh representing physical geometry
    cout << "Initializing Geometry..." << endl;
    geometry = new Geometry(XMAX, YMAX, MESH_SIZE);
    cout << "Initializing Potentials..." << endl;
    geometry->initPotentials(INIT_GUESS);
    cout << "Initializing Boundaries..." << endl;
    geometry->initBoundaries(FNAME_BOUNDARY);

    cout << "Starting Iteration..." << endl;
    uint16_t iterations = 0;
    while(geometry->iterate(ACCEL_FACT) > PRECISION)
    {
        iterations++;
    }

    cout << "Iterations: " << iterations << endl;

    geometry->save(FNAME_OUT);

    delete geometry;
}
