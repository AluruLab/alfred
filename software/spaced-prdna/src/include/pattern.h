/*
=====================================================================
pattern: Generates pattern
author: Chris-Andre Leimeister
====================================================================
*/
#ifndef PATTERN_H
#define PATTERN_H

#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;
unsigned long long fac(unsigned long long n);
void getAllPattern(vector<string>& pattern, int k, int l);
void getRandomPattern(vector<string>& pattern, int k, int l, int n);
void getRandomPatternVar(vector<string>& pattern, int weight, int length, int amount);


#endif
