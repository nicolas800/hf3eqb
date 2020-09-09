/*
 *        gmswin\maincmd.cpp
 *    application windows génerique
 *		gestion des messages utilisateurs
 *    Fichier d'implementation
 *    Nicolas Witczak aout 1995
 */

#include <io_ext\sdstream.h>

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commdlg.h>
#include <mem.h>
#include <mmsystem.h>

#include "applic.h"
#include "maincmd.h"
#include "mainwnd.h"
#include "resource.h"

#include "wave_io.h"
#include "lin_para.h"
#include "lin_flt.h"
//#include "gen_flt.h"
#include "flt_proc.h"

#define sizeofFileChar 1024

static char pszOpenRecallDir[sizeofFileChar]="c:\\";
static char pszSaveRecallDir[sizeofFileChar]="c:\\";

static char pszDefaultExt[64]=
	"wave files \0*.wav\0all files\0*.*\0";
static char pszRecallExt[64]=
	"custum files \0*.wav\0";
static char pszFileopenDlgTitle[]="Open";
static char pszFileSaveDlgTitle[]="Save as";

// current file opened
CFile curFile;
CFile outFile;
static char pszCurrentFile[sizeofFileChar]="";

// real time audio
HWAVEOUT	hWaveDev;
int iRealTimeBlock = 0;
#define RTBLOCK_MAX	5
// work state
bool bJobToDo = false;
bool bWaveSaturated = false;
CLPFilter filter;
CLinFltParam  filterParam;

// title management
#define NO_FILE "no file"
static char pszTitle[100];
int iWorkProgress = 0;


void
	SetWindowTitle()
	{
		iWorkProgress = -1;
		strcpy(pszTitle,pszAppName);
		strcat(pszTitle," - ");
		if(pszCurrentFile[0] == '\0' )
			strcat(pszTitle,NO_FILE);
		else if(strrchr(pszCurrentFile,'\\') == 0)
			strcat(pszTitle,pszCurrentFile);
		else
			strcat(pszTitle,strrchr(pszCurrentFile,'\\')+1);
		SetWindowText(hThisWnd,pszTitle);
	}

void
	SetProgressState(int iNewProgress)
	{

		if( (iNewProgress >=0) && (iNewProgress != iWorkProgress))
		{
			char pszBuf[5];
			char* pszProgressTitle = new char[100];
			iWorkProgress = iNewProgress ;
			strcpy(pszProgressTitle,pszTitle);
			itoa(iNewProgress,pszBuf,10);
			strcat(pszProgressTitle," ");
			strcat(pszProgressTitle,pszBuf);
			strcat(pszProgressTitle," %");
			SetWindowText(hThisWnd,pszProgressTitle);
			delete [] pszProgressTitle;
		}
	}

BOOL
	DlgFileOpen(char* pszFileName,int sizeString)
	{
		OPENFILENAME ofn;
		bool bIsOk;
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize	= sizeof(OPENFILENAME);
		ofn.hwndOwner		= hThisWnd;
		ofn.hInstance		= hInst ;
		ofn.lpstrFilter	= pszDefaultExt;
		ofn.lpstrCustomFilter  =  pszRecallExt;
		ofn.nMaxCustFilter	  = sizeof(pszRecallExt) ;
		ofn.nFilterIndex	= 1 ;
		ofn.lpstrFile		= pszFileName;
		ofn.nMaxFile		= sizeString;
		ofn.lpstrFileTitle	= 0;
		ofn.nMaxFileTitle 	= 0;
		ofn.lpstrInitialDir	= pszOpenRecallDir;
		ofn.lpstrTitle		= pszFileopenDlgTitle;
		ofn.Flags			= OFN_FILEMUSTEXIST  | OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt	= 0;
		ofn.lCustData		= 0;
		ofn.lpfnHook		= 0;
		ofn.lpTemplateName= 0;
		bIsOk=GetOpenFileName(&ofn);
		if(bIsOk)
		{
			memcpy(pszOpenRecallDir,pszFileName,ofn.nFileOffset);
			pszOpenRecallDir[ofn.nFileOffset]='\0';
		}
		return bIsOk ;
	}

