/*
 * 	gen_para.cpp
 *		Nicolas Witczak septembre 1995
 *		modèle parametre de filtre pour traitement audio
 *		fichier header
 */

#ifndef	LIN_PARA_H
	#define LIN_PARA_H

#include <assert.h>
#include <portable\types.h>
#include <io_ext\sdstream.h>

#include "gen_para.h"

/////////////////////////////////////////
// classes

/* descr : generic class to hand parameters to the filter obj
*/
class CLinFltParam : public CDIWParam
{
	public:
	CLinFltParam();
	~CLinFltParam();
	// specialized
	WORD m_ShiftVal;
	double m_Gain;
	int cbFrequ;
	SWORD* m_pGain;		//in dB
	double* m_pFreqList;
	int m_iPrecision;	// in dB
};


#endif	//LIN_PARA_H
