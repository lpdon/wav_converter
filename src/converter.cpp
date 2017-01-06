#include <iostream>
#include <fstream>
#include <locale>
#include <inttypes.h>
#include <sys/types.h>
#include <dirent.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include "converter.hpp"

extern "C"
{
  #include "lame.h"
}

std::string cConverter::dir;
pthread_mutex_t cConverter::mutexCout;
pthread_mutex_t cConverter::mutexThreadsCounter;
pthread_cond_t cConverter::condThreadsAvailable;
uint16_t cConverter::threadsCounter = 0U;

void cConverter::convertFilesFromDir( const std::string arg_dir )
{
  const std::vector<sWavFile> loc_files = readFiles( arg_dir );

#ifdef _WIN32
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  const int loc_maxThreads = ( sysinfo.dwNumberOfProcessors != 0 ) ? sysinfo.dwNumberOfProcessors : 1;
#else
  const int loc_maxThreads = ( sysconf( _SC_NPROCESSORS_ONLN ) != 0 ) ? sysconf( _SC_NPROCESSORS_ONLN ) : 1;
#endif
  pthread_t loc_threads[ loc_files.size() ];
  pthread_mutex_init( &mutexCout, NULL );
  pthread_mutex_init( &mutexThreadsCounter, NULL );
  pthread_cond_init( &condThreadsAvailable, NULL );

  std::cout << "CPU threads available: " << loc_maxThreads << std::endl;
  threadsCounter = loc_maxThreads;

  /*
   * One thread instance for each file.
   * A thread is created every time a CPU core is available.
   */
  for ( uint16_t loc_idx_file = 0U; loc_idx_file < loc_files.size(); ++loc_idx_file )
  {
    pthread_mutex_lock( &mutexCout );
    std::cout << "[Thread" << loc_idx_file << "] : Converting: " << loc_files[ loc_idx_file ].name << std::endl;
    pthread_mutex_unlock( &mutexCout );

    pthread_create( &loc_threads[ loc_idx_file ], NULL, convertFileThread, (void *)&loc_files[ loc_idx_file ] );

    pthread_mutex_lock( &mutexThreadsCounter );
    threadsCounter--;
    while( threadsCounter == 0U )
    {
      pthread_cond_wait( &condThreadsAvailable, &mutexThreadsCounter);
    }
    pthread_mutex_unlock( &mutexThreadsCounter );
  }

  for ( uint16_t loc_idx_file = 0U; loc_idx_file < loc_files.size(); ++loc_idx_file )
  {
    pthread_join( loc_threads[ loc_idx_file ], NULL );
  }
}

std::vector<sWavFile> cConverter::readFiles( const std::string arg_dir )
{
  std::vector<sWavFile> loc_files;

  DIR * loc_pDir = opendir( arg_dir.c_str() );

  if ( loc_pDir != NULL)
  {
    dir = arg_dir;
    struct dirent * loc_pEnt = readdir ( loc_pDir );

    while ( loc_pEnt != NULL )
    {
      std::string loc_string = loc_pEnt->d_name;

      // convert to lower case to avoid problems with names
      std::string loc_lower_string = loc_string;

      for ( uint16_t loc_i = 0U; loc_i < loc_lower_string.length(); ++loc_i )
      {
        loc_lower_string[ loc_i ] = std::tolower( loc_lower_string[ loc_i ] );
      }

      const std::string::size_type loc_pos = loc_lower_string.find( ".wav" );

      if ( loc_pos != std::string::npos )
      {
        std::string loc_path = ( arg_dir + "/" + loc_string );
        std::string loc_name = loc_string.substr( 0U, loc_pos );
        std::string loc_ext = loc_string.substr( loc_pos + 1U );
        WAVHEADER loc_header;

        if ( wav_loadheader( loc_path.c_str(), &loc_header ) )
        {
          sWavFile loc_file = { loc_header, loc_path, loc_name, loc_ext };
          loc_files.push_back( loc_file );
        }
      }
      loc_pEnt = readdir ( loc_pDir );
    }
    closedir (loc_pDir);
  }
  else
  {
    /* failed to open, no files added */
  }

  return loc_files;
}

