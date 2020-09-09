/*
 *
 *    wave_io.h
 *    entrée sorties sur fichier wave
 *    Nicolas Witczak aout 1995
 *		fichier header
 */

#ifndef WAVE_IO_H
	#define WAVE_IO_H

#include <assert.h>
#include <io_ext\sdstream.h>
#include <portable\types.h>
#include <scalaire\int_algo.h>


/////////////////////////////////////////
// RIFF constants

#define CHUNK_RIFF		0x46464952UL
#define CHUNK_WAVE      0x45564157UL
#define CHUNK_WAVE_fmt	0x20746d66UL
#define CHUNK_WAVE_data	0x61746164UL
#define CHUNK_INFO      0x4f464e49UL
#ifndef WAVE_FORMAT_PCM
	#define WAVE_FORMAT_PCM 1
#endif // WAVE_FORMAT_PCM

#define cbWAVE_INFO  36
#define cbWAVE_HEADER_INFO 16
/////////////////////////////////////////
// structures

typedef struct tagRiffForm
{
	DWORD	m_dwChunkID;
	DWORD	m_dwSize;
} SRiffForm;

typedef struct tagWaveHeaderForm
{
	tagRiffForm m_sChunk;
//data
	WORD	m_wFormat;	// must be WAVE_FORMAT_PCM
	WORD	m_wChannel;
	DWORD	m_dwFrequ;
	DWORD	m_dwDataRate;
	WORD	m_wBlockAlign;
	WORD	m_wBitPerSample;
} SWaveHeaderForm ;

typedef struct tagWaveDataForm
{
	SRiffForm m_sChunk;
} SWaveDataForm ;

typedef struct tagWaveForm
{
	DWORD	m_dwChunkID;
	/*rq : no size field */
	// info field ->skipped
	SWaveHeaderForm m_sHeader;
	SWaveDataForm	m_sData;
} SWaveForm;



/////////////////////////////////////////
// function prototypes

	// file operations
CFile&
	operator>>(CFile& file ,SRiffForm& sRiff );
CFile&
	operator<<(CFile& file ,const SRiffForm& sRiff );

CFile&
	operator>>(CFile& file ,SWaveHeaderForm& sWaveInfo );
CFile&
	operator<<(CFile& file ,const SWaveHeaderForm& sWaveInfo );

CFile&
	operator>>(CFile& file ,SWaveDataForm& sWaveData );
CFile&
	operator<<(CFile& file ,const SWaveDataForm& sWaveData );

CFile&
	operator>>(CFile& file ,SWaveForm& sWave );
CFile&
	operator<<(CFile& file ,const SWaveForm& sWave );

/////////////////////////////////////////
// template function implementation


#endif //WAVE_IO
