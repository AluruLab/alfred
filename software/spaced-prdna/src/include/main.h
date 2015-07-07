#ifndef MAIN_H
#define	MAIN_H

#include "hashmap.h"
#include "parser.h"
#include "pattern.h"
#include "distances.h"
#include "hash.h"
#include "main.h"
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <vector> 
#include <sstream>
#include <unistd.h>
#include <stdio.h>

/**
 * Parse user-given run parameters.
 * @param argc
 * @param argv
 */
void parseParameters(int argc, char** argv);

/**
 * Set patterns according to settings.
 */
void setPatterns(vector<string>& patternVector);

/**
 * Set DNA mode.
 * @param dna
 */
void setDNA(bool dna);

int getNumberOfThreads();

int getDistanceType();

void setPatternWeight(int weight);

void setPatternDontCare(int dontcare);

string getDistanceString();

#endif	/* MAIN_H */

