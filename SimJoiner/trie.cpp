#include "trie.h"

Trie::Trie()
{
    id = -1;
}

Trie* Trie::next(int ch)
{
    if (!ps[ch]) {
    	ps[ch] = newTrie();
	}
    return ps[ch];
}
