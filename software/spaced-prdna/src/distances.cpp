/*
=====================================================================
distances.cpp: Implementation of the euclidean distance and the Jensen Shannon divergence. 
author: Chris-Andre Leimeister
====================================================================
 */
#include "distances.h"
#include "hash.h"

void* morgenstern(void* param) {
    thread_param_dist *tp = (thread_param_dist*) param;
    int occurence1, occurence2;
    double spacedWordMatches = 0;
    // Idea: count matching spaced words in both sequences
    for (int i = 0; i < tp->m1->map->size(); i++) {
        occurence1 = tp->m1->map->at(i).second;
        if (occurence1 == 0)        
            continue;
        occurence2 = tp->m2->getValue(tp->m1->map->at(i).first);
        spacedWordMatches += occurence1 * occurence2;        
    }
    tp->dmat[tp->i][tp->j] = spacedWordMatches;
    tp->dmat[tp->j][tp->i] = spacedWordMatches;
}

double jukesCantor(double p){
	return -0.75 * log(1.0-(4.0/3.0)*p);
}

double getValueUnderRoot(double N, int m, int L1, int L2, int k, int l, double Pmatch){
   	return 0.5*((N / (m * (L1 - l + 1)) - (L2 - l) * pow(Pmatch, k))+(N / (m * (L2 - l + 1)) - (L1 - l) * pow(Pmatch, k)));
}

double getPmatch(string s1, string s2){
	double A1=0; double C1=0; double G1=0; double T1=0;
	double A2=0; double C2=0; double G2=0; double T2=0;
	for(int i=0; i<s1.length();i++){
		if(s1[i]=='A')
			A1++;
		else if(s1[i]=='C')
			C1++;
		else if(s1[i]=='G')
			G1++;
		else if(s1[i]=='T')
			T1++;
	}
	for(int i=0; i<s2.length();i++){
		if(s2[i]=='A')
			A2++;
		else if(s2[i]=='C')
			C2++;
		else if(s2[i]=='G')
			G2++;
		else if(s2[i]=='T')
			T2++;
	}
	A1=A1/s1.length();C1=C1/s1.length();G1=G1/s1.length();T1=T1/s1.length();
	A2=A2/s2.length();C2=C2/s2.length();G2=G2/s2.length();T2=T2/s2.length();
	return A1*A2+C1*C2+G1*G2+T1*T2;
}

void morgenstern_finalize(double** distanceMatrix, vector<string>& sequences, double k, double m, double l) {
    cout << "Morgenstern distance computation... ";
    cout.flush();            

	for (int i = 0; i < sequences.size(); i++){
	        for (int j = i + 1; j < sequences.size(); j++) {       
			double Pmatch=getPmatch(sequences[i], sequences[j]);
			double valueUnderRoot = getValueUnderRoot(distanceMatrix[j][i], m, sequences[i].length(), sequences[j].length(), k, l, Pmatch);              
			while(valueUnderRoot<=0){
				distanceMatrix[j][i]++;
				valueUnderRoot = getValueUnderRoot(distanceMatrix[j][i], m, sequences[i].length(), sequences[j].length(), k, l, Pmatch);
			}
		
		double p = 1.0-pow(valueUnderRoot, (1.0 / k));
		while(p>=0.75){
				distanceMatrix[j][i]++;
				valueUnderRoot = getValueUnderRoot(distanceMatrix[j][i], m, sequences[i].length(), sequences[j].length(), k, l, Pmatch);
				p = 1.0-pow(valueUnderRoot, (1.0 / k));
			}
            distanceMatrix[j][i] = jukesCantor(p);
            distanceMatrix[i][j] = distanceMatrix[j][i];
	        }
	}	
    cout << "done." << endl;
}

void* jensenShannon(void* param) {
    thread_param_dist *tp = (thread_param_dist*) param;
    double tmpA, tmpB;
    double pm = 0;
    double qm = 0;
    for (int p = 0; p < tp->m1->map->size(); p++) {
        if (tp->m1->map->at(p).second != 0) {
            tmpA = (double) tp->m1->map->at(p).second / tp->n;
            tmpB = (double) tp->m2->getValue(tp->m1->map->at(p).first) / tp->m;
            pm = pm + log(tmpA / (0.5 * (tmpA + tmpB))) * tmpA;
            if (tmpB != 0)
                qm = qm + log(tmpB / (0.5 * (tmpA + tmpB))) * tmpB;
        }
    }
    for (int p = 0; p < tp->m2->map->size(); p++) {
        if (tp->m2->map->at(p).second != 0) {
            tmpA = (double) tp->m2->map->at(p).second / tp->m;
            tmpB = (double) tp->m1->getValue(tp->m2->map->at(p).first) / tp->n;
            if (tmpB == 0)
                qm = qm + log(tmpA / (0.5 * (tmpA + tmpB))) * tmpA;
        }
    }
    tp->dmat[tp->j][tp->i] = (0.5 * qm + 0.5 * pm);
    tp->dmat[tp->i][tp->j] = tp->dmat[tp->j][tp->i];
}

