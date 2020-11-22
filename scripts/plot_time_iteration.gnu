set output "total_time.png"
set term png
set title "Iteration Execution Time, 2D Electrostatic Solver"
set grid
set xrange[32:10000]
set logscale x 2
set xlabel "n, Active Data Points in X and Y Dimension";
set ylabel "Execution Time in Seconds";
set key center top;
plot "time_serial.txt" with linespoints t "Serial Ref",
"time_1.txt" with linespoints t "1 Thread",
"time_2.txt" with linespoints t "2 Threads",
"time_4.txt" with linespoints t "4 MPI Procs",
"time_8.txt" with linespoints t "8 MPI Procs",
"time_16.txt" with linespoints t "16 MPI Procs",
"time_32.txt" with linespoints t "32 MPI Procs",
"time_64.txt" with linespoints t "64 MPI Procs";
