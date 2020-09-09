/*
 * 	gen_para.cpp
 *		Nicolas Witczak septembre 1995
 *		modèle de parametre de filtre pour traitement audio
 *		fichier header
 */

#include <io_ext\sdstream.h>
#include "wave_io.h"
#include "lin_para.h"

/////////////////////////////////////////
// function implementation
CLinFltParam::CLinFltParam()
{
	cbFrequ = 0;
	m_ShiftVal = 12;
   m_Gain = 0;
   m_iPrecision = 73 ;
	m_pGain = new SWORD[32];
	m_pFreqList = new double[32];
}



CLinFltParam::~CLinFltParam()
{
	delete [] m_pGain;
	delete [] m_pFreqList ;
}


