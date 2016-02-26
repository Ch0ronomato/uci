#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <chrono>
#include <thread>

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

Result
findPalindrome(std::string s, int l)
{
    Result r{0,0,0};   
    int start;
    int len = findLongest(s, start);
    r.lineNumber = l;
    r.firstChar = start;
    r.length = len;
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
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int th_id, nthreads, num=std::atoi(argv[2]);
    std::ifstream ifs(argv[1]); 
    std::array<std::string, INPUT_SIZE> lines = strip(ifs);
    Result resultA(0,0,0);
    
    // Part A
    start = std::chrono::system_clock::now();
    #pragma omp parallel for schedule(static) private(th_id) shared(nthreads) shared(resultA) num_threads(num)
    for (int i = 0; i < INPUT_SIZE; i++) 
    {
        Result r = findPalindrome(lines[i], i);
        resultA = resultA < r ? r : resultA;
    }

    // ... Eventually.. 
    DoOutput(resultA);

    // Part B
    Result resultB(0,0,0);
    start = std::chrono::system_clock::now();
    #pragma omp parallel for schedule(dynamic, 1000) private(th_id) shared(nthreads) shared(resultB) num_threads(num)
    for (int i = 0; i < INPUT_SIZE; i++) 
    {
        Result r = findPalindrome(lines[i], i);
        resultB = resultB < r ? r : resultB;
    }
    // ... Eventually.. 
    DoOutput(resultB);

    return 0;
}
