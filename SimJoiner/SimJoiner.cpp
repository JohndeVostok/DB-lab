#include "SimJoiner.h"
#include "trie.h"

#include <algorithm>
#include <sstream>
#include <cstring>

using namespace std;

const int SEED = 357;
int ED_LIMIT = 2;

int nstr;
int length[MAXN];
int qFlag[MAXN];
int pos[MAX_LENGTH][4], len[MAX_LENGTH][4];
unsigned pw[MAX_LENGTH];
unsigned shash[MAXN][256];
char str[MAXN][MAX_LENGTH];
vector <pair <unsigned, int>> vec[MAX_LENGTH][4];

SimJoiner::SimJoiner() {
	nquery = 1;
}

SimJoiner::~SimJoiner() {
}

void getHash(unsigned *hash, const char *str, int len) {
    hash[0] = str[0];
    for (int i = 1; i < len; i++) {
    	hash[i] = hash[i - 1] * SEED + str[i];
    }
}

unsigned searchHash(unsigned *hash, int l, int r) {
    if (l > r) return 0;
    unsigned x = l ? hash[l - 1] * pw[r - l + 1] : 0;
    return hash[r] - x;
}

void insert(unsigned* hash, int l, int id) {
    int baselen = l / (ED_LIMIT + 1);
    int nr = l % (ED_LIMIT + 1), p = 0;
    for (int i = 0; i <= ED_LIMIT; i++) {
        int tmplen = baselen + (i + nr > ED_LIMIT);
        pos[l][i] = p;
        len[l][i] = tmplen;
        unsigned h = searchHash(hash, p, p + tmplen - 1);
        vec[l][i].emplace_back(h, id);
        p += tmplen;
    }
}

void SimJoiner::createIndexED(const char *filename) {
	FILE *pf = fopen(filename, "r");
	setvbuf(pf, new char[1 << 20], _IOFBF, 1 << 20);

	memset(pos, -1, sizeof(pos));
	pw[0] = 1;
	for (int i = 1; i < MAX_LENGTH; i++){
		pw[i] = pw[i - 1] * SEED;
	}

	for (int i = 1; i < MAX_LENGTH; i++) {
		for (int j = 0; j <= ED_LIMIT; j++) {
			vec[i][j].clear();
		}
	}

	nstr = 0;
	while (fgets(str[nstr], MAX_LENGTH, pf)) {
		if (str[nstr][strlen(str[nstr]) - 1] == '\n') str[nstr][strlen(str[nstr]) - 1] = 0;
		length[nstr] = strlen(str[nstr]);
		getHash(shash[nstr], str[nstr], length[nstr]);
		insert(shash[nstr], length[nstr], nstr);
		nstr++;
	}
	fclose(pf);
	
	for (int i = 0; i < MAX_LENGTH; i++) {
		if (pos[i][0] == -1) continue;
		for (int j = 0; j <= ED_LIMIT; j++) {
			sort(vec[i][j].begin(), vec[i][j].end());
		}
	}
}

void SimJoiner::searchED(const char *qstr, int qlen, int qid, vector <EDJoinResult> &result) {
    nquery++;
    static unsigned qhash[MAX_LENGTH];
    getHash(qhash, qstr, qlen);
    int lmin = max(1, qlen - ED_LIMIT), lmax = min(MAX_LENGTH - 1, qlen + ED_LIMIT);
    for (int ilen = lmin; ilen <= lmax; ilen++) {
        if (pos[ilen][0] == -1) {
        	continue;
        }
        int dlen = qlen - ilen;
        for (int i = 0; i <= ED_LIMIT; i++) {
            int tlen = len[ilen][i], tpos = pos[ilen][i];
            vector <pair <unsigned, int>> &tvec = vec[ilen][i];
            int l = tpos + max(-i, dlen - (ED_LIMIT - i));
            int r = tpos + min(i, dlen + (ED_LIMIT - i));
            int s = tvec.size();
            l = max(0, l);
            r = min(r, qlen - tlen);
            int tl = i, tr = ED_LIMIT - i;
            for (int j = l; j <= r; j++)
            {
                unsigned h = searchHash(qhash, j, j + tlen - 1);
                vector <pair <unsigned, int>>::iterator it = lower_bound(tvec.begin(), tvec.end(), make_pair(h, 0));
                if (it == tvec.end()) {
                	continue;
                }
                int idx = it - tvec.begin();
                int ans, ans1, ans2;
                for (int k = idx; k < s; k++) {
                    if (tvec[k].first != h) {
                    	break;
                    }
                    int id = tvec[k].second;
                    if (qFlag[id] == nquery) {
                    	continue;
                    }
                    if (tl) {
                    	ans1 = getED(qstr, qstr + j, str[id], str[id] + tpos, tl);
                    } else {
                    	ans1 = 1 - (searchHash(qhash, 0, j - 1) == searchHash(shash[id], 0, tpos - 1));
                    }
                    if (ans1 > tl) {
                    	continue;
                    }
                    if (tr) {
                    	ans2 = getED(qstr + j + tlen, qstr + qlen, str[id] + tpos + tlen, str[id] + length[id], tr);
                    }
                    else {
                    	ans2 = 1 - (searchHash(qhash, j + tlen, qlen - 1) == searchHash(shash[id], tpos + tlen, length[id] - 1));
                    }
                    if (ans2 > tr) {
                    	continue;
                    }
                    ans = ans1 + ans2;
                    if (ans <= ED_LIMIT) {
                    	qFlag[id] = nquery;
                    	result.emplace_back(qid, id, ans);
                    }
                }
            }
        }
    }
}

