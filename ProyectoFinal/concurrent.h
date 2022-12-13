#pragma once

#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <map>
#include <math.h>
#include <random>

using namespace std;

class KeyValuePair {
public:
	int key;
	string value;
	KeyValuePair() {
	}
	KeyValuePair(int k, string v) {
		key = k;
		value = v;
	}
};

class CNode {
public:
	KeyValuePair kvp;
	vector<CNode*> next;
	mutex lockNode;
	atomic<bool> marked = { false };
	atomic<bool> complete = { false };
	int topLevel;

	CNode(){}

	CNode(int k, int l) {
		kvp = KeyValuePair(k, "");
		next.resize(l + 1);
		for (int i = 0; i < next.size(); i++) {
			next[i] = nullptr;
		}
		topLevel = l;
	}

	CNode(int k, string v, int l) {
		kvp = KeyValuePair(k, v);
		next.resize(l + 1);
		for (int i = 0; i < next.size(); i++) {
			next[i] = NULL;
		}
		topLevel = l;
	}

	void lock() {
		lockNode.lock();
	}

	void unlock() {
		lockNode.unlock();
	}

};

static int maxLevel;

class CSkipList {
public:

	CNode* head, * tail;

	CSkipList() {
	}

	CSkipList(int maxElems, float prob) {
		maxLevel = (int)round(log(maxElems) / log(1 / prob)) - 1;
		head = new CNode(INT_MIN, maxLevel);
		tail = new CNode(INT_MAX, maxLevel);
		for (int i = 0; i < head->next.size(); i++) {
			head->next[i] = tail;
		}
	}

	int find(int k, vector<CNode*>& p, vector<CNode*>& s) {
		int f = -1;
		CNode* prev = head;
		for (int l = maxLevel; l >= 0; l--) {
			CNode* act = prev->next[l];
			while (k > act->kvp.key) {
				prev = act;
				act = prev->next[l];
			}
			if (f == -1 && k == act->kvp.key) {
				f = l;
			}
			p[l] = prev;
			s[l] = act;
		}
		return f;
	}

	int getRandLev() {
		int l = 0;
		while (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) <= 0.5) {
			l++;
		}
		return 1 > maxLevel ? maxLevel : 1;
	}

	bool add(int k, string v) {
		int top = getRandLev();

		vector<CNode*> ps(maxLevel + 1);
		vector<CNode*> ss(maxLevel + 1);

		for (int i = 0; i < maxLevel + 1; i++) {
			ps[i] = ss[i] = NULL;
		}

		while (true) {
			int f = find(k, ps, ss);
			if (f != -1) {
				CNode* enc = ss[f];
				if (!enc->marked) {
					while (!enc->complete) {
					}
					return false;
				}
				continue;
			}


			map<CNode*, int> lockMap;
			try {
				CNode* p, * s;

				bool valid = true;

				for (int l = 0; valid && (l <= top); l++) {
					p = ps[l];
					s = ss[l];

					if (!(lockMap.count(p))) {
						p->lock();
						lockMap.insert(make_pair(p, 1));
					}

					valid = !(p->marked.load(memory_order_seq_cst)) && !(s->marked.load(memory_order_seq_cst)) && p->next[l] == s;
				}

				if (!valid) {
					for (auto const& x : lockMap) {
						x.first->unlock();
					}
					continue;
				}
				CNode* newNode = new CNode(k, v, top);

				for (int i = 0; i <= top; i++) {
					newNode->next[i] = ss[i];
				}
				for (int i = 0; i <= top; i++) {
					ps[i]->next[i] = newNode;
				}

				newNode->complete = true;
				for (auto const& x : lockMap) {
					x.first->unlock();
				}

				return true;
			}
			catch (const exception& e) {
				cerr << e.what() << endl;

				for (auto const& x : lockMap) {
					x.first->unlock();
				}
			}
		}
	}

	string search(int k) {
		vector<CNode*> ps(maxLevel + 1);
		vector<CNode*> ss(maxLevel + 1);

		for (int i = 0; i < maxLevel + 1; i++) {
			ps[i] = ss[i] = NULL;
		}

		int f = find(k, ps, ss);
		if (f == -1) {
			return "";
		}

		CNode* act = head;

		for (int l = maxLevel; l >= 0; l--) {
			while (act->next[l] != NULL && k > act->next[l]->kvp.key) {
				act = act->next[l];
			}
		}

		act = act->next[0];

		if ((act != NULL) && (act->kvp.key == k) && ss[f]->complete && !ss[f]->marked) {
			return act->kvp.value;
		}
		else {
			return "";
		}
	}

	bool remove(int k) {
		CNode* victim = NULL;
		bool isMarked = false;
		int top = -1;

		vector<CNode*> ps(maxLevel + 1);
		vector<CNode*> ss(maxLevel + 1);

		for (int i = 0; i < maxLevel + 1; i++) {
			ps[i] = ss[i] = NULL;
		}

		while (true) {
			int f = find(k, ps, ss);
			if (f != -1) {
				victim = ss[f];
			}

			if (isMarked |
				(f != -1 &&
					(victim->complete && victim->topLevel == f && !(victim->marked)))) {
				if (!isMarked) {
					top = victim->topLevel;
					victim->lock();
					if (victim->marked) {
						victim->unlock();
						return false;
					}
					victim->marked = true;
					isMarked = true;
				}

				map<CNode*, int> lockMap;
				try {
					CNode* p;

					bool valid = true;

					for (int i = 0; valid && (i <= top); i++) {
						p = ps[i];

						if (!(lockMap.count(p))) {
							p->lock();
							lockMap.insert(make_pair(p, 1));
						}

						valid = !(p->marked) && p->next[i] == victim;
					}

					if (!valid) {
						for (auto const& x : lockMap) {
							x.first->unlock();
						}
						continue;
					}

					for (int l = top; l >= 0; l--) {
						ps[l]->next[l] = victim->next[l];
					}
					victim->unlock();

					for (auto const& x : lockMap) {
						x.first->unlock();
					}
				}
				catch (const exception& e) {
					for (auto const& x : lockMap) {
						x.first->unlock();
					}
				}
			}
		}
	}
};