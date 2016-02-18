#include "mpi.h"

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <array>
#include <iostream>
using Lines = std::array<std::string, 100000>;
// Don't CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------

struct Result
{
    Result()
    : lineNumber(0), firstChar(0), length(0)
    {}

    Result(int lineNumber_, int firstChar_, int length_)
    : lineNumber(lineNumber_), firstChar(firstChar_), length(length_)
    {}

    // This allows you to compare results with the < (less then) operator, i.e. r1 < r2
    bool
    operator<(Result const& o)
    {
        // Line number can't be equal
        return length < o.length || 
            (length == o.length && lineNumber >  o.lineNumber) ||
            (length == o.length && lineNumber == o.lineNumber  && firstChar > o.firstChar);
    }

    int lineNumber, firstChar, length;
};

void DoOutput(Result r)
{
    std::cout << "Result: " << r.lineNumber << " " << r.firstChar << " " << r.length << std::endl;
}

// CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------
Lines strip(std::ifstream& file)
{
    Lines result;
    std::string workString;
    int i = 0;
    while(std::getline(file,workString))
    {
        //Strip non alpha characters
        workString.erase(std::remove_if(workString.begin(), workString.end(),
                                        [] (char c) { return !std::isalpha(c); }
                                        ), workString.end());
        result[i++] = workString;
        workString.clear();
    }
    return result;
}

int main(int argc, char* argv[])
{
    int processId;
    int numberOfProcesses;

    // Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &numberOfProcesses);

    // Two arguments, the program name and the input file. The second should be the input file
    if(argc != 2)
    {
        if(processId == 0)
        {
            std::cout << "ERROR: Incorrect number of arguments. Format is: <filename>" << std::endl;
        }
        MPI_Finalize();
        return 0;
    }

    // ....... Your SPMD program goes here ............
    Lines lines = split(argv[2]);

    // ... Eventually.. 
    if(processId == 0)
    {
        Result result(0,0,0);
        DoOutput(result);
    }

    MPI_Finalize();

    return 0;
}
