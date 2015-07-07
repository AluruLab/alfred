/*
====================================================================
parser.cpp: Fasta Parser
author: Sebastian Lindner
====================================================================
 */
#include "parser.h"
#include "main.h"

int longestSeqLength = 0;

int parse(FILE *stream, vector<string>& taxa, vector<string>& sequences){
	vector<char> seqBuffer;
	vector<char> taxaBuffer;
	char c=getc(stream);
	bool isDNA = true;
	while(c!=EOF){
		if(c=='>'){
			while((c=getc(stream))!='\n'){
				taxaBuffer.push_back(c);		
			}
		}
		else if(c==';')
			while((c=getc(stream))!='\n');
		else{
			while((c=getc(stream))!='>' && c!=EOF){
				if(!isalpha(c)){
                    			if(c>31 && c!=127 && c!=' ')
						if(!isDNA)
							c='X';
						else
							c='N';
                                                else
						continue;
				}
				c=toupper(c);
				if(c=='A'||c=='T'||c=='G'||c=='C'||c=='N'||c=='U')
					seqBuffer.push_back(c);	
				else{
					seqBuffer.push_back(c);	
					isDNA=false;
				}
			}	
		}
		if(seqBuffer.size()>0){
			string taxaString(taxaBuffer.begin(),taxaBuffer.end());
			string seqString(seqBuffer.begin(),seqBuffer.end());
			taxa.push_back(taxaString);
			sequences.push_back(seqString);
			seqBuffer.clear();
			taxaBuffer.clear();
			if(seqString.length()>longestSeqLength)
				longestSeqLength=seqString.length();
		}
	}
        if (!isDNA)
		setDNA(false);
	return FASTA_OK;
}

int write_dmat(double** DMat, vector<string>& taxa, string name) {
    ofstream outfile;
    outfile.open(name.c_str());
    outfile << taxa.size() << endl;
    for (int i = 0; i < taxa.size(); i++) {
        outfile << left << setw(11) << taxa.at(i).substr(0, 10);
        for (int j = 0; j < taxa.size(); j++) {
            if (i != j) {
                outfile << setprecision(12) << DMat[j][i] << "  ";
            } else
                outfile << setprecision(12) << "0" << "  ";
        }
        outfile << endl;
    }
    outfile.close();
    return FASTA_OK;
}

int write_pattern(vector<string>& pattern, string name) {
    ofstream outfile;
    outfile.open(name.c_str());
    for (int i = 0; i < pattern.size(); i++) {
        outfile << pattern[i] << endl;
    }
    outfile.close();
    return FASTA_OK;
}

void parse_patterns(string filePath, vector<string>& patternVector) {
    string line = "";
    char character;
    ifstream file(filePath.c_str());
    int weight = 0, weightCounter = 0, dontcare = 0, dontcareCounter = 0;
    if (file.is_open()) {
        while (file >> noskipws >> character) {
            if (character == '1') {
                line += character;
                weightCounter++;
            } else if (character == '0') {
                line += character;
                dontcareCounter++;
            } else if (line.size() > 0) {
                patternVector.push_back(line);
                line = "";
                if (weightCounter > weight)
                    weight = weightCounter;
                weightCounter = 0;
                if (dontcareCounter > dontcare)
                    dontcare = dontcareCounter;
                dontcareCounter = 0;
            }
        }
    } else {
        cerr << "Could not open pattern file." << endl;
        exit (-1);
    }
    setPatternWeight(weight);
    setPatternDontCare(dontcare);
}

int getLongestSequenceLength() {
    return longestSeqLength;
}
