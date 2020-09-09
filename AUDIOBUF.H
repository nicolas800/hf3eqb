/*
 *
 *    audiobuf.h
 *    gestion d'un buffer sur une sequence audio
 *    Nicolas Witczak aout 1995
 *		fichier header
 */

#ifndef AUDIOBUF_H
	#define AUDIOBUF_H

#include <iostream.h>
#include <fstream.h>
#include <portable\types.h>
#include <io_ext\sdstream.h>
#include "wave_io.h"

///////////////////////////////////////////////
// class definition

/* descr: this class handle a window on an audio file
	internal repersentation signed DWORD
*/
class CAudioBuf
	{
		private:
		//members data
		SDWORD*	m_pBufMin;
		SDWORD*	m_pBufMax;
	/* m_pBufMin: first valid in buf , m_pBufMax: last valid in buf
	*/
		SDWORD*	m_pWinMin;
		SDWORD*	m_pWinRead;
		SDWORD*	m_pWinMax;
		//SDWORD m_dwMin,m_dwMax;

	/* m_dwMin: first valid in window ,m_dwMax: last valid in window
	*/
		// member funtion
		public:
		CAudioBuf();
		void
			Init( SDWORD idxMin ,SDWORD idxMax );
		CAudioBuf( SDWORD idxMin ,SDWORD idxMax );
		~CAudioBuf();
		SDWORD*
			GetMin() { return m_pWinMin ;};
		SDWORD*
			GetMax() { return m_pWinMax ;};
		SDWORD*
			GetCur() { return m_pWinRead ;};
		SDWORD&
			GetSmp() { return *m_pWinRead ;};
		SDWORD&
			operator[](SDWORD idx)  { return m_pWinRead[idx] ;};
		void
			ShiftToBegin();
		void
			Shift()
			{
				if( m_pWinMax < m_pBufMax )
				{
					m_pWinMin++;
					m_pWinRead++;
					m_pWinMax++;
				}
				else
					ShiftToBegin();
			}
		SDWORD*
			operator*() { return m_pWinRead;};	// postfixed
	};


/////////////////////////////////////////////
// global function header



#endif //AUDIOBUF_H

