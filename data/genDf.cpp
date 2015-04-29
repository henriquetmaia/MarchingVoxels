#include <iostream>
#include <fstream>

#include <stdlib.h> 

using namespace std;

int main () {
  ofstream myfile;
  myfile.open ("example.df");

  int sampleSize = 1000;

	for ( int s = 0; s < sampleSize; ++s ){
		myfile << "vd " << rand() % 100 << " " << rand() % 100 << " " << rand() % 100 << " " << rand() % 100 << endl;
	}
  myfile.close();
  return 0;
}