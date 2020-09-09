/*
 *
 *    audiobuf.cpp
 *    gestion d'un buffer sur une sequence audio
 *    Fichier d'implementation
 *    Nicolas Witczak aout 1995
 */

#include "audiobuf.h"
#include "wave_io.h"

/////////////////////////////////////////////
// member function implementation

CAudioBuf::CAudioBuf() { m_pBufMin = 0 ;};

void
	CAudioBuf::Init( SDWORD idxMin ,SDWORD idxMax )
	{
		delete[] m_pBufMin;
		idxMin*=-1;
		DWORD dwBufSize= (1+idxMax+idxMin) * 4 ;
		m_pBufMin = new SDWORD[dwBufSize] ;
		m_pBufMax = m_pBufMin + dwBufSize -1 ;
		m_pWinMin  = m_pBufMin;
		m_pWinRead = m_pWinMin + idxMin ;
		m_pWinMax  = m_pWinRead + idxMax ;
	}

CAudioBuf::CAudioBuf( SDWORD idxMin ,SDWORD idxMax )
	{ Init(idxMin,idxMax); }

CAudioBuf::~CAudioBuf()
{
	delete[] m_pBufMin;
}

void
	CAudioBuf::ShiftToBegin()
	{
		SDWORD* pCurSrc,*pCurDest;
		for(pCurSrc=m_pWinMin+1,pCurDest=m_pBufMin;pCurSrc<=m_pWinMax; )
			*pCurDest++=*pCurSrc++;
		m_pWinRead=m_pBufMin+(m_pWinRead-m_pWinMin);
		m_pWinMax=m_pBufMin+(m_pWinMax-m_pWinMin);
		m_pWinMin=m_pBufMin;
	}

/////////////////////////////////////////////
// global function implementation

