#ifndef __EXP2_SIMJOINER_H__
#define __EXP2_SIMJOINER_H__

#include <vector>

template <typename IDType, typename SimType>
class JoinResult {
public:
    IDType id1;
    IDType id2;
    SimType s;
    JoinResult(IDType a, IDType b, SimType c) {
		id1 = a;
		id2 = b;
		s = c;
    }
    friend bool operator < (const JoinResult &a, const JoinResult &b) {
		return (a.id1 == b.id1) ? a.id2 < b.id2 : a.id1 < b.id1;
    }
};

typedef JoinResult<unsigned, double> JaccardJoinResult;
typedef JoinResult<unsigned, unsigned> EDJoinResult;

const int SUCCESS = 0, FAILURE = 1;

const int MAXN = 1 << 18;
const int MAX_LENGTH = 512;

class SimJoiner {
public:
	int nquery;
    SimJoiner();
    ~SimJoiner();

    void createIndexED(const char *filename);
    void searchED(const char *query, int queryLength, int __n, std::vector<EDJoinResult> &result);
    int joinED(const char *filename1, const char *filename2, unsigned threshold, std::vector <EDJoinResult> &result);
	unsigned getED(const char* st1, const char* ed1, const char* st2, const char* ed2, unsigned thres);

    void createIndexJaccard(const char *filename);      
    void insertTrie(int id);
    void searchJaccard(const char *query, int queryLength, double threshold, int __n, std::vector<JaccardJoinResult> &result);
    int joinJaccard(const char *filename1, const char *filename2, double threshold, std::vector<JaccardJoinResult> &result);
};

#endif
