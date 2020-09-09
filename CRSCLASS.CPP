/*
 *        clsclass.cpp
 *    definition d'une classe curseur
 *    Fichier d'implementation
 *    Nicolas Witczak octobre 1995
 */

#include <windows.h>
#include <windowsx.h>
#include <portable\bool.h>
#include <stdlib.h>
#include <string.h>
#include "applic.h"
#include "crsclass.h"

////////////////////////////////////////////////
// global data initialisation
bool CCursor::m_InstCount = 0 ;
HPEN CCursor::m_Black ;
HPEN CCursor::m_White ;
HPEN CCursor::m_DkGray ;
HPEN CCursor::m_LtGray ;
HPEN CCursor::m_Red ;
HPEN CCursor::m_Green;
HFONT CCursor::m_NbFont;
static char pszCursClassName[] = "HFCursor";

////////////////////////////////////////////////
// member function implementation
void
	CCursor::StaticInit(HINSTANCE hInst)
	{

	//register window class
		WNDCLASS wc;
		wc.lpfnWndProc   = CursWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = DLGWINDOWEXTRA;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = GetStockObject(NULL_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = pszCursClassName;
		wc.style         = CS_VREDRAW | CS_HREDRAW;
		RegisterClass(&wc);
	 //create pens
		m_Black	= CreatePen(PS_SOLID,1,BLACK);
		m_White	= CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
		m_DkGray	= CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
		m_Red		= CreatePen(PS_SOLID,1,RED);
		m_Green 	= CreatePen(PS_SOLID,1,GREEN);
		m_LtGray = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));

	// create font
		int sizeFont = -8;
		m_NbFont = CreateFont(sizeFont,0,0,FW_THIN,0,0,0,0,0,0,0,0,0,"Arial");
	}

void
	CCursor::StaticDelete()
	{
		DeleteObject(m_Black);
		DeleteObject(m_White);
		DeleteObject(m_DkGray);
		DeleteObject(m_Red);
		DeleteObject(m_Green);
		DeleteObject(m_LtGray);
		DeleteObject(m_NbFont);
	}

CCursor::CCursor(HINSTANCE hInst,HWND hParent,int width,int xpos
		,float frequ,int dBMax)
{
	RECT tmpRect;
	if(m_InstCount == 0)
		StaticInit(hInst);
	m_InstCount++;
	GetClientRect(hParent,&tmpRect);

	//set up internal data members
	m_width = width - 1 ;
	m_Center = m_width / 2 ;
	m_CursUp = tmpRect.top + FROM_BORDER_HEIGHT + 2;
	m_CursDown = tmpRect.bottom -
		(2*FONT_HEIGHT + 2*PARENTFRAME_WIDTH + 3*FROM_BORDER_HEIGHT ) ;
   m_CursDown += 8 ;
	m_CurPos = m_CursUp + ((m_CursDown - m_CursUp)/2);

	dBLinePos = m_CursDown + FROM_BORDER_HEIGHT+ FONT_HEIGHT + 2 ;

	m_FreqTextRect.top    = m_CursDown + 6  ;
	m_FreqTextRect.left   = 1;
	m_FreqTextRect.right  = m_width ;
	m_FreqTextRect.bottom = dBLinePos -2 ;

	m_dBTextRect.top = dBLinePos + 4  ;
	m_dBTextRect.left = 1 ;
	m_dBTextRect.right = m_width ;
	m_dBTextRect.bottom = (tmpRect.bottom-tmpRect.top)-(2*PARENTFRAME_WIDTH)-0;

	m_dBVal = 0 ;
	m_dBMax = dBMax ;
	m_FrequVal = frequ ;

	char* pszFrequ;
	int decPointpos;
	int unused;

	if(m_FrequVal>=1000.0)
		pszFrequ=fcvt(m_FrequVal/1000,1,&decPointpos,&unused);
	else
		pszFrequ=fcvt(m_FrequVal,1,&decPointpos,&unused);
	if(decPointpos < 2)
	{
		pszFrequ[decPointpos+1]=pszFrequ[decPointpos];
		pszFrequ[decPointpos]='.';
		pszFrequ[decPointpos+2]='\0';
	}
	else
		pszFrequ[decPointpos]='\0';
	strcpy(m_pszFrequ,pszFrequ);
	if(frequ == 0.0)
		m_pszFrequ[0]='G',m_pszFrequ[1]='\0';
	// create window
	DWORD	style = WS_CHILD | WS_VISIBLE ;
	m_hwnd = CreateWindow(pszCursClassName,0,style
	,xpos,PARENTFRAME_WIDTH
	,width,(tmpRect.bottom-tmpRect.top)-(2*PARENTFRAME_WIDTH)
	,hParent,0,hInst,0);

	// save our this pointer in the hwnd struct
	SetWindowLong(m_hwnd,DWL_USER,(DWORD)this);
	ReleaseCapture();
	m_bMouseIn = false ; // set inital activation state
	m_bTrackCursor =false ;
	ShowWindow(m_hwnd,SW_SHOWNORMAL );
	UpdateWindow(m_hwnd) ;
}

