#include "SimSearcher.h"

using namespace std;

int main(int argc, char **argv)
{
	SimSearcher searcher;

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;

	unsigned q = 3, edThreshold = 7;
	double jaccardThreshold = 0.2;

	searcher.createIndex(argv[1], q);
	searcher.searchJaccard("query", jaccardThreshold, resultJaccard);

	for (int i = 0; i < resultJaccard.size(); i++)
	{
		printf("%d %f\n", resultJaccard[i].first, resultJaccard[i].second);
	}

	searcher.searchED("query", edThreshold, resultED);
	for (int i = 0; i < resultED.size(); i++)
	{
		printf("%d %d\n", resultED[i].first, resultED[i].second);
	}

	return 0;
}

