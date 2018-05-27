import pickle

def dist(nodes, node0, node1):
    return ((nodes[node0][0] - nodes[node1][0]) ** 2 + (nodes[node0][1] - nodes[node1][1]) ** 2) ** 0.5

def detour(nodes, cars, carnode, node, destnode, tours):
    n = len(tours) + 1
    detours = [dist(nodes, carnode, node) - dist(nodes, node, tours[i]) for i in range(len(tours))]
    detours.append(-dist(nodes, node, destnode))
    dest = [i for i in tours]
    dest.append(destnode)
    flag = [0 for i in range(n)]
    tmpnode = node
    order = []
    for i in range(n):
        mn = 10000000
        tmp = -1
        for j in range(n):
            if flag[j] == 0 and dist(nodes, tmpnode, dest[j]) < mn:
                mn = dist(nodes, tmpnode, dest[j])
                tmp = j
        for j in range(n):
            if flag[j] == 0:
                detours[j] += mn
        order.append(tmp)
        flag[tmp] = 1
    return detours, order

def validate(nodes, cars, node, car, destnode):
    if len(cars[car][1]) > 3:
        return False, []
    if dist(nodes, node, cars[car][0]) > 10000:
        return False, []
    if len(cars[car][1]) > 0:
        detours, order = detour(nodes, cars, cars[car][0], node, destnode, cars[car][1])
        if (max(detours) > 10000):
            return False, []
        else:
            return True, order
    return True, [0]

def search(nodes, cars, gridcars, node, destnode):
    grid = 5000
    stepList = [[0, 0], [0, 1], [1, 0], [0, -1], [-1, 0], [1, 1], [1, -1], [-1, -1],
                [-1, 1], [0, 2], [2, 0], [0, -2], [-2, 0], [1, 2], [2, 1], [-1, 2],
                [-2, 1], [1, -2], [2, -1], [-1, -2], [-2, -1], [2, 2], [-2, 2],
                [2, -2], [-2, -2]]

    gx = int(nodes[node][0] / 5000)
    gy = int(nodes[node][1] / 5000)

    res = []
    for step in stepList:
        px = gx + step[0]
        py = gy + step[1]
        if px < 0 or px >= 38:
            continue
        if py < 0 or py >= 32:
            continue
        for car in gridcars[px * 32 + py]:
            flag, order = validate(nodes, cars, node, car, destnode)
            if (flag):
                res.append([car, order])
                if len(res) == 5:
                    break
        if len(res) == 5:
            break
    return res

if __name__ == "__main__":
    res = search(5000, 5001)
    print(res)
