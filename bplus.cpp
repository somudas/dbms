#include <iostream>
#include <vector>
#include <cstring> 
#include <algorithm>
#include <queue>
using namespace std;

struct node {
	int n;
	int* keys;
	node** ptrs;
	node* parent;
	int size;
	bool isLeaf;

	node(int n, bool s = false) {
		this->n = n;
		keys = new int[n + 2];
		ptrs = new node * [n + 3];
		for (int i = 0; i < n + 3; i++) ptrs[i] = nullptr;
		size = 0;
		isLeaf = s;
		parent = nullptr;
	}
	bool isFull() { return size == n; }
	void insert(int k) {
		int ptr = 0;
		while (ptr < size and keys[ptr] < k) ptr++;
		size++;
		for (int i = size - 1; i > ptr; i--) {
			keys[i] = keys[i - 1];
			ptrs[i + 1] = ptrs[i];
		}
		keys[ptr] = k;
	}
	void print() {
	    for(int i=0;i<size;i++) cout << keys[i] << " "; cout << "\n";
	}
    void printp() {
	    for(int i=0;i<=size;i++) cout << ptrs[i] << " "; cout << "\n";
	}
};



struct btree {
private:
	struct status {
		node* split;
		int key;
	};
	node* root;
	int n;

	bool find(node* root, int key) {
		int k = root->size;
		int* arr = root->keys;
		int idx = lower_bound(arr, arr + k, key) - arr;

		if (root->isLeaf) {
			if (idx == (n - 1)) return false;
			return arr[idx] == key;
		}

		return find(root->ptrs[idx], key);
	}


	status _insert(node* root, node* par, int key) {
		if (root->isLeaf) {
			if (!root->isFull()) {
				root->insert(key);
				return { nullptr, key };
			}
			node* next = new node(n, true);
			next->ptrs[n] = root->ptrs[n];
			root->ptrs[n] = next;
			int j = (n + 1 + ((n + 1) & 1)) / 2;
			vector<int> temp;
			for (int i = 0; i < n; i++) temp.push_back(root->keys[i]);
			temp.push_back(key);
			sort(temp.begin(), temp.end());
			root->size = j;
			for (int i = 0; i < j; i++) root->keys[i] = temp[i];
			for (int i = j; i < temp.size(); i++) next->keys[i - j] = temp[i];
			next->size = n - j + 1;
			return { next, temp[j - 1] };
		}
		int k = root->size;
		int* arr = root->keys;
		int idx = lower_bound(arr, arr + k, key) - arr;
		status stat = _insert(root->ptrs[idx], root, key);        
		if (stat.split) {
		    
			root->insert(stat.key);
			idx = upper_bound(arr, arr + root->size, stat.key) - arr;
			root->ptrs[idx] = stat.split;
			if (root->size <= n) {
				return { nullptr, key };
			}

			int j = (n + 1) / 2;
			node* next = new node(n);
			for (int i = j + 1; i < root->size; i++) {
				next->keys[i - j - 1] = root->keys[i];
				next->ptrs[i - j - 1] = root->ptrs[i];
			}
			next->ptrs[root->size - j - 1] = root->ptrs[root->size];
			next->size = n - j;
			root->size = j;
			return { next, root->keys[j] };
		}
		return { nullptr, key };
	}

public:
	btree(int n) {
		root = new node(n, true);
		this->n = n;
	}

	bool find(int key) {
		return find(root, key);
	}
    
	void print() {
        queue<pair<node*, int>> q;
        q.push({root, 0});
        while(q.size() > 0) {
            auto p = q.front(); q.pop();
            node* r = p.first; int l = p.second;
            cout << l << " | ";
            r->print();
            if(r->isLeaf) continue;
            for(int i=0;i<=r->size;i++) q.push({r->ptrs[i], l+1});
        }
	}

	void insert(int key) {
		if (find(key))
			return;
		status stat = _insert(root, nullptr, key);
		if (stat.split) {
			node* par = new node(n);
			par->insert(stat.key);
			par->ptrs[0] = root;
			par->ptrs[1] = stat.split;
			this->root = par;
		}
	}
};

int main(void) {
	btree bt(2);
	int arr[] = { 20, 11, 14, 25, 30, 12, 22, 23, 24 };
	for (int i : arr) {
        cout << "inserting " << i << "\n";
		bt.insert(i);
		bt.print();
		cout << "------------------\n";
	}
	return 0;
}