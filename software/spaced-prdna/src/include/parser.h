/*
====================================================================
parser.h: Fasta parser
author: Robin Martinjak
====================================================================
*/
#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

enum
{
    FASTA_OK,
    FASTA_EOF,
    FASTA_ENOMEM,
    FASTA_EINVAL,
    FASTA_EBADF,
    FASTA_ERROR
};


struct fasta_reader
{
    char *line;
    long line_len;
    size_t line_sz, line_no;
    char *header, *comment, *seq;
    size_t header_len, comment_len, seq_len;
    size_t header_sz, comment_sz, seq_sz, seq_sz_hint;
};

/**
 * Parse sequences from a file into vectors.
 * @param stream File
 * @param taxa Sequence Headers
 * @param sequences Sequence Data
 * @return 
 */
int parse(FILE *stream, vector<string>& taxa, vector<string>& sequences);

/**
 * Write given distance matrix DMat to a file.
 * @param DMat Distance Matrix
 * @param taxa Sequence Headers
 * @param name File Name
 * @return 
 */
int write_dmat(double** DMat, vector<string>& taxa, string name);

/**
 * Write patterns to a file.
 * @param pattern Patterns
 * @param name File Name
 * @return 
 */
int write_pattern(vector<string>& pattern, string name);

/**
 * Read all patterns in a file, check if they are of correct format and parse them into the given vector.
 * @param filePath
 * @param patternVector
 */
void parse_patterns(string filePath, vector<string>& patternVector);

/**
 * @return Length of the longest sequence parsed.
 */
int getLongestSequenceLength();
#endif
