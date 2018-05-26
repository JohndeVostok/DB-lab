import numpy
import pickle

if __name__ == "__main__":
    with open("../data/map.pkl", "rb") as f:
        (nodes, edges) = pickle.load(f)
    with open("../data/car.in", "r") as f:
        lines = f.readlines()
    cars = []
    for line in lines[1:]:
        tmp = line[:-1].split(" ")
        cars.append([int(tmp[1]), [int(tmp[2 + i]) for i in range(int(tmp[0]))]])

    cnt = numpy.zeros((38, 32), dtype = int)
    gridCars = [[] for i in range(38 * 32)]
    for idx, car in enumerate(cars):
        cnt[int(nodes[car[0]][0] / 5000)][int(nodes[car[0]][1] / 5000)] += 1
        gridCars[int(nodes[car[0]][0] / 5000) * 32 + int(nodes[car[0]][1] / 5000)].append(idx)

    with open("../data/cars.pkl", "wb") as f:
        pickle.dump(cars, f)
    with open("../data/gridcar.pkl", "wb") as f:
        pickle.dump(gridCars, f)
