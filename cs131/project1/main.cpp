//
//  main.cpp
//  CS131Project1
//
//  Created by schweer on 1/26/16.
//  Copyright © 2016 schweer. All rights reserved.
//

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <cctype>
#include <fstream>
#include <ctime>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <locale>

// Just makes the code clearer.
using Lines = std::vector<std::string>;
using it = Lines::const_iterator;
using thread_pod = std::pair<std::pair<it, it>, int>; // {{begin iter, end iter}, file line number offset}

struct Result
{
    Result(int lineNumber_, int firstChar_, int length_)
    : lineNumber(lineNumber_), firstChar(firstChar_), length(length_)
    {}
    
    // This allows you to compare results with the < (less then) operator, i.e. r1 < r2
    bool
    operator<(Result const& o)
    {
        // Line number can't be equal
        return length < o.length ||
        (length == o.length && lineNumber <  o.lineNumber) ||
        (length == o.length && lineNumber == o.lineNumber  && firstChar < o.firstChar);
    }
    
    int lineNumber, firstChar, length;
};

// Removes all non letter characters from the input file and stores the result in a Lines container
Lines
strip(std::ifstream& file)
{
    Lines result;
    result.reserve(50000); // If reading is too slow try increasing this value
    
    std::string workString;
    
    while(std::getline(file,workString))
    {
        //Strip non alpha characters
        workString.erase(std::remove_if(workString.begin(), workString.end(),
                                        [] (char c) { return !std::isalpha(c); }
                                        ), workString.end());
        result.push_back(workString);
        workString.clear();
    }
    return result;
}

// CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------
/**  SHARED **/
//std::mutex mutex_pod_lock, mutex_member_lock;
class ThreadPodManager {
public:
    ThreadPodManager() = delete;
    explicit ThreadPodManager(const std::vector<std::string>& lines, size_t pod_size);
    thread_pod next(bool send_start = false, int *start_pos = nullptr);
    bool hasNext();
private:
    std::vector<std::string> mVec_lines;
    size_t mSizet_pod_size;
    int mInt_distance, mInt_progress;
    it mConstIter_last_pod_end;
};

ThreadPodManager::ThreadPodManager(const std::vector<std::string>& lines, size_t pod_size) {
    mVec_lines = lines;
    mSizet_pod_size = pod_size;
    mConstIter_last_pod_end = lines.begin();
    mInt_distance = 0;
    mInt_progress = -1 * (int)mSizet_pod_size;
}

thread_pod ThreadPodManager::next(bool send_start, int *start_pos) {
    thread_pod pod;
    int i = 0, lower_bound = (int)mSizet_pod_size;
//    mutex_pod_lock.lock();
//    mutex_member_lock.lock();
    pod.first.first = mConstIter_last_pod_end;
    pod.second = mInt_progress;
    lower_bound = ((mVec_lines.size() - mInt_progress) > mSizet_pod_size)
        ? (int)mSizet_pod_size : (int)mVec_lines.size() - mInt_progress;
    if (send_start) *start_pos = (int)mInt_progress;
//    mutex_member_lock.unlock();
//    mutex_pod_lock.unlock();
//    mutex_member_lock.lock();
    while (i < lower_bound && i++ < mSizet_pod_size) {
        if (mConstIter_last_pod_end == mVec_lines.end())
            break;
        mConstIter_last_pod_end++;
    }
    pod.first.second = mConstIter_last_pod_end;
//    mutex_member_lock.unlock();
    return pod;
}

bool ThreadPodManager::hasNext() {
    // @todo: Figure out why == .end() is not working.
    bool to_return = false;
//    mutex_pod_lock.lock();
    int distance = mInt_distance, i = 0;
    if (mInt_distance == mVec_lines.size()) {
//        mutex_pod_lock.unlock();
        return false;
    }
//    mutex_pod_lock.unlock();
    // increment
    while ((distance + i) < mVec_lines.size() + 1 && (i++) < mSizet_pod_size) { }
    
//    mutex_pod_lock.lock();
    mInt_distance += i - 1;
    mInt_progress += (int)mSizet_pod_size;
    to_return = mInt_distance < mVec_lines.size() + 1;
//    mutex_pod_lock.unlock();
    return to_return;
}

bool checkIfPalindrome(std::string str) {
    bool to_return = true;
    int start = str.length() % 2 == 1 ? (int)(str.length() / 2) : (int)floor(str.length() / 2);
    
    for (int i = 0, j = (int)str.length() - 1; i < start; i++,j--) {
        if (str[i] == str[j]) to_return &= true;
        else to_return = false;
    }
    
    return to_return;
}

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

