//Tim McMullen 06222757 159.735 Assignment 2, Parallel Bucket Sort
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int compare(const void* x1, const void* x2);

int main(int argc,char *argv[]){

  //Check input
  if(argc!=2){
    printf("The program requires the user to input the sample size\n");
    return -1;
  }

  int numproc, myid, x, N, NDATA, np;
  const float xmin = 1.0;
  const float xmax = 101.0;
  double starttime = 0,  endtime = 0;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  float stepsize = (xmax - xmin) / numproc;
  //Check input and changes it if needed
  NDATA = atoi(argv[1]);
  if(NDATA%numproc != 0){
    printf("rounding may occur to prevent errors\n");
    NDATA -= NDATA%numproc;
  }
  
  float recvbuf[NDATA];
  float *sendbuf;
  np = NDATA/numproc;
  sendbuf = (float*)malloc(NDATA * sizeof(float));

  for (x = 0; x < NDATA; x++){
    sendbuf[x] = drand48()*(xmax-xmin-1)+xmin;
  }
  starttime = MPI_Wtime();
  //Sends each proccessor its required data
  MPI_Scatter(sendbuf, np, MPI_FLOAT, recvbuf, NDATA, MPI_FLOAT, 0, MPI_COMM_WORLD);

  //Creates the buckets
  float *buckets = calloc(NDATA, sizeof(float *));
  for(x =0; x < NDATA; x++){
    buckets[x] = 0;
  }
  
  int *nitems = malloc(numproc * sizeof(int));
  for(x = 0; x < numproc; x++){
    nitems[x] = 0;
  }
  //loads each bucket with the required data
  for(x = 0; x < np; x++){
    int bktno = (int)floor((recvbuf[x] - xmin) / stepsize);
    int idx = bktno * np + nitems[bktno];
    buckets[idx] = recvbuf[x];
    ++nitems[bktno];
  }
  
  for(x = 0; x < NDATA;x++){
    if(buckets[x] != 0){
      if(x == 0){
        nitems[0]++;
      }else{
        nitems[x/np]++;
      }
    }
  }

  for(x = 0; x < numproc; x++){
    nitems[x] = nitems[x]/2;
  }
  int *bucketrecv = malloc(numproc * sizeof(int));
  //finds out how much data is able to be sent
  MPI_Alltoall(nitems, 1, MPI_INT,bucketrecv, 1, MPI_INT, MPI_COMM_WORLD);

  //recv buffer
  int totalsize = 0;
  for(x = 0; x < numproc; x++){
    totalsize += bucketrecv[x];
  }

  int totalrecv =0;
  //recv displs
  int *rdispls = (int*)malloc(numproc *sizeof(int));
  rdispls[0] = 0;
  for(x = 1; x < numproc; x++){
    rdispls[x] = rdispls[x-1] + bucketrecv[x-1];
  }
  //create send buffer
  int totalsendsize = 0;
  for(x = 0; x < numproc; x++){
    totalsendsize += nitems[x];
  }

  //new send buffer
  float *newsendbuffer = (float *)malloc(totalsendsize * sizeof(float));

  //purge the 0 from the send buffer
  int y = 0;
  for(x = 0, y = 0; x < NDATA; x++){
    if(buckets[x] > 0){
        newsendbuffer[y] = buckets[x];
      y++;
    }
  }

  //send displs
  int *sdispls = (int*)malloc(numproc *sizeof(int));
  sdispls[0] = 0;
  for(x = 1; x < numproc; x++){
    sdispls[x] = sdispls[x-1] + nitems[x-1];
  }

  float *newrecvbuffer = (float *)malloc(totalsize * sizeof(float));

  //Sends each other the required bucket
  MPI_Alltoallv(newsendbuffer,nitems, sdispls, MPI_FLOAT, newrecvbuffer,bucketrecv,rdispls,MPI_FLOAT, MPI_COMM_WORLD);

  //Once the bucket has been recived Qsort is used to sort the data
  qsort(&newrecvbuffer[0], totalsize, sizeof(float), compare);

  int *finalsendadd = (int*)malloc(sizeof(int));
  finalsendadd[0] = totalsize;

  int *finalrecvadd = (int*)malloc(numproc * sizeof(int));

  //Finds the size of data to gather from each node
  MPI_Gather(finalsendadd,1,MPI_INT,finalrecvadd,1,MPI_INT,0,MPI_COMM_WORLD);

  //Final displs
  int *finaldispls = (int*)malloc(numproc *sizeof(int));
  finaldispls[0] = 0;
  for(x = 1; x < numproc; x++){
    finaldispls[x] = finaldispls[x-1] + finalrecvadd[x-1];
  }
  //Creates the final bucket for all numbers to be collected into
  float *finalbucket = (float *)malloc(NDATA * sizeof(float));
  //takes data from each node and adds the incoming buffers together
  MPI_Gatherv(newrecvbuffer, totalsize, MPI_FLOAT, finalbucket, finalrecvadd, finaldispls, MPI_FLOAT, 0,MPI_COMM_WORLD);

  if(myid == 0){
  //Prints out the numbers in a list
    //for(x = 0; x < NDATA; x++){
      //  printf("myid %d,%f\n",myid,finalbucket[x]);
      //}
    endtime = MPI_Wtime();
    //Prints out the time
    fprintf(stdout, "Time taken: %lf\n", endtime-starttime);

  }
  MPI_Finalize();
}


/*****************************************************************************/
// The comparison function to use with the library qsort
// function. This will tell qsort to sort the numbers in ascending
// order.
int compare(const void* x1, const void* x2) {
  const float* f1 = x1;
  const float* f2 = x2;
  float diff = *f1 - *f2;

  return (diff < 0) ? -1 : 1;
}

