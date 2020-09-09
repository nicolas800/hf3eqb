/*
 * 	flt_proc.h
 *		Nicolas Witczak septembre 1995
 *		boucle principale de traitement audio
 *		fichier header
 */

#ifndef	FLT_PROC_H
	#define FLT_PROC_H
#include <assert.h>
#include <io_ext\sdstream.h>
#include <portable\types.h>
#include <scalaire\int_algo.h>

#include "gen_para.h"
#include "audiobuf.h "
#include "media_io.h"

/////////////////////////////////////////
// function prototypes

	//sound process dispatcher
template < class TObjIn,class TParam,class TSoundObj > inline void
	ProcessLinInit(TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight);

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > inline void
	ProcessLinBlock(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight);

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > inline void
	ProcessLinLast(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight);

template < class TOut,class TIn,class TParam,class TSoundObj > bool
	ProcessLinWave(TOut& outStream
		,TIn& inStream,TParam& param,TSoundObj& SoundProc);

	// specialized sound process

template < class TObjIn,class TParam , class TSoundObj > void
	ProcessLinInitMono(TObjIn& inObj,CAudioBuf& bufMono
		,TParam& param,TSoundObj& SoundProc);

template < class TObjOut,class TObjIn,class TParam , class TSoundObj > void
	ProcessLinBlockMono(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufMono
		,TParam& param,TSoundObj& SoundProc);

template < class TObjOut,class TObjIn,class TParam , class TSoundObj > void
	ProcessLinLastMono(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufMono
		,TParam& param,TSoundObj& SoundProc);


template < class TObjIn,class TParam,class TSoundObj > void
	ProcessLinInitStereo(TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight);

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinBlockStereo(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight);

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinLastStereo(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight);



/////////////////////////////////////////
// templates function implementation

	/////////////////////////////////////////
	//sound process dispatcher

#define PROCESS_FIRST  0
#define PROCESS_MIDDLE 1
#define PROCESS_LAST   2



template < class TObjIn,class TParam,class TSoundObj > void
	ProcessLinInit(TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight)
	{
		if(param.m_cbChannels == 1)
			ProcessLinInitMono(inObj,bufLeft,param,SoundProcLeft);
		else	// stereo sound
			ProcessLinInitStereo(inObj,bufLeft,
				bufRight,param,SoundProcLeft,SoundProcRight);
	}

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinBlock(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight)
	{
		if(param.m_cbChannels == 1)
			ProcessLinBlockMono(outObj,inObj,bufLeft,param,SoundProcLeft);
		else
			ProcessLinBlockStereo(outObj,inObj,bufLeft,bufRight,param,
				SoundProcLeft,SoundProcRight);
	}

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinLast(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight)
	{
		if(param.m_cbChannels == 1)
			ProcessLinLastMono(outObj,inObj,bufLeft,param,SoundProcLeft);
		else
			ProcessLinLastStereo(outObj,inObj,bufLeft,bufRight,
				param,SoundProcLeft,SoundProcRight);
	}

