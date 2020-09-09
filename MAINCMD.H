/*
 *       maincmd.h
 *    application traitement audio
 *		gestion des messages utilisateurs
 *    Fichier header
 *    Nicolas Witczak aout 1995
 */
#ifndef MAINCMD_H
	#define MAINCMD_H

#include<io_ext\sdstream.h>
#include <shellapi.h>
#include "lin_para.h"
#include "lin_flt.h"

extern CFile curFile;
extern CFile outFile;
extern CLPFilter filter;
extern CLinFltParam  filterParam;
extern bool bJobToDo;
extern bool bWaveSaturated;

////////////////////////////////////////////////////
// helpers
void
	SetWindowTitle();
void
	SetProgressState(int iNewProgress);
BOOL
	DlgFileOpen(char* pszFileName,int sizeString);
BOOL
	DlgFileSaveAs(char* pszFileName,int sizeString);

////////////////////////////////////////////////////
// sound process

// common
void
	FillFilterParameters();
void
	IdleProcessWave(CFile& outFile,CFile& inFile);
void
	IdlePlayWave(CFile& inFile);
////////////////////////////////////////////////////
// dialog proc
BOOL CALLBACK
	MainDialogProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////
// user command parser
void
	Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

////////////////////////////////////////////////////
// user command handler
typedef void (*PFNSmpHandler)();
typedef void (*PFNCpxHandler)(HWND hwndCtl,UINT codeNotify);

void
	AboutBox();
BOOL CALLBACK _export
	AboutDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void
	Cls_OnOpen();

void
	Cls_OnDropFiles(HWND hwnd, HDROP hdrop);
void
	Cls_OnSave();
void
	Cls_OnPlay();
void
	Cls_OnSaveAs();
void
	Cls_OnClose();
void
	Cls_OnTestPersClass();
void
	Cls_OnStop();
void
	Cls_OnHelp();
#endif //MAINCMD_H
