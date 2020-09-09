/*
 *        gmswin\applic.cpp
 *    application windows génerique
 *		partie initialisation de l'application
 *    Fichier header
 *    Nicolas Witczak aout 1995
 */

#include <windows.h>
#include <ctl3d.h>

#include "applic.h"
#include "mainwnd.h"
#include "maincmd.h"

////////////////////////////////////////
// variables of the application

char pszAppName[] = "HF 3rd band equalizer";
char pszAuthor[]="Nicolas Witczak 1996";
char pszVersion[] = "version beta.07";
char pszDecrApp[]="win32 shareware application";
long  dwVerifAppName=0x0006f800 ;
long  dwVerifAuthor=0x00069506 ;

HINSTANCE hInst;
HINSTANCE hPrev;
MSG msg;
/* descr : current message being processed*/
char argc ;
char** argv;
/* descr: if command line parsing wished*/


////////////////////////////////////////
// function implementation
int
	WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance
		,LPSTR lpCmdLine,int nCmdShow)
	{
		HANDLE hAccelTable;
	// init external variables
		hInst = hInstance ;
		hPrev = hPrevInstance ;
		ProcessCmpLine(lpCmdLine);
	// application one time init
		if (!hPrevInstance)
			if (!InitApplic())
				return (false);

	// instance init
		if (!InitInstance( nCmdShow ))
			return (false);

		hAccelTable = LoadAccelerators(hInstance, pszAppName);
      VerifAllStr();
	// message loop for the application
		while( GetMessage(&msg,NULL,0,0) )
		{
			if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)  )
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return (msg.wParam); // Returns the value from PostQuitMessage
	}

bool
	InitApplic()
	{
		return Register() ;
	}

bool
	InitInstance(int nCmdShow )
	{
		return Lauch(nCmdShow);
	}

void
	ProcessCmpLine(LPSTR lpCmdLine)
	{
	}



