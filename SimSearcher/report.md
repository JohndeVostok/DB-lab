# 数据库专题训练实验报告

计54 马子轩 2015012283

## 实验内容

### 综述

实验内容为近似查询，具体就是根据编辑距离(ED)和Jaccard距离建立倒排列表。针对查询筛选相应的结果。

### 实验框架

Simsearcher类中三个方法:

createIndex()

读入数据，预处理

searchJaccard()

按照Jaccard方式查询

searchED()

按照编辑距离查询

## 实验过程

### createIndex

我在这个过程中主要是分gram，建立倒排列表。

其中ED需要按gram长度分gram，而Jaccard按空格直接分割即可。

```cpp
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
```

### searchJaccard

将输入query按空格分割，查询，并计算Jaccard值。

```cpp
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
```

### searchED

将输入分成gram，在倒排列表中查询。其中，编辑距离较小的查询，只能全部搜索，这种情况单独处理。其他情况，匹配的gram数是有最小值的，因此，可以直接按gram查询，查出部分结果，再计算编辑距离二次筛选。

```cpp
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
```

### search

查询gram数匹配的条目，这部分我花了较多功夫。我的实现方式是，按gram取出相应的列表。然后每次把列表头放进堆，在堆中pop出要求数量的gram，判断这些是否属于相同的条目。如果属于，说明这个条目满足了匹配gram数，如果不属于，那么他不匹配，同时说明query中该gram无效，此时进行剪枝。每次匹配后，将每个列表的列表的无效开头(不可能再被匹配)去掉。最后即可得到结果。具体过程见代码实现。

```cpp
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
```

## 实验结果

### 代码结构

makefile

Simsearcher.cpp

Simsearcher.h

main.cpp

### github repository

https://github.com/JohndeVostok/DB-lab

### 运行过程

make

./Simsearcher sample.txt

### 运行结果

exp1: 78s

exp1-final: 480s

### 性能分析

根据编写过程中的情况，在查询过程中，不针对失效gram个数进行剪枝是无法通过所有数据的。

另外，在处理无效gram的时候，列表指针的跳跃我编写了3种不同的算法:顺序查询，二分查找，先倍增再二分。结论上，这三种方式差别不大。不构成性能瓶颈。

同时，我还测试了两种其他的查询方式，都是基于对需要查询的gram列表进行合并，一种是直接连接，排序。一种是启发式归并。第一个复杂度为logNL，第二个是NL。这两种都无法通过所有测试点。原因是因为两者扩展性较差，不能够合理进行剪枝优化，对大的数据性能非常差。

最后我的方法，堆中的数据不会太多，因此和之前提到的第二种方式理论复杂度相同，但是能剪枝，因此选用了此算法。

## 实验总结

这个实验需要在课程理解的基础上，进行较多小优化。同时还要管理较多的边界条件。实际过程中还是遇到了很多困难的，在通过和同学讨论交流后，选择了较为合适的算法才通过了测试。希望能在下个实验中吸取教训。