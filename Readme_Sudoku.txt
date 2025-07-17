OVERVIEW:
This program implements a Sudoku validator using three different methods:
1.Sequential Method
2.Chunk Method (Dividing the work into chunks assigned to multiple threads)
3.Mixed Method (Distributing the work across threads in a cyclic manner)

The program reads a Sudoku grid from an input file, validates whether it follows Sudoku rules, and writes the result along with execution time to an output file.
-Supports validation of N x N Sudoku grids.
-Uses pthread library for multithreading.
-Provides three different methods for validation to compare performance.
-Outputs validation results with execution time.

Installations Required:
-c compiler(gcc)
-pthread library  //not necessarily

COMPILATION AND EXECUTION:
Without termination file:
"gcc -o sudoku Sudoku_Main.c -lpthread"
"./sudoku"

Then,
You will have to choose a validation method:
1. Chunk method
2. Mixed method
3. Sequential method
Enter the method you want to implement: (enter here from(1 to 3))

INPUT AND OUTPUT:
Input Format(inp.txt)
K N
<sudoku grid>
Where:
-K is the number of threads.
-N is the dimension of the Sudoku grid (N x N).
-The next N lines contain N space-separated integers representing the Sudoku grid.

Output Format(output.txt)
Thread-wise validation results and for sequential Row-wise then column-wise and then grid-wise

Sudoku is valid/invalid.
The total time taken is <time> microseconds.
