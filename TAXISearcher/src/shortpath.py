import numpy
from heapq import heappush, heappop  

class PriorityQueue:
    def __init__(self):  
        self._queue = []  
  
    def push(self, s, x):  
        heappush(self._queue, (s, x))
  
    def pop(self):
        return heappop(self._queue)

    def empty(self):
        return (len(self._queue) == 0)

def init(ns, es):
    global nodes, edges, idx, mp
    nodes = ns
    edges = es
    n = len(nodes)
    m = len(edges)
    idx = [-1 for i in range(n)]
    mp = []
    for i in range(m):
        x = edges[i][0]
        y = edges[i][1]
        s = edges[i][2]
        mp.append([y, s, idx[x]])
        idx[x] = 2 * i
        mp.append([x, s, idx[y]])
        idx[y] = 2 * i + 1

def getRoad(a, b):
    global nodes, edges, idx, mp
    n = len(nodes)
    m = len(edges)
    f = [1000000000 for i in range(n)]
    g = [-1 for i in range(n)]
    f[a] = 0
    q = PriorityQueue()
    q.push(0, a)

    while(not q.empty()):
        s, x = q.pop()
        if (f[x] < s):
            continue
        i = idx[x]
        while (i != -1):
            y = mp[i][0]
            c = mp[i][1]
            if (f[x] + c < f[y]):
                f[y] = f[x] + c
                g[y] = x
                q.push(f[y], y)
            if (y == b):
                break
            i = mp[i][2]
        if y == b:
            break
    tour = []
    while (y != -1):
        tour.append(y)
        y = g[y]
    tour.reverse()
    return f[b], tour

            





