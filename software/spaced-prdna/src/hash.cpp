/*
=====================================================================
hash.cpp: Maps a spaced k-mer to a hash value.
author: Chris-Andre Leimeister
====================================================================
 */

#include "include/hash.h"

vector<int> faultyWindowsVector;

unsigned long long rol(int k, unsigned long long h) { // rotate on left
    // ASSEMBLER
    __asm__("rol %b1, %0"
            : "+r" (h)
            : "Jc"(k)
            );
    return h;
}

/** Hash calculation of (spaced) k-mers. */
int calcHash(hashmap* m, char* seq, int n, vector<int>& pos) {
    unsigned long long h;
    int faultyWindows = 0; 
    for (int i = 0; i < n - pos.back(); i++) {
        h = 0;        
        bool validWindow = true;
        for (int j = 0; j < pos.size(); j++) {
            if (seq[i + pos[j]] == 'N') {
                validWindow = false;
                break;
            }
            h ^= rol(pos[j], hash_table[seq[i + pos[j]]]);            
        }
        if (validWindow)
            m->add(h);
        else
            faultyWindows++;        
    }
    return faultyWindows;
}

void* threadFunc(void* param) {
    thread_param *tp = (thread_param*) param;
    char* str;
    int n = 13;
    hashmap *m;
    int bits = tp->bits;
    for (int i = tp->i; i <= tp->to; i++) {
        vector<hashmap*>* maps = (*tp).maps;
        if (bits > n)
            n = bits;        
        m = new hashmap(n + 1);
        str = new char[tp->sequences.at(i).length() + 1];
        strcpy(str, tp->sequences.at(i).c_str());
        int faultyWindows = calcHash(m, str, tp->sequences.at(i).length(), tp->pos);
        faultyWindowsVector[i] = faultyWindows;
        (*maps)[i] = m;
        delete [] str;
    }
    pthread_exit(NULL);
}

void getHashMap(vector<hashmap*>* maps, vector<string>& sequences, vector<int>& pos, int bits) {
    faultyWindowsVector.clear();
    for (int i = 0; i < sequences.size(); i++)
        faultyWindowsVector.push_back(0);
       
    vector<pthread_t> threads(getNumberOfThreads(), pthread_t());
    int workload = maps->size() / getNumberOfThreads();
    int count = 0;
    vector<thread_param> tp_struct(maps->size(), thread_param());
    for (int i = 0; i < getNumberOfThreads(); i++) {
        tp_struct[i].maps = maps;
        tp_struct[i].sequences = sequences;
        tp_struct[i].pos = pos;
        tp_struct[i].i = i * workload;
        tp_struct[i].to = (i == getNumberOfThreads() - 1) ? maps->size() - 1 : tp_struct[i].i + workload - 1; // make sure that the last thread finishes up
        tp_struct[i].bits = bits;
        pthread_create(&threads[i], NULL, threadFunc, (void*) &tp_struct[i]);
        count++;
    }
    for (int i = 0; i < count; i++) {        
        pthread_join(threads[i], NULL);        
    }
    
    // uncomment for no thread variant, comment out the code above in that case
//    int n = 13;
//    char* str;
//    hashmap *m;
//    for (int i = 0; i < sequences.size(); i++) {
//        if (bits > n)
//            n = bits;        
//        m = new hashmap(n + 1);        
//        str = new char[sequences.at(i).length() + 1];        
//        strcpy(str, sequences.at(i).c_str());        
//        int faultyWindows = calcHash(m, str, sequences.at(i).length(), pos);        
//        faultyWindowsVector[i] = faultyWindows;        
//        (*maps)[i] = m;
//        delete[] str;
//    }
}

vector<int> getFaultyWindowsVector() {
    return faultyWindowsVector;
}
