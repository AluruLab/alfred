/*
=====================================================================
hashmap.cpp: Implementation of a simple hashmap
author: Chris-Andre Leimeister
====================================================================
*/

#ifndef HASHMAP_H
#define HASHMAP_H

#include <utility>
#include <vector>
#include <list>
#include <iostream>
#include <cmath>
using namespace std;

class hashmap
{
public:                    
	hashmap(int n){map=new vector<pair<unsigned long long, int> > ((int)pow(2.0,(double)n),pair<unsigned long long, int>(0,0));this->n=n ;} 
        /**
         * @param key
         * @return the n most-significant bits
         */
	int getHash(unsigned long long key);
        
        /**
         * @param key
         * @return Value for given key
         */
	int getValue(unsigned long long key); 
        
	hashmap(const hashmap& a);  
        
	~hashmap(){delete map;} 

	void add(unsigned long long key);
        
        /** The Hashmap */
	vector<pair<unsigned long long, int> > *map;
        
        /** Number of bits */
	int n;
};
#endif