CCursor::~CCursor()
{

	m_InstCount--;
	if(m_InstCount == 0)
		StaticDelete();
}

void
	CCursor::DrawGuide(HDC hDC)
	{
		SelectObject(hDC,m_LtGray);
		MoveToEx(hDC,m_Center,m_CursUp,0);
		LineTo(hDC,m_Center,m_CursDown+2);
		SelectObject(hDC,m_DkGray);
		MoveToEx(hDC,m_Center+1,m_CursUp-1,0);
		LineTo(hDC,m_Center-1,m_CursUp-1);
		LineTo(hDC,m_Center-1,m_CursDown+2);
		SelectObject(hDC,m_White);
		MoveToEx(hDC,m_Center-1,m_CursDown+2,0);
		LineTo(hDC,m_Center+1,m_CursDown+2);
		LineTo(hDC,m_Center+1,m_CursUp-1);
	}

void
	CCursor::DrawGraduation(HDC hDC)
	{
		int stepGrad = 5 ;
		int idMax = m_dBMax/stepGrad;
		int vertPos ;
		for(int id=0;id<=2*idMax;id++)
		{
			if(id != idMax)
			{
				vertPos = m_CursUp + ((m_CursDown-m_CursUp)*id)/(2*idMax);
				SelectObject(hDC,m_Black);
				MoveToEx(hDC,m_Center+3,vertPos,0);
				LineTo(hDC,m_Center+5,vertPos);
				SelectObject(hDC,m_White);
				MoveToEx(hDC,m_Center+4,vertPos+1,0);
				LineTo(hDC,m_Center+6,vertPos+1);
			}
			else
			{
				vertPos = m_CursUp + ((m_CursDown-m_CursUp)*idMax)/(2*idMax);
				SelectObject(hDC,m_Black);
				MoveToEx(hDC,m_Center+3,vertPos,0);
				LineTo(hDC,m_Center+6,vertPos);
				SelectObject(hDC,m_White);
				MoveToEx(hDC,m_Center+4,vertPos+1,0);
				LineTo(hDC,m_Center+7,vertPos+1);
			}
		}
	}

void
	CCursor::DrawCursor(HDC hDC)
	{
		int curLeft,curRight;
		curLeft = m_Center - (m_width/2) + FROM_BORDER_WIDTH ;
		curRight = m_Center + (m_width/2) - FROM_BORDER_WIDTH ;

		MoveToEx(hDC,curLeft+2,m_CurPos-2,0);
		SelectObject(hDC,m_White);
		LineTo(hDC,curRight-1,m_CurPos-2);

		MoveToEx(hDC,curLeft+2,m_CurPos-1,0);
		SelectObject(hDC,m_LtGray);
		LineTo(hDC,curRight-1,m_CurPos-1);

		MoveToEx(hDC,curLeft+3,m_CurPos,0);
		if(IsMouseInWnd())
			SelectObject(hDC,m_Red);
		else
			SelectObject(hDC,m_DkGray);
		LineTo(hDC,curRight-2,m_CurPos);

		SelectObject(hDC,m_DkGray);
		MoveToEx(hDC,curLeft+2,m_CurPos+1,0);
		LineTo(hDC,curRight-1,m_CurPos+1);
		MoveToEx(hDC,curLeft+2,m_CurPos+3,0);
		LineTo(hDC,curRight,m_CurPos+3);
		LineTo(hDC,curRight,m_CurPos-3);

		SelectObject(hDC,m_Black);
		MoveToEx(hDC,curLeft+1,m_CurPos-3,0);
		LineTo(hDC,curRight-1,m_CurPos-3);
		LineTo(hDC,curRight-1,m_CurPos+2);
		LineTo(hDC,curLeft+1,m_CurPos+2);
		LineTo(hDC,curLeft+1,m_CurPos-3);
	}

