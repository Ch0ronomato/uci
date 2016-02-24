#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <omp.h>

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
const int INPUT_SIZE=100000;
std::array<std::string, INPUT_SIZE>
strip(std::ifstream& file)
{
    std::array<std::string, INPUT_SIZE> result;
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

void
DoOutput(Result r)
{
    std::cout << "Result: " << r.lineNumber << " " << r.firstChar << " " << r.length << std::endl;
}

// CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------
Result
findPalindrome(std::string s, int l)
{
    Result r{0,0,0};   
    int mid = floor(s.length() / 2);
    int offset = mid & 1;
    while (mid >= 0)
    {
        int j = 0;
        for (int i = mid + offset + 1; i < s.length(); i++) 
        {
            if (s[mid - j] == s[i])
            {
                Result r2 {l, mid-j, i - mid - j + 1};
                r = r < r2 ? r2 : r;
            }
        }
        mid--;
    }
    return r;
}

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
    int th_id, nthreads, num=std::atoi(argv[2]);
    std::ifstream ifs(argv[1]); 
    std::array<std::string, INPUT_SIZE> lines = strip(ifs);
    Result resultA(0,0,0);

    // Part A
    #pragma omp parallel for schedule(static) private(th_id) shared(nthreads) shared(resultA) num_threads(num)
    for (int i = 0; i < INPUT_SIZE; i++) 
    {
        Result r = findPalindrome(lines[i], i);
        resultA = resultA < r ? r : resultA;
    }
    std::cout << "Palindrome: " << lines[resultA.lineNumber] << std::endl;

    // ... Eventually.. 
    DoOutput(resultA);

    // Part B
    Result resultB(0,0,0);
    #pragma omp parallel for schedule(dynamic, 1) private(th_id) shared(nthreads) shared(resultB) num_threads(num)
    for (int i = 0; i < INPUT_SIZE; i++) 
    {
        Result r = findPalindrome(lines[i], i);
        resultB = resultB < r ? r : resultB;
    }
    std::cout << "Palindrome: " << lines[resultB.lineNumber] << std::endl;

    // ... Eventually.. 
    DoOutput(resultB);

    return 0;
}
