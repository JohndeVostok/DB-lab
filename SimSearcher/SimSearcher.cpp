#include "SimSearcher.h"
#include <unordered_set>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <queue>

using namespace std;

SimSearcher::SimSearcher() {}

SimSearcher::~SimSearcher() {}

unsigned getED(const char* st1, const char* ed1, const char* st2, const char* ed2, unsigned thres);

int SimSearcher::createIndex(const char *filename, unsigned q_in) {
	mpj.clear();
	mped.clear();
	q = q_in;
	char buf[2050];
	int index = 0;
	data.clear();
	ngram.clear();

	ifstream infile(filename);

	while (infile.getline(buf, 2050)) {
		int len = strlen(buf);
		int index = data.size();
		data.push_back(buf);
		ngram.push_back(0);
		for (int i = 0; i + q <= len; i++) {
			auto &gram_list = mped[string(buf + i, q)];
			if (gram_list.empty() || gram_list.back().first != index) {
				gram_list.emplace_back(index, 1);
			} else {
				gram_list.back().second++;
			}
		}

		unordered_set<string> tokens;
		istringstream buf_stream(buf);
		for (string token; buf_stream >> token; tokens.emplace(token));
		for (auto &token : tokens) {
			for (int i = 0; i + q <= token.length(); i++) {
				auto &gram_list = mpj[token.substr(i, q)];
				if (gram_list.empty() || gram_list.back().first != index) {
					gram_list.emplace_back(index, 1);
				} else {
					gram_list.back().second++;
				}
				ngram.back()++;
			}
		}
	}
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result) {
	result.clear();
/*
	unordered_set<string> tokens;
	istringstream buf_stream(query);
	for (string token; buf_stream >> token; tokens.insert(token));

	int n = 0;
	unordered_map<string, int> gram_count;
	for (auto &token: tokens) {
		for (int i = 0; i + q <= token.length(); i++) {
			string gram = token.substr(i, q);
			if (!gram_count.count(gram)) {
				gram_count[gram] = 0;
			}
			gram_count[gram]++;
			n++;
		}
	}

	vector <pair <int, int>> res;

	priority_queue <pair <int, vector <pair <int, int>>*>> hp;

	int tmpn = 0;

	for (const auto &i : gram_count) {
		if (mpj.count(i.first)) {
			hp.push(make_pair(-mpj[i.first].size(), &mpj[i.first]));
			tmpn++;
		}
	}

	vector <vector <pair <int, int>>> tmp(tmpn);
	int tmpid = 0;

	for (int i = 0; i < tmpn - 1; i++) {
		auto pa = hp.top();
		hp.pop();
		auto pb = hp.top();
		hp.pop();
		auto &va = *pa.second;
		auto &vb = *pb.second;
		for (int ia = 0, ib = 0; ia < va.size() || ib < vb.size();) {
			if (ib == vb.size() || (ia < va.size() && va[ia].first < vb[ib].first)) {
				if (tmp[i].empty() || tmp[i].back().first != va[ia].first) {
					tmp[i].push_back(va[ia]);
				} else {
					tmp[i].back().second += va[ia].second;
				}
				ia++;
			} else {
				if (tmp[i].empty() || tmp[i].back().first != vb[ib].first) {
					tmp[i].push_back(vb[ib]);
				} else {
					tmp[i].back().second += vb[ib].second;
				}
				ib++;
			}
		}
		hp.push(make_pair(-tmp[i].size(), &tmp[i]));
	}

	auto pc = hp.top();
	auto &vc = *pc.second;

	for (const auto &i : vc) {
		res.push_back(make_pair(i.first, i.second));
	}

	for (auto &i: res) {
		auto &idx = i.first;
		auto &cnt = i.second;
		if (cnt >= ceil((n + ngram[idx] - cnt) * threshold)) {
			result.emplace_back(idx, cnt / (double)(n + ngram[idx] - cnt));
		}
	}
*/
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result) {
	result.clear();

	int len = strlen(query);
	int n = 0;

	int decay = q * threshold + q;

	if (len < decay) {
		for (int i = 0; i < data.size(); i++)
		{
			int dist = getED(query, query + len, data[i].c_str(), data[i].c_str() + data[i].length(), threshold);
			if (dist <= threshold) {
				result.emplace_back(i, dist);
			}
		}
		return SUCCESS;
	}

	unordered_map<string, unsigned> gram_count;
	for (int i = 0; i + q <= len; i++) {
		string gram = string(query + i, q);
		if (!gram_count.count(gram)) {
			gram_count[gram] = 0;
		}
		gram_count[gram]++;
		n++;
	}

	vector <pair <unsigned, unsigned>> res;

	int bound = len - decay + 1;

	vector <vector <pair <unsigned, unsigned>>*> entries;
	vector <unsigned> pe;

	int tmpn = 0;

	for (const auto &i : gram_count) {
		if (mped.count(i.first)) {
			entries.push_back(&mped[i.first]);
			pe.push_back(0);
			tmpn++;
		}
	}

	priority_queue <pair <unsigned, unsigned>> hp;

	int tmpid = 0;

	for (int i = 0; i < tmpn; i++) {
		hp.push(make_pair(-(*entries[i])[pe[i]].first, i));
	}

	vector <pair <unsigned, unsigned>> tmp;
	tmp.clear();

	while (!hp.empty()) {
		int i;
		tmp.clear();

		for (i = 0; i < bound && !hp.empty();) {
			tmp.push_back(hp.top());
			hp.pop();
			i += (*entries[tmp.back().second])[pe[tmp.back().second]].second;
		}


		int head = -tmp.back().first;
		
		if (tmp.back().first == tmp.front().first){
			while (!hp.empty() && hp.top().first == tmp.front().first) {
				tmp.push_back(hp.top());
				hp.pop();
				i += (*entries[tmp.back().second])[pe[tmp.back().second]].second;
			}
			res.emplace_back(-tmp.front().first, i);
			head++;
		}
		for (const auto &i : tmp) {
//			printf("%d %d %d   ", i.first, i.second, (*entries[i.second])[pe[i.second]].second);
			int t = i.second;

			int pw = 1;
			while (pe[t] + pw < (*entries[t]).size() && (*entries[t])[pe[t] + pw].first < head) {
				pw += pw;
			}

			int l = pe[t], r, mid;

			if (l + pw < (*entries[t]).size()) {
				r = l + pw;
			} else {
				r = (*entries[t]).size();
			}
	
			while (l + 1 < r) {
				mid = (l + r) >> 1;
				if ((*entries[t])[mid].first < head) l = mid; else r = mid;
			}
			if ((*entries[t])[l].first < head) l++;
			pe[t] = l;

			while (pe[t] < (*entries[t]).size() && (*entries[t])[pe[t]].first < head) {
				pe[t]++;
			}

			if (pe[t] < (*entries[t]).size()) hp.push(make_pair(-(*entries[t])[pe[t]].first, t));
		}
	}

	for (auto &i: res) {
		int dist = getED(query, query + len, data[i.first].c_str(), data[i.first].c_str() + data[i.first].length(), threshold);
		if (dist <= threshold) {
			result.emplace_back(i.first, dist);
		}
	}
	return SUCCESS;
}

