#include <iostream>
#include <vector>
#include <assert.h>
using namespace std;

struct Bucket {
	int buckSize;
	int ptr;
	int* container;
	int level;
	Bucket(int size, int lvl = 1) {
		buckSize = size;
		container = (int*)malloc(sizeof(int) * buckSize);
		ptr = 0;
		level = lvl;
	}
	int find(int x) { for (int i = 0; i < ptr; i++) if (container[i] == x) return i; return buckSize; }
	vector<int> get() {
		vector<int> t;
		for (int i = 0; i < ptr; i++) t.push_back(container[i]); return t;
	}
	void erase(int x) {
		int idx = find(x);
		ptr--;
		for (int i = idx; i < buckSize - 1; i++) container[i] = container[i + 1];
	}
	bool isEmpty() { return ptr < buckSize; }
	void push(int x) {
		assert(ptr < buckSize);
		container[ptr++] = x;
	}
	void show() {
		printf("level: %d; ", level);
		if (ptr == 0) printf("(null)");
		for (int i = 0; i < ptr; i++) printf("%d ", container[i]);
		printf("\n");
	}
	~Bucket() {
		free(container);
	}
};

class Hash {
	vector<Bucket*> global_table;
	int global_lvl;
	int bits;
	int bucketSize;

	// util functions
	int hash(int x) {
		return x & ((1 << (bits)) - 1);
	}
	int hash(int x, int b) {
		return x & ((1 << (b)) - 1);
	}

	void bucket_split(int p, vector<Bucket*> dup = vector<Bucket*>(), bool f = 0) {
		vector<int> temp;
		int lvl = (global_table[p]->level) + 1;
		for (int i = 0; i < bucketSize; i++) temp.push_back(global_table[p]->container[i]);
		delete global_table[p];
		if (f) global_table = dup;

		int diff = global_lvl - lvl + 1;
		int suff = hash(temp[0], lvl - 1);
		Bucket* a = new Bucket(bucketSize, lvl);
		Bucket* b = new Bucket(bucketSize, lvl);
		for (int i = 0; i < (1 << diff); i++) {
			int x = (i << (lvl - 1)) | suff;
			if (i & 1) global_table[x] = a;
			else global_table[x] = b;
		}

		for (int element : temp) {
			global_table[hash(element)]->push(element);
		}
	}

	void directory_expansion(int p) {
		global_lvl++;
		bits++;
		vector<Bucket*> dup(global_table.size() * 2, nullptr);
		for (int i = 0; i < (1 << bits); i++) {
			dup[i] = global_table[hash(i, bits - 1)];
		}
		bucket_split(p, dup, true);

	}

	void shrink_table(int p) {
		merge(hash(p));
		bool shrink = true;
		for (int i = 0; i < global_table.size(); i++) {
			if (global_table[i]->level == global_lvl) shrink = false;
		}
		if (!shrink) return;
		global_lvl--; bits--;
		global_table.resize(global_table.size() / 2);
		shrink_table(p);
	}

public:
	Hash(int bs) {
		bucketSize = bs;
		global_lvl = 1;
		bits = 1;
		global_table.assign(2, nullptr);
		global_table[0] = new Bucket(bs);
		global_table[1] = new Bucket(bs);
	}

	bool find(int x) {
		return global_table[hash(x)]->find(x) != bucketSize;
	}
	void show() {
		printf("global depth: %d\n", global_lvl);
		for (int i = 0; i < global_table.size(); i++) {
			printf("%02d (%p) --> ", i, global_table[i]);
			global_table[i]->show();
		}
	}
	void insert(int x) {
		int p = hash(x);
		if (global_table[p]->isEmpty()) {
			global_table[p]->push(x);
			return;
		}
		// Bucket Split
		if (global_lvl > global_table[p]->level) {
			printf("<bucket split>\n");
			bucket_split(p);
			insert(x);
			return;
		}

		// Directory Expansion
		printf("<directory expansion>\n");
		directory_expansion(p);
		insert(x);
		return;
	}
	void merge(int p) {
		Bucket* a = global_table[p];
		Bucket* b = global_table[p ^ (1 << (bits - 1))];
		if ((a->ptr + b->ptr) <= bucketSize and a != b and a->level == b->level) {
			// bucket Merge
			vector<int> temp;
			for (int i : a->get()) temp.push_back(i);
			for (int i : b->get()) temp.push_back(i);
			int lvl = a->level - 1;
			delete a; delete b;
			global_table[p] = global_table[p ^ (1 << (bits - 1))] = new Bucket(bucketSize, lvl);
			for (int i : temp) global_table[p]->push(i);
		}
	}
	void erase(int x) {
		if (!find(x)) return;
		int p = hash(x);
		global_table[p]->erase(x);
		shrink_table(p);
	}
};


int main(void) {

	int arr[] = { 45, 56, 60, 72, 12, 31, 29, 57, 11 };
	Hash h(2);
	for (int i : arr) {
		printf("\ninserting %d:\n", i);
		h.insert(i);
		h.show();
	}
	int arr2[] = { 45, 56, 12, 31, 11 };
	for (int i : arr2) {
		printf("\ndeleting %d:\n", i);
		h.erase(i);
		h.show();
	}

	return 0;
}

