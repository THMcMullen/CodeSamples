//Tim McMullen, 06222757 Parrallel Assigment 1
#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

long modular( long x, long n, long p){
  long y = 1;
  long u = 0;
  u = x % p;
  while (n > 0){
    if((n % 2) != 0){
      y = (y * u) %p;
    };
    n = n / 2;
    u =(u * u) % p;
  };
  return y;
}
  
int main(argc,argv)int argc;char *argv[]; 
{
  double time_Dif; //find the time taken to run
  double start;
  double end;
  int numproc, myid, namelen;
  int N,i,steps;
  double result,sum0,sum1;
  double pietally;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  float pi;

  //Both master and slave nodes require these values
  long m = 2147483647;
  long a = 16807;
  long bigA = 0;
  srand((unsigned)time(NULL));
  long temp = rand() % m;
  long newtemp = 0;
  
  MPI_Status Stat;//status variable, so operations can be checked

  MPI_Init(&argc,&argv);//INITIALIZE
  MPI_Comm_size(MPI_COMM_WORLD, &numproc); //how many processors??
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);    //what is THIS processor-ID?

  bigA = modular(a, numproc, m);

  //what is THIS processor name (hostname)?
  MPI_Get_processor_name(processor_name,&namelen);
  //printf("ID=%d: %s\n", myid, processor_name);

  N=atoi(argv[1]);//get the sample size from the user 
  steps = N/numproc;
  if (myid == 0) {//this is the master
    start = MPI_Wtime();
    printf("sample size is %d \n", N);

    long zeronode = 0;
    //creates a number for itself
    newtemp = (a * temp) % m;
    temp = newtemp;
    zeronode = temp;

    for  (i=1;i<numproc;i++) {//send to all nodes
      newtemp = (a * temp) % m;//c is ignored as it is 0
      temp = newtemp;
      //printf("sending %d to node %d \n", temp, i);
      int ret = MPI_Send(&temp, 1, MPI_INT, i,0, MPI_COMM_WORLD);
      //printf("%d ret=%d\n", i, ret);
    }	
    //sum0=0;//partial result for node 0

    temp = zeronode;//puts the saved number in temp
     // printf("for node 0, the number given was %d \n", temp);
    for(i=1;i<=steps;i++){
      newtemp = (bigA*temp) % m;
      temp = newtemp;
      pietally = (double)temp/(double)m;
      sum0 = sum0+(sqrt(1-(pietally*pietally)));
 
    }   
    //printf("node zero has a total of %f \n", sum0);
  
    result=sum0;
    for (i=1;i<numproc;i++) {//receive from all nodes
      int fret = MPI_Recv(&sum1, 1, MPI_DOUBLE, i,0, MPI_COMM_WORLD, &Stat);
    //  printf("%d ret=%d\n", i, fret);
      result=result+sum1;//adds the various sums
    }
   // fprintf(stdout,"The sum from 1 to %d is %f \n",N,result);
    pi = ((4*result)/N);
    printf("the estimated value of pie is: %f\n",pi);
    end = MPI_Wtime();
    time_Dif = end - start;
    printf ("it took %f seconds to run\n", time_Dif);

  } 

  else {//this is not the master
    MPI_Recv(&temp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &Stat);
    sum1=0;
    long localtemp = temp;//local values
    long localnew = 0;
    for(i=1;i<=(steps);i++){
      localnew = (bigA*localtemp) % m;
      localtemp = localnew;
      pietally = (double)localtemp/(double)m;
      sum1=sum1+(sqrt(1-(pietally*pietally)));
    }	
   // printf("my id is %d, and the number given is %d, and i will send back %f \n", myid, temp, sum1);
    MPI_Send(&sum1, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  MPI_Finalize();
}