template < class TOut,class TIn,class TParam,class TSoundObj > bool
	ProcessLinWave(TOut& outStream
		,TIn & inStream,TParam& param,TSoundObj& SoundProc)
	{
		static int ProcessStep = PROCESS_FIRST ;
		static CIn8File  inObj8Stream;
		static CIn16File inObj16Stream;
		static COut8File outObj8Stream;
		static COut16File outObj16Stream;
		static CAudioBuf inBufLeft,inBufRight;
		static TSoundObj SoundProcRight;
		bool retVal ;
		switch(ProcessStep)
		{
			case PROCESS_FIRST:
			{
				inObj8Stream.Init(inStream);
				inObj16Stream.Init(inStream);
				outObj8Stream.Init(outStream);
				outObj16Stream.Init(outStream);
				retVal = true ;
				ProcessStep = PROCESS_MIDDLE;
				if(param.m_bitPerSample == 8 )
					ProcessLinInit(inObj8Stream,inBufLeft,
						inBufRight,param,SoundProc,SoundProcRight);
				else
					ProcessLinInit(inObj16Stream,inBufLeft,
						inBufRight,param,SoundProc,SoundProcRight);
				break;
			}
			case PROCESS_MIDDLE:
			{
				if(param.m_bitPerSample == 8 )
					ProcessLinBlock(outObj8Stream,inObj8Stream
						,inBufLeft,inBufRight,param,SoundProc,SoundProcRight);
				else
					ProcessLinBlock(outObj16Stream,inObj16Stream,inBufLeft
						,inBufRight,param, SoundProc,SoundProcRight);
				if(param.m_cbBlockRemain == 1 )
					ProcessStep = PROCESS_LAST;
				retVal = true ;
				break;
			}
			case PROCESS_LAST :
			{
				if(param.m_bitPerSample == 8 )
					ProcessLinLast(outObj8Stream,inObj8Stream,inBufLeft
						,inBufRight,param,SoundProc,SoundProcRight);
				else
					ProcessLinLast( outObj16Stream ,inObj16Stream,inBufLeft
						,inBufRight,param,SoundProc,SoundProcRight);
				retVal = false ; // no more job to do
				ProcessStep = PROCESS_FIRST;
				break;
			}
		}
		return retVal ;
	}


	/////////////////////////////////////////
	// specialized sound process

template < class TObjIn,class TParam,class TSoundObj > void
	ProcessLinInitMono(TObjIn& inObj,CAudioBuf& bufMono
		,TParam& param,TSoundObj& SoundProc)
	{
		SDWORD idx;
		SDWORD* pCurSmp ;

		// init sequence
		SoundProc.PreInit(param);
		bufMono.Init(SoundProc.BufferMin(),SoundProc.BufferMax());

		param.m_cbOfFlushBlock = bufMono.GetMax()-bufMono.GetCur() ;

		// init TParam object
		assert(param.m_cbSample >= param.m_cbOfFlushBlock );
		param.m_cbTotalBlocks = ( (param.m_cbSample - param.m_cbOfFlushBlock )
			 /param.m_cbOfMiddleBlock)  ;
		param.m_cbOfLastBlock = param.m_cbSample -
			(param.m_cbOfMiddleBlock*(param.m_cbTotalBlocks-1) + param.m_cbOfFlushBlock) ;
		param.m_cbBlockRemain = param.m_cbTotalBlocks ;
		assert((param.m_cbTotalBlocks - 1 )*param.m_cbOfMiddleBlock +
			param.m_cbOfFlushBlock + param.m_cbOfLastBlock == param.m_cbSample );

		// init future part of the buffer
		pCurSmp = bufMono.GetCur();
		pCurSmp[1] = 0 ;
		for(idx = 1;idx <= param.m_cbOfFlushBlock;idx++)
			inObj >> pCurSmp[idx] ;

		// fill past part of buffer assuming continuous wave
		for(idx = bufMono.GetMin()-bufMono.GetCur();idx <=0;idx++)
			pCurSmp[idx] = pCurSmp[1] ;

		SoundProc.PostInit(bufMono,bufMono);
	}


template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinBlockMono(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufMono
		,TParam& param,TSoundObj& SoundProc)
	{
		DWORD cbSample = param.m_cbOfMiddleBlock ;
		for(;cbSample>0;cbSample--)
		{
			inObj >> bufMono ;
			outObj << SoundProc.MainProc(bufMono,bufMono) ;
		}
		param.m_cbBlockRemain-- ;
	}


template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinLastMono(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufMono
		,TParam& param,TSoundObj& SoundProc)
	{
		DWORD cbSample = param.m_cbOfLastBlock ;
		for(;cbSample>0;cbSample--)
		{
			inObj >> bufMono ;
			outObj << SoundProc.MainProc(bufMono,bufMono) ;
		}
		cbSample = param.m_cbOfFlushBlock ;
		for(;cbSample>0;cbSample--)
		{
			bufMono.Shift(); // assume future part of sound is continuous
			*(bufMono.GetMax())=*(bufMono.GetMax()-1) ;
			outObj << SoundProc.MainProc(bufMono,bufMono) ;
		}
	}

