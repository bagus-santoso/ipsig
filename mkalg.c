#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <sys/time.h>
#include <openssl/sha.h>
#include <time.h>

#include "define_IP2S.h"
#include "func_pivot_gauss_jordan.h"
#include "functions_NLEN_NNMAT.h"
#include "functions_MLEN_MMMAT.h"
#include "functions_NNMATxM.h"

unsigned char LoadKeys(NNMATRIXxM *G){
  FILE *fp;
  int i,j,k;
  if((fp=fopen("pkG.bin","rb"))==NULL){
    printf("File pkG.bin can't open as readable.\n");
    return 1;
  }
  
  for(i=0;i<NUM_M;i++){
    for(j=0;j<NUM_N;j++){
      for(k=0;k<4*INTS_N;k++){
	G->No[i].Matrix[j]._1byte[k]._8bit=fgetc(fp);
      }
    }
  }
  NNMATRIXxMtoTRANSPOSE(G);
  fclose(fp);

  return 0;
}

int main(){
  struct timeval tv;
  gettimeofday(&tv,NULL);
  gsl_rng *r;
  gsl_rng_env_setup();
  r=gsl_rng_alloc(gsl_rng_default);
  gsl_rng_set(r,tv.tv_sec+tv.tv_usec);

  NNMATRIXxM *G=(NNMATRIXxM*)malloc(sizeof(NNMATRIXxM));
  if(LoadKeys(G)){
    free(G);
    free(r);
    return 1;
  }
  NNMATRIX *R=(NNMATRIX*)malloc(sizeof(NNMATRIX)*NUM_L);
  MMMATRIX *L=(MMMATRIX*)malloc(sizeof(MMMATRIX)*NUM_L);
  NNMATRIXxM *Y=(NNMATRIXxM*)malloc(sizeof(NNMATRIXxM)*NUM_L);

  FILE *fp;
  int i,j,k,l;
  
  for(i=0;i<NUM_L;i++){
    randomNNMATRIX(&(R[i]),r);
    randomMMMATRIX(&(L[i]),r);
    MMMATRIXoNNMATRIXxMoNNMATRIX(&(Y[i]),&(L[i]),G,&(R[i]));
  }

  if((fp=fopen("alR.bin","wb"))==NULL){
    printf("File alR.bin can't open as writable.\n");
    free(R);
    free(L);
    free(Y);    
    free(r);
    free(G);
    return 1;
  }
  
  for(i=0;i<NUM_L;i++){
    for(j=0;j<NUM_N;j++){
      for(k=0;k<4*INTS_N;k++){
	fputc(R[i].Matrix[j]._1byte[k]._8bit,fp);
      }
    }
  }
  fclose(fp);
  
  if((fp=fopen("alL.bin","wb"))==NULL){
    printf("File alL.bin can't open as writable.\n");
    free(R);
    free(L);
    free(Y);
    free(r);
    free(G);
    return 1;
  }
  
  for(i=0;i<NUM_L;i++){
    for(j=0;j<NUM_M;j++){
      for(k=0;k<4*INTS_M;k++){
	fputc(L[i].Matrix[j]._1byte[k]._8bit,fp);
      }
    }
  }
  fclose(fp);

  if((fp=fopen("alY.bin","wb"))==NULL){
    printf("File alY.bin can't open as writable.\n");
    free(R);
    free(L);
    free(Y);
    free(r);
    free(G);
    return 1;
  }
  
  for(i=0;i<NUM_L;i++){
    for(j=0;j<NUM_M;j++){
      for(k=0;k<NUM_N;k++){
	for(l=0;l<4*INTS_N;l++){
	  fputc(Y[i].No[j].Matrix[k]._1byte[l]._8bit,fp);
	}
      }
    }
  }
  fclose(fp);

  free(r);
  free(Y);
  free(R);
  free(L);
  free(G);
  return 0;
}
