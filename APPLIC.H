/*
 *        gmswin\applic.h
 *    application windows génerique
 *		partie initialisation de l'application
 *    Fichier header
 *    Nicolas Witczak aout 1995
 */

#ifndef APPLIC_H
	#define APPLIC_H

#include <portable\bool.h>
#include <windows.h>
#include <windowsx.h>


////////////////////////////////////////
// external variables of the application

extern char pszAppName[] ;
extern char pszAuthor[];
extern char pszVersion[];
extern char pszDecrApp[];
extern HINSTANCE hInst;
extern HINSTANCE hPrev;
extern MSG msg;
extern char argc ;
extern char** argv;
extern long  dwVerifAppName ;
extern long  dwVerifAuthor ;
////////////////////////////////////////
// function headers

int
	WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance
		,LPSTR lpCmdLine,int nCmdShow);
bool
	InitApplic();
/*descr : one time initialisation , place to register window class*/
bool
	InitInstance(int nCmdShow );
/*descr : one time initialisation , place to lauch windows*/

void
	ProcessCmpLine(LPSTR lpCmdLine);

inline long
	MakeVerifNo(const char* pszString);

inline void
	VerifAllStr();

/////////////////////////////////////////////////////
// inline functions implementation

long
	MakeVerifNo(const char* pszString)
	{
		long ret=0;
		for( ;pszString[1] != '\0';pszString++)
			ret+= ( (((long)(unsigned char)(*pszString))<<8)
				+ ((unsigned char)(*pszString))  ) ^ 0x00e5 ;
		return ret;
	}

void
	VerifAllStr()
	{
		long dwCalcAppName = MakeVerifNo(pszAppName);
		long dwCalcAuthor = MakeVerifNo(pszAuthor);
		if( (dwCalcAppName !=dwVerifAppName ) || (dwCalcAuthor != dwVerifAuthor) )
			FatalAppExit(0,"Programm has been modified ! ");
	}
#endif // APPLIC_H
