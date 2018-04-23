#ifndef __EXP2_SIMJOINER_TRIE_H__
#define __EXP2_SIMJOINER_TRIE_H__

const int MAX_NODE = 1 << 20;
const int N_ASCII = 128;

class Trie {
public:
    Trie* ps[N_ASCII];
    int id;
    Trie();
    Trie* next(int ch);
};

Trie* newTrie();

#endif
