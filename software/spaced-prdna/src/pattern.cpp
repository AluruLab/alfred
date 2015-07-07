/*
====================================================================
pattern: Generates pattern
author: Chris-Andre Leimeister
====================================================================
 */

#include "include/pattern.h"

unsigned long long fac(unsigned long long n) {
    return (n == 1 || n == 0) ? 1 : fac(n - 1) * n;
}

void getAllPattern(vector<string>& pattern, int k, int l) {
    string s = "";

    for (int i = 0; i < l; i++) {
        s = s + '0';
    }

    for (int i = 0; i < k; i++) {
        s = s + '1';
    }
    do {
        //if (s.at(0) != '0' && s.at(s.size() - 1) != '0')
            pattern.push_back(s);
    } while (next_permutation(s.begin(), s.end()));
}

int isInArray(int rand, vector<int>& tmp) {
    for (int j = 0; j < tmp.size(); j++) {
        if (rand == tmp[j])
            return 1;
    }
    return 0;
}

void getRandomPattern(vector<string>& pattern, int k, int l, int n) {
    int rnd;
    srand(time(NULL));
    int i = 0;
    int j = 0;
    while (j < n) {
        vector<char> pat(k + l, 49);
        vector<int> tmp;
        i = 0;
        while (i < l) {
            rnd = rand() % (l + k);
            if (isInArray(rnd, tmp))
                continue;
            tmp.push_back(rnd);
            i++;
        }
        for (int p = 0; p < tmp.size(); p++) {
            pat[tmp[p]] = '0';
        }
        string s(pat.begin(), pat.end());
        if (!(find(pattern.begin(), pattern.end(), s) != pattern.end())) {
            pattern.push_back(s);
            j++;
        }
    }
}

void getRandomPatternVar(vector<string>& pattern, int weight, int length, int amount) {
    int l;
    int rnd;
    srand(time(NULL));
    int i = 0;
    int j = 0;
    int fails = 0;
    while (j < amount) {
        l = length == 0 ? 0 : rand() % length + 1;
        vector<char> pat(weight + l, 49);
        vector<int> tmp;
        i = 0;
        while (i < l) {
            rnd = rand() % (l + weight);
            if (isInArray(rnd, tmp))
                continue;
            tmp.push_back(rnd);
            i++;
        }
        for (int p = 0; p < tmp.size(); p++) {
            pat[tmp[p]] = '0';
        }
        string s(pat.begin(), pat.end());        
        if (!(find(pattern.begin(), pattern.end(), s) != pattern.end())) {
            pattern.push_back(s);
            j++;
            fails = 0;
        }
        fails++;
        if (fails >= 500) {
            cout << "Too many fails trying to generate " << amount << " variable patterns, stopping now." << endl;
            return;
        }
    }
}
