#include <iostream>
#include <fstream>
#include <sys/time.h>
//#include <omp.h>

#include <geometry.h>

using namespace std;

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
#define FNAME_MESH_OUT  "output/mesh.out"
#define FNAME_STAT_OUT  "output/stat.out"

#define NUM_ARGS    6
#define USAGE       "./bin/main [NUM_THREADS] [MESH_CFG_PATH] [BOUNDARY_CFG_PATH] [SOR_CFG_PATH] [MESH_OUTPUT_PATH] [STAT_OUTPUT_PATH]"
// #define FNAME_MESH_CFG  argv[1]
// #define FNAME_BOUNDARY  argv[2]
// #define FNAME_SOR_CFG   argv[3]
// #define FNAME_OUT       argv[4]

// ################################################################################
// TIMER COMPONENTS
// Adapted from UAH CPE-512 heat_2d_serial.c 
// Written by Dr. Buren Wells
// ################################################################################
#define TIMER_CLEAR     (tv1.tv_sec = tv1.tv_usec = tv2.tv_sec = tv2.tv_usec = 0)
#define TIMER_START     gettimeofday(&tv1, (struct timezone*)0)
#define TIMER_ELAPSED   (double) (tv2.tv_usec- tv1.tv_usec)/1000000.0+(tv2.tv_sec-tv1.tv_sec)
#define TIMER_STOP      gettimeofday(&tv2, (struct timezone*)0)
struct timeval tv1,tv2;

// ################################################################################
// MAIN
// ################################################################################
int main( int argc, char *argv[] )
{
    Geometry *geometry;
    uint16_t iterations;
    ofstream statFile;

    // if(argc != NUM_ARGS)
    //     cout << USAGE << endl;
    
    // Initialize 2D voltage mesh representing physical geometry
    cout << "Initializing Geometry..." << endl;
    geometry = new Geometry(XMAX, YMAX, MESH_SIZE);
    cout << "Initializing Potentials..." << endl;
    geometry->initPotentials(INIT_GUESS);
    cout << "Initializing Boundaries..." << endl;
    geometry->initBoundaries(FNAME_BOUNDARY);

    // Start iteration
    cout << "Starting Iteration..." << endl;
    TIMER_CLEAR;
    TIMER_START;
    
    iterations = 0;
    while(geometry->iterate(ACCEL_FACT) > PRECISION)
    {
        iterations++;
    }
    
    TIMER_STOP;

    // Display run information on screen
    cout << "Iterations: " << iterations << endl;
    cout << "Elapsed Time: " << TIMER_ELAPSED << endl;

    // Save mesh model to output file
    geometry->save(FNAME_MESH_OUT);

    // Append run statistics to output file
    statFile.open(FNAME_STAT_OUT, ios_base::app);
    statFile << geometry->getNumNodes() << " " << TIMER_ELAPSED << endl; 

    delete geometry;
}
