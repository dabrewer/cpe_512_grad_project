#include <iostream>
#include <string.h>
#include <fstream>
#include <sys/time.h>
//#include <omp.h>

#include <geometry.h>

using namespace std;

// ################################################################################
// CMD LINE ARG DEFINITIONS
// ################################################################################
#define NUM_ARGS    7
#define USAGE       "./bin/main [MESH_CFG_PATH] [BOUNDARY_CFG_PATH] [SOR_CFG_PATH] [MESH_OUTPUT_PATH] [STAT_OUTPUT_PATH]"
#define FNAME_MESH_CFG   argv[1]
#define FNAME_BOUNDARY   argv[2]
#define FNAME_SOR_CFG    argv[3]
#define FNAME_MESH_OUT   argv[4]
#define FNAME_STAT_OUT   argv[5]

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
// GLOBAL VARIABLES
// ################################################################################
double INIT_GUESS;
double PRECISION;
double ACCEL_FACT;

// ################################################################################
// FUNCTION DECLARATIONS
// ################################################################################
void loadSorCfg(char* fname);

// ################################################################################
// MAIN
// ################################################################################
int main( int argc, char *argv[] )
{
    Geometry *geometry;
    uint16_t iterations;
    ofstream statFile;

    if(argc != NUM_ARGS)
        cout << USAGE << endl;

    // Load iteration parameters
    cout << "Loading SOR Iteration Params..." << endl;
    loadSorCfg(FNAME_SOR_CFG);

    // Initialize 2D voltage mesh representing physical geometry
    cout << "Initializing Geometry..." << endl;
    geometry = new Geometry(FNAME_MESH_CFG);
    // cout << "Initializing Potentials..." << endl;
    // geometry->initPotentials(INIT_GUESS);
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
    cout << "Saving Results..." << endl;
    cout << "Iterations: " << iterations << endl;
    cout << "Elapsed Time: " << TIMER_ELAPSED << endl;

    // Save mesh model to output file
    geometry->save(FNAME_MESH_OUT);

    // Append run statistics to output file
    statFile.open(FNAME_STAT_OUT, ios_base::app);
    statFile << geometry->getNumNodes() << " " << TIMER_ELAPSED << endl; 

    delete geometry;
}

// ################################################################################
// HELPER FUNCTIONS
// ################################################################################
void loadSorCfg(char *fname)
{
    char buf[100];

    FILE *fp = fopen(fname, "r");
    while(fgets(buf, 100, fp))
    {
        char key[100];
        double value;

        sscanf(buf, "%s : %lf", key, &value);

        if(strcmp("ACCEL_FACT", key) == 0)
        {
            ACCEL_FACT = value;
            cout << "ACCEL FACTOR: " << ACCEL_FACT << endl;
        }
        if(strcmp("PRECISION", key) == 0)
        {
            PRECISION = value;
            cout << "PRECISION: " << PRECISION << endl;
        }
    }
}