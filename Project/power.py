from __future__ import print_function
import random
# intra-chiplet/inter-chiplet index map
#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P


# P(1/n1 +1/n2...) For each core intra-chiplet power calculation (256 cores total)
# k(1/m1+1/m2...) for each chiplet inter-chiplet power multiplier (16 chiplets total)
# baseline power 2W (or higher?)
# allocation policies checkerboard, mintemp, In the corners, row by row
# networks mesh for intra-chiplet
# networks mesh, butterfly, clos? for inter-chiplet
# create a list for each network hardcode that list.
# Assume even spacing for this system (the weights will be updated)

#network1
def mesh():
    return {
        'A': {'B': 1, 'E': 1},
        'B': {'A': 1, 'F': 1, 'C': 1},
        'C': {'B': 1, 'G': 1, 'D': 1},
        'D': {'C': 1, 'H': 1},
        'E': {'A': 1, 'F': 1, 'I': 1},
        'F': {'B': 1, 'E': 1, 'J': 1, 'G': 1},
        'G': {'F': 1, 'C': 1, 'H': 1, 'K': 1},
        'H': {'D': 1, 'G': 1, 'L': 1},
        'I': {'E': 1, 'J': 1, 'M': 1},
        'J': {'F': 1, 'I': 1, 'N': 1, 'K': 1},
        'K': {'G': 1, 'L': 1, 'J': 1, 'O': 1},
        'L': {'H': 1, 'K': 1, 'P': 1},
        'M': {'I': 1, 'N': 1},
        'N': {'M': 1, 'J': 1, 'O': 1},
        'O': {'N': 1, 'K': 1, 'P': 1},
        'P': {'O': 1, 'L': 1}
    }
# intra-chiplet/inter-chiplet index map
#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
#network2
def butterfly():
    return {
        'A': {'B': 1, 'E': 1,'C':2, 'I':2},
        'B': {'A': 1, 'F': 1, 'J': 2, 'D':2},
        'C': {'D': 1, 'G': 1, 'A': 2, 'K':2},
        'D': {'C': 1, 'H': 1, 'L': 2, 'B':2},
        'E': {'A': 1, 'F': 1, 'M': 2, 'G':2},
        'F': {'B': 1, 'E': 1, 'H': 2, 'N': 2},
        'G': {'C': 1, 'H': 1, 'E': 2, 'O': 2},
        'H': {'D': 1, 'G': 1, 'P': 2, 'F': 2},
        'I': {'M': 1, 'J': 1, 'A': 2, 'K': 2},
        'J': {'I': 1, 'N': 1, 'B': 2, 'L': 2},
        'K': {'L': 1, 'O': 1, 'I': 2, 'C': 2},
        'L': {'K': 1, 'P': 1, 'J': 2, 'D': 2},
        'M': {'I': 1, 'N': 1, 'O': 2, 'E': 2},
        'N': {'M': 1, 'J': 1, 'F': 2, 'P': 2},
        'O': {'K': 1, 'P': 1, 'M': 2, 'G': 2},
        'P': {'O': 1, 'L': 1, 'N': 2, 'H': 2}
    }


# intra-chiplet/inter-chiplet index map
#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
#network3
def butterdonut():
    return {
        'A': {'J': 3,'C':2, 'B':1},
        'B': {'I': 3, 'D': 2, 'G': 2, 'A':1},
        'C': {'L': 3, 'A': 2, 'F': 2, 'D':1},
        'D': {'K': 3,'B':2, 'C':1},
        'E': {'N': 3,'G':2, 'F':1},
        'F': {'M': 3, 'H': 2, 'C': 2, 'E':1},
        'G': {'P': 3, 'E': 2, 'B': 2, 'H':1},
        'H': {'O': 3,'F':2, 'G':1},
        'I': {'B': 3,'K':2, 'J':1},
        'J': {'A': 3, 'L': 2, 'O': 2, 'I':1},
        'K': {'D': 3, 'I': 2, 'N': 2, 'L':1},
        'L': {'C': 3,'J':2, 'K':1},
        'M': {'F': 3,'O':2 ,'N':1},
        'N': {'E': 3, 'K': 2, 'P': 2, 'M':1},
        'O': {'H': 3, 'J': 2, 'M': 2, 'P':1},
        'P': {'G': 3,'N':2, 'O':1}
    }
# intra-chiplet/inter-chiplet index map
#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
def foldedtorus(): #Under Construction
    return {
        'A': {'J': 3,'C':2, 'B':10000},
        'B': {'I': 3, 'D': 2, 'G': 2},
        'C': {'L': 3, 'A': 2, 'F': 2},
        'D': {'K': 3,'B':2},
        'E': {'N': 3,'G':2},
        'F': {'M': 3, 'H': 2, 'C': 2},
        'G': {'P': 3, 'E': 2, 'B': 2},
        'H': {'O': 3,'F':2},
        'I': {'B': 3,'K':2},
        'J': {'A': 3, 'L': 2, 'O': 2},
        'K': {'D': 3, 'I': 2, 'N': 2},
        'L': {'C': 3,'J':2},
        'M': {'F': 3,'O':2},
        'N': {'E': 3, 'K': 2, 'P': 2},
        'O': {'H': 3, 'J': 2, 'M': 2},
        'P': {'G': 3,'N':2}
    }

