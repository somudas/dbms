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
    void erase(int key) {
        int ptr = 0;
        while (ptr < size and keys[ptr] != key) ptr++;
        size--;
        while (ptr < size) {
            keys[ptr] = keys[ptr + 1];
            ptrs[ptr] = ptrs[ptr + 1];
            ptr++;
        }
        ptrs[size] = ptrs[size + 1];
    }
    void print() {
        for (int i = 0; i < size; i++) cout << keys[i] << " "; cout << "\n";
    }
    void printp() {
        for (int i = 0; i <= size; i++) cout << ptrs[i] << " "; cout << "\n";
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

    status _delete(node* root, node* par, int index, int key, bool isLeaf) {
        root->erase(key);
        node* left = (index > 0) ? par->ptrs[index - 1] : nullptr;
        node* right = (index < par->size) ? par->ptrs[index + 1] : nullptr;
        if (left and left->size > (n + 1) / 2) {
            int rightMostValue = left->keys[left->size - 1];
            left->ptrs[left->size - 1] = root;
            left->size--;
            root->insert(rightMostValue);
            par->keys[index - 1] = left->keys[left->size - 1];
            return { nullptr, key };
        }
        else if (right and right->size > (n + 1) / 2) {
            int leftMostValue = right->keys[0];
            right->erase(leftMostValue);
            root->insert(leftMostValue);
            root->ptrs[root->size] = right;
            return { nullptr, key };
        }
        else if (left) {
            for (int i = 0; i < root->size; i++) left->insert(root->keys[i]);
            left->ptrs[left->size] = root->ptrs[root->size];
            delete root;
            par->ptrs[index] = left;
            return { left, par->keys[index - 1] };
        }
        else if (right) {
            for (int i = 0; i < root->size; i++) right->insert(root->keys[i]);
            delete root;
            return { right, par->keys[index] };
        }
    }

    status _delete2(node* root, node* par, int index, int key) {
        root->erase(key);
        if (root->size >= (n + 1) / 2) {
            return { nullptr, key };
        }
        node* left = (index > 0) ? par->ptrs[index - 1] : nullptr;
        node* right = (index < par->size) ? par->ptrs[index + 1] : nullptr;
        if (left and left->size > (n + 1) / 2) {
            int rightMostValue = left->keys[left->size - 1];
            node* leftDonatePtr = left->ptrs[left->size];
            swap(par->keys[index - 1], rightMostValue);
            left->size--;
            root->insert(rightMostValue);
            root->ptrs[0] = leftDonatePtr;
            return { nullptr, key };
        }
        else if (right and right->size > (n + 1) / 2) {
            int leftMostValue = right->keys[0];
            node* donatePtr = right->ptrs[0];
            right->erase(leftMostValue);
            right->size--;
            swap(par->keys[index], leftMostValue);
            root->insert(leftMostValue);
            root->ptrs[root->size] = donatePtr;
            return { nullptr, key };
        }
        else if (left) {
            left->insert(par->keys[index - 1]);
            left->ptrs[left->size] = root->ptrs[0];
            for (int i = 0; i < root->size; i++) {
                left->insert(root->keys[i]);
                left->ptrs[left->size] = root->ptrs[i+1];
            }
            par->ptrs[index] = left;
            delete root;
            return { left, par->keys[index - 1] };
        }
        else if (right) {
            root->insert(par->keys[index]);
            root->ptrs[root->size] = right->ptrs[0];
            for (int i = 0; i < right->size; i++) {
                root->insert(right->keys[i]);
                root->ptrs[root->size] = right->ptrs[i + 1];
            }
            par->ptrs[index+1] = root;
            delete right;
            return { root, par->keys[index] };
        }
    }

    status _erase(node* root, node* par, int index, int key) {
        if (root->isLeaf) {
            if (root->size > (n + 1) / 2 or par == nullptr) {
                root->erase(key);
                return { nullptr, key };
            }
            return _delete(root, par, index, key, true);
        }
        int k = root->size;
        int* arr = root->keys;
        int idx = lower_bound(arr, arr + k, key) - arr;
        status stat = _erase(root->ptrs[idx], root, idx, key);
        if (!stat.split) return { nullptr, key };
        if (root == this->root) {
            root->erase(stat.key);
            if (root->size == 0) {
                delete root;
                this->root = stat.split;
            }
            return { nullptr, key };
        }
        return _delete2(root, par, index, stat.key);
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
        q.push({ root, 0 });
        while (q.size() > 0) {
            auto p = q.front(); q.pop();
            node* r = p.first; int l = p.second;
            cout << l << " | " << r << " | ";
            r->print();
            if (r->isLeaf) continue;
            for (int i = 0; i <= r->size; i++) q.push({ r->ptrs[i], l + 1 });
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
    bool erase(int key) {
        if (!find(key)) return false; // key not found
        _erase(root, nullptr, -1, key);
        return true;
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
    int del[] = { 20, 22, 24, 14, 12, 23 };
    for (int i : del) {
        cout << "deleting " << i << "\n";
        bt.erase(i);
        bt.print();
        cout << "------------------\n";
    }
    return 0;
}
