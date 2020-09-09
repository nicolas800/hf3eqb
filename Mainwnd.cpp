/*
 *        mainwnd.cpp
 *    application windows génerique
 *		partie gestion d'une fenetre principale
 *    Fichier d'implementation
 *    Nicolas Witczak aout 1995
 */

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <math.h>

#include "applic.h"
#include "mainwnd.h"
#ifndef RESOURCE_H
	#define RESOURCE_H
	#include "resource.h"
#endif

#include "maincmd.h"
#include "crsclass.h"

// initialisation variables
static char pszClass[]="hfclass";

DWORD dwStyle = WS_OVERLAPPEDWINDOW	| WS_MINIMIZEBOX | WS_SYSMENU ;
DWORD dwExStyle = WS_EX_ACCEPTFILES | WS_EX_DLGMODALFRAME;
UINT	dwClassstyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;
WORD	initIcon = SOUNDP_ICO;
WORD  initMenu = SOUNDP_MENU;
WORD	initCursor = 0 ;
int	initBrush= LTGRAY_BRUSH;

// store equalizer curors
CCursor* pCursEq[30];
CCursor* pCursGain;

// window management variable

HWND		hThisWnd	= 0;
HICON		hDefIcon = 0;
HPEN		hDKGrayPen = 0;
HPEN		hWhitePen = 0;

bool
	Register()
	{
		WNDCLASS wndclass ;
		bool bRet;
		hDefIcon = LoadIcon(hInst,MAKEINTRESOURCE(SOUNDP_ICO));
		wndclass.style         = dwClassstyle ;
		wndclass.lpfnWndProc   = MainWinProc ;
		wndclass.cbClsExtra    = 0 ;
		wndclass.cbWndExtra    = DLGWINDOWEXTRA ;
		wndclass.hInstance     = hInst ;
		wndclass.hIcon         = hDefIcon ;
		wndclass.hCursor       = LoadCursor(0,IDC_ARROW) ;
		wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		wndclass.lpszMenuName  = MAKEINTRESOURCE(SOUNDP_MENU) ;
		wndclass.lpszClassName =  pszClass;
		bRet=RegisterClass (&wndclass) ;
		return bRet;
	}

bool
	Lauch(int nCmdShow )
	{
		bool bRet;
		hDKGrayPen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
		hWhitePen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
		hThisWnd = CreateDialog(hInst,MAKEINTRESOURCE(SOUNDP_DLG)
			,NULL,MakeProcInstance((FARPROC)MainDialogProc,hInst));
		bRet =(hThisWnd != 0);
		SetWindowXSize(hThisWnd,600);
		SetWindowTitle();
		DragAcceptFiles(hThisWnd,TRUE);
		ShowWindow(hThisWnd,nCmdShow) ;

		int xPrevPos=8;
		float CurFrequ;
		CCursor** ppCurCurs=pCursEq;
		for(int indexCurs=-15;indexCurs<=13;indexCurs++,xPrevPos+=19)
		{
			CurFrequ = (float)(1000.0*pow(2,((double)indexCurs)/3)) ;
			*ppCurCurs=new CCursor(hInst,hThisWnd,19,xPrevPos,CurFrequ);
			ppCurCurs++;
		}
      *ppCurCurs = 0 ; //null terminated array
		pCursGain = new CCursor(hInst,hThisWnd,23,xPrevPos+3);
		return bRet ;
	}

void
	SetWindowXSize(HWND hwnd,int cx)
	{
		RECT sThisWindow;
		GetWindowRect(hwnd,&sThisWindow);
		MoveWindow(hwnd,sThisWindow.left,sThisWindow.top,cx
			,sThisWindow.bottom-sThisWindow.top,TRUE);
		UpdateWindow(hThisWnd) ;
	}

