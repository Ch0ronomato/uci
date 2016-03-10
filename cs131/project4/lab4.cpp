#include "mpi.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>

// Don't CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------

void
CreateElectionArray(int numberOfProcesses,int electionArray[][2])
{

    std::vector<int> permutation;
    for(int i = 0; i < numberOfProcesses; ++i)
        permutation.push_back(i);
    std::random_shuffle(permutation.begin(), permutation.end());

    for(int i = 0; i < numberOfProcesses; ++i)
    {
        electionArray[i][0] = permutation[i];
        int chance = std::rand() % 4; // 25 % chance of inactive
        electionArray[i][1] = chance != 0; // 50% chance, 
    }

    //Check that there is at least one active
    bool atLeastOneActive = false;
    for(int i = 0; i < numberOfProcesses; ++i)
    {
        if(electionArray[i][1] == 1)
            atLeastOneActive = true;
    }
    if(!atLeastOneActive)
    {
        electionArray[std::rand() % numberOfProcesses][1] = 1;
    }
}

void
PrintElectionArray(int numberOfProcesses, int electionArray[][2])
{
    for(int i = 0; i < numberOfProcesses; ++i)
    {
        std::printf("%-3d ", electionArray[i][0]);
    }
    std::cout << std::endl;
    for(int i = 0; i < numberOfProcesses; ++i)
    {
        std::printf("%-3d ", electionArray[i][1]);
    }
    std::cout << std::endl;
}
void
PrintElectionResult(int winnerMPIRank, int round, int numberOfProcesses, int electionArray[][2])
{
    std::cout << "Round " << round << std::endl;
    std::cout << "ELECTION WINNER IS " << winnerMPIRank << "(" << electionArray[winnerMPIRank][0] << ")  !!!!!\n";
    std::cout << "Active nodes where: ";
    for(int i = 0; i < numberOfProcesses; ++i)
    {
        if(electionArray[i][1] == 1)
            std::cout << i << "(" << electionArray[i][0] << "), ";
    }
    std::cout << std::endl;
    PrintElectionArray(numberOfProcesses, electionArray);
    for(int i = 0; i < numberOfProcesses*4-2; ++i)
        std::cout << "_";
    std::cout << std::endl;
}

// CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------
int getActiveNode(int direction, int idx, int total, int election[][2]) {
    int neighbor = -1;
    bool done = false;
    for (int i = 0; i < total && !done; i++) {
        int index = (idx + (i * direction)) % total;
        if (index < 0) index = total + index;
        if (index != idx && election[index][1] == 1) {
            neighbor = index;    
            done = true;
        }
    }
    return neighbor;
}
int 
getLastNode(int numProcesses, int myProcessId, int election[][2]) { 
    return getActiveNode(-1, myProcessId, numProcesses, election);
}

int
getNextNode(int numProcesses, int myProcessId, int election[][2]) {
    return getActiveNode(1, myProcessId, numProcesses, election);
}

int
main(int argc, char* argv[])
{
    int processId;
    int numberOfProcesses;

    // Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &numberOfProcesses);

    // Two arguments, the program name, the number of rounds, and the random seed
    if(argc != 3)
    {
        if(processId == 0)
        {
            std::cout << "ERROR: Incorrect number of arguments. Format is: <numberOfRounds> <seed>" << std::endl;
        }
        MPI_Finalize();
        return 0;
    }

    const int numberOfRounds = std::atoi(argv[1]);
    const int seed           = std::atoi(argv[2]);
    std::srand(seed); // Set the seed

    auto electionArray = new int[numberOfProcesses][2]; // Bcast with &electionArray[0][0]...
    std::ofstream f("output-" + std::to_string(processId));

    for(int round = 0; round < numberOfRounds; ++round)
    {
        if(processId == 0)
            CreateElectionArray(numberOfProcesses, electionArray);
    
        // broadcast ElectionArray from master to all other nodes.
        MPI_Bcast(&electionArray[0][0], numberOfProcesses*2, MPI_INT, 0, MPI_COMM_WORLD);
        int next = getNextNode(numberOfProcesses, processId, electionArray);
        int last = getLastNode(numberOfProcesses, processId, electionArray);
        bool active = electionArray[processId][1];
        int lastSeen = -1;
        f << "next=" << next << ", last=" << last << std::endl;
        // do the election
        int activeCount = 0, send[numberOfProcesses], recv[numberOfProcesses];
        for (int i = 0; i < numberOfProcesses; i++) {
            int electionId = electionArray[i][0];
            activeCount += electionArray[i][1];
            if (electionArray[i][1] == 0) { 
                send[electionId] = -2;
                recv[electionId] = -2;
            } else {
                lastSeen = i;
                send[electionId] = -1;
                recv[electionId] = -1;
            }
            f << electionId << "=" << send[electionId] << ", ";
        }
        if (activeCount == 1) {
            send[numberOfProcesses - 1] = lastSeen;
        } else {
            f << std::endl;
            send[electionArray[processId][0]] = processId;
            for (int i = 0; i < activeCount; i++) {
                if (!active) continue;
                MPI_Request req;
                MPI_Status stat;
                int tempSend[numberOfProcesses];
                MPI_Isend(&send[0], numberOfProcesses, MPI_INT, next, 0, MPI_COMM_WORLD, &req);
                MPI_Recv(&recv[0], numberOfProcesses, MPI_INT, last, 0, MPI_COMM_WORLD, nullptr);
                MPI_Wait(&req, &stat); 
                
                f << "I sent: ";
                for (int i = 0; i < numberOfProcesses; i++)
                    f << "send[" << i << "]=" << send[i] << ", ";
                f << std::endl;
                f << "I recv: ";
                for (int i = 0; i < numberOfProcesses; i++)
                    f << "recv[" << i << "]=" << recv[i] << ", ";
                f << std::endl;

                // swap sendArray with recv
                for (int i = 0; i < numberOfProcesses; i++) {
                    tempSend[i] = send[i];
                    send[i] = recv[i];
                    recv[i] = tempSend[i];
                }
                send[electionArray[processId][0]] = processId;
                f << "Finished run " << i << std::endl;
            }
            
            for (int i = 0; i < numberOfProcesses; i++) {
                f << "send[" << i << "]=" << send[i];
                f << ", recv[" << i << "]=" << recv[i] << std::endl;;
            }

            // swap send with recv one more time.
            int temp[numberOfProcesses];
            for (int i = 0; i < numberOfProcesses; i++) {
                temp[i] = recv[i];
                recv[i] = send[i];
                send[i] = temp[i];
            }
        }
        int winnerNum = 0;
        for (int i = numberOfProcesses; i > 0; i--) {
            if (send[i - 1] != -2) {
                // I WIN!
                winnerNum = send[i - 1];
                break;
            }
        }
        if(processId == 0)
        {
            int winner;
            if (winnerNum != 0) {
                MPI_Recv(&winner, 1, MPI_INT, winnerNum, 0, MPI_COMM_WORLD, nullptr); 
            }
            PrintElectionResult(winner, round, numberOfProcesses, electionArray);
        } else {
            if (winnerNum == processId) {
                f << "I WIN!" << std::endl;
                MPI_Send(&processId, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
    }

    f.close();
    delete[] electionArray;

    MPI_Finalize();

    return 0;
}
