/*
 * 	gen_flt.cpp
 *		Nicolas Witczak septembre 1995
 *		algorithmes generales
 *		pour traitement audio
 *		fichier header
 */

#ifndef	GEN_ALGO_H
	#define GEN_ALGO_H

#include <assert.h>
#include <math.h>
#include <portable\types.h>
#include <scalaire\int_algo.h>
#include <fstream.h>

/////////////////////////////////////////
// macro implementation

#define PI (4*atan(1))

#define ALOG10(x)		(pow(10,(x)))
#define ALOG2(x)		(pow(2,(x)))
#define DB_TO_LIN(x)	(pow(10,((double)(x))/20))
#define LIN_TO_DB(x)	((log((x))/log(10))*20)
#define LOG2(x)		(log((x))/log(2))
#define LOG10(x)     (log((x))/log(10))


/////////////////////////////////////////
// function prototypes
double
	FrequToOctave(double frequ,double refFrequ=440 );
double
	OctaveToFrequ(double octave,double refFrequ=440 );
void
	SampleFrequRsp(DWORD samplFrq,SWORD* pGain,double* pFrequList,int cbFrequ
		,double*& pCoef,SDWORD& cbCoef);
/* descr in : (pGain,pFrequList,cbFrequ) filter frequ response in dB
				:  samplFrq
			out: (pCoef,cbCoef) filter frequ response (linear scale )
			 with constant step	, pCoef is allocated by the function
*/

double*
	MkCosTable(int iDen);       // make ftc(x)=cos(2*PI*x/den)
double
	AccCos(double* pCostable,long den ,long Idx);


void
	DCT(double*& pArray,int cbArray);
void
	InvDCT(double*& pArray,int cbArray);
#define pfnArrayFileName	"d:\\temp\\array.txt"
template < class T > void
	ArrayToFile(T* pArray,int cbArray);

	// samples adaptation
inline SDWORD
	Wave8ToLong(BYTE bsmp);
inline
	SDWORD Wave16ToLong(WORD wSmp);
inline BYTE
	LongTo8Wave(SDWORD lSmp);
inline WORD
	LongTo16Wave(SDWORD lSmp);
template < class T > void
	ArrayToFile(T* pArray,long cbArray,const char*pszName);

/////////////////////////////////////////
// inline function implementation

inline SDWORD
	Wave8ToLong(BYTE bsmp)
	{
		return (((SDWORD)(DWORD)bsmp)-128);
	}

inline
	SDWORD Wave16ToLong(WORD wSmp)
	{
		return (SDWORD)(SWORD)wSmp;
	}

inline BYTE
	LongTo8Wave(SDWORD lSmp)
	{
		return (BYTE)(DWORD)(lSmp+128);
	}

inline WORD
	LongTo16Wave(SDWORD lSmp)
	{
		return (WORD)(SWORD)lSmp ;
	}

/////////////////////////////////////////
// templates function implementation

template < class T > void
	ArrayToFile(T* pArray,long cbArray,const char*pszName)
	{
		int id;
      ofstream outf(pszName);
		for(id=0;id<cbArray;id++)
			outf << ((double)(pArray[id])) <<"  ";
	}


#endif	//GEN_ALGO_H
