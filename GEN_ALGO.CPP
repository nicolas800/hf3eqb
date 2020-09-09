/*
 * 	gen_flt.cpp
 *		Nicolas Witczak septembre 1995
 *		algorithmes generales
 *		pour traitement audio
 *		fichier implementation
 */

#include <math.h>
#include "gen_algo.h"

/////////////////////////////////////////
// function implementation
double
	FrequToOctave(double frequ,double refFrequ )
	{
		return LOG2(frequ/refFrequ);
	}

double
	OctaveToFrequ(double octave,double refFrequ )
	{
		return ALOG2(octave)*refFrequ;
	}


void
	SampleFrequRsp(DWORD samplFrq,SWORD* pGain,double* pFrequList,int cbFrequ
		,double*& pCoef,SDWORD& cbCoef)
	{
      samplFrq/=2;
		DWORD cbFrqStep=(WORD)((samplFrq/pFrequList[0])*2)-1  ;
		cbCoef=cbFrqStep+1;
		double FrqStep=((double)samplFrq)/cbFrqStep;
		pCoef=new double[cbCoef];

		pCoef[0]=DB_TO_LIN(pGain[0]);
		pCoef[cbFrqStep]=DB_TO_LIN(pGain[cbFrequ-1]);
		int idFrequList=0;
		SWORD idx;
		for(idx=1;idx<cbFrqStep;idx++)
		{
			if( FrqStep*idx <= pFrequList[0] )
				pCoef[idx]=DB_TO_LIN(pGain[0]);
			else if( FrqStep*idx >=pFrequList[cbFrequ-1]  )
				pCoef[idx]=DB_TO_LIN(pGain[cbFrequ-1]);
			else
			{
				for(;pFrequList[idFrequList+1]<FrqStep*idx;idFrequList++);
				pCoef[idx]=pGain[idFrequList]
					+(
						(pGain[idFrequList+1]-pGain[idFrequList])
						*(LOG2(FrqStep*idx)-LOG2(pFrequList[idFrequList]))
						/(LOG2(pFrequList[idFrequList+1])-LOG2(pFrequList[idFrequList]))
					);
				pCoef[idx]=DB_TO_LIN(pCoef[idx]);
			}
		}
	}

double*
	MkCosTable(int iDen)
	{
		double* pRetTable = new double[iDen];
		for(int iDCur=0;iDCur<iDen;iDCur++)
			pRetTable[iDCur] = cos((2*PI*iDCur)/iDen);
		return  pRetTable ;
	}

inline double
	AccCos(double* pCostable,long den ,long Idx)
	{
		if(Idx<0)
			Idx = - Idx ;   // cos is even function
		Idx %= den ;
		return pCostable[Idx] ;
	}

void
	InvDCT(double*& pArray,int cbArray)
	{
		int iT = (2*cbArray)-1 ;
		double* pDest=new double[cbArray];
		double* pCosTb = MkCosTable(iT);
		int idx,idFrequList;
		for(idx=0;idx<cbArray;idx++)
		{
			pDest[idx]=pArray[0];
			for(idFrequList=1;idFrequList<cbArray;idFrequList++)
				pDest[idx]+=2*
				AccCos(pCosTb,iT,((long)idFrequList)*idx)
					*(pArray[idFrequList]);
			pDest[idx]/=(2*cbArray)-1;
		}
		delete [] pArray;
		delete [] pCosTb;
		pArray=pDest;
	}




