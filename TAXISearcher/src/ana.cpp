#include <cstdio>
#include <cstring>
#include <vector>

using namespace std;

const int MAXN = 338024;
const int MAXM = 440525;
const int MAXC = 100000;

class Edge {
public:
	int x, y, s;
	Edge(int a, int b, int c) {
		x = a;
		y = b;
		s = c;
	}
};

class Car {
public:
	int pos;
	vector <int> lp;
	vector <int> dist;
	Car(int p) {
		pos = p;
	}
};

vector <Edge> edges;
vector <Car> cars;
int nxt[MAXN << 1], idx[MAXN];

void addEdge(int x, int y, int s) {
	edges.emplace_back(x, y, s);
	nxt[edges.size()] = idx[x];
	idx[x] = edges.size();
}

int getDist(int x, int y) {
	
}

int main() {
	FILE *pf = fopen("../data/road.nedge", "r");
	setvbuf(pf, new char[1 << 20], _IOFBF, 1 << 20);

	int n, m;
	fscanf(pf, "%d%d", &n, &m);
	edges.reserve(m << 1);
	memset(nxt, 0, sizeof(nxt));
	memset(idx, -1, sizeof(idx));

	for (int i = 0; i < m; i++) {
		int x, y, s;
		fscanf(pf, "%d%d%d", &x, &y, &s);
		addEdge(x, y, s);
		addEdge(y, x, s);
	}

	fclose(pf);
	pf = fopen("../data/car.in", "r");
	setvbuf(pf, new char[1 << 20], _IOFBF, 1 << 20);
	int nc, np, pos;
	fscanf(pf, "%d", &nc);
	for (int i = 0; i < nc; i++) {
		fscanf(pf, "%d", &np);
		fscanf(pf, "%d", &pos);
		cars.emplace_back(pos);
		for (int j = 0; j < np; j++) {
			fscanf(pf, "%d", &pos);
			cars.back().lp.emplace_back(pos);
		}
		if (cars.back().lp.size() == 4) {
			cars.pop_back();
		}
	}
}
