#ifndef __EXP2_SIMJOINER_HASHER_H__
#define __EXP2_SIMJOINER_HASHER_H__

const int BASE = 357;

class Hash {
public:
	Hash ();
	~Hash ();
	void get(unsigned *hash, const char *str, int len);
	void search(unsigned &res, unsigned *hash, int l, int r);
};

#endif
