#include <stdio.h>

#include "wav.h"

uint8_t wav_loadheader(const char* path, WAVHEADER* header){
	FILE * fp;

	fp = fopen(path, "r");

	if(!fp)
		return 0;

	fgets( (char *) header, sizeof(WAVHEADER), (FILE*)fp );
	fclose( fp );
	
	return 1;
}
