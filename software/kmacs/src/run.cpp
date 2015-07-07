#include "parser.h"
#include "kmacs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
	int c;
	int rmq =0;
	while ((c = getopt (argc, argv, "r:")) != -1)
		switch (c){
		case 'r':
	        rmq = 1;
        	break;
		if (isprint (optopt))
                	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             	else
               		fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
		return 1;
		default:
		abort ();
	}
    	FILE *stream;
	vector<string> taxa;
	vector<string> sequences;

    if (argc < 3) {
        fprintf(stderr, "usage: %s file.fasta k\n", argv[0]);
        return EXIT_FAILURE;
    }
	stream = fopen(argv[1], "r");
    if (!stream) {
        perror("can't open input file");
	fprintf(stderr, "usage: %s file.fasta k\n", argv[0]);
		return EXIT_FAILURE;
    }
	int k = atoi(argv[2]);
	double time=0.0;
	double start =clock();
	cout << "parsing" << endl;
	parse(stream, taxa, sequences);
	cout << "done" << endl;

	double** dmat = new double*[sequences.size()];
		for(int i=0;i<sequences.size();i++){
			dmat[i] = new double[sequences.size()];
	}
	
	calcDmat(dmat,  sequences, k, rmq);
	writeDmat(dmat, taxa, argc > 3 ? argv[3] : 0);
	
	time += clock() - start;		
	time = time/CLOCKS_PER_SEC;
	cout << "Computation time: " << time << endl;
	cout << "Distane matrix saved as " << (argc > 3 ? argv[3] : "DMat") << endl;

	for(int i=0;i<sequences.size();i++){
		delete [] dmat[i];
	}
	delete [] dmat;
	return EXIT_SUCCESS;
}