int SimJoiner::joinED(const char *filename1, const char *filename2, unsigned threshold, vector<EDJoinResult> &result) {
	result.clear();

	ED_LIMIT = threshold;
    createIndexED(filename2);

	FILE *pf = fopen(filename1, "r");
	setvbuf(pf, new char[1 << 20], _IOFBF, 1 << 20);

	int cquery = 0, length;
	char buf[MAX_LENGTH];
	while (fgets(buf, MAX_LENGTH, pf)) {
		if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;
		searchED(buf, strlen(buf), cquery, result);
		cquery++;
	}
	fclose(pf);

    sort(result.begin(), result.end());
	return SUCCESS;
}

void denew(unsigned &x, unsigned y) {
	if (x > y) x = y;
}

unsigned SimJoiner::getED(const char* st1, const char* ed1, const char* st2, const char* ed2, unsigned thres) {
	if (ed1 - st1 > ed2 - st2) {
		swap(st1, st2);
		swap(ed1, ed2);
	}
	while (st1 < ed1 && *st1 == *st2) {
		st1++;
		st2++;
	}
	while (st1 < ed1 && *(ed1 - 1) == *(ed2 - 1)) {
		ed1--;
		ed2--;
	}
	if (st1 == ed1) {
		return ed2 - st2;
	}
	unsigned l1 = ed1 - st1, l2 = ed2 - st2;
	if (thres > l2) {
		thres = l2;
	}
	if (l1 + thres < l2) {
		return thres + 1;
	}

	unsigned range = thres << 1 | 1;
	vector <unsigned> d0(range), d1(range);

	for (unsigned j = 0; j <= thres; j++) {
		d0[j + thres] = j;
	}
	for (unsigned i = 1; i <= l1; i++) {
		unsigned lowb = i < thres ? 0 : i - thres;
		unsigned upb = min(l2, i + thres);
		bool f = 0;
		for (unsigned j = lowb, pos = j + thres - i; j <= upb; j++, pos++) {
			d1[pos] = thres + 1;
			if (j > lowb) {
				denew(d1[pos], d1[pos - 1] + 1);
			}
			if (j > 0) {
				denew(d1[pos], d0[pos] + (st1[i - 1] != st2[j - 1]));
			}
			if (j < i + thres) {
				denew(d1[pos], d0[pos + 1] + 1);
			}
			f |= (d1[pos] <= thres);
		}
		if (!f) {
			return thres + 1;
		}
		swap(d0, d1);
	}
	return d0[l2 + thres - l1];
}

int nterm;
int tmpSize[MAXN];
int termSize[MAXN], termMin[MAXN]; // wordN

vector <vector <int>> invList;

Trie triePool[MAX_NODE];
Trie *pr;
int trieSize;

Trie *newTrie () {
	return &triePool[trieSize++];
}

void SimJoiner::insertTrie (int id) {
	Trie *px = 0;
	vector <string> tokens;
	vector <int> terms;
	istringstream bufStream(str[id]);
	for (string token; bufStream >> token; tokens.emplace_back(token));
	for (auto &token : tokens) {
		px = pr;
		for (int i = 0; i < token.length(); i++) {
			px = px->next(token[i]);
		}
		if (px->id == -1) {
			termMin[nterm] = MAX_LENGTH;
			px->id = nterm++;
			invList.emplace_back(0);
		}
		terms.emplace_back(px->id);
	}
	sort(terms.begin(), terms.end());
	auto uniqueEnd = unique(terms.begin(), terms.end());
	termSize[id] = uniqueEnd - terms.begin();
	terms.erase(uniqueEnd, terms.end());

	for (const auto &term : terms) {
		invList[term].emplace_back(id);
		if (terms.size() < termMin[term]) {
			termMin[term] = terms.size();
		}
	}
}

