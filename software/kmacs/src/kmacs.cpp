#include "kmacs.h"

int extension(unsigned char* T,  int i, int j, int k, int s1, int n){
	int count=0;
	if(k==0)
		return 0;	
	while(k>=0 && i<s1 && j<n){
		if(T[i]==T[j]){
			count++;i++;j++;
		}
		else{
			i++;j++;k--;
		}
	}
	return count;
}

int RMQextension(unsigned char* T,  int *iSA , int* LCP , RMQ_succinct* rmqSuc, int i, int j, int k, int s1, int n){
	int count=0;
	int tmp=0;	
	if(k==0)
		return 0;	
	i++;j++;
	while(k>0 && i<s1 && j<n){
		tmp=0;
		if(T[i]==T[j]){
			if (iSA[i] > iSA[j])
				tmp = LCP[rmqSuc->query(iSA[j]+1,iSA[i])];
			else
				tmp= LCP[rmqSuc->query(iSA[i]+1,iSA[j])];
		}
		i=tmp+i+1;
		j=tmp+j+1;
		count=count+tmp;
		k--;
	}
	return count;
}

double km(string seq, int s1, int s2, int k, int rmq){
	int n=seq.length();
	unsigned char *T = new unsigned char[n+1]();
	int *SA = new int[n+1]();
	int *iSA;
	int *LCP = new int[n+1]();
	int *LCP2 = new int[n]();
	int* pos= new int[n]();
	int* pos2= new int[n]();
	RMQ_succinct* rmqSuc;
	double avgS1=0; double avgS2=0;
	strcpy( (char*) T, seq.c_str());
	sais(T, SA, LCP, n);
	if(rmq){
		iSA = new int[n+1]();
		for(int i=0; i<n;i++){
			iSA[SA[i]]= i;
		}
		rmqSuc = new RMQ_succinct(LCP, n);
	}	
	int min=LCP[1]; 
	int minPos=1;
	int p;int tmp; int max;
	pos[1]=0; pos[0]=0; LCP2[1]=0; pos[n-1]=0;
	for(int i=1;i<n-1;i++){
		if(((SA[i]>s1 && SA[i+1]>s1) || (SA[i]<s1 && SA[i+1]<s1))){
			if(LCP[i+1]<min){
				min=LCP[i+1];
			}
			LCP2[i+1]=min;
			pos[i+1]=minPos;
		}
		else{
			minPos=i;
			pos[i+1]=minPos;
			min=LCP[i+1];	
			LCP2[i+1]=LCP[i+1];	
		}		
	}
	min=0;minPos=0;
	for(int i=n-1;i>0;i--){
		if(((SA[i]>s1 && SA[i-1]>s1) || (SA[i]<s1 && SA[i-1]<s1))){
			if(LCP[i]<min)
				min=LCP[i];
			if(min>LCP2[i-1]){
				LCP2[i-1]=min;
				pos[i-1]=0;
				pos2[i-1]=minPos;
			}
			else if(LCP2[i-1]>min)
				pos2[i-1]=0;
			else
				pos2[i-1]=minPos;
		}
		else{
			min=LCP[i];	
			minPos=i;
			if(min>LCP2[i-1]){
				LCP2[i-1]=min;
				pos[i-1]=0;
				pos2[i-1]=minPos;
			}
			else if(LCP2[i-1]>min)
				pos2[i-1]=0;
			else
				pos2[i-1]=minPos;
		}
		max=0;
		if(SA[i]<s1){
			if(pos[i]>0 && SA[i]+LCP2[i]+1<s1){
				p=pos[i];
				while(LCP[p+1]>=LCP2[i]&& p>0){
					if(SA[p]>s1){
						if(rmq)
							tmp=RMQextension(T, iSA, LCP, rmqSuc, SA[i]+LCP2[i], SA[p]+LCP2[i], k, s1, n);
						else
							tmp=extension(T, SA[i]+LCP2[i], SA[p]+LCP2[i], k, s1, n);
						if(tmp>max)
							max=tmp;
					}
					p--;		
				}
			}
			if(pos2[i]>0&& SA[i]+LCP2[i]+1<s1){
				p=pos2[i];
				while(LCP[p]>=LCP2[i]&& p<n){
					if(SA[p]>s1){
						if(rmq)
							tmp=RMQextension(T, iSA, LCP, rmqSuc, SA[i]+LCP2[i], SA[p]+LCP2[i], k, s1, n);
						else
							tmp=extension(T, SA[i]+LCP2[i], SA[p]+LCP2[i], k, s1, n);
						if(tmp>max)
							max=tmp;
					}	
					p++;
				}
			}
			avgS1=avgS1+LCP2[i]+max;
		}
		else if(SA[i]>s1){
			if(pos[i]>0&&SA[i]+LCP2[i]+1<n){
				p=pos[i];
				while(LCP[p+1]>=LCP2[i] && p>0){
					if(SA[p]<s1){
						if(rmq)
							tmp=RMQextension(T, iSA, LCP, rmqSuc, SA[p]+LCP2[i], SA[i]+LCP2[i], k, s1, n);
						else
							tmp=extension(T, SA[p]+LCP2[i] , SA[i]+LCP2[i], k, s1, n);
						if(tmp>max)
							max=tmp;
					}
					p--;		
				}
			}
			if(pos2[i]>0&&SA[i]+LCP2[i]+1<n){
				p=pos2[i];
				while(LCP[p]>=LCP2[i] && p<n){
					if(SA[p]<s1){
						if(rmq)
							tmp=RMQextension(T, iSA, LCP, rmqSuc, SA[p]+LCP2[i], SA[i]+LCP2[i], k, s1, n);
						else
							tmp=extension(T, SA[p]+LCP2[i] , SA[i]+LCP2[i], k, s1, n);
						if(tmp>max)
							max=tmp;
					}	
					p++;
				}
			}
			avgS2=avgS2+LCP2[i]+max;
		}
	}
	delete [] SA; delete [] LCP; delete [] T; delete [] LCP2; delete [] pos; delete [] pos2; 
	if(rmq){
		delete[] iSA; delete rmqSuc;	
	}
	
	return ((log10(s1)/(avgS2/s2))-((2.0*log10(s2))/s2) + (log10(s2)/(avgS1/s1))-((2.0*log10(s1))/s1))*0.5;
}

