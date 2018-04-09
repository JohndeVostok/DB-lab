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

struct Entry {
	typedef vector <pair <unsigned, unsigned>> entry_t;
	entry_t *list;
	unsigned cnt, idx;
	Entry(vector <pair <unsigned, unsigned>> *list, int cnt, int idx = 0) : list(list), cnt(cnt), idx(idx) {}
	entry_t &get() {
		return *list;
	}
	pair <unsigned, unsigned> front() {
		return (*list)[idx];
	}
};

void search(unordered_map<string, unsigned> const& gram_count, unordered_map <string, vector <pair<unsigned, unsigned>>> &mp, unsigned bound, vector <pair <unsigned, unsigned>> &res);
unsigned getED(const char* st1, const char* ed1, const char* st2, const char* ed2, unsigned thres);

int SimSearcher::createIndex(const char *filename, unsigned q_in) {
	mpj.clear();
	mped.clear();
	ngram.clear();
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
			mpj[token].emplace_back(index, 1);
		}
		ngram.push_back(tokens.size());
	}
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result) {
	result.clear();

	unordered_map <string, unsigned> gram_count;
	istringstream buf_stream(query);
	for (string token; buf_stream >> token; gram_count[token] = 1);

	int n = gram_count.size();

	int bound = ceil(n * threshold);

	vector <pair <unsigned, unsigned>> res;

	search(gram_count, mpj, bound ? bound : 1, res);

	for (auto &i: res) {
		auto &idx = i.first;
		auto &cnt = i.second;
		if (cnt >= ceil((n + ngram[idx] - cnt) * threshold)) {
			result.emplace_back(idx, cnt / (double)(n + ngram[idx] - cnt));
		}
	}
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

	int bound = len - decay + 1;
	vector <pair <unsigned, unsigned>> res;

	search(gram_count, mped, bound, res);

	for (auto &i: res) {
		int dist = getED(query, query + len, data[i.first].c_str(), data[i.first].c_str() + data[i.first].length(), threshold);
		if (dist <= threshold) {
			result.emplace_back(i.first, dist);
		}
	}
	return SUCCESS;
}

inline void denew(unsigned &x, unsigned y) {
	if (x > y) x = y;
}

unsigned jump(Entry& entry, unsigned target)
{
	auto &data = entry.get();
	auto &index = entry.idx;
	unsigned lowbit;
	while (true)
	{
		lowbit = index ? (index & -index) : 1;
		if (index + lowbit >= data.size()) break;
		if (data[index + lowbit].first > target) break;
		index += lowbit;
	}
	while (lowbit >>= 1)
		if ((index | lowbit) < data.size() && data[index | lowbit].first <= target)
			index |= lowbit;
	++index;
	return data[index - 1].first == target ? data[index - 1].second : 0;
}

void search(unordered_map<string, unsigned> const& gram_count, unordered_map <string, vector <pair<unsigned, unsigned>>> &mp, unsigned bound, vector <pair <unsigned, unsigned>> &res) {
	vector <Entry> entries;

	int total = 0;

	for (const auto &i : gram_count) {
		if (mp.count(i.first)) {
			entries.emplace_back(&mp[i.first], i.second, 0);
			total += i.second;
		}
	}

	if (total < bound) {
		return;
	}

	priority_queue <pair <unsigned, unsigned>, vector <pair<unsigned, unsigned>>, greater <pair<unsigned, unsigned>>> hp;

	for (int i = 0; i < entries.size(); i++) {
		if (!entries[i].get().empty()) {
			hp.emplace(entries[i].front().first, i);
		}
	}

	vector <unsigned> tmp;
	tmp.clear();

	int cnt = 0, idx = 0;
	while (total >= bound) {
		tmp.clear();

		for (int i = 0; i < bound;) {
			i += entries[hp.top().second].cnt;
			idx = hp.top().first;
			tmp.push_back(hp.top().second);
			hp.pop();
		}

		while (!hp.empty() && hp.top().first == idx) {
			tmp.push_back(hp.top().second);
			hp.pop();
		}

		cnt = 0;
		for (auto i : tmp) {
			cnt += min(jump(entries[i], idx), entries[i].cnt);
			if (entries[i].idx < entries[i].get().size()) {
				hp.emplace(entries[i].front().first, i);
			} else {
				total -= entries[i].cnt;
			}
		}
		if (cnt >= bound) res.emplace_back(idx, cnt);
	}
}

unsigned getED(const char* st1, const char* ed1, const char* st2, const char* ed2, unsigned thres) {
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
