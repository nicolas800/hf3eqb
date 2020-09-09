/*
 *
 *    media_io.h
 *    entrée sorties sur fichier wave
 *    Nicolas Witczak aout 1995
 *		fichier header
 */

#include <assert.h>
#include <io_ext\sdstream.h>
#include "media_io.h"

CIn8File::CIn8File(CFile& inFile)
{
	m_File = inFile ;
}

void
	CIn8File::Init(CFile& inFile)
{
	m_File = inFile ;
}

COut8File::COut8File(CFile& inFile)
{
	m_File = inFile ;
}

void
	COut8File::Init(CFile& inFile)
{
	m_File = inFile ;
}

CIn16File::CIn16File(CFile& inFile)
{
	m_File = inFile ;
}

void
	CIn16File::Init(CFile& inFile)
{
	m_File = inFile ;
}

COut16File::COut16File(CFile& inFile)
{
	m_File = inFile ;
}

void
	COut16File::Init(CFile& inFile)
{
	m_File = inFile ;
}
