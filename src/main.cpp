#include <iostream>

using namespace std;

int main( int argc, char** argv )
{
	if( argc < 3 )
	{
		cerr << "usage: " << argv[0] << " argument_2.obj argument_3 " << endl;
		return 1;
	}

	return 0;
}

