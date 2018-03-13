#include "SimSearcher.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdio>

using namespace std;

bool operator < (const pair <int, int> &a, const pair <int, int> &b) {
	return a.second > b.second;
}

SimSearcher::SimSearcher() {}

SimSearcher::~SimSearcher() {}

int SimSearcher::createIndex(const char *filename, unsigned q_in) {
	mp.clear();
	q = q_in;
	FILE *pFile = fopen (filename ,"r");
	setvbuf(pFile, NULL, _IOFBF, 0xfffff);
	char buf[1024];
	int index = 0;
	data.clear();
	while (fgets(buf, 1024, pFile) != 0) {
		data.push_back(buf);
		int len = strlen(buf);
		for (int i = 0; i < len - q; i++) {
			string tmp(buf + i, q);
			if (!mp.count(tmp)) {
				mp[tmp] = vector <int> (1, index);
			} else {
				mp[tmp].push_back(index);
			}
		}
		index++;
	}
	fclose(pFile);
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result) {
	result.clear();
	gram_list.clear();
	gram_index.clear();
	int len = strlen(query);
	int gram_max = ceil(len - q + 1 - threshold * q);
	vector <int> empty_list;
	empty_list.clear();
	for (int i = 0; i <= len - q; i++) {
		string tmp(query + i, q);
		if (!mp.count(tmp)) {
			gram_list.push_back(&empty_list);
		} else {
			gram_list.push_back(&mp[tmp]);
		}
		gram_index.push_back(0);
	}
	while (!gram_heap.empty()) {
		gram_heap.pop();
	}
	for (int i = 0; i <= len - q; i++) {
		if (gram_index[i] < gram_list[i]->size()) {
			gram_heap.push(make_pair(i, (*gram_list[i])[gram_index[i]]));
			printf("!!%d %d\n", i, (*gram_list[i])[gram_index[i]]);
		}
	}
	
	printf("!!!iii%d\n", gram_max);
	pair <int, int> gram = make_pair(0, 0), last_gram = make_pair(-1, -1);
	int gram_same = 0;
	while (!gram_heap.empty()) {
		gram = gram_heap.top();
		gram_heap.pop();
		gram_index[gram.first]++;
		if (gram_index[gram.first] < gram_list[gram.first]->size()) {
			gram_heap.push(make_pair(gram.first, (*gram_list[gram.first])[gram_index[gram.first]]));
					printf("!!%d %d\n", gram.first, (*gram_list[gram.first])[gram_index[gram.first]]);
		}

		printf("?%d %d\n", gram.first, gram.second);
		if (gram.second != last_gram.second) {
			if (gram_same >= gram_max) {
				result.push_back(make_pair(gram.second, 0));
			}
			last_gram = gram;
			gram_same = 0;
		}
		gram_same++;
	}
	if (gram_same >= gram_max) {
		result.push_back(make_pair(gram.second, 0));
	}
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result) {
	result.clear();
	return SUCCESS;
}