#allocation policy1
#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
def checkerboard():
    return {
        'A': 1,
        'B': 0,
        'C': 1,
        'D': 0,
        'E': 0,
        'F': 1,
        'G': 0,
        'H': 1,
        'I': 1,
        'J': 0,
        'K': 1,
        'L': 0,
        'M': 0,
        'N': 1,
        'O': 0,
        'P': 1
    }
#allocation policy2
#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
def interweaving():
    return {
        'A': 1,
        'B': 1,
        'C': 1,
        'D': 1,
        'E': 0,
        'F': 0,
        'G': 0,
        'H': 0,
        'I': 1,
        'J': 1,
        'K': 1,
        'L': 1,
        'M': 0,
        'N': 0,
        'O': 0,
        'P': 0
    }
def allallocate():
    return {
        'A': 1,
        'B': 1,
        'C': 1,
        'D': 1,
        'E': 1,
        'F': 1,
        'G': 1,
        'H': 1,
        'I': 1,
        'J': 1,
        'K': 1,
        'L': 1,
        'M': 1,
        'N': 1,
        'O': 1,
        'P': 1
    }

#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
def clumped():
    return {
        'A': 1,
        'B': 1,
        'C': 0,
        'D': 0,
        'E': 0,
        'F': 0,
        'G': 1,
        'H': 1,
        'I': 1,
        'J': 1,
        'K': 0,
        'L': 0,
        'M': 0,
        'N': 0,
        'O': 1,
        'P': 1
    }
#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
def clumped96():
    return {
        'A': 1,
        'B': 1,
        'C': 0,
        'D': 0,
        'E': 0,
        'F': 0,
        'G': 0,
        'H': 1,
        'I': 1,
        'J': 0,
        'K': 0,
        'L': 0,
        'M': 0,
        'N': 0,
        'O': 1,
        'P': 1
    }

#   A   B   C   D
#   E   F   G   H
#   I   J   K   L
#   M   N   O   P
def clumped160():
    return {
        'A': 1,
        'B': 1,
        'C': 0,
        'D': 1,
        'E': 0,
        'F': 0,
        'G': 1,
        'H': 1,
        'I': 1,
        'J': 1,
        'K': 0,
        'L': 0,
        'M': 1,
        'N': 0,
        'O': 1,
        'P': 1
    }
def powergrid(nodes, network, policy):
    # pseudo code for intra-chiplet power calc:
    # for each chiplet in list of chiplets
    # for each core in in a chiplet
    # for each core(connected in network) that is "on" (allocated)
    # calculate 1/n values for intra-chiplet


    # will need djkstras for min path calculation (assumes no contention amount paths)
    # OUTPUT= calculate IPS(total) and Power(per core)

    power_map = {}
    for current in nodes: #scan through all the nodes to get shortest distance calc for all
        unvisited = {node: None for node in nodes}  # using None as +inf
        visited = {}
        currentDistance = 0
        unvisited[current] = currentDistance
        while True:
            for neighbour, distance in network[current].items():
                if neighbour not in unvisited: continue
                newDistance = currentDistance + distance
                if unvisited[neighbour] is None or unvisited[neighbour] > newDistance:
                    unvisited[neighbour] = newDistance
            visited[current] = currentDistance
            del unvisited[current]
            if not unvisited: break
            candidates = [node for node in unvisited.items() if node[1]]
            current, currentDistance = sorted(candidates, key=lambda x: x[1])[0]
        allocated=0.0
        #print visited
        for key in visited:
            if visited[key]*policy[key]!=0:
                allocated += 1.0/(visited[key]*policy[key])
        for key in visited:
            if visited[key]==0:
                power_map[key]=allocated



    return power_map

if __name__ == "__main__":
    nodes = ('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I','J','K','L','M','N','O','P')
    intra_chiplet_network = mesh() #take in the network for intra-chiplet as an input
    intra_chiplet_policy = clumped160() #the allocation policy
    intra_chiplet_grid=powergrid(nodes, intra_chiplet_network, intra_chiplet_policy)
    inter_chiplet_network = mesh()
    inter_chiplet_policy = allallocate()
    inter_chiplet_grid=powergrid(nodes,inter_chiplet_network,inter_chiplet_policy)
    #print(inter_chiplet_grid)
    #print(intra_chiplet_grid)
    #inter_chiplet_grid = sorted(dictUsers.keys(), key=lambda x: x.lower())
    print("0.0 0.0 0.0 0.0", end=' ')
    sum=0.0
    powermap = [[0 for x in range(16)] for y in range(16)]
    for index1,key in enumerate(nodes):
        for index2,key2 in enumerate(nodes):
            powermap[index1][index2]=inter_chiplet_grid[key]*intra_chiplet_grid[key2]*intra_chiplet_policy[key2]/10
            #contant changes based on number of cores powered on. Aim is for 2 waats per core on average.
            #print(index2)
            sum +=powermap[index1][index2]
            print(powermap[index1][index2]/10+random.random()*intra_chiplet_policy[key2]/10, end=' ')
            print(powermap[index1][index2], end=' ')

    for i in range (51):
        print("0.0", end=' ')
    #print(*powermap)
    print(' ')
    for i in range (256):
        print("L2_"+str(i), end=' ')
        print("Core_"+str(i), end=' ')
    for i in range(51):
        print("WS_" + str(i), end=' ')
    print(' ')
    print(11*sum/10)