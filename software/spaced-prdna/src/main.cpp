/*
 * ====================================================================
 * Authors: Chris-Andre Leimeister, Sebastian Lindner
 * ====================================================================
 */

#include <stdio.h>
#include <limits.h>
#include "main.h"

using namespace std;

bool saveSingleMatrices = false,
            combinedMatrix = true,
            variablePatternLengths = true,
        quiet = false, isDNA = true;
int patterns_amount = 10, patterns_dontcare = -1, patterns_weight = -1, threads = -1, distanceType = 0;
std::string patterns_filepath = "",
        output_matrix_filepath = "",
        output_patterns_filepath = "",
        help = "Alignment-free sequence comparison tool using spaced k-mer frequencies"
    "\nUsage: ./spaced [options] <sequence file>"
    "\nOptions:"        
    "\n\t -h: print this help and exit"
    "\n\t -f <file>: specify file containing patterns that should be used"
    "\n\t -p <file>: specify file in which used patterns will be saved afterwards"
    "\n\t -o <file>: specify the output file name (default: <sequence file> + .dmat)"
    "\n\t -k <integer>: specify pattern weight"
    "\n\t -l <integer>: specify pattern don't care positions"
    "\n\t -n <integer>: specify amount of patterns you want to generate"    
    "\n\t -t <integer>: specify amount of threads that should be used (default: number of sequences, fastest)"
    "\n\t -d EU | JS | MS: change distance type to Euclidean, Jensen-Shannon, Morgenstern (default: Euclidean)"
    "\n\t -s: save single matrices as well as the combined matrix"
    "\n\t -v: fixed pattern lengths (default: variable)"    
    "\n\t -quiet: no status messages (errors and progress will still be printed)"        
    "\n";

int main(int argc, char **argv) {
    double time=0.0;
    double start =clock();
    // parse run parameters
    parseParameters(argc, argv);    
    FILE *stream;
    stream = fopen(argv[argc - 1], "r");
    if (!stream) {
        perror("Can't open input file");
        return EXIT_FAILURE;
    }
    string seqFileName(argv[argc - 1]);
    if (seqFileName.find_last_of('/') != -1) // -1 indicates no match
        seqFileName = seqFileName.substr(seqFileName.find_last_of('/') + 1, seqFileName.length() - 1);
    // parse sequence file    
    if (!quiet) {
        cout << "Parsing \"" << argv[argc - 1] << "\"... ";
        cout.flush();
    }
    vector<string> taxa, sequences;
    parse(stream, taxa, sequences);
    // error parsing?
    if (taxa.size() == 0) {
        cerr << "something went wrong!\nPlease make sure your sequences are in correct FASTA format (semicolons are not supported, use '>')." << endl;
        exit(-1);
    }
    // sequences too short?
    if (getLongestSequenceLength() < 4) {
        cerr << "sequences too short!" << endl;
        exit(-1);
    }    
    fclose(stream);
    // default number of threads
    if (threads == -1 || threads > taxa.size())
        threads = taxa.size();
    // default weight
    if (patterns_weight == -1)
        patterns_weight = isDNA ? (int) (log(getLongestSequenceLength()) / log(4) + 1) : 4;
    if (patterns_dontcare == -1)
        patterns_dontcare = patterns_weight;
    if (!quiet) {
        string type = isDNA ? "DNA" : "Protein";
        cout << taxa.size() << " sequences of type " << type << " read." << endl;
        cout << "Distance type: " << getDistanceString() << endl;
    }        
    // set patterns
    vector<string> patternVector;
    setPatterns(patternVector);
    // initialize matrices
    double** distanceMatrix = new double*[sequences.size()]();
    double** distanceMatrix_tmp = new double*[sequences.size()]();
    for (int i = 0; i < sequences.size(); i++) {
        distanceMatrix[i] = new double[sequences.size()]();
        distanceMatrix_tmp[i] = new double[sequences.size()]();
    }        
    // bits in hash table
    int bits = (int) (log(getLongestSequenceLength()) / log(2)); 
    // positions of care positions (1's) in each pattern (rewritten for every pattern)
    vector<int> carePositionsVector;    
    // for every pattern    
    for (int i = 0; i < patternVector.size(); i++) {
        cout << "\r" << "Executing (" << i + 1 << "/" << patternVector.size() << ")... ";
        cout.flush();
        // set care positions
        for (int j = 0; j < patternVector.at(i).length(); j++) {
            if (patternVector.at(i)[j] != '0') {
                carePositionsVector.push_back(j);
            }
        }
        // frequency vector of found words as hash table
        vector<hashmap*> *maps = new vector<hashmap*>(sequences.size(), 0);
        // find words
        getHashMap(maps, sequences, carePositionsVector, bits);
        // compute distance matrix from frequency vector
        computeDist(maps, distanceMatrix_tmp, sequences, carePositionsVector.back());
        // save single matrices?
        if (saveSingleMatrices) {
            // convert integer to string
            stringstream ss;
            ss << i;
            // save matrix to file
            write_dmat(distanceMatrix_tmp, taxa, ss.str());
            cout << "Saved matrix to " << ss.str() << endl;
        }
        // combined matrix?
        if (combinedMatrix) {
            for (int i = 0; i < sequences.size(); i++) {
                for (int j = 0; j < sequences.size(); j++) {         
                    // add temporary fields to final matrix
                    distanceMatrix[j][i] += distanceMatrix_tmp[j][i]; 
                }
            }
        }
        // reset position vector
        carePositionsVector.clear();
        // delete frequency vectors
        for (int i = 0; i < sequences.size(); i++)
            delete (*maps)[i];
        delete maps;
    }
    cout << "done." << endl;
    
    // combined matrix - continued
    if (combinedMatrix) {
        // find average
        if (distanceType != 2) { // unless it's Morgenstern distance
            for (int i = 0; i < sequences.size(); i++) {
                for (int j = 0; j < sequences.size(); j++) {
                    distanceMatrix[j][i] /= patternVector.size();
                }
            }
        } else { // Morgenstern distance computation
            morgenstern_finalize(distanceMatrix, sequences, (double) patterns_weight, (double) patternVector.size(), (double) (patterns_weight + patterns_dontcare));
        }
    }
    // write to file
    if (output_matrix_filepath == "") { // default file name if it was not set by user        
        output_matrix_filepath = seqFileName + ".dmat";            
    } 
    write_dmat(distanceMatrix, taxa, output_matrix_filepath);
    if (!quiet)
        cout << "Saved distance matrix to \"" << output_matrix_filepath << "\". " << endl;
    // write patterns to file
    if (output_patterns_filepath != "") {
        write_pattern(patternVector, output_patterns_filepath);
        if (!quiet)
            cout << "Saved patterns to \"" << output_patterns_filepath << "\"." << endl;
    }
    // tidy up
    for (int i = 0; i < sequences.size(); i++) {
        delete [] distanceMatrix[i];
        delete [] distanceMatrix_tmp[i];
    }
    delete distanceMatrix;
    delete distanceMatrix_tmp;
    time += clock() - start;		
    time = time/CLOCKS_PER_SEC;
    std::cout << "Computation time: " << time << std::endl;
    return 0;
}

void setPatterns(vector<string>& patternVector) {
     // parse from file        
    if (patterns_filepath != "") {        
        parse_patterns(patterns_filepath, patternVector);
        if (!quiet)
            cout << patternVector.size() << " patterns read from file." << endl;
    } 
    // auto-generate with variable length
    else if (variablePatternLengths) {
        getRandomPatternVar(patternVector, patterns_weight, patterns_dontcare, patterns_amount);
        if (!quiet)
            cout << patternVector.size() << " patterns of variable length generated with weight k=" << patterns_weight << " and maximum don't care positions l=" << patterns_dontcare << "." << endl;
    } 
    // auto-generate with fixed length ...
    else {        
        // ... if the user wants fewer patterns than could be theoretically generated
        if (patterns_weight + patterns_dontcare - 2 > 20 // would result in overflow
                || patterns_amount < fac(patterns_weight + patterns_dontcare - 2) / (fac(patterns_weight - 2) * fac(patterns_dontcare)))
            getRandomPattern(patternVector, patterns_weight, patterns_dontcare, patterns_amount);
        // ... otherwise use this method
        else
            getAllPattern(patternVector, patterns_weight, patterns_dontcare);        
        // we can still have too many patterns now, if so, randomly pick patterns until we have the desired amount (ineffective, shouldn't happen too often)
        if (patterns_amount < patternVector.size()) { 
            int min = 0, max = patternVector.size();
            vector<string> tempVec;
            for (int i = 0; i < patterns_amount; i++) {
                string pattern = patternVector.at(min + (rand() % (int)(max - min + 1))); // random int between 0 and patternVector.size()
                if (std::find(patternVector.begin(), patternVector.end(), pattern) == patternVector.end()) {
                    i--;
                    continue;
                } else
                    tempVec.push_back(pattern);                
            }
            patternVector.clear();
            patternVector = tempVec;            
        }
        if (!quiet)
            cout << patternVector.size() << " patterns generated with weight k=" << patterns_weight << " and don't care positions l=" << patterns_dontcare << "." << endl;
    }
}

void parseParameters(int argc, char** argv) {
    // too few?
    if (argc < 2) {
        cout << help << endl; // print help
        exit(0); // and exit
    }
    // only parameter -h?
    if (argc == 2)
        if (strcmp("-h", argv[1]) == 0) {
            cout << help << endl;
            exit(0);
        }
    for (int i = 1; i < argc - 1; i++) { // first argument is run dir, last should be sequence file
        // help & exit
        if (strcmp("-h", argv[i]) == 0) {
            cout << help << endl;
            exit(0);
        }
        // patterns that should be used
        else if (strcmp("-f", argv[i]) == 0) {
            patterns_filepath = argv[++i];
            if (i == argc - 1) {
                cerr << "Flag -f needed an argument!" << endl;
                exit(-1);
            }
        }
        // output patterns file
        else if (strcmp("-p", argv[i]) == 0) {
            output_patterns_filepath = argv[++i];
            if (i == argc - 1) {
                cerr << "Flag -p needed an argument!" << endl;
                exit(-1);
            }
        }
        // output matrix file
        else if (strcmp("-o", argv[i]) == 0) {
            output_matrix_filepath = argv[++i];
            if (i == argc - 1) {
                cerr << "Flag -o needed an argument!" << endl;
                exit(-1);
            }            
        }
        // auto-generate pattern weight
        else if (strcmp("-k", argv[i]) == 0) {
            patterns_weight = atoi(argv[++i]);
            if (i == argc - 1) {
                cerr << "Flag -k needed an argument!" << endl;
                exit(-1);
            }                
        }
        // auto-generate pattern length
        else if (strcmp("-l", argv[i]) == 0) {
            patterns_dontcare = atoi(argv[++i]);
            if (i == argc - 1) {
                cerr << "Flag -l needed an argument!" << endl;
                exit(-1);
            }
        }
        // number of combined matrices
        else if (strcmp("-n", argv[i]) == 0) {
            patterns_amount = atoi(argv[++i]);
            if (i == argc - 1) {
                cerr << "Flag -n needed an argument!" << endl;
                exit(-1);
            }            
        }
        // number of threads
        else if (strcmp("-t", argv[i]) == 0) {
            threads = atoi(argv[++i]);
            if (i == argc - 1) {
                cerr << "Flag -t needed an argument!" << endl;
                exit(-1);
            }            
        }
        // single pattern
        else if (strcmp("-s", argv[i]) == 0)
            saveSingleMatrices = true;
        // variable pattern length
        else if (strcmp("-v", argv[i]) == 0)
            variablePatternLengths = false;
        // no status messages
        else if (strcmp("-quiet", argv[i]) == 0)
            quiet = true;
        // distance type
        else if (strcmp("-d", argv[i]) == 0) {
            i++;
            if (i == argc - 1) {
                cerr << "Flag -d needed an argument!" << endl;
                exit(-1);
            }   
            if (strcmp("eu", argv[i]) == 0 || strcmp("EU", argv[i]) == 0) {
                distanceType = 0;
            } else if (strcmp("js", argv[i]) == 0 || strcmp("JS", argv[i]) == 0) {
                distanceType = 1;            
            } else if (strcmp("ms", argv[i]) == 0 || strcmp("MS", argv[i]) == 0) {
                distanceType = 2;
            }
        }
        // unknown flag
        else {            
            cout << help << endl;
            cerr << "Unknown flag: " << argv[i] << endl;
            exit(-1);
        }
    }
}

void setDNA(bool dna) {
    isDNA = dna;
}

int getNumberOfThreads() {
    return threads;
}

int getDistanceType() {
    return distanceType;
}

string getDistanceString() {
    switch (distanceType) {
        case 0: return "Euclidean";
        case 1: return "Jensen-Shannon";
        case 2: return "Morgenstern";
    }
}

void setPatternWeight(int weight) {
    patterns_weight = weight;
}

void setPatternDontCare(int dontcare) {
    patterns_dontcare = dontcare;
}
