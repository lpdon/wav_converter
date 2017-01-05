#include <iostream>

#include "converter.hpp"

int main( int argc, char** argv )
{
  if( argc != 2)
  {
   std::cout <<"Usage: converter dirwithwavfiles" << std::endl;
   return -1;
  }

  cConverter::convertFilesFromDir( argv[1] );
	return 0;
}
