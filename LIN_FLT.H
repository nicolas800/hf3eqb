/*
 * 	lin_flt.h
 *		Nicolas Witczak septembre 1995
 *		filtre linéaire transversal
 *		fichier source
 */

#ifndef LIN_FLT_H
	 #define LIN_FLT_H

#include "audiobuf.h"
#include "lin_para.h"
#include <portable\bool.h>
#include <portable\types.h>
#include <scalaire\int_algo.h>

#define LPF_GRANULARITY	25
#define LPF_IMPRSPDC		1
#define LPF_IMPRSP1		50
#define LPF_IMPRSP2     500
#define LPF_MAX_IMPRSP	(LPF_IMPRSP1+LPF_IMPRSP2)
#define LPF_IMPRSP_GAIN 4096

//////////////////////////////////////////////////////////
// class CLPFilter : finit impulsionnal response filter
/*desrc : receive at construction time a symetrical impulsionnal response
	(in fact right part of it ) and performs filtering on audio 
*/
class CLPFilter
{
public:
	SDWORD*	m_pImpRsp1;
	SDWORD*	m_pImpRspMax1;	// last valid impulsionnal response value
	SDWORD*	m_pImpRsp2;
	SDWORD*	m_pImpRspMax2;
	SDWORD*	m_pTmpSumPlus;
	SDWORD*	m_pTmpSumPlusMax;
	SDWORD*	m_pTmpSumMinus;
	SDWORD*	m_pTmpSumMinusMax;
	SWORD		m_wSizeofImpRsp;


	public:
	CLPFilter() ;
	void
		CleanUp();
	~CLPFilter() ;
	SDWORD
		BufferMin() const {	return - m_wSizeofImpRsp ;};
	SDWORD
		BufferMax() const {	return m_wSizeofImpRsp ;};
	/* inform about buffer requierement */
	bool
		IsBlockProcess()	{ return false; };
	void
		PreInit(const CLinFltParam& param);
	void
		PostInit(CAudioBuf& mainInBuf,CAudioBuf& otherInBuf) ;
	// effective process
SDWORD
	MainProc(CAudioBuf& mainInBuf,CAudioBuf& otherInBuf);
};


#define sumOncePart1 \
			dwSum+=((*pBufPlus--)+(*pBufMinus++))*(*pFltCur--)

#define sum5timePart1 \
			sumOncePart1; \
			sumOncePart1; \
			sumOncePart1; \
			sumOncePart1; \
			sumOncePart1;


#define sum10timePart1 \
			sum5timePart1;\
			sum5timePart1


#define sumPart1	\
			sum10timePart1;	\
			sum10timePart1;	\
			sum10timePart1;	\
			sum10timePart1;	\
			sum10timePart1

#define sumOncePart2	\
			dwTmpSum += (*pBufPlus--);	\
			dwTmpSum += (*pBufMinus++)

#define sum5timePart2 \
			sumOncePart2;		\
			sumOncePart2;		\
			sumOncePart2;		\
			sumOncePart2;		\
			sumOncePart2


#define sum10timePart2 \
			sum5timePart2;		\
			sum5timePart2

#define sumPart2 \
			sum10timePart2;	\
			sum10timePart2;	\
			sum5timePart2



#endif //LIN_FLT_H
