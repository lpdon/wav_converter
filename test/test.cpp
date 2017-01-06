#include <iostream>
#include <string>
#include <fstream>

#include "gtest/gtest.h"
#include "converter.hpp"

using namespace std;

const string testDir = "data";

TEST( ReadFilesTestCase, NumFiles )
{
  vector<sWavFile> loc_files = cConverter::readFiles( testDir );
  EXPECT_EQ( loc_files.size(), 9U );
}

TEST( ReadFilesTestCase, InvalidDir )
{
  vector<sWavFile> loc_files = cConverter::readFiles( "bla" );
  EXPECT_EQ( loc_files.size(), 0U );
}

TEST( ConvertTestCase, SingleThread )
{
  vector<sWavFile> loc_files = cConverter::readFiles( testDir );
  sWavFile loc_file = loc_files[ 0U ];
  cConverter::convertFile( loc_file );

  //check if mp3 exists
  string loc_path = testDir + "/" + loc_file.name + ".mp3";
  ifstream loc_ifstream( loc_path.c_str(), ios::binary );
  EXPECT_TRUE( loc_ifstream.is_open() );

  if ( loc_ifstream.is_open() )
  {
    loc_ifstream.close();
    remove( loc_path.c_str() );
  }
}

TEST( ConvertTestCase, MultiThread )
{
  vector<sWavFile> loc_files = cConverter::readFiles( testDir );
  cConverter::convertFilesFromDir( testDir );

  //check if mp3 exists
  for ( uint16_t loc_idx_file = 0U; loc_idx_file < loc_files.size(); ++loc_idx_file )
  {
    sWavFile loc_file = loc_files[ loc_idx_file ];
    string loc_path = testDir + "/" + loc_file.name + ".mp3";
    ifstream loc_ifstream( loc_path.c_str(), ios::binary );
    EXPECT_TRUE( loc_ifstream.is_open() );

    if ( loc_ifstream.is_open() )
    {
      loc_ifstream.close();
      remove( loc_path.c_str() );
    }
  }
}