void
	CCursor::DrawText(HDC hDC)
	{
		int curLeft,curRight;
		curLeft = m_Center - (m_width/2) + FROM_BORDER_WIDTH ;
		curRight = m_Center + (m_width/2) - FROM_BORDER_WIDTH ;

		if(IsMouseInWnd()) // then print dB value
		{
			char pszdB[4];
			if(m_dBVal > 0) // then add plus sign
			{
				pszdB[0]='+';
				itoa(m_dBVal,pszdB+1,10);
			}
			else
				itoa(m_dBVal,pszdB,10);
			SetTextColor(hDC,GetSysColor(COLOR_MENUTEXT));
			SelectObject(hDC,m_NbFont);
			::DrawText(hDC,pszdB,strlen(pszdB),&m_dBTextRect,DT_SINGLELINE | DT_CENTER | DT_VCENTER );

			SelectObject(hDC,m_DkGray);
			MoveToEx(hDC,curLeft,dBLinePos,0);
			LineTo(hDC,curRight,dBLinePos);

			SelectObject(hDC,m_White);
			MoveToEx(hDC,curLeft+1,dBLinePos+1,0);
			LineTo(hDC,curRight+1,dBLinePos+1);
		}
		SelectObject(hDC,m_NbFont);
		if(IsMouseInWnd())
			SetTextColor(hDC,GetSysColor(COLOR_MENUTEXT));
		else
			SetTextColor(hDC,GetSysColor(COLOR_GRAYTEXT));
		::DrawText(hDC,m_pszFrequ,strlen(m_pszFrequ),&m_FreqTextRect,DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	}

void
	CCursor::DrawAll()
	{
		HDC hDC;
		hDC=GetDC(m_hwnd);
		SetBkMode(hDC,TRANSPARENT);
		PaintBkg(hDC);
		DrawGuide(hDC);
		DrawGraduation(hDC);
		DrawCursor(hDC);
		DrawText(hDC);
		ReleaseDC(m_hwnd,hDC);
	}

void
	CCursor::PaintBkg(HDC hDC)
	{
		RECT rect;
		GetClientRect(m_hwnd,&rect);
		FillRect(hDC,&rect,(HBRUSH)(COLOR_BTNFACE+1));
	}

bool
	CCursor::IsMouseInWnd()
	{
		return m_bMouseIn ;
	}
bool
		IsMouseOnCursor();
////////////////////////////////////////////////
// global function implementation
CCursor*
	This(HWND hwnd)
	{	return (CCursor*)GetWindowLong(hwnd,DWL_USER); }

LRESULT CALLBACK _export
	CursWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
	{
		switch (uMess)
		{
			HANDLE_MSG(hwnd,WM_PAINT,Curs_OnPaint);
			HANDLE_MSG(hwnd,WM_MOUSEMOVE,Curs_OnMouseMove);
			HANDLE_MSG(hwnd,WM_DESTROY,Curs_OnDestroy);
			HANDLE_MSG(hwnd,WM_LBUTTONDOWN,Curs_OnLButtonDown);
         HANDLE_MSG(hwnd,WM_LBUTTONUP,Curs_OnLButtonUp);
			default:
				return DefWindowProc(hwnd,uMess,wParam,lParam);
		}
	}

void
	Curs_OnPaint(HWND hwnd)
	{
		CCursor* pCur = This(hwnd);
		pCur->DrawAll();
		FORWARD_WM_PAINT(hwnd,DefWindowProc);
	}

void
	Curs_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
	{
		CCursor* pCur = This(hwnd);
		RECT rect;
		POINT p;
		GetWindowRect(pCur->m_hwnd,&rect);
		GetCursorPos(&p);
		bool wantsDrawn = false;
		// if gain mouse cursor
		if( (p.x >=rect.left) && (p.x <= rect.right ) &&
			(p.y>=rect.top) && (p.y<=rect.bottom))
		{
			if(pCur->m_bMouseIn == false )
			{
				pCur->m_bMouseIn = true;
				wantsDrawn = true;
				SetCapture(hwnd);
			}
		}
		else // loose  mouse cursor
		{
			if(pCur->m_bMouseIn == true )
			{
				pCur->m_bMouseIn = false;
            pCur->m_bTrackCursor = false ;
				wantsDrawn = true;
				ReleaseCapture();
			}
		}
		if(pCur->m_bTrackCursor) //then update curs pos
		{
			pCur->m_CurPos =p.y - rect.top ;
			if(pCur->m_CurPos >=pCur->m_CursDown)
				pCur->m_CurPos =pCur->m_CursDown ;
			if(pCur->m_CurPos<=pCur->m_CursUp)
				pCur->m_CurPos=pCur->m_CursUp ;
			pCur->m_dBVal = -pCur->m_dBMax +
				(((2*pCur->m_dBMax)*(pCur->m_CursDown-pCur->m_CurPos))
					/(pCur->m_CursDown-pCur->m_CursUp));
			wantsDrawn = true;
		}
		if(wantsDrawn)
			pCur->DrawAll();
		FORWARD_WM_MOUSEMOVE(hwnd,x,y,keyFlags,DefWindowProc);
	}

void
	Curs_OnDestroy(HWND hwnd)
	{
		CCursor* pThisCurs;
		pThisCurs = This(hwnd);
		delete pThisCurs;
		FORWARD_WM_DESTROY(hwnd,DefWindowProc);
	}
void
	Curs_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x,
		int y, UINT keyFlags)
	{	// see if user tries to handle the cursor
		RECT rect;
		POINT p;
		CCursor* pCur = This(hwnd);
		GetWindowRect(pCur->m_hwnd,&rect);
		GetCursorPos(&p);
		if( (p.x >= rect.left + FROM_BORDER_WIDTH) &&
			 (p.x <= rect.right - FROM_BORDER_WIDTH) &&
			 (p.y>= rect.top+pCur->m_CurPos-3) &&
			 (p.y<= rect.top+pCur->m_CurPos+3)
			)
				pCur->m_bTrackCursor = true ;
		FORWARD_WM_LBUTTONDOWN(hwnd,fDoubleClick,x,y,
			keyFlags,DefWindowProc);
	}
void
	Curs_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
	{
		CCursor* pCur = This(hwnd);
      pCur->m_bTrackCursor = false ;
		FORWARD_WM_LBUTTONUP(hwnd,x,y,keyFlags,DefWindowProc);
	}