BOOL
	DlgFileSaveAs(char* pszFileName,int sizeString)
	{
		OPENFILENAME ofn;
		bool bIsOk;
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize	= sizeof(OPENFILENAME);
		ofn.hwndOwner		= hThisWnd;
		ofn.hInstance		= hInst ;
		ofn.lpstrFilter	= pszDefaultExt;
		ofn.lpstrCustomFilter  =  pszRecallExt;
		ofn.nMaxCustFilter	  = sizeof(pszRecallExt) ;
		ofn.nFilterIndex	= 1 ;
		ofn.lpstrFile		= pszFileName;
		ofn.nMaxFile		= sizeString;
		ofn.lpstrFileTitle	= 0;
		ofn.nMaxFileTitle 	= 0;
		ofn.lpstrInitialDir	= pszSaveRecallDir;
		ofn.lpstrTitle		= pszFileSaveDlgTitle;
		ofn.Flags			= OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt	= 0;
		ofn.lCustData		= 0;
		ofn.lpfnHook		= 0;
		ofn.lpTemplateName= 0;
		bIsOk=GetSaveFileName(&ofn);
		if(bIsOk)
		{
			memcpy(pszSaveRecallDir,pszFileName,ofn.nFileOffset);
			pszSaveRecallDir[ofn.nFileOffset]='\0';
		}
		return bIsOk ;
	}


void
	FillFilterParameters()
	{
		filterParam.cbFrequ = 0 ;
		for(int iDx=0;pCursEq[iDx]!=0;iDx++)
		{
			(filterParam.m_pGain)[iDx] =(SWORD)((pCursEq[iDx])->GetdBVal());
			(filterParam.m_pFreqList)[iDx] = (pCursEq[iDx])->GetFrequVal();
			filterParam.cbFrequ++;
		}
		(filterParam.m_pGain)[iDx]=(filterParam.m_pGain)[iDx-1];
		(filterParam.m_pFreqList)[iDx] = 48000.0;
		filterParam.cbFrequ++;
		filterParam.m_Gain=pCursGain->GetdBVal();
	}

