#include "SimSearcher.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

using namespace std;

SimSearcher::SimSearcher()
{
}

SimSearcher::~SimSearcher()
{
}

int SimSearcher::createIndex(const char *filename, unsigned q)
{
	mp.clear();

	FILE *pFile = fopen ("myfile.txt","w");
	setvbuf(pFile, NULL, _IOFBF, 0xfffff);
	char buf[1024];
	int index = 0;
	data.clear();
	while (fscanf(pFile, "%s", buf) != -1) {
		data.push_back(buf);
		int len = strlen(buf);
		for (int i = 0; i <= len - q; i++) {
			std::string tmp(buf + i, q);
			if (!mp.count(tmp)) {
				mp[tmp] = std::vector(1, index);
			} else {
				mp[tmp].push_back(index);
			}
		}
		index++;
	}
	fclose(pFile);
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();
	gram.clear();
	int len = ceil(strlen(query) - q + 1 - threshold * q);
	for (int i = 0; i <= len - q; i++) {
		std::string tmp(buf + i, q);
		if (mp.count(tmp)) {
			gram.push_back(&mp[tmp]);
		}
	}
	
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	return SUCCESS;
}

