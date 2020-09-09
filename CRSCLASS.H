/*
 *        clsclass.h
 *    definition d'une classe curseur
 *    Fichier header
 *    Nicolas Witczak octobre 1995
 */

#ifndef CRSCLASS_H
	#define CRSCLASS_H

#include <windows.h>
#include <windowsx.h>
#include <portable\bool.h>


#define WHITE	RGB(255,255,255)
#define BLACK	RGB(0,0,0)
#define RED		RGB(255,0,0)
#define GREEN	RGB(0,255,0)
#define LTGRAY	RGB(192,192,192)
#define DKGRAY	RGB(130,130,130)

#define PARENTFRAME_WIDTH 2
#define FONT_HEIGHT	10
#define FROM_BORDER_HEIGHT 6
#define FROM_BORDER_WIDTH 1

class CCursor
{
	public:
// data members
	// general use
	HWND m_hwnd;
	static int	m_InstCount;
	//drawing shared objects
	static HPEN m_Black,m_White,m_LtGray,m_DkGray,m_Red,m_Green;
	static HFONT m_NbFont;

	// control state
	bool m_bMouseIn;
	bool m_bTrackCursor;
	// center
	int m_Center,m_width;
	// cursor
	int m_CursUp,m_CursDown;
	int m_CurPos;

	// frequ value
	int m_FreqLinePos;
	RECT m_FreqTextRect;
	float m_FrequVal;
   char m_pszFrequ[5];
	// dB value
	int dBLinePos;
	RECT m_dBTextRect;
	int m_dBVal,m_dBMax;
// member functions
	void
		StaticInit(HINSTANCE hInst);
	void
		StaticDelete();
	CCursor(HINSTANCE hInst,HWND hParent,int width,int xpos
		,float frequ=0,int dBMax=15);
	~CCursor();

// cursors functions
	void
		DrawGuide(HDC hDC);
	void
		DrawGraduation(HDC hDC);
	void
		DrawCursor(HDC hDC);
	void
		DrawText(HDC hDC);
	void
   	DrawAll();
	void
		PaintBkg(HDC hDC);
		//mouse helper
	bool
		IsMouseInWnd();
// data exchange function
	double
		GetFrequVal()	{ return m_FrequVal;};
	int
		GetdBVal()	{ return m_dBVal; } ;
};

////////////////////////////////////////////////
// global function headers
CCursor*
	This(HWND hwnd);

LRESULT CALLBACK _export
	CursWndProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);
void
	Curs_OnPaint(HWND hwnd);
void
	Curs_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
void
	Curs_OnDestroy(HWND hwnd);
void
	Curs_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x,
		int y, UINT keyFlags);
void
	Curs_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
#endif //CRSCLASS_H

