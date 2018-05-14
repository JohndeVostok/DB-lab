R = 75000
grid = 10000

def dist(ida, idb):
	tmp = (nodes[ida][0] - nodes[idb][0]) ** 2
	tmp += (nodes[ida][1] - nodes[idb][1]) ** 2
	tmp = tmp ** 0.5
	return tmp

if __name__ == "__main__":
    with open("../data/road.cnode") as f:
        lines = f.readlines()
    nodes = []
    for line in lines:
        tmp = line.split(" ")
        nodes.append([float(tmp[1]) * R, float(tmp[2]) * R])
    with open("../data/road.nedge") as f:
    	lines = f.readlines()
    lines.pop(0)
#    print(mapWidth - max([tmp[0] for tmp in nodes]), min([tmp[0] for tmp in nodes]))
#    print(mapHeight - max([tmp[1] for tmp in nodes]), min([tmp[1] for tmp in nodes]))

    mapxMin = int((min([tmp[0] for tmp in nodes]) / 10000) - 1) * 10000
    mapxMax = int((max([tmp[0] for tmp in nodes]) / 10000) + 2) * 10000
    mapyMin = int((min([tmp[1] for tmp in nodes]) / 10000) - 1) * 10000
    mapyMax = int((max([tmp[1] for tmp in nodes]) / 10000) + 2) * 10000
    mapWidth = mapxMax - mapxMin
    mapHeight = mapyMax - mapyMin

    print(mapWidth, mapHeight)

    edges = []
    for line in lines:
    	tmp = line.split(" ")
    	edges.append([int(tmp[0]), int(tmp[1]), int(tmp[2])])
    cnt = 0
