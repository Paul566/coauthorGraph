import numpy as np
import matplotlib.pyplot as plt
from igraph import *
import random
import json
from collections import deque
from scipy.stats import binom


def genNormLaplaceER(n, p):
    # generates a normalized Laplacian matrix of a random graph in Erdos-Renyi model
    l = np.zeros((n, n))
    for i in range(n):
        for j in range(i + 1, n):
            x = random.random()
            if x < p:
                l[i][j] = -1
                l[j][i] = -1
                l[i][i] += 1
                l[j][j] += 1
    for i in range(n):
        for j in range(n):
            if i == j or l[i][i] == 0 or l[j][j] == 0:
                continue
            l[i][j] = l[i][j] / np.sqrt(l[i][i] * l[j][j])
    for i in range(n):
        if l[i][i]:
            l[i][i] = 1.
    return l


def plotSpectrumER(n, p):
    # saves the spectrum of the normalized Laplacian matrix of a random graph in Erdos-Renyi model to filename
    L = genNormLaplaceER(n, p)
    vals = np.sort(np.linalg.eigvalsh(L))
    print("least eigenvalue:", vals[0])
    print("spectral gap:", vals[1])
    plt.figure()
    plt.yticks([])
    plt.hist(vals, density=True, bins=round(np.sqrt(n)))
    plt.savefig('spectrum_n' + str(n) + '_p' + str(p) + '.png', dpi=200)


def readData():
    with open('ids_to_names.json') as f:
        ids_to_names = json.load(f)
    with open('edges.json') as f:
        list_edges = json.load(f)
    edges = set()
    for l in list_edges:
        edges.add((l[0], l[1]))
    return ids_to_names, edges


def getVertexVector(idsToNames):
    nodeIds = []
    for key in idsToNames:
        nodeIds.append(key)
    return nodeIds


def getIdsToIndices(nodeIds):
    idsToIndices = {}
    for i in range(len(nodeIds)):
        idsToIndices[nodeIds[i]] = i
    return idsToIndices


def getAdjList(edges, idsToIndices):
    a = []
    for i in range(len(idsToIndices)):
        a.append([])
    for e in edges:
        myfrom = idsToIndices[e[0]]
        to = idsToIndices[e[1]]
        if not to in a[myfrom]:
            a[myfrom].append(to)
            a[to].append(myfrom)
    return a


def plotDegDist(edges, v, picfilename):
    # plots the distribution of the degrees of vertices in the graph
    degs = np.zeros()
    for e in edges:
        degs.append(len(l))
    maxdeg = max(degs)
    mindeg = min(degs)
    print(mindeg, maxdeg)
    plt.figure()
    plt.yticks([])
    plt.hist(degs, density=True, bins=maxdeg - mindeg + 1)
    plt.savefig(picfilename + '.png', dpi=200)


def plotSubGraph(indicesToPlot, nodeIds, adjList, ids_to_names, idsToIndices, filename, rotate=0):
    # plots a subgraph that consists of the specified vertices
    print('plotting subgraph...')
    g = Graph()
    g.add_vertices(len(indicesToPlot))
    for i in range(len(indicesToPlot)):
        g.vs[i]['label'] = ids_to_names[nodeIds[indicesToPlot[i]]]
        for to in adjList[indicesToPlot[i]]:
            if to in indicesToPlot:
                g.add_edge(i, indicesToPlot.index(to))

    visual_style = {'vertex_size': 5,
                    'vertex_label_size': 16,
                    'vertex_label_dist': 3,
                    'bbox': (1000, 1000),
                    'margin': 100,
                    'edge_curved': True}

    # coords = g.layout_kamada_kawai()
    # coords = g.layout_reingold_tilford()
    coords = g.layout_lgl()
    coords.rotate(rotate)
    plot = Plot(target=filename + '.png', bbox=(1000, 1000), background="white")
    plot.add(g, layout=coords, **visual_style)
    plot.redraw()
    plot.save()


def plotConnectedSubGraph(indicesToPlot, nodeIds, adjList, ids_to_names, idsToIndices, filename, rotate=0):
    print('building a subgraph...')
    selected = set([indicesToPlot[0]])
    for index in indicesToPlot:
        dist = [239] * len(idsToIndices)
        dist[index] = 0
        queue = deque([index])
        joint = -1
        while queue:
            cur = queue.popleft()
            if cur in selected:
                joint = cur
                break
            for to in adjList[cur]:
                if dist[to] > dist[cur] + 1:
                    queue.append(to)
                    dist[to] = dist[cur] + 1
                    if to in selected:
                        joint = to
                        break
            if joint != -1:
                break

        while joint != index:
            for to in adjList[joint]:
                if dist[to] == dist[joint] - 1:
                    selected.add(to)
                    joint = to
                    break

    plotSubGraph(list(selected), nodeIds, adjList, ids_to_names, idsToIndices, filename)


def dist(index1, index2, adjList):
    dist = [239] * len(idsToIndices)
    dist[index1] = 0
    queue = deque([index1])
    while queue:
        cur = queue.popleft()
        for to in adjList[cur]:
            if dist[to] > dist[cur] + 1:
                queue.append(to)
                dist[to] = dist[cur] + 1
                if to == index2:
                    return dist[to]
    return -1


def plotDegHist(v, e, adjList, filename, maxdeg=321, deglim=30):
    n = range(maxdeg + 1)
    cnt = [0] * (maxdeg + 1)
    randomcnt = [0] * (maxdeg + 1)
    p = e * 2 / v / (v - 1)
    for vert in adjList:
        cnt[len(vert)] += 1 / v
    print(v, 'vertices')
    print(cnt)

    for i in range(maxdeg + 1):
        randomcnt[i] = binom.pmf(i, v - 1, p)

    fig, ax = plt.subplots(1)
    plt.xlabel('degree')
    plt.xlim(0, deglim)
    plt.ylim(0, 0.40)
    plt.step(n, cnt, where='mid', label='coauthorship graph')
    plt.step(n, randomcnt, where='mid', label='random graph')
    plt.legend()
    plt.savefig(filename + '.png', dpi=200)


if __name__ == '__main__':
    print('reading files...')
    ids_to_names, edges = readData()
    print('creating a list of ids...')
    nodeIds = getVertexVector(ids_to_names)
    print('creating a dictionary of ids...')
    idsToIndices = getIdsToIndices(nodeIds)
    print('creating adjacency list...')
    adjList = getAdjList(edges, idsToIndices)
    interestingIds = ['cVeVZ1YAAAAJ',  # Erdos
                      'SEYNBgoAAAAJ',  # Karasev
                      'zbtQMR8AAAAJ',  # Arutiunov
                      '4K00_vwAAAAJ',  # Popolitov
                      'QSCIqYIAAAAJ',  # Sleptsov
                      'fzY42_QAAAAJ',  # Raigorodskii
                      '4aqLjLag3ckC',  # Levitov
                      'qNMv3IwAAAAJ',  # Gorsky
                      'GhMJ1J0AAAAJ',  # Stegailov
                      'TFx_gLQAAAAJ',  # Tao
                      '-78r30sAAAAJ',  # Sokolov
                      'kX5ye5oAAAAJ',  # Belavin
                      'rKyTmpkAAAAJ',  # Smale
                      'Z-EXYCkAAAAJ',  # Witten
                      '08DoXDMAAAAJ',  # Balitskiy
                      'A3hQegYAAAAJ'  # Norman
                      ]
    interestingIndices = [idsToIndices[i] for i in interestingIds]
    plotDegHist(len(adjList), len(edges), adjList, 'degHist')
