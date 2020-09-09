/*
 *        gmswin\mainwnd.h
 *    application windows génerique
 *		partie gestion d'une fenetre principale de dialogue
 *    Fichier header
 *    Nicolas Witczak aout 1995
 */
#ifndef MAINWND_H
	#define MAINWND_H

#include <windows.h>
#include <portable\bool.h>
#include "crsclass.h"

#define MainDefProc	DefDlgProc
// exported variables
extern HWND		hThisWnd	;
extern HPEN		hDKGrayPen ;
extern HPEN		hWhitePen ;
extern HICON	hDefIcon ;
extern CCursor* pCursEq[30];
extern CCursor* pCursGain;
// window creation
bool
	Register();
/*descr: call only on initapplic()
*/
bool
	Lauch(int nCmdShow );

//window management


void
	CenterWindow(HWND hwnd);
void
	SetWindowXSize(HWND hwnd,int cx);
void
	Draw3dFrame(HWND hwnd,HDC hDC);

// windows proc
LRESULT CALLBACK _export
	MainWinProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK
	MainDialogProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);

// message handlers

void
	Cls_OnDestroy(HWND hwnd);
BOOL
	Cls_OnEraseBkgnd(HWND hwnd, HDC hdc);
void
	Cls_OnPaint(HWND hwnd);
void
	Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
BOOL
	Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

#endif // MAINWND_H
