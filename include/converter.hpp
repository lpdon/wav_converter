#include <vector>
#include <string>
#include <locale>
#include <inttypes.h>

#include <sys/types.h>
#include <dirent.h>

extern "C"
{
  #include "wav.h"
}

struct sWavFile
{
  WAVHEADER header;
  std::string path;
  std::string name;
  std::string ext;
};

class cConverter
{
private:
  std::string dir;
  std::vector<sWavFile> files;

  std::vector<sWavFile> readFiles( const std::string arg_dir );
  void convertFile( const sWavFile &arg_file );
  static void * convertFileThread( void * arg_file );

public:
  explicit cConverter( const std::string arg_dir )
  : dir( arg_dir )
  {
    files = readFiles( arg_dir );
  }

  bool convert( void );
};
