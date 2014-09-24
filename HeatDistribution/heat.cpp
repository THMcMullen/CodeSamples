//Tim McMullen - 06222757 - Assignment 3 Parrallel

// Sequential implementation of heat transfer problem
// To compile:
//   make heat
#include <iostream>
#include <cmath>
#include <ctime>
#include <string>

#include "fitsfile.h"

#include "mpi.h"

void timestamp(const std::string message)

  /* Print current CPU and wall clock times. */

{
  std::cout << "## " << message << " at ";
  std::cout << clock() / (double)CLOCKS_PER_SEC << " " << time(NULL) << std::endl;
}

/*float** getarray(int nrows, int ncols)
{
  float** a = new float*[nrows];
  for (int i = 0; i < nrows; ++i) a[i] = new float[ncols];
  return a;
}*/

void put_hotspots(float* h, int nrows, int ncols)
// Set-up an initial temperature distribution on the image
{
  // Initialize the edges
  float t1 = 2;
  float t2 = 7;
  for (int i = 0; i < nrows; ++i) {
    h[i*ncols] = t1;
    h[(i*ncols)+ncols-1] = t2;
  }
  for (int j = 0; j < ncols; ++j) {
    float t = t1 + (t2 - t1) * (float)j/(float)ncols;
    h[j] = t;
    h[((nrows-1)*ncols)+j] = t;
  }

  // Put a couple of hot spots on the lower boundary
  for (int i=ncols/2-ncols/4; i<ncols/2+ncols/4; ++i) h[i] = 8;
  for (int i=ncols/2-ncols/8; i<ncols/2+ncols/8; ++i) h[i] = 0.3;

}


void write_to_fits(float* h, int nrows, int ncols, std::string filename)
{
  std::cout << "## Writing to " << filename << std::endl;

  FitsFile ff;
  ff.createFile(filename);
  ff.createImage2D(ncols, nrows, FLOAT_IMG);

  // Write to image file row by row
  for (int i = 0; i < nrows; ++i) {
    long firstel = i * ncols + 1;
    long ntotal = ncols;
    ff.writeImage(&h[i*ncols], firstel, ntotal);
  }
  ff.closeFile();

}