double acs(string seq, int s1, int s2){
	int n=seq.length();
	unsigned char *T = new unsigned char[n+1]();
	int *SA = new int[n+1]();
	int *LCP = new int[n+1]();
	int *LCP2 = new int[n]();
	double avgS1=0; double avgS2=0;
	strcpy( (char*) T, seq.c_str());
	sais(T, SA, LCP, n);
	int min=LCP[1]; 
	LCP2[1]=0;
	for(int i=1;i<n-1;i++){
		if(((SA[i]>s1 && SA[i+1]>s1) || (SA[i]<s1 && SA[i+1]<s1))){
			if(LCP[i+1]<min)
				min=LCP[i+1];
			LCP2[i+1]=min;
		}
		else{
			min=LCP[i+1];
			LCP2[i+1]=LCP[i+1];	
		}		
	}
	min=0;
	for(int i=n-1;i>0;i--){
		if(((SA[i]>s1 && SA[i-1]>s1) || (SA[i]<s1 && SA[i-1]<s1))){
			if(LCP[i]<min)
				min=LCP[i];
			LCP2[i-1]=max(min,LCP2[i-1]);
		}
		else{
			min=LCP[i];	
			LCP2[i-1]=max(LCP[i], LCP2[i-1]);
		}
		if(SA[i]<s1)
			avgS1=avgS1+LCP2[i];
		else
			avgS2=avgS2+LCP2[i];
	}
	delete [] SA; delete [] LCP; delete [] T; delete [] LCP2; 
	return ((log10(s1)/(avgS2/s2))-((2.0*log10(s2))/s2) + (log10(s2)/(avgS1/s1))-((2.0*log10(s1))/s1))*0.5;
}

void calcDmat(double** dmat, vector<string>& sequences, int k, int rmq){
	string seq;
	for(int i=0;i<sequences.size();i++){
		for(int j=i+1;j<sequences.size();j++){
			seq=sequences.at(i)+"$"+sequences.at(j);
			if(i!=j){
				if(k>0)
					dmat[j][i]=km(seq, sequences.at(i).length(), sequences.at(j).length(), k, rmq);
				else
					dmat[j][i]=acs(seq, sequences.at(i).length(), sequences.at(j).length());
				dmat[i][j]=dmat[j][i];
			}
			else{
				dmat[j][i]=0;
				dmat[i][j]=0;
			}
		}
	}
}