void* euclidean(void* param) {
    thread_param_dist *tp = (thread_param_dist*) param;
    double tmpA, tmpB;
    double sum = 0;
    for (int p = 0; p < tp->m1->map->size(); p++) {
        if (tp->m1->map->at(p).second != 0) {
            tmpA = (double) tp->m1->map->at(p).second;
            tmpB = (double) tp->m2->getValue(tp->m1->map->at(p).first);
            sum = sum + (tmpA - tmpB) * (tmpA - tmpB);
        }
    }
    for (int p = 0; p < tp->m2->map->size(); p++) {
        if (tp->m2->map->at(p).second != 0) {
            tmpA = (double) tp->m2->map->at(p).second;
            tmpB = (double) tp->m1->getValue(tp->m2->map->at(p).first);
            if (tmpB == 0)
                sum = sum + (tmpA) * (tmpA);
        }
    }
    tp->dmat[tp->j][tp->i] = sqrt(sum);
    tp->dmat[tp->i][tp->j] = tp->dmat[tp->j][tp->i];
}

void computeDist(vector<hashmap*> *maps, double** dmat, vector<string>& sequences, int k) {
    vector<int> faultyWindowsVector = getFaultyWindowsVector();

    vector<pthread_t> threads((maps->size()*(maps->size() - 1)) / 2 + 1, pthread_t());
    int count = 0, count2 = 0, lastThread = 0;
    vector<thread_param_dist> tp_struct((maps->size()*(maps->size() - 1)) / 2 + 1, thread_param_dist());
    for (int i = 0; i < maps->size(); i++) {
        for (int j = i + 1; j < maps->size(); j++) {
            tp_struct[count2].m1 = (*maps)[i];
            tp_struct[count2].m2 = (*maps)[j];
            tp_struct[count2].n = sequences.at(i).size() - k - faultyWindowsVector.at(i);
            tp_struct[count2].m = sequences.at(j).size() - k - faultyWindowsVector.at(j);
            tp_struct[count2].dmat = dmat;
            tp_struct[count2].i = i;
            tp_struct[count2].j = j;
            if (getDistanceType() == 0)
                pthread_create(&threads[count2], NULL, euclidean, (void*) &tp_struct[count2]);
            else if (getDistanceType() == 1)
                pthread_create(&threads[count2], NULL, jensenShannon, (void*) &tp_struct[count2]);
            else if (getDistanceType() == 2)
                pthread_create(&threads[count2], NULL, morgenstern, (void*) &tp_struct[count2]);
            if (count == getNumberOfThreads()) {
                int k = lastThread;
                while (k < lastThread + count)
                    pthread_join(threads[k++], NULL);
                lastThread = k;
                count = 0;
            }
            count++;
            count2++;
        }
    }
    // finish up
    for (int i = lastThread; i < count2; i++) {
        pthread_join(threads[i], NULL);
    }

    // uncomment for no thread variant, comment out the code above in that case
    //    for (int i = 0; i < maps->size(); i++) {
    //        for (int j = i + 1; j < maps->size(); j++) {
    //            double tmpA, tmpB;
    //            double pm = 0;
    //            double qm = 0;
    //            hashmap *m1 = (*maps)[i], *m2 = (*maps)[j];
    //            int n = sequences.at(i).size() - k - faultyWindowsVector.at(i),
    //                    m = sequences.at(j).size() - k - faultyWindowsVector.at(j);
    //            for (int p = 0; p < m1->map->size(); p++) {
    //                if (m1->map->at(p).second != 0) {
    //                    tmpA = (double) m1->map->at(p).second / n;
    //                    tmpB = (double) m2->getValue(m1->map->at(p).first) / m;
    //                    pm = pm + log(tmpA / (0.5 * (tmpA + tmpB))) * tmpA;
    //                    if (tmpB != 0)
    //                        qm = qm + log(tmpB / (0.5 * (tmpA + tmpB))) * tmpB;
    //                }
    //            }
    //            for (int p = 0; p < m2->map->size(); p++) {
    //                if (m2->map->at(p).second != 0) {
    //                    tmpA = (double) m2->map->at(p).second / m;
    //                    tmpB = (double) m1->getValue(m2->map->at(p).first) / n;
    //                    if (tmpB == 0)
    //                        qm = qm + log(tmpA / (0.5 * (tmpA + tmpB))) * tmpA;
    //                }
    //            }
    //            dmat[j][i] = (0.5 * qm + 0.5 * pm);
    //            dmat[i][j] = dmat[j][i];
    //        }
    //    }    
}
