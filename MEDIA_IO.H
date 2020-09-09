/*
 *
 *    media_io.h
 *    entrée sorties sur fichier wave
 *    Nicolas Witczak aout 1995
 *		fichier header
 */

#ifndef MEDIA_IO_H
	#define MEDIA_IO_H

#include <assert.h>
#include <io_ext\sdstream.h>
#include <portable\types.h>
#include "gen_algo.h"
#include "audiobuf.h"

//////////////////////////////////////////////////
// 8 bits adaptors
class CIn8File
{
private:
	CFile	  m_File;
public:
	CIn8File() {};
	CIn8File(CFile& inFile);
	void
		Init(CFile& inFile);
	void
		operator>>(CAudioBuf& buf)
		{
			BYTE bToPush;
			m_File >> bToPush;
			buf.Shift();
			*(buf.GetMax())=Wave8ToLong(bToPush);
		};
	void
		operator>>(SDWORD& dwSmp)
		{
			BYTE bToPush;
			m_File >> bToPush;
			dwSmp=Wave8ToLong(bToPush);
		};
};

class COut8File
{
private:
	CFile	  m_File;
public:
   COut8File() {};
	COut8File(CFile& outFile);
	void
		Init(CFile& inFile);
	void
		operator<<(SDWORD smp)
		{ m_File << LongTo8Wave(smp) ; };
};

//////////////////////////////////////////////////
// 16 bits adaptors
class CIn16File
{
private:
	CFile	  m_File;
public:
	CIn16File() {};
	CIn16File(CFile& inFile);
	void
		Init(CFile& inFile);
	void
		operator>>(CAudioBuf& buf)
		{
			SWORD wToPush;
			m_File >> wToPush;
			buf.Shift();
			*(buf.GetMax())=Wave16ToLong(wToPush);
		};
	void
		operator>>(SDWORD& dwSmp)
		{
			SWORD wToPush;
			m_File >> wToPush;
			dwSmp=Wave16ToLong(wToPush);
		};
};

class COut16File
{
private:
	CFile	  m_File;
public:
	COut16File() {};
	COut16File(CFile& outFile);
	void
		Init(CFile& inFile);
	void
		operator<<(SDWORD smp)
		{ m_File << LongTo16Wave(smp) ; };
};

class COut8Buf
{
	char* m_pCurPtr;
public:
	COut8Buf() {};
	void
		Attach(char* pcBuf) {m_pCurPtr=pcBuf;};
	void
		operator<<(SDWORD smp){ *m_pCurPtr= LongTo8Wave(smp);m_pCurPtr++;};
};

class COut16Buf
{
	char* m_pCurPtr;
public:
	COut16Buf() {};
	void
		Attach(char* pcBuf) {m_pCurPtr=pcBuf;};
	void
		operator<<(SDWORD smp)
	{
		*((WORD*)m_pCurPtr)=LongTo16Wave(smp);
		m_pCurPtr+=2;
	};
};

#endif //MEDIA_IO_H
