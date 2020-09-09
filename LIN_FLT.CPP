/*
 * 	lin_flt.cpp
 *		Nicolas Witczak septembre 1995
 *		filtre linéaire transversal
 *		fichier implementation
 */
#include <math.h>

#include "audiobuf.h"
#include "lin_flt.h"
#include "gen_algo.h"

CLPFilter::CLPFilter()
{
	m_pImpRsp1 = 0;
	m_pImpRspMax1 = 0;
	m_pImpRsp2 = 0;
	m_pImpRspMax2 = 0;
	m_wSizeofImpRsp = 0 ;
	m_pTmpSumPlus = 0;
	m_pTmpSumPlusMax = 0;
	m_pTmpSumMinus = 0;
	m_pTmpSumMinusMax = 0;
};

void
	CLPFilter::CleanUp()
	{
		if(m_pImpRsp1 != 0)
		{
			delete m_pImpRsp1;
			delete m_pImpRsp2;
			delete m_pTmpSumPlus;
			delete m_pTmpSumMinus;
			m_pImpRsp1 = 0;
			m_pImpRsp2 = 0;
			m_pTmpSumPlus = 0;
         m_pTmpSumMinus = 0;
		}
		m_wSizeofImpRsp = 0 ;
	}

CLPFilter::~CLPFilter()
{	CleanUp(); };


void
	CLPFilter::PreInit(const CLinFltParam& param)
	{
		double* pFrequCoef,tmpDouble;
		SDWORD cbFrequCoef,idx;
		double theslord = DB_TO_LIN(-param.m_iPrecision);
		double linGain = DB_TO_LIN(param.m_Gain);

		// delete previous impulsional response
		CleanUp();

		// perform Fourier transorm
		SampleFrequRsp(param.m_SampleFreq,param.m_pGain,param.m_pFreqList
			,param.cbFrequ,pFrequCoef,cbFrequCoef);
		InvDCT(pFrequCoef,cbFrequCoef);

		// compute impulsionnal response size
		for(idx=cbFrequCoef-1;Abs(pFrequCoef[idx])<=theslord;idx--) ;
		idx = ( ((idx-LPF_IMPRSP1) / LPF_GRANULARITY ) * LPF_GRANULARITY ) + LPF_IMPRSP1;
		for(;(idx>=cbFrequCoef)||(idx>=LPF_MAX_IMPRSP);idx-=LPF_GRANULARITY);
		if(idx<=LPF_IMPRSP1)
			idx = LPF_IMPRSP1  ;
		assert( (idx >= LPF_IMPRSP1) && (idx<=LPF_MAX_IMPRSP) && (idx<cbFrequCoef) ) ;
		m_wSizeofImpRsp = idx + 1 ;

		//generate impulsionnal response
		m_pImpRsp1 = new SDWORD[LPF_IMPRSP1+1];
		m_pImpRspMax1  = m_pImpRsp1 + LPF_IMPRSP1 ;

		idx = (((m_wSizeofImpRsp-1)-LPF_IMPRSP1)/LPF_GRANULARITY) ;
		m_pImpRsp2 = new SDWORD[idx+1];
		m_pImpRspMax2  =  m_pImpRsp2 + idx - 1 ;

		for(idx=0;idx<=LPF_IMPRSP1;idx++)
		{
			tmpDouble = (linGain*pFrequCoef[idx])*LPF_IMPRSP_GAIN;
			m_pImpRsp1[idx]=((SDWORD)(tmpDouble));
		}

		SWORD tmpIdx2=0;
		while(idx<m_wSizeofImpRsp)
		{
			tmpDouble = 0;
			for(WORD i=0;i<LPF_GRANULARITY;i++)
				tmpDouble += (linGain*pFrequCoef[idx++])*LPF_IMPRSP_GAIN ;
			tmpDouble/= LPF_GRANULARITY ;
			m_pImpRsp2[tmpIdx2]=((SDWORD)(tmpDouble));
			tmpIdx2++;
		}
		delete pFrequCoef ;
	}

void
	CLPFilter::PostInit(CAudioBuf& mainInBuf,CAudioBuf& otherInBuf)
	{
      // initialization of partials sums
		SDWORD dwTmpsize,idx,*pBufPlus,*pBufMinus  ;
		dwTmpsize = m_pImpRspMax2 - m_pImpRsp2  ;
		pBufPlus=mainInBuf.GetMax()-1;
		pBufMinus=mainInBuf.GetMin()+1;
		m_pTmpSumPlus = new SDWORD[dwTmpsize+1];
		m_pTmpSumMinus = new SDWORD[dwTmpsize+1];
		m_pTmpSumPlusMax = m_pTmpSumPlus + dwTmpsize ;
		m_pTmpSumMinusMax = m_pTmpSumMinus + dwTmpsize ;
		for(;dwTmpsize>=0;dwTmpsize--)
		{
			m_pTmpSumPlus[dwTmpsize]=0;
			m_pTmpSumMinus[dwTmpsize]=0;
			for(idx=0;idx<LPF_GRANULARITY;idx++)
			{
				m_pTmpSumPlus[dwTmpsize]+=*pBufPlus--;
				m_pTmpSumMinus[dwTmpsize]+=*pBufMinus++;
			}
		}
	}


SDWORD
	CLPFilter::MainProc(CAudioBuf& mainInBuf,CAudioBuf& otherInBuf)
	{
		SDWORD dwSum,*pFltCur,*m_pTmpSumPlusCur,*m_pTmpSumMinusCur,*pBufPlus,*pBufMinus,outSmp;
		dwSum=0;
		pBufPlus=mainInBuf.GetMax()-1;
		pBufMinus=mainInBuf.GetMin();
		pFltCur=m_pImpRspMax2;
		m_pTmpSumPlusCur  = m_pTmpSumPlusMax  ;
		m_pTmpSumMinusCur = m_pTmpSumMinusMax ;
		for(;pFltCur>=m_pImpRsp2;pFltCur--)
		{
			*m_pTmpSumMinusCur-=*pBufMinus;
			pBufMinus+=LPF_GRANULARITY;
			*m_pTmpSumMinusCur+=*pBufMinus;
			dwSum +=((*pFltCur)*(*m_pTmpSumMinusCur--));
			*m_pTmpSumPlusCur+=*pBufPlus;
			pBufPlus-=LPF_GRANULARITY;
			*m_pTmpSumPlusCur-=*pBufPlus;
			dwSum +=((*pFltCur)*(*m_pTmpSumPlusCur--));
		}
		pFltCur=m_pImpRspMax1;
		sumPart1 ;
		dwSum += ((*m_pImpRsp1)*(*pBufPlus));
		outSmp=(	dwSum	/ LPF_IMPRSP_GAIN );
		return outSmp ;
	}


/*
SDWORD
	CLPFilter::MainProc(CAudioBuf& mainInBuf,CAudioBuf& otherInBuf)
	{
		SDWORD dwSum,dwTmpSum,*pFltCur,*pBufPlus,*pBufMinus,outSmp;
		dwSum=0;
		pBufPlus=mainInBuf.GetMax()-1;
		pBufMinus=mainInBuf.GetMin()+1;
		pFltCur=m_pImpRspMax2;
		for(;pFltCur>=m_pImpRsp2;pFltCur--)
		{
			dwTmpSum = 0;
			sumPart2;
			dwSum +=((*pFltCur)*dwTmpSum);
		}
		pFltCur=m_pImpRspMax1;
		sumPart1 ;
		dwSum += ((*m_pImpRsp1)*(*pBufPlus));
		outSmp=(	dwSum	/ LPF_IMPRSP_GAIN );
		return outSmp ;
	}
*/


