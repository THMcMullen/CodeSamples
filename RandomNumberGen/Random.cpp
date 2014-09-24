//Assignment 2, 159.732
//Tim McMullen, 06222757

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

using namespace std;

#define array_size 4096
#define j 273//not really working
#define k 607
#define power 4294967295

unsigned long MWC[array_size];
int oldC = 362436;
int index = array_size;
unsigned int current = 1;


//creates a Seed number to start the other generator with
 unsigned int seed (){
	unsigned int output;
	//long long m = (long long)pow(2,32);
	int a = 1103515245;
	int c = 12345;
	output = ((a*current + c) % power);
	current = output;
	return output;	
}

//Complementary multiply-with-carry
unsigned long complementary(){
	//long long b = (long long)pow(2,32);
	long long a = 262650799;
	index = (index + 1) % array_size;
	MWC[index] = ((power - 1) - ((a*MWC[index]) + oldC)) % power;
	oldC = ((a*MWC[index]) + oldC) / power;
	return MWC[index];
}

//Lagged Fibonacci ---- yn = (yn-j * yn-k) mod m 
// Does not work but gave it a go, gives 0 at some points and will go 0 for awhile then works again
unsigned long lagged(){
	
	long long m = (long long)pow(2,32);
	if(index >= 4096){
		index = k+1;
	}
	
	MWC[index] = (MWC[index-j] * MWC[index-k]) % m;
	
	return MWC[index++];
	
}

//////////////////////////////////////////////////////////////////

//The gaussian function
float max_one(){
	//calls the wanted RNG
	double max = pow(2,32);
	unsigned long one = complementary();//uses a Complementary multiply-with-carry change to another RNG function to use that one
	return (one/max);
}

unsigned long user_max(int max_not_inclusive){
	return floor(max_one() * max_not_inclusive);// change the function in max_one to chang ethe RNG used	
}

float gaussian(float mean, float sd){//Taken from ftp://ftp.taygeta.com/pub/c/boxmuller.c

	float x1, x2, w, y1, y2 =0;
	static int use_last = 0;

	if (use_last){		        /* use value from previous call */
		y1 = y2;					  //This will return the second value created from the previous iteration
		use_last = 0;
	}else{
		do {
			x1 = 2.0 * max_one() - 1.0;//Change max_ones function call if you want to use a different function
			x2 = 2.0 * max_one() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while (w>=1.0);

		w = sqrt( (-2.0*log(w))/w);
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}
	return(mean+y1*sd);
}
//savs data to a file
void save(){
	ofstream myfile;
	myfile.open ("RNG.txt");//change rng.txt to what you want your saved file called
	for(int i = 0; i < array_size ;i++){
		myfile << MWC[i] << endl;
	}
	cout << "saved" << endl;
	myfile.close();

}
//Loads in data from a file 
void load(){
	int i = 0;
	string str;
	ifstream myfile;
	myfile.open ("RNG.txt");//change rng.txt to what ever the file you want to load is called
	if(myfile.is_open()){
		while(!myfile.eof()){
			getline(myfile,str);
			sscanf(str.c_str(),"%u",&MWC[i]);
			i++;
		}
	cout << "loaded" << endl;
	}	
	myfile.close();
}

int main(){
	
	int num = 1000;//How many time you want to run the selected RNG
	float avg = 0; //Need to change this type based on the returned value fo the functiuon called
	
//Quickly fulls an array with random numbers for others to work from
	
	for(int n = 0; n < array_size;n++){
		MWC[n] = seed();
	}	
//Example function calls
	
//Uniform-distributed random numbers, can be used with seed or Complementary functions
	//unsigned long x = complementary();
	//unsigned long y = seed();

//by calling gaussian(0,1) we get a value between 0 - 1 returned
	//float z = gaussian(0,1);

//by calling user_max(max value required) we are returned with a value ranging between 0 - that value
	
	//unsigned long a = user_max(10); //will return a value between 0 - 10
	
	
//Perfoms the gaussian calculation till the required amount of numbers have been returned, then saves data to a file, and displays the average value 
	
	ofstream myfile;
	myfile.open ("gaussian.txt");
	for(int i = 0; i < num ;i++){
		myfile << gaussian(0,1) << endl;
		avg += (gaussian(0,1)/num);
	}
	cout << "gaussian" << endl;
	//cout << avg << endl;
	myfile.close();
	
	
//saves the array which stores all needed valuse to continue the RNG
save();
	
//Reads in from a save file, and fulls the array with the last x amount of variables used
//load();
	
}