inline void denew(unsigned &x, unsigned y) {if (x > y) x = y;}

unsigned getED(const char* st1, const char* ed1, const char* st2, const char* ed2, unsigned thres)
{
	if (ed1 - st1 > ed2 - st2) {swap(st1, st2); swap(ed1, ed2);}
	while (st1 < ed1 && *st1 == *st2) st1++, st2++;
	while (st1 < ed1 && *(ed1-1) == *(ed2-1)) ed1--, ed2--;
	if (st1 == ed1) return ed2 - st2;
	unsigned l1 = ed1 - st1, l2 = ed2 - st2;
	if (thres > l2) thres = l2;
	if (l1 + thres < l2) return thres + 1;

	unsigned range = thres << 1 | 1;
	vector <unsigned> d0(range), d1(range);
	for (unsigned j=0; j<=thres; j++) d0[j + thres] = j;
	for (unsigned i=1; i<=l1; i++)
	{
		unsigned lowb = i < thres ? 0 : i - thres;
		unsigned upb = min(l2, i + thres);
		bool f = 0;
		for (unsigned j=lowb, pos=j+thres-i; j<=upb; j++, pos++)
		{
			d1[pos] = thres + 1;
			if (j > lowb) denew(d1[pos], d1[pos - 1] + 1);
			if (j > 0) denew(d1[pos], d0[pos] + (st1[i-1] != st2[j-1]));
			if (j < i + thres) denew(d1[pos], d0[pos + 1] + 1);
			f |= (d1[pos] <= thres);
		}
		if (!f) return thres + 1;
		swap(d0, d1);
	}
	return d0[l2 + thres - l1];
}
