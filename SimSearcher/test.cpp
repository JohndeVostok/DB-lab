#include <cstdio>

int main() {
	unsigned x = 0, y = 1;
	printf("%u %u %u %u %d\n", x, y, -x, -y, (-x < -y));
}