template < class TObjIn,class TParam,class TSoundObj > void
	ProcessLinInitStereo(TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight)
	{
		SDWORD idx;
		SDWORD* pCurSmpLeft,*pCurSmpRight ;

		// init sequence
		SoundProcLeft.PreInit(param);
		SoundProcRight.PreInit(param);

		bufLeft.Init(SoundProcLeft.BufferMin(),SoundProcLeft.BufferMax());
		bufRight.Init(SoundProcRight.BufferMin(),SoundProcRight.BufferMax());

		// init TParam object
		param.m_cbOfFlushBlock = bufLeft.GetMax()-bufLeft.GetCur() ;
		assert(param.m_cbSample >= param.m_cbOfFlushBlock );
		param.m_cbTotalBlocks = ( (param.m_cbSample - param.m_cbOfFlushBlock )
			 /param.m_cbOfMiddleBlock)  ;
		param.m_cbOfLastBlock = param.m_cbSample -
			(param.m_cbOfMiddleBlock*(param.m_cbTotalBlocks-1) + param.m_cbOfFlushBlock) ;
		param.m_cbBlockRemain = param.m_cbTotalBlocks ;
		assert((param.m_cbTotalBlocks - 1 )*param.m_cbOfMiddleBlock +
			param.m_cbOfFlushBlock + param.m_cbOfLastBlock == param.m_cbSample );

		// init future part of the buffer
		pCurSmpLeft = bufLeft.GetCur();
		pCurSmpLeft[1] = 0 ;
		pCurSmpRight = bufRight.GetCur();
		pCurSmpRight[1] = 0 ;
		for(idx = 1;idx <= param.m_cbOfFlushBlock;idx++)
			inObj >> pCurSmpLeft[idx],inObj >> pCurSmpRight[idx] ;

		// fill past part of buffer assuming continuous wave
		for(idx = bufLeft.GetMin()-bufLeft.GetCur();idx <=0;idx++)
			pCurSmpLeft[idx] = pCurSmpLeft[1],pCurSmpRight[idx] = pCurSmpRight[1] ;


		SoundProcLeft.PostInit(bufLeft,bufRight);
		SoundProcRight.PostInit(bufRight,bufLeft);
	}

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinBlockStereo(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight)
	{
		DWORD cbSample = param.m_cbOfMiddleBlock ;
		for(;cbSample>0;cbSample--)
		{
			inObj >> bufLeft , inObj >> bufRight  ;
			outObj << SoundProcLeft.MainProc(bufLeft,bufRight) ;
			outObj << SoundProcRight.MainProc(bufRight,bufLeft) ;
		}
		param.m_cbBlockRemain-- ;
	}

template < class TObjOut,class TObjIn,class TParam,class TSoundObj > void
	ProcessLinLastStereo(TObjOut& outObj,TObjIn& inObj,CAudioBuf& bufLeft
		,CAudioBuf& bufRight,TParam& param,TSoundObj& SoundProcLeft,TSoundObj& SoundProcRight)
	{
		DWORD cbSample = param.m_cbOfLastBlock ;
		for(;cbSample>0;cbSample--)
		{
			inObj >> bufLeft , inObj >> bufRight  ;
			outObj << SoundProcLeft.MainProc(bufLeft,bufRight) ;
			outObj << SoundProcRight.MainProc(bufRight,bufLeft) ;
		}
		cbSample = param.m_cbOfFlushBlock ;
		for(;cbSample>0;cbSample--)
		{
			bufLeft.Shift(),bufRight.Shift(); // assume future part of sound is continuous
			*(bufLeft.GetMax())=*(bufLeft.GetMax()-1) ;
			outObj << SoundProcLeft.MainProc(bufLeft,bufRight) ;
			outObj << SoundProcRight.MainProc(bufRight,bufLeft) ;
		}
	}

#endif	//FLT_PROC_H
