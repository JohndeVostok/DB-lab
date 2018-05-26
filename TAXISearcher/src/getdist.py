import pickle

def dist(node0, node1):
    return ((node0[0] - node1[0]) ** 2 + (node0[1] - node1[1]) ** 2) ** 0.5

def validate(nodes, cars, node, car):
    if len(cars[car][1]) > 3:
        return False
    elif dist(nodes[node], nodes[cars[car][0]]) > 10000:
        return False
    elif len(cars[car][1]) == 1:
        
    return True

def search(node):
    grid = 5000
    stepList = [[0, 0], [0, 1], [1, 0], [0, -1], [-1, 0], [1, 1], [1, -1], [-1, -1],
                [-1, 1], [0, 2], [2, 0], [0, -2], [-2, 0], [1, 2], [2, 1], [-1, 2],
                [-2, 1], [1, -2], [2, -1], [-1, -2], [-2, -1], [2, 2], [-2, 2],
                [2, -2], [-2, -2]]

    with open("../data/map.pkl", "rb") as f:
        nodes, edges = pickle.load(f)
    with open("../data/cars.pkl", "rb") as f:
        cars = pickle.load(f)
    with open("../data/gridcar.pkl", "rb") as f:
        gridcars = pickle.load(f)

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
            if validate(nodes, cars, node, car):
                res.append(car)
                if len(res) == 5:
                    break
        if len(res) == 5:
            break
    return res

if __name__ == "__main__":
    res = search(5000)
    print(res)
