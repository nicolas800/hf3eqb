/*
 * 	gen_para.cpp
 *		Nicolas Witczak septembre 1995
 *		modèle parametre de filtre pour traitement audio
 *		fichier header
 */

#ifndef	GEN_PARA_H
	#define GEN_PARA_H

#include <assert.h>
#include <portable\types.h>
 #include <io_ext\sdstream.h>

/////////////////////////////////////////
// classes

/* descr : generic class to hand parameters to the filter obj
*/
class CDIWParam
{
	public:
// info about samples
	DWORD	m_cbSample;
	DWORD m_SampleFreq;
	WORD m_cbChannels;
	WORD m_bitPerSample;

// treatement progression
	DWORD m_cbTotalBlocks;
	DWORD m_cbBlockRemain;  //  include last block(m_cbOfLastBlock+m_cbFlushBlock)

	DWORD m_cbOfMiddleBlock ;
	DWORD m_cbOfLastBlock ;
	DWORD m_cbOfFlushBlock ;

// constructor , destructor
	CDIWParam() ;
	~CDIWParam() ;
};



/////////////////////////////////////////
// function prototypes

CFile&
	operator>>(CFile& file,CDIWParam& param);
CFile&
	operator<<(CFile& file,CDIWParam& param);

/////////////////////////////////////////
// macro implementation

/////////////////////////////////////////
// templates function implementation



#endif	//GEN_PARA_H
