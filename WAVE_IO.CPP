/*
 *
 *    wave_io.h
 *    entrée sorties sur fichier wave
 *    Nicolas Witczak aout 1995
 *		fichier header
 */

#include <assert.h>
#include <io_ext\sdstream.h>
#include "wave_io.h"

CFile&
	operator>>(CFile& file ,SRiffForm& sRiff )
	{
		file >> sRiff.m_dwChunkID >> sRiff.m_dwSize;
		return file;
	}

CFile&
	operator<<(CFile& file ,const SRiffForm& sRiff )
	{
		file << sRiff.m_dwChunkID << sRiff.m_dwSize;
		return file;
	}

CFile&
	operator>>(CFile& file ,SWaveHeaderForm& sWaveInfo )
	{
		file >> sWaveInfo.m_sChunk;
		assert( sWaveInfo.m_sChunk.m_dwChunkID == CHUNK_WAVE_fmt );
		file >> sWaveInfo.m_wFormat >> sWaveInfo.m_wChannel;
		file >> sWaveInfo.m_dwFrequ >> sWaveInfo.m_dwDataRate;
		file >> sWaveInfo.m_wBlockAlign >> sWaveInfo.m_wBitPerSample ;
		return file;
	}

CFile&
	operator<<(CFile& file ,const SWaveHeaderForm& sWaveInfo )
	{
		assert(sWaveInfo.m_sChunk.m_dwChunkID == CHUNK_WAVE_fmt) ;
		file << sWaveInfo.m_sChunk;
		file << sWaveInfo.m_wFormat << sWaveInfo.m_wChannel;
		file << sWaveInfo.m_dwFrequ << sWaveInfo.m_dwDataRate;
		file << sWaveInfo.m_wBlockAlign << sWaveInfo.m_wBitPerSample ;
		return file;
	}

CFile&
	operator>>(CFile& file ,SWaveDataForm& sWaveData )
	{
		file >> sWaveData.m_sChunk;
		assert( sWaveData.m_sChunk.m_dwChunkID == CHUNK_WAVE_data );
		return file;
	}

CFile&
	operator<<(CFile& file ,const SWaveDataForm& sWaveData )
	{
		assert( sWaveData.m_sChunk.m_dwChunkID == CHUNK_WAVE_data );
		file << sWaveData.m_sChunk;
		return file;
	}

CFile&
	operator>>(CFile& file ,SWaveForm& sWave )
	{
		file >> sWave.m_dwChunkID;
		assert( sWave.m_dwChunkID == CHUNK_WAVE );
		file >> sWave.m_sHeader  ;
		file >> sWave.m_sData ;
		return file;
	}

CFile&
	operator<<(CFile& file ,const SWaveForm& sWave )
	{
		assert( sWave.m_dwChunkID == CHUNK_WAVE );
		file << sWave.m_dwChunkID ;
		file << sWave.m_sHeader;
		file << sWave.m_sData ;
		return file ;
	}


