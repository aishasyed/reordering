#!/usr/bin/python

#
# An implementation of the RD sequence regeneration algorithm.
#

import sys

def constructGraph(R, N):
    K = len(R)
    bipartites = [[ None for j in range(N)] for i in range(K) ]
    
    # bipartities[k][left_vertex] = the connected right_vertex or None
    for i in range(K):
        disp = R[i][0] # R[i].displacement
        for j in range(N):
            right_vertex = j + disp
            if right_vertex >= 0 and right_vertex < N:
                bipartites[i][j] = right_vertex
    return bipartites

def solveStepOriginal(bipartites, N, rem_pkts, solution, step):
    if step >= N:
        return True

    K = len(bipartites)
    for i in range(K):
        if rem_pkts[i] == 0:
            continue

        right_vertex = bipartites[i][step]
        if right_vertex != None:
            if solution[right_vertex] != None:
                continue

            solution[right_vertex] = step # found a place for this step's packet
            rem_pkts[i] -= 1 # decrement for this RD

            if solveStepOriginal(bipartites, N, rem_pkts, solution, step + 1):
                return True

            rem_pkts[i] += 1
            solution[right_vertex] = None
                
    return False

def solve(R):
    K = len(R)

    rem_pkts = []
    for i in range(K):
        rem_pkts.append(R[i][1])  # initialize rem_pkts[i] with R[i].count

    N = sum(rem_pkts)
    solution = [None] * N

    bipartites = constructGraph(R, N)

    if solveStepOriginal(bipartites, N, rem_pkts, solution, 0):
        return solution
    else:
        return None

def main(argv):
    sys.setrecursionlimit(10000)
    
    RD = []
    RD.append((0, 1))
    RD.append((-1, 1))
    RD.append((1, 1))
    
    print solve(R)

if __name__ == "__main__":
    main(sys.argv[1:])