void
	IdleProcessWave(CFile& outFile,CFile& inFile)
	{
		MSG msg;
		inFile.Begin();
      FillFilterParameters();
		inFile >> filterParam;
		outFile << filterParam;
		bJobToDo = true ;
		while(bJobToDo)
		{
			if(PeekMessage(&msg,0,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				bJobToDo=ProcessLinWave(outFile,inFile,filterParam,filter);
				SetProgressState(((filterParam.m_cbTotalBlocks-
				filterParam.m_cbBlockRemain)*100)/filterParam.m_cbTotalBlocks);
			}
		}
		outFile.Close();
		SetWindowTitle();
	}
void
	IdlePlayWave(CFile& inFile)
	{
		MSG msg;
		PCMWAVEFORMAT waveFormat;
		inFile.Begin();
		if(inFile.IsOpened())
		{
			FillFilterParameters();
			inFile >> filterParam;

			// try to open audio device
			waveFormat.wf.wFormatTag=WAVE_FORMAT_PCM;
			waveFormat.wf.nChannels= filterParam.m_cbChannels;
			waveFormat.wf.nBlockAlign=(WORD) (filterParam.m_cbChannels
				* (filterParam.m_bitPerSample / 8));
			waveFormat.wf.nSamplesPerSec= filterParam.m_SampleFreq;
			waveFormat.wf.nAvgBytesPerSec=waveFormat.wf.nBlockAlign * waveFormat.wf.nSamplesPerSec ;
			waveFormat.wBitsPerSample = filterParam.m_bitPerSample ;
			if(waveOutOpen((LPHWAVEOUT)&hWaveDev, (UINT)WAVE_MAPPER,(LPWAVEFORMATEX)(&waveFormat),
				(UINT)hThisWnd, 0L, (DWORD)CALLBACK_WINDOW) !=0)
			{
				bJobToDo = false ;
            MessageBox(hThisWnd,"unable to open wave mapper ",
					pszAppName,MB_ICONEXCLAMATION);
				return ;
			}
			else
				bJobToDo = true ;
         iRealTimeBlock = 0;
			//prepare filter proc
			CLPFilter filterRight;
			CIn8File  inObj8;
			CIn16File inObj16;
			COut8Buf  outObj8;
			COut16Buf outObj16;
			inObj8.Init(inFile);
			inObj16.Init(inFile);
			CAudioBuf inBufLeft,inBufRight;
			if(filterParam.m_bitPerSample == 8 )
				ProcessLinInit(inObj8,inBufLeft,
					inBufRight,filterParam,filter,filterRight);
			else
				ProcessLinInit(inObj16,inBufLeft,
					inBufRight,filterParam,filter,filterRight);
			// start

			while(bJobToDo)
			{
				if(PeekMessage(&msg,0,0,0,PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
					if((filterParam.m_cbBlockRemain > 1) && (iRealTimeBlock < RTBLOCK_MAX))
					{  // wavHeader block to prepare
						DWORD dwDataSize = ( filterParam.m_bitPerSample/8 )
							* filterParam.m_cbOfMiddleBlock * filterParam.m_cbChannels;
						HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE , dwDataSize);
						LPSTR lpData = (LPSTR)GlobalLock(hData);
						HGLOBAL hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
									(DWORD) sizeof(WAVEHDR));
						LPWAVEHDR lpWaveHdr = (LPWAVEHDR) GlobalLock(hWaveHdr);
						lpWaveHdr->lpData=lpData;
						lpWaveHdr->dwBufferLength=dwDataSize;
						lpWaveHdr->dwBytesRecorded = 0L;
						lpWaveHdr->dwUser=(DWORD)hData;
						lpWaveHdr->dwFlags = 0L;
						lpWaveHdr->dwLoops = 0L;

						SetProgressState(((filterParam.m_cbTotalBlocks-
							filterParam.m_cbBlockRemain)*100)/filterParam.m_cbTotalBlocks);
						if(filterParam.m_bitPerSample == 8)
							outObj8.Attach(lpData);
						else
							outObj16.Attach(lpData);
						if(filterParam.m_bitPerSample == 8 )
							ProcessLinBlock(outObj8,inObj8
								,inBufLeft,inBufRight,filterParam,filter,filterRight);
						else
							ProcessLinBlock(outObj16,inObj16,inBufLeft
								,inBufRight,filterParam,filter,filterRight);
						waveOutPrepareHeader(hWaveDev, lpWaveHdr, sizeof(WAVEHDR));
						waveOutWrite(hWaveDev, lpWaveHdr, sizeof(WAVEHDR));
						if(filterParam.m_cbBlockRemain <=1)
							bJobToDo = false;
						iRealTimeBlock++;
					};
			}
		}
		SetWindowTitle();
	}


#ifndef HANDLE_WM_DROPFILES
#define HANDLE_WM_DROPFILES(hwnd, wParam, lParam, fn) \
	 ((fn)((hwnd), (HDROP)(wParam)), 0L)
#define FORWARD_WM_DROPFILES(hwnd, hdrop, fn) \
	 (void)(fn)((hwnd), WM_DROPFILES, (WPARAM)(HDROP)(hdrop), 0L)
#endif  //HANDLE_WM_DROPFILES defined

BOOL CALLBACK _export
	MainDialogProc(HWND hwnd, UINT uMess, WPARAM wParam, LPARAM lParam)
	{
		switch (uMess)
		{
			case MM_WOM_DONE :
			{
				LPWAVEHDR lpToFree = (LPWAVEHDR)lParam ;
				waveOutUnprepareHeader(hWaveDev,lpToFree,sizeof(WAVEHDR));
				GlobalUnlock((HGLOBAL)(lpToFree->dwUser));
				GlobalFree((HGLOBAL)(lpToFree->dwUser));
				iRealTimeBlock--;
				if( (bJobToDo==false) && (iRealTimeBlock<=0))
				{
					waveOutClose(hWaveDev);
               hWaveDev = 0 ;
				}
			}
			HANDLE_MSG(hwnd,WM_COMMAND,Cls_OnCommand);
			HANDLE_MSG(hwnd,WM_DROPFILES,Cls_OnDropFiles);
			default:
				return 0;
		}
	}

#define HANDLE_SMPCMD(hookId,fn)	\
	case hookId:					\
		fn();							\
		break

void
	Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
	{
		switch(id)
		{
			HANDLE_SMPCMD(IDM_HELP_ABOUT,AboutBox);
			HANDLE_SMPCMD(IDM_HELP_HELP,Cls_OnHelp);
			HANDLE_SMPCMD(IDM_CONTROL_STOP,Cls_OnStop);
			HANDLE_SMPCMD(IDM_FILE_OPEN,Cls_OnOpen);
			HANDLE_SMPCMD(IDM_FILE_SAVE,Cls_OnSave);
			HANDLE_SMPCMD(IDM_FILE_SAVEAS,Cls_OnSaveAs);
			HANDLE_SMPCMD(IDM_FILE_CLOSE,Cls_OnClose);
      	HANDLE_SMPCMD(IDM_CONTROL_PLAY,Cls_OnPlay);
		}
	}

void
	AboutBox()
	{
		DialogBox( hInst ,MAKEINTRESOURCE(ABOUT_DLG)
			,hThisWnd,MakeProcInstance((FARPROC)AboutDlgProc,hInst));
	}

BOOL CALLBACK _export
	AboutDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
		VerifAllStr();
		switch(msg)
		{
			case WM_COMMAND:
				if(GET_WM_COMMAND_ID(wParam,lParam) == IDOK )
				{
					EndDialog(hwnd,TRUE);
					return TRUE;
				}
			case WM_INITDIALOG:
			{
				CenterWindow(hwnd);
				return TRUE;
			}
			case WM_ERASEBKGND:
			{
				RECT rect;
				HDC hDC;
				GetClientRect(hwnd,&rect);
				hDC=GetDC(hwnd);
				FillRect(hDC,&rect,(HBRUSH)(COLOR_BTNFACE+1));
				ReleaseDC(hwnd,hDC);
				return TRUE;
			}
			case WM_PAINT:
			{
				HPEN hOldPen ;
				RECT rect;
				HDC hDC;
				hDC=GetDC(hwnd);
				GetClientRect(hwnd,&rect);
				Draw3dFrame(hwnd,hDC);
				DrawIcon(hDC,30,22,hDefIcon);
				SetBkMode(hDC,TRANSPARENT);
				TextOut(hDC,100,22,pszAppName,strlen(pszAppName));
				TextOut(hDC,74,48,pszDecrApp,strlen(pszDecrApp));
				TextOut(hDC,28,80,pszVersion,strlen(pszVersion));
				TextOut(hDC,150,80,pszAuthor,strlen(pszAuthor));
				ReleaseDC(hwnd,hDC);

				hOldPen = SelectObject(hDC,hDKGrayPen);
				MoveToEx(hDC,rect.left+6,rect.bottom-38,0);
				LineTo(hDC,rect.right-6,rect.bottom-38);
				SelectObject(hDC,hWhitePen);
				MoveToEx(hDC,rect.left+7,rect.bottom-37,0);
				LineTo(hDC,rect.right-5,rect.bottom-37);
				SelectObject(hDC,hOldPen);
				return 0;
			}
		}
		return 0;
	}

