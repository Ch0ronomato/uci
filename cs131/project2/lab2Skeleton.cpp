#include "mpi.h"

#include <algorithm>
#include <functional>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <array>
#include <iostream>
const static int ARRAY_SIZE = 8;
using Lines = char[ARRAY_SIZE][16];
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
int findLongest(std::string text, int &s) {
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
    int N = (int)text.length();
    N = 2*N + 1; //Position count
    int L[N]; //LPS Length Array
    L[0] = 0;
    L[1] = 1;
    int C = 1; //centerPosition
    int R = 2; //centerRightPosition
    int i = 0; //currentRightPosition
    int iMirror; //currentLeftPosition
    int maxLPSLength = 0;
    int maxLPSCenterPosition = 0;
    int start = -1;
    int end = -1;
    int diff = -1;
    
    //Uncomment it to print LPS Length array
    //printf("%d %d ", L[0], L[1]);
    for (i = 2; i < N; i++)
    {
        //get currentLeftPosition iMirror for currentRightPosition i
        iMirror  = 2*C-i;
        L[i] = 0;
        diff = R - i;
        //If currentRightPosition i is within centerRightPosition R
        if(diff > 0)
            L[i] = std::min(L[iMirror], diff);
        
        //Attempt to expand palindrome centered at currentRightPosition i
        //Here for odd positions, we compare characters and
        //if match then increment LPS Length by ONE
        //If even position, we just increment LPS by ONE without
        //any character comparison
        while ( ((i + L[i]) < N && (i - L[i]) > 0) &&
               ( ((i + L[i] + 1) % 2 == 0) ||
                (text[(i + L[i] + 1)/2] == text[(i - L[i] - 1)/2] )))
        {
            L[i]++;
        }
        
        if(L[i] > maxLPSLength)  // Track maxLPSLength
        {
            maxLPSLength = L[i];
            maxLPSCenterPosition = i;
        }
        
        //If palindrome centered at currentRightPosition i
        //expand beyond centerRightPosition R,
        //adjust centerPosition C based on expanded palindrome.
        if (i + L[i] > R)
        {
            C = i;
            R = i + L[i];
        }
    }
    start = (maxLPSCenterPosition - maxLPSLength)/2;
    end = start + maxLPSLength - 1;
    s = start;
    return end - start + 1;
}

void strip(std::ifstream& file, Lines &result)
{
    std::string workString;
    int i = 0;
    while(std::getline(file,workString))
    {
        //Strip non alpha characters
        workString.erase(std::remove_if(workString.begin(), workString.end(),
                                        [] (char c) { return !std::isalpha(c); }
                                        ), workString.end());
	memset(result[i], '\0', 16);
	memcpy(result[i++], workString.c_str(), workString.length());
        workString.clear();
    }
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
    Lines lines;
    if (processId == 0) {
        std::ifstream i(argv[1]);
        strip(i, std::ref(lines));
    }
    char buf[(ARRAY_SIZE / numberOfProcesses) * 16];
    // send contiguious memory out to the processes.
    if (MPI_Scatter(lines, (ARRAY_SIZE / numberOfProcesses) * 16, MPI_CHAR,
	&buf, (ARRAY_SIZE / numberOfProcesses) * 16, MPI_CHAR, 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
	perror("Shit got fucked");
	exit(1);
    }
    printf("Process %d received elements: ", processId);
    vector<std::string> data(ARRAY_SIZE / numberOfProcesses);
    for (int i = 0; i < (ARRAY_SIZE / numberOfProcesses) * 16; i += 16) {
        char s[16];
        memcpy(s, &buf[i], 16);
        std::cout << s << std::endl;
      	data[i] = std::string(s); 
    }
    // ... Eventually.. 
    if(processId == 0)
    {
        Result result(0,0,0);
        DoOutput(result);
    }

    MPI_Finalize();

    return 0;
}