void
	Draw3dFrame(HWND hwnd,HDC hDC)
	{
		HPEN hOldPen ;
		RECT rect;
		DWORD dwVersion ;
		dwVersion = GetVersion();
		if(LOBYTE(LOWORD(dwVersion))< 4 )
		{
			GetClientRect(hwnd,&rect);

			hOldPen = SelectObject(hDC,hWhitePen);
			rect.right--,rect.bottom--;
			MoveToEx(hDC,rect.left,rect.top,0);
			LineTo(hDC,rect.left,rect.bottom);
			MoveToEx(hDC,rect.left,rect.top,0);
			LineTo(hDC,rect.right,rect.top);

			MoveToEx(hDC,rect.left+1,rect.top+1,0);
			LineTo(hDC,rect.left+1,rect.bottom-1);
			MoveToEx(hDC,rect.left+1,rect.top+1,0);
			LineTo(hDC,rect.right-1,rect.top+1);

			SelectObject(hDC,hDKGrayPen);
			MoveToEx(hDC,rect.right,rect.bottom,0);
			LineTo(hDC,rect.left,rect.bottom);
			MoveToEx(hDC,rect.right,rect.bottom,0);
			LineTo(hDC,rect.right,rect.top);

			MoveToEx(hDC,rect.right-1,rect.bottom-1,0);
			LineTo(hDC,rect.left+1,rect.bottom-1);
			MoveToEx(hDC,rect.right-1,rect.bottom-1,0);
			LineTo(hDC,rect.right-1,rect.top+1);

			SelectObject(hDC,hOldPen);
		}
	}

void
	CenterWindow(HWND hwnd)
	{
		RECT sMainScreen;
		RECT sThisWindow;
		int x,y,cx,cy;
		GetWindowRect(GetDesktopWindow(),&sMainScreen);
		GetWindowRect(hwnd,&sThisWindow);
		cx=sThisWindow.right-sThisWindow.left;
		cy=sThisWindow.bottom-sThisWindow.top;
		x=sMainScreen.right/2-cx/2;
		y=sMainScreen.bottom/2-cy/2;
		MoveWindow(hwnd,x,y,cx,cy,TRUE);
	}

LRESULT CALLBACK _export
	MainWinProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
	{
		switch (uMess)
		{
			HANDLE_MSG(hwnd,WM_DESTROY,Cls_OnDestroy);
			HANDLE_MSG(hwnd,WM_INITDIALOG,Cls_OnInitDialog);
			HANDLE_MSG(hwnd,WM_CLOSE,DestroyWindow);
			HANDLE_MSG(hwnd,WM_ERASEBKGND,Cls_OnEraseBkgnd);
			case WM_KEYDOWN :
				HANDLE_WM_KEYDOWN(hwnd,wParam,lParam,Cls_OnKey);
				return 0L;
			case WM_PAINT:
				HANDLE_WM_PAINT(hwnd,wParam,lParam,Cls_OnPaint);
				return DefWindowProc(hwnd,uMess,wParam,lParam);
			case WM_SIZE:
         	return 0L ;
			default:
				return MainDefProc(hwnd,uMess,wParam,lParam) ;
		}
	}


void
	Cls_OnDestroy(HWND hwnd)
	{
		DestroyIcon(hDefIcon);
		DeleteObject(hWhitePen);
		DeleteObject(hDKGrayPen);
      Cls_OnClose();
		PostQuitMessage(0) ;
	}

BOOL
	Cls_OnEraseBkgnd(HWND hwnd, HDC hdc)
	{
		RECT rect;
		HDC hDC;
		GetClientRect(hwnd,&rect);
		hDC=GetDC(hwnd);
		FillRect(hDC,&rect,(HBRUSH)(COLOR_BTNFACE+1));
		ReleaseDC(hwnd,hDC);
		return TRUE ;
	}

void
	Cls_OnPaint(HWND hwnd)
	{
		HDC hDC;
		hDC=GetDC(hwnd);
		HPEN hOldPen ;
		RECT rect;
		GetClientRect(hwnd,&rect);
		rect.left+=2 , rect.right-=2 ;
		hOldPen = SelectObject(hDC,hDKGrayPen);

		MoveToEx(hDC,rect.left,rect.top,0);
		LineTo(hDC,rect.right,rect.top);

		SelectObject(hDC,hWhitePen);
		MoveToEx(hDC,rect.left+1,rect.top+1,0);
		LineTo(hDC,rect.right+1,rect.top+1);

		SelectObject(hDC,hOldPen);

		ReleaseDC(hwnd,hDC);
		FORWARD_WM_PAINT(hwnd,MainDefProc);
	}

void
	Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
	{
		if(vk == VK_F1)
			Cls_OnHelp();
		FORWARD_WM_KEYDOWN(hwnd,vk,cRepeat,flags,MainDefProc);
	}

BOOL
	Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
	{
      CenterWindow(hwnd);
		return FORWARD_WM_INITDIALOG(hwnd,hwndFocus,lParam,DefDlgProc);
	}