int nword;

void SimJoiner::createIndexJaccard(const char *filename) {
    nstr = 0;
    nterm = 0;
    invList.clear();
    trieSize = 0;
    pr = newTrie();

	FILE *pf = fopen(filename, "r");
	setvbuf(pf, new char[1 << 20], _IOFBF, 1 << 20);
	while (fgets(str[nstr], MAX_LENGTH, pf)) {
		if (str[nstr][strlen(str[nstr]) - 1] == '\n') str[nstr][strlen(str[nstr]) - 1] = 0;
		length[nstr] = strlen(str[nstr]);
		insertTrie(nstr);
		nstr++;
	}
	fclose(pf);

    for (int i = 0; i < invList.size(); i++) {
    	sort(invList[i].begin(), invList[i].end());
    }
}

void SimJoiner::searchJaccard(const char *qstr, int qlen, double threshold, int qid, vector <JaccardJoinResult> &result) {
    nquery++;
    int p = 0, qTermSize = 0, appearWordN = 0;

	vector <string> tokens;
	vector <int> terms;
	Trie *px = 0;
	istringstream bufStream(qstr);
	for (string token; bufStream >> token; tokens.emplace_back(token));
	for (auto &token : tokens) {
		px = pr;
		for (int i = 0; i < token.length(); i++) {
			px = px->next(token[i]);
		}
		if (px->id != -1) {
			terms.emplace_back(px->id);
		}
		qTermSize++;
	}
	sort(terms.begin(), terms.end());
	auto uniqueEnd = unique(terms.begin(), terms.end());
	qTermSize -= terms.end() - uniqueEnd;
	terms.erase(uniqueEnd, terms.end());

    int minSize = MAX_LENGTH;
	for (auto term : terms) {
		minSize = min(minSize, termMin[term]);
	}
    int bound = ceil(max(double(qTermSize), (qTermSize + minSize) / (1 + threshold)) * threshold);

    vector <pair <int, int>> invIdx;

	for (auto term : terms) {
		invIdx.emplace_back(invList[term].size(), term);
	}
	
    if (invIdx.size() < bound) {
    	return;
    }

    sort(invIdx.begin(), invIdx.end());


    int DD = max(0, bound - 2);
    vector <int> candidate1;

	for (int i = 0; i < invIdx.size() - DD; i++) {
		int term = invIdx[i].second;
		for (auto &id : invList[term]) {
			if (termSize[id] < bound) {
				continue;
			}
			if (qFlag[id] != nquery) {
				qFlag[id] = nquery;
				tmpSize[id] = 0;
			}
			tmpSize[id]++;
			if (tmpSize[id] == bound - DD) {
				candidate1.emplace_back(id);
			}
		}
	}

	vector <int> candidate2;

	for (auto &id : candidate1) {
		for (int i = invIdx.size() - DD; i < invIdx.size(); i++) {
			if (tmpSize[id] + invIdx.size() - i < bound) {
				break;
			}
			int term = invIdx[i].second;
			if (binary_search(invList[term].begin(), invList[term].end(), id)) {
				tmpSize[id]++;
			}
		}
		if (tmpSize[id] >= bound) {
			candidate2.emplace_back(id);
		}
	}
    sort(candidate2.begin(), candidate2.end());
    for (auto &id : candidate2) {
        double jac = tmpSize[id] / double(qTermSize + termSize[id] - tmpSize[id]);
        if (jac >= threshold) result.push_back(JaccardJoinResult(qid, id, jac));
    }
}

int SimJoiner::joinJaccard(const char *filename1, const char *filename2, double threshold, vector <JaccardJoinResult> &result)
{
    result.clear();
    createIndexJaccard(filename2);

	FILE *pf = fopen(filename1, "r");
	setvbuf(pf, new char[1 << 20], _IOFBF, 1 << 20);
	int cquery = 0;
	char buf[MAX_LENGTH];
	while (fgets(buf, MAX_LENGTH, pf)) {
		if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;
		searchJaccard(buf, strlen(buf), threshold, cquery, result);
		cquery++;
	}
	fclose(pf);
    return SUCCESS;
}
