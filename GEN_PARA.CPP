/*
 * 	gen_para.cpp
 *		Nicolas Witczak septembre 1995
 *		modèle de parametre de filtre pour traitement audio
 *		fichier header
 */

#include <io_ext\sdstream.h>
#include "wave_io.h"
#include "gen_para.h"

/////////////////////////////////////////
// function implementation

CDIWParam::CDIWParam()
{
	m_cbOfMiddleBlock = 4096 ;
	m_cbSample = 0 ;
	m_cbBlockRemain = 0 ;
}

CDIWParam::~CDIWParam()
{}


CFile&
	operator>>(CFile& file,CDIWParam& param)
	{
		SRiffForm riff;
		SWaveForm waveInfo ;

		// load wave info first
		file >> riff >> waveInfo ;
		// then store it in our structure
		param.m_SampleFreq   = waveInfo.m_sHeader.m_dwFrequ;
		param.m_cbChannels   = waveInfo.m_sHeader.m_wChannel;
		param.m_bitPerSample = waveInfo.m_sHeader.m_wBitPerSample;
		param.m_cbSample =
			( waveInfo.m_sData.m_sChunk.m_dwSize / param.m_cbChannels)
				/ ( param.m_bitPerSample / 8 ) ;
		return file ;
	}

CFile&
	operator<<(CFile& file,CDIWParam& param)
	{
		SRiffForm riff;
		SWaveForm waveInfo ;
		// fill RIFF structures first
			// wave chunk
		waveInfo.m_dwChunkID = CHUNK_WAVE ;	
			// wave header
		waveInfo.m_sHeader.m_sChunk.m_dwChunkID	= CHUNK_WAVE_fmt ;
		waveInfo.m_sHeader.m_sChunk.m_dwSize		= cbWAVE_HEADER_INFO ;
		waveInfo.m_sHeader.m_wFormat = WAVE_FORMAT_PCM ;
		waveInfo.m_sHeader.m_wChannel = param.m_cbChannels;
		waveInfo.m_sHeader.m_dwFrequ  = param.m_SampleFreq;
		waveInfo.m_sHeader.m_wBitPerSample = param.m_bitPerSample;
		waveInfo.m_sHeader.m_wBlockAlign =(WORD)
			(param.m_cbChannels * (param.m_bitPerSample / 8)) ;
		waveInfo.m_sHeader.m_dwDataRate  = waveInfo.m_sHeader.m_wBlockAlign *
			param.m_SampleFreq ;
			// wave data
		waveInfo.m_sData.m_sChunk.m_dwChunkID = CHUNK_WAVE_data ;
		waveInfo.m_sData.m_sChunk.m_dwSize =
			waveInfo.m_sHeader.m_wBlockAlign * param.m_cbSample ;
			// RIFF header
		riff.m_dwChunkID	= CHUNK_RIFF;
		riff.m_dwSize = waveInfo.m_sData.m_sChunk.m_dwSize + cbWAVE_INFO;
		// then store them
		file << riff << waveInfo ;
		return file ;
	}

