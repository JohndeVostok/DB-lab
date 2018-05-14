import pickle

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
#	print(mapWidth - max([tmp[0] for tmp in nodes]), min([tmp[0] for tmp in nodes]))
#	print(mapHeight - max([tmp[1] for tmp in nodes]), min([tmp[1] for tmp in nodes]))

	mapxMin = int((min([tmp[0] for tmp in nodes]) / grid) - 1) * grid
	mapxMax = int((max([tmp[0] for tmp in nodes]) / grid) + 2) * grid
	mapyMin = int((min([tmp[1] for tmp in nodes]) / grid) - 1) * grid
	mapyMax = int((max([tmp[1] for tmp in nodes]) / grid) + 2) * grid
	mapWidth = mapxMax - mapxMin
	mapHeight = mapyMax - mapyMin

	for i in range(len(nodes)):
		nodes[i][0] -= mapxMin
		nodes[i][1] -= mapyMin

	edges = []
	for line in lines:
		tmp = line.split(" ")
		edges.append([int(tmp[0]), int(tmp[1]), int(tmp[2])])

	with open("../data/map.pkl", "wb") as f:
		pickle.dump((nodes, edges), f)
