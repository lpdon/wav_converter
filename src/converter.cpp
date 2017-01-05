#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <locale>
#include <inttypes.h>
#include <thread>

#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>

#include "converter.hpp"

extern "C"
{
  #include "lame.h"
}

std::vector<sWavFile> cConverter::readFiles( const std::string arg_dir )
{
  std::vector<sWavFile> loc_files;

  DIR * loc_pDir = opendir( arg_dir.c_str() );

  if ( loc_pDir != NULL)
  {
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

void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}
//
//int main (int argc, char *argv[])
//{
//   pthread_t threads[NUM_THREADS];
//   int rc;
//   long t;
//   for(t=0; t<NUM_THREADS; t++){
//      printf("In main: creating thread %ld\n", t);
//      rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
//      if (rc){
//         printf("ERROR; return code from pthread_create() is %d\n", rc);
//         exit(-1);
//      }
//   }
//
//   /* Last thing that main() should do */
//   pthread_exit(NULL);
//}

bool cConverter::convert( void )
{
  const uint16_t loc_numThreads = std::thread::hardware_concurrency();
  pthread_t loc_threads[ loc_numThreads ];

  std::cout << "Available threads: " << loc_numThreads << std::endl;

//  for ( uint16_t loc_i = 0U; loc_i < files.size(); ++loc_i )
  for ( uint16_t loc_i = 0U; loc_i < loc_numThreads; ++loc_i )
  {
    std::cout << "[Thread" << loc_i << "] : Converting: " << files[ loc_i ].name << std::endl;
    pthread_create( &loc_threads[ loc_i ], NULL, convertFileThread, (void *)&files[ loc_i ] );
//    pthread_create( &loc_threads[ loc_i ], NULL, PrintHello, (void *)loc_i );
//    convertFile( files[ loc_i ] );
  }

  pthread_exit( NULL );
}

void cConverter::convertFile( const sWavFile &arg_file )
{
  const WAVHEADER loc_header = arg_file.header;
  uint32_t loc_wavSizeLeft = loc_header.subchunck2Size;
  const std::string loc_outputPath = dir + arg_file.name + ".mp3";

  std::ifstream loc_input( arg_file.path.c_str(), std::ios::binary );
  std::ofstream loc_output( loc_outputPath.c_str(), std::ios::binary );

  if ( loc_input.is_open() && loc_output.is_open() )
  {
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

      if ( loc_input.gcount() != 0 )
      {
        const int loc_bytesEnc = lame_encode_buffer_interleaved( loc_lame, loc_pcmBuffer, loc_input.gcount() / (2 * sizeof(short int)), loc_mp3Buffer, MP3_SIZE );
        loc_output.write( reinterpret_cast<char *>( loc_mp3Buffer ), loc_bytesEnc );
        loc_wavSizeLeft -= static_cast<uint32_t>( loc_input.gcount() );
      }
      else
      {
        const int loc_bytesFlushed = lame_encode_flush( loc_lame, loc_mp3Buffer, MP3_SIZE );
        loc_output.write( reinterpret_cast<char *>( loc_mp3Buffer ), loc_bytesFlushed );
        loc_wavSizeLeft = 0U;
      }
    }

    std::cout << "Finished converting: " << arg_file.name << std::endl;
    loc_output.close();
    lame_close( loc_lame );
  }

  pthread_exit( NULL );
}

void * cConverter::convertFileThread ( void * arg_file )
{
  sWavFile * loc_pFile = static_cast<sWavFile *>( arg_file );
  convertFile( *loc_pFile );
}