void
	Cls_OnDropFiles(HWND hwnd, HDROP hdrop)
	{
		UINT iFile=0;
		char *pszFileName=new char[sizeofFileChar];
		DragQueryFile(hdrop,iFile,pszFileName,sizeofFileChar);
		DragFinish(hdrop);
		if( (pszFileName[0]!='\0')
			&& (( strcmp(&(pszFileName[strlen(pszFileName)-3]),"WAV")==0)
			||( strcmp(&(pszFileName[strlen(pszFileName)-3]),"wav")==0) ))
		{
			curFile.Close();
			if(!(curFile.OpenRead(pszFileName)))
			{
				pszCurrentFile[0]='\0';
				MessageBox(hThisWnd,"unable to open wave file ",
					pszAppName,MB_ICONEXCLAMATION);
			}
			else
				strcpy(pszCurrentFile,pszFileName);
			VerifAllStr();
			SetWindowTitle();
		}
		delete pszFileName;
	}

void
	Cls_OnOpen()
	{
		char * pszTmpFileName = new char[sizeofFileChar];
		pszTmpFileName[0]='\0';
		Cls_OnStop();
		if(DlgFileOpen(pszTmpFileName,sizeofFileChar))
		{
			curFile.Close();
			if(!(curFile.OpenRead(pszTmpFileName)))
			{
				pszCurrentFile[0]='\0';
				MessageBox(hThisWnd,"unable to open wave file ",
					pszAppName,MB_ICONEXCLAMATION);
			}
			else
				strcpy(pszCurrentFile,pszTmpFileName);
			VerifAllStr();
			SetWindowTitle();
		}
		delete [] pszTmpFileName ;
	}