void cConverter::convertFile( const sWavFile &arg_file )
{
  const WAVHEADER loc_header = arg_file.header;
  uint32_t loc_wavSizeLeft = loc_header.subchunck2Size;
  const std::string loc_outputPath = dir + "/" + arg_file.name + ".mp3";

  std::ifstream loc_input( arg_file.path.c_str(), std::ios::binary );
  std::ofstream loc_output( loc_outputPath.c_str(), std::ios::binary );

  if ( loc_input.is_open() && loc_output.is_open() )
  {
    /*
     * Ignore the first bytes of the file containing the header
     */
    loc_input.seekg( static_cast<std::ifstream::seekdir>( sizeof( WAVHEADER ) ) );

    lame_global_flags * loc_lame = lame_init();
    lame_set_VBR( loc_lame, vbr_default );
    lame_set_in_samplerate( loc_lame, loc_header.sampleRate );
    lame_set_num_channels( loc_lame, loc_header.numChannels );
    lame_set_brate( loc_lame, loc_header.byteRate );
    lame_init_params( loc_lame );

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;

    short int loc_pcmBuffer[ PCM_SIZE*2 ];
    unsigned char loc_mp3Buffer[ MP3_SIZE ];

    while ( loc_wavSizeLeft > 0U )
    {
      loc_input.read( reinterpret_cast<char *>( loc_pcmBuffer ), 2 * sizeof(short int) * PCM_SIZE );
      std::streamsize loc_bytesRead = loc_input.gcount();

      if ( loc_bytesRead != 0 )
      {
        /*
         * Number of samples depending on channels
         */
        const int loc_numSamples = loc_bytesRead / (2 * sizeof(short int));
        const int loc_bytesEnc = lame_encode_buffer_interleaved( loc_lame, loc_pcmBuffer, loc_numSamples, loc_mp3Buffer, MP3_SIZE );

        if ( loc_bytesEnc >= 0 )
        {
          loc_output.write( reinterpret_cast<char *>( loc_mp3Buffer ), loc_bytesEnc );
          loc_wavSizeLeft -= static_cast<uint32_t>( loc_input.gcount() );
        }
        else
        {
          pthread_mutex_lock( &mutexCout );
          std::cout << "             Failed converting - LAME error: " << arg_file.name << std::endl;
          pthread_mutex_unlock( &mutexCout );
        }
      }
      else
      {
        /*
         * No frames left to be read, flush the buffer
         */
        const int loc_bytesFlushed = lame_encode_flush( loc_lame, loc_mp3Buffer, MP3_SIZE );
        loc_output.write( reinterpret_cast<char *>( loc_mp3Buffer ), loc_bytesFlushed );
        loc_wavSizeLeft = 0U;
      }
    }

    pthread_mutex_lock( &mutexCout );
    std::cout << "             Finished converting: " << arg_file.name << std::endl;
    pthread_mutex_unlock( &mutexCout );
    loc_input.close();
    loc_output.close();
    lame_close( loc_lame );
  }
  else
  {
    pthread_mutex_lock( &mutexCout );
    std::cout << "             Failed converting - bad paths: " << arg_file.name << std::endl;
    pthread_mutex_unlock( &mutexCout );
  }
}

void * cConverter::convertFileThread ( void * arg_file )
{
  sWavFile * loc_pFile = reinterpret_cast<sWavFile *>( arg_file );
  sWavFile loc_file = *loc_pFile;
  convertFile( loc_file );

  pthread_mutex_lock( &mutexThreadsCounter );
  threadsCounter++;
  pthread_cond_signal( &condThreadsAvailable );
  pthread_mutex_unlock( &mutexThreadsCounter );

  pthread_exit( NULL );
  return NULL;
}
