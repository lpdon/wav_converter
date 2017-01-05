#ifndef WAV_H
#define WAV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
	char chunckID[4];
	uint32_t chunckSize;
	uint32_t format;
	char subchunck1ID[4];
	uint32_t subchunck1Size;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	char subchunck2ID[4];
	uint32_t subchunck2Size;
}WAVHEADER;
#pragma pack(pop)

uint8_t wav_loadheader(const char* path, WAVHEADER* header); 

#endif