Result max(Result r1, Result r2) {
    if (r1 < r2) return r2;
    else return r1;
}

void findHighestResult(thread_pod data, Result *results_ptr) {
    Result r(0,0,0);
    int line = 0;
    for (; data.first.first != data.first.second; data.first.first++) {
        int longest = 0, start = 0;
        longest = findLongest(*data.first.first, start);
        r = max(r, {data.second + line, start, longest});
        line++;
    }
    *results_ptr = r;
}

void findHighestResult_temp(thread_pod data, Result *results_ptr) {
    Result r(0,0,0);
    int line = 0;
    for (; data.first.first != data.first.second; data.first.first++) {
        int longest = 0, start = 0;
        longest = findLongest(*data.first.first, start);
        r = max(r, {line++, start, longest});
    }
    *results_ptr = r;
}

// PART A
Result
FindPalindromeStatic(Lines const& lines, int numThreads)
{
    Result r(0,0,0);
    std::vector<Result*> results(numThreads);
    std::vector<std::thread> ts;
    int range = round((double)lines.size() / (double)numThreads);
    int used = 0;
    std::vector<thread_pod> pods(numThreads + 1);
    
    ThreadPodManager mnger(lines, range);
    while (mnger.hasNext()) {
        results[used] = new Result(0,0,0);
        pods[used++] = mnger.next();
    }
    pods[0].first.first = lines.begin();
    
    // make threads and push back
    for (int i = 0; i < numThreads; i++) {
        ts.push_back(std::thread(&findHighestResult, pods[i], results[i]));
    }
    
    for (auto& t : ts)
        t.join();
    for (auto& x : results) {
        r = max(r, *x);
    }
    return r;
}
std::mutex temp_mutex;
void findHighestChunked(ThreadPodManager *mnger, Result *result) {
    Result highest(0,0,0);
    temp_mutex.lock();
    while (mnger->hasNext()) {
        int start = 0;
        thread_pod x = mnger->next(true, &start);
        temp_mutex.unlock();
        Result current(0,0,0);
        findHighestResult_temp(x, &current);
        current.lineNumber += start;
        highest = max(highest, current);
        temp_mutex.lock();
    }
    temp_mutex.unlock();
    
    *result = highest;
}

// PART B
Result
FindPalindromeDynamic(Lines const& lines, int numThreads, int chunkSize)
{
    // construct ThreadPodManager
    Result r(0,0,0);
    std::vector<std::thread> ts;
    std::vector<Result*> results(numThreads);
//    mutex_member_lock.unlock();
//    mutex_pod_lock.unlock();
    ThreadPodManager mnger(lines, chunkSize);
    
    // build threads.
    for (int i = 0; i < numThreads; i++) {
        results[i] = new Result(0,0,0);
        ts.push_back(std::thread(&findHighestChunked, &mnger, results[i]));
    }
    
    // execute.
    for (auto& t: ts)
        t.join();
    for (auto& x : results) {
        r = max(r, *x);
    }
    return r;
}

// DONT CHANGE THIS -----------------------------------------------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
    if(argc != 4)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <filename> <numThreads> <chunkSize>" << std::endl;
        return 0;
    }
    
    std::ifstream theFile(argv[1]);
    if(!theFile.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
        return 0;
    }
    int numThreads = std::atoi(argv[2]);
    int chunkSize  = std::atoi(argv[3]);
    clock_t t;
    std::cout << "Process " << argv[1] << " with " << numThreads << " threads using a chunkSize of " << chunkSize << " for dynamic scheduling\n" << std::endl;
    
    Lines lines = strip(theFile);
    
    //Part A
    t = clock();
    Result aResult = FindPalindromeStatic(lines, numThreads);
    t = clock() - t;
    std::cout << "PartA: " << aResult.lineNumber << " " << aResult.firstChar << " " << aResult.length << ":\t" << lines.at(aResult.lineNumber).substr(aResult.firstChar, aResult.length) << std::endl;
    std::cout << "It took " << ((float)t) / CLOCKS_PER_SEC << " seconds." << std::endl;
    //Part B
    t = clock();
    Result bResult = FindPalindromeDynamic(lines, numThreads, chunkSize);
    t = clock() - t;
    std::cout << "PartB: " << bResult.lineNumber << " " << bResult.firstChar << " " << bResult.length << ":\t" << lines.at(bResult.lineNumber).substr(bResult.firstChar, bResult.length) << std::endl;
    std::cout << "It took " << ((float)t) / CLOCKS_PER_SEC << " seconds." << std::endl;    
    return 0;
}


