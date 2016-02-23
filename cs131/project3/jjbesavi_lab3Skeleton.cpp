#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>

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

void
DoOutput(Result r)
{
    std::cout << "Result: " << r.lineNumber << " " << r.firstChar << " " << r.length << std::endl;
}

// CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <filename> <numThreads> " << std::endl;
        return 0;
    }

    // ....... Your OpenMP program goes here ............

    // Probably some common code...

    // Part A

    // ... Eventually.. 
    Result resultA(0,0,0);
    DoOutput(resultA);

    // Part B

    // ... Eventually.. 
    Result resultB(0,0,0);
    DoOutput(resultB);

    return 0;
}
