#pragma once
#include <queue>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

const int SUCCESS = 0;
const int FAILURE = 1;

class SimSearcher
{
private:
	int q;

	std::unordered_map <std::string, std::vector<std::pair<unsigned, unsigned>>> mpj, mped;
	std::vector <std::string> data;
	std::vector <int> ngram;

public:
	SimSearcher();
	~SimSearcher();

	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
};

