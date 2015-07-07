/*
=====================================================================
distances.h: Implementation of the euclidean distance and the Jensen Shannon divergence. 
author: Chris-Andre Leimeister
====================================================================
*/

#ifndef DISTANCES_H
#define DISTANCES_H

#include <iostream>
#include <vector>
#include <cmath>
#include <list> 
#include <pthread.h>
#include "hashmap.h"

using namespace std;

struct thread_param_dist{
	hashmap* m1;
	hashmap* m2;
	int n;
	int m;
	double** dmat;
	int i;
	int j;
};

void computeDist(vector<hashmap*> *maps, double** dmat, vector<string>& sequences, int k);

void morgenstern_finalize(double** distanceMatrix, vector<string>& sequences, double k, double m, double l);

#endif
