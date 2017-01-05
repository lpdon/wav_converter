#include <iostream>

#include "converter.hpp"

int main( int argc, char** argv )
{
  if( argc != 2)
  {
   std::cout <<"Usage: converter dirwithwavfiles" << std::endl;
   return -1;
  }

  cConverter loc_converter( argv[1] );

  loc_converter.convert();

//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
