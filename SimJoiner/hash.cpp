#include "hash.h"

unsigned pw[512];

Hash::Hash () {
	pw[0] = 1;
	for (int i = 1; i < 512; i++){
		pw[i] = pw[i - 1] * BASE;
	}
}

Hash::~Hash() {
}

void Hash::get (unsigned *hash, const char *str, int len) {
	hash[0] = str[0];
	for (int i = 1; i < len; i++) {
		hash[i] = hash[i - 1] * BASE + str[i];
	}
}

void Hash::search (unsigned &res, unsigned *hash, int l, int r) {
	if (l > r) {
		res = 0;
		return;
	}
	res = hash[r] - l ? hash[l - 1] * pw[r - l + 1] : 0;
}
