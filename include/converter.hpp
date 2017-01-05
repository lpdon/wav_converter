#include <vector>
#include <string>

#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>

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
  static std::string dir;
  static pthread_mutex_t mutexCout;
  static pthread_mutex_t mutexThreadsCounter;
  static pthread_cond_t condThreadsAvailable;
  static uint16_t threadsCounter;

  static std::vector<sWavFile> readFiles( const std::string arg_dir );
  static void convertFile( const sWavFile &arg_file );
  static void * convertFileThread( void * arg_file );

  cConverter(void);
  cConverter(const cConverter &arg_copy);
  cConverter& operator=(const cConverter &arg_copy);

public:
  static void convertFilesFromDir( const std::string arg_dir );
};
