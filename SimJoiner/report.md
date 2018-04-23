# 数据库专题训练实验报告

计54 马子轩 2015012283

## 实验内容

### 题目描述

近似连接

给定两篇文档，按行拆分，给定一个阈值，返回所有满足阈值条件的语句对。

其中条件分为编辑距离和Jaccard两种。

### 实验框架

实现SimJoiner类中2个方法:

joinED()

joinJaccard()

## 实验过程

### ED

#### 算法分析

我采用了filter-validate方式设计

length filter -> partition filter -> prefix filter -> suffix filter -> validate

length filter: 按长度筛选掉一定不可能的entry

partition filter: 分段，如果有一段完全相同，那么他是备选的entry

prefix filter: 判断前缀，如果编辑距离过大，筛掉

suffix filter: 计算后缀的编辑距离，求和，得到entry的编辑距离，判断筛选

validate: 计算编辑距离

其中使用了hash方式进行字符串的比较，加快程序运行速度。

####createIndex

```cpp
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
```

####search

```cpp
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
```

### Jaccard

#### 算法分析

我采用了filter-validate方式设计

length filter -> inverted list -> 

length filter: 根据切分出的term数筛选一轮

inverted list: 构造倒排列表

prefix prune: 根据前缀中的匹配项筛选，如果达到要求数，进入下一轮

suffix prune: 根据后缀匹配项筛选，如果总匹配数达到要求，进入下一轮

validate: 计算Jaccard值

其中使用了trie来构造global order。

#### createIndex

```cpp
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
```

#### search

```cpp
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
        if (jac >= threshold) {
        	result.emplace_back(qid, id, jac);
        }
    }
}
```

## 实验结果

### 代码结构

makefile

SimJoiner.cpp

SimJoiner.h

Trie.cpp

Trie.h

main.cpp

### github repository

https://github.com/JohndeVostok/DB-lab

### 运行过程

make

./simjoiner a.txt b.txt

### 运行结果

|Test|ID|Homework|Upload Timestamp|Status|Memory(GB)|Time(s)|
|-|-|-|-|-|-|-|
|exp2|7148|exp2|2018/04/23 15:09:51|Correct.|1.348|1.11|
|exp2-final|7106(Marked)|exp2-final|2018/04/23 14:35:50|Correct.|1.393|5.758|

### 性能分析

算法中影响最大的部分实际是字符串的比较方法。针对ED测试，没使用hash的时候初始化已经使用了几分钟，而后续查询根本无法完成。使用hash方式之后，字符串比较便的简便容易，而可以增加多种filter方法而不增加性能代价了。

而Jaccard中，我使用了Trie进行global order的构造，写法简便有效，也方便查询。后续filter就是实现了课件上的算法。

比较有效的优化方式：

1、读入优化，整个读进来针对字符处理。(麻烦)

2、避免使用vector等c++的方法，使用c数组等，能够减小动态开内存的时间开销。

## 实验总结

这个实验比起上一个来说，实现课件上的算法即可得到比较不错的结果了。希望大作业中能应用到这两次作业的成果。