void
	Cls_OnSave()
	{
		Cls_OnStop();
		if(curFile.IsOpened())
		{
			if(!(outFile.OpenUpdate(pszCurrentFile)))
			{
				MessageBox(hThisWnd,"unable to save wave file ",
					pszAppName,MB_ICONEXCLAMATION);
				return;
			}
			VerifAllStr();
			IdleProcessWave(outFile,curFile);
		}
	}

void
	Cls_OnPlay()
	{
		Cls_OnStop();
      VerifAllStr();
		IdlePlayWave(curFile);
		curFile.Close();
		curFile.OpenRead(pszCurrentFile);
	}

void
	Cls_OnSaveAs()
	{
		Cls_OnStop();
		if(curFile.IsOpened())
		{
			char *pszFileSave  =new char[sizeofFileChar];
			pszFileSave[0]= '\0' ;
			if(DlgFileSaveAs(pszFileSave,sizeofFileChar))
			{
				if(outFile.OpenRead(pszFileSave))
				{
					outFile.Close();
					if(MessageBox(hThisWnd,"File will be modified ",
						pszAppName,MB_OKCANCEL) == IDCANCEL )
							// prompt the user of modifying the file
					{
						delete [] pszFileSave ;
						return;
					}
               outFile.OpenCreate(pszFileSave);
				}
				else if((!(outFile.OpenCreate(pszFileSave))))
				{
					MessageBox(hThisWnd,"unable to save wave file ",
						pszAppName,MB_ICONEXCLAMATION);
					outFile.Close();
					delete [] pszFileSave ;
					return;
				}
            VerifAllStr();
				IdleProcessWave(outFile,curFile);
			}
			delete [] pszFileSave ;
		}
	}

void
	Cls_OnClose()
	{
		Cls_OnStop();
		curFile.Close();
		pszCurrentFile[0]='\0';
		SetWindowTitle();
	}

void
	Cls_OnStop()
	{
		bJobToDo = false;
		waveOutReset(hWaveDev);
		waveOutClose(hWaveDev);
		hWaveDev = 0 ;
		outFile.Close();
		SetWindowTitle();
	}

void
	Cls_OnHelp()
	{
		// WinHelp(hThisWnd, "HF3BEQ.HLP",HELP_CONTENTS,0) -> bugue
		if(ShellExecute(hThisWnd,0,"hf3beq.hlp",0,0,SW_SHOW)<=((HINSTANCE)32))
		{
			MessageBox(hThisWnd, "Unable to activate help",
							pszAppName, MB_SYSTEMMODAL|MB_OK|MB_ICONHAND);
		}
	}