int main(int argc, char* argv[]){

  if(argc!=3){
    printf("The program requires the user to input the sample size\n");
    return -1;
  }
  
  const int nrows = atoi(argv[1]);
  const int ncols = atoi(argv[2]);
  int numproc, myid, *size;
  int *recv_size = new int[1];
  float **twoddata, **twoddatb, **currentTab, **previousTab, *above, *below;
  float *row;
  int *displ; 
  int *sendcount;
  double startTime = 0, endTime = 0;

  bool active, BottomTable, TopTable;
  //float** g = getarray(nrows, ncols);

  active = false;
  BottomTable = true;
  TopTable = true;
  


  //timestamp("Starting");
  MPI_Status Stat;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  
  float* h = new float[nrows*ncols];

  if(myid == 0){

    put_hotspots(h, nrows, ncols);
    write_to_fits(h, nrows, ncols, "heat0.fit");
    startTime = MPI_Wtime();

    sendcount = new int[numproc];
    size = new int[numproc];
    displ = new int[numproc];
    for(int i =0; i<numproc; i++){
      size[i] = nrows/numproc;
      if(i < nrows%numproc){
        size[i]++;
      }
      sendcount[i] = size[i]*ncols;
    //printf("proc %d will recv %d numbers\n", i, size[i]);
    }
  
    displ[0] = 0;
    for(int x = 1; x < numproc; x++){
      displ[x] = displ[x-1]+(sendcount[x-1]);
    } 

  }
//sends the amount of data to be recved
  MPI_Scatter(size, 1, MPI_INT, recv_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int row_size = recv_size[0]*ncols;
  row = new float[row_size];

  //Collects data
  MPI_Scatterv (h,sendcount,displ,MPI_FLOAT, row,row_size, MPI_FLOAT,0, MPI_COMM_WORLD);
  

//converts single array to a 2d array
  twoddata = new float*[recv_size[0]];
  twoddatb = new float*[recv_size[0]];
  for (int i = 0; i < recv_size[0]; ++i) {
    twoddata[i] = new float[ncols];\
    twoddatb[i] = new float[ncols];
    for (int j= 0; j < ncols; j++){
      twoddata[i][j] = twoddatb[i][j] = row[(i*ncols)+j];
    }
  }
 
  if(myid!=0){
    TopTable = false;
  }
  if(myid!=(numproc-1)){
    BottomTable = false;
  }

  MPI_Request SendA, SendB, RecvA, RecvB;
  MPI_Status stRecvA, stRecvB;

  int Changed, Change, End;
  int *Updated = new int[numproc];
  int *Finished = new int[numproc];
  
  int count =0;
  
  End = 1;
  //for (int z = 0; z < 11; z++){
  while(End!=0){
    
    count++;
  //Switchs between which table to write to, and compare to
    active = !active;
    if(active){
      currentTab = twoddata;
      previousTab = twoddatb;
    }else {
      previousTab = twoddata;
      currentTab = twoddatb;
    }

    //printf("%f,%f\n",previousTab[1][1],currentTab[1][1]);

//Sends the required top and bottom rows with none blocking sends ans recvs
    if(!TopTable){
      MPI_Isend(&previousTab[0][0],ncols,MPI_FLOAT,myid-1,0,MPI_COMM_WORLD,&SendA);
    }
    
    if(!BottomTable){
      MPI_Isend(&previousTab[recv_size[0]-1][0],ncols,MPI_FLOAT,myid+1,0,MPI_COMM_WORLD,&SendB);
    }

    if(!BottomTable){
      below = new float[ncols];
      MPI_Irecv(below,ncols,MPI_FLOAT,myid+1,0,MPI_COMM_WORLD,&RecvA);
    }

    if(!TopTable){
      above = new float[ncols];
      MPI_Irecv(above,ncols,MPI_FLOAT,myid-1,0,MPI_COMM_WORLD,&RecvB);
    }
    //applies the formula to the data
    for(int y=1;y<recv_size[0]-1;y++){
      for(int x=1;x<ncols-1;x++){
        currentTab[y][x] = 0.25 * (previousTab[y-1][x] + previousTab[y+1][x] + previousTab[y][x-1] + previousTab[y][x+1]);
      }
    }
    //updats the top row, but makes sure the data is upto date
    if(!TopTable){
      MPI_Wait( &RecvB, &stRecvB );
      for(int x=1;x<ncols-1;x++){
        currentTab[0][x] = 0.25 * (above[x] + previousTab[1][x] + previousTab[0][x-1] + previousTab[0][x+1]);
      }
      delete above;
    }
    //updates the bottom row, but makes sure the data is upto date
    if(!BottomTable){
      MPI_Wait( &RecvA, &stRecvA );
      for(int x=1;x<ncols-1;x++){
        currentTab[recv_size[0]-1][x] = 0.25 * (below[x] + previousTab[recv_size[0]-2][x] + previousTab[recv_size[0]-1][x-1] + previousTab[recv_size[0]-1][x+1]);
      }
      delete below;
    }

    Change = 0;
    for(int y=0;y<recv_size[0];y++){
      for(int x=1;x<ncols-1;x++){
	if(currentTab[y][x] != previousTab[y][x]){
	  Change = 1;
	  break;
	}
      }
      if(Change==1)break;
    }
//Checks on the master node to see if there were changes and tells all proccessors to stop or continue
   MPI_Gather(&Change,1,MPI_INT,Updated,1,MPI_INT,0,MPI_COMM_WORLD);

    if(myid==0){
      Changed = 0;
      for(int i=0; i<numproc;i++){
	if(Updated[i] != 0){
	  Changed = 1;
	  break;
	}
      }
    }
  
    for(int i=0;i<numproc;i++){
      Finished[i] = Changed;
    }

    MPI_Scatter(Finished,1,MPI_INT,&End,1,MPI_INT,0,MPI_COMM_WORLD);

//////////////////////////////////loop ends here////////////////////////////////////////
}
  

  for(int i=0;i<recv_size[0];i++){
    for(int j=0;j<ncols;j++){
       row[(i*ncols)+j] = ( active ? twoddata[i][j] : twoddatb[i][j] );
    }
  }
//Collects all the data from each node
  MPI_Gatherv(row,recv_size[0]*ncols,MPI_FLOAT,h,sendcount,displ,MPI_FLOAT, 0,MPI_COMM_WORLD);

  if(myid==0){
    endTime = MPI_Wtime();
    write_to_fits(h, nrows, ncols, "heat1.fit");
    printf("Completed %i iterations on %i nodes in %fls\n", count, numproc, endTime-startTime);
  }

   MPI_Finalize();

}
