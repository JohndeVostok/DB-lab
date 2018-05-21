if __name__ == "__main__":
	with open("../data/car.txt", "r") as f:
		lines = f.readlines()
	res = [str(len(lines)) + "\n"]
	for line in lines:
		tmp = line.split(" ")
		n = int(tmp[1])
		t = [tmp[1]]
		for i in range(n + 1):
			t.append(tmp[2 + i].split(",")[2])
		res.append(" ".join(t))
	with open("../data/car.in", "w") as f:
		f.writelines(res)

	cars = res[1:]
	mp = {}
	for dt in cars:
		car = dt.split(" ")
		n = int(car[0])
		for i in range(n):
			mp[car[i + 1]] = 1;
	print(len(mp))
