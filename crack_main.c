//////////////////////////////////////////////////////
//
// Projet CISCOCRACK.   Mai 05 Ph.jounin
// File   ciscomain.c
//
//
//////////////////////////////////////////////////////


#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include "ciscocrack.h"

HWND hDbgMainWnd;



/* ------------------------------------------------- */
/* Ressources                                        */
/* ------------------------------------------------- */

// descripteur du menu système
enum { IDM_CRACK_HIDE = 0x1001 };		// should be under 0xF000


enum
{ 
      WM_INIT_APPLICATION =  (WM_APP+100),
      WM_NOTIFYTASKTRAY,
      WM_FREEMEM,
      WM_REFRESH,
};


int CALLBACK MyEditProc (HWND hWnd, UINT message, WPARAM wParam, LONG lParam);
WNDPROC WindowsEditProc;

/* ------------------------------------------------- */
/* Manage task tray                                  */
/* ------------------------------------------------- */
void TrayMessage(HWND hDlg, DWORD dwMessage, HICON hIcon)
{
NOTIFYICONDATA IconData;

   IconData.cbSize = sizeof IconData;
   IconData.hWnd = hDlg;
   IconData.uID = TASKTRAY_ID ;
   IconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
   IconData.uCallbackMessage = WM_NOTIFYTASKTRAY;
   IconData.hIcon = hIcon;
   GetWindowText (hDlg, IconData.szTip, sizeof IconData.szTip);
   Shell_NotifyIcon (dwMessage, &IconData);
} // TrayMessage


/* ------------------------------------------------- */
/* Manage GUI's items                                */
/* ------------------------------------------------- */

static int Handle_VM_Notify (HWND hDlgWnd, WPARAM wParam, LPNMHDR pnmh)
{
WORD wItem = LOWORD (wParam);

   switch ( wItem )
    {
       case 0 :
           break;
   }
return 0;
} // Handle_VM_Notify


static int Handle_VM_Command (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
int wItem = (int) LOWORD (wParam);
char szEncryptedPwd [256], szClearPwd [256];
int  nLength; 

   switch (wItem)
   {
       case IDOK :
	        memset (szEncryptedPwd, 0, sizeof szEncryptedPwd);
	        memset (szClearPwd,     0, sizeof szClearPwd);

            nLength = GetDlgItemText (hWnd, IDC_CRYPTED_PWD, szEncryptedPwd, sizeof szEncryptedPwd);
            // complete half hexa values with a trailing '0'
            if (nLength &  0x01)  szEncryptedPwd [nLength] = '0';
            
            cdecrypt (szEncryptedPwd, szClearPwd); 
            SetDlgItemText (hWnd, IDC_CLEAR_PWD, szClearPwd);
            break;

   }
return FALSE;
} // Handle_VM_Command



/* ------------------------------------------------- */
/* Main CallBack                                     */
/* ------------------------------------------------- */
int CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LONG lParam)
{
static HICON    hIcon;
HMENU     hMenu;
int       Rc;
HWND	  hNW;

  switch (message)
  {
       case WM_INITDIALOG :
		   	 hDbgMainWnd = hWnd;  // pour debug

             // Inits Windows : Enregistrer notre icone
             hIcon = LoadIcon  (GetWindowInstance(hWnd), MAKEINTRESOURCE (IDI_CISCOCRACK));
             SetClassLong (hWnd, GCL_HICON, (LONG) hIcon );
             // Ajout du menu Hide
             hMenu = GetSystemMenu (hWnd, FALSE);
             if (hMenu != NULL)
             {
                AppendMenu (hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu (hMenu, MF_STRING, IDM_CRACK_HIDE, "Hide Window");
             }
             // SubClass EditText
             PostMessage (hWnd, WM_INIT_APPLICATION, 0, 0);
             break;



       case WM_INIT_APPLICATION :
             // Ajout de l'icone dans la barre des taches
             TrayMessage (hWnd, NIM_ADD, hIcon);
             // SubClass Edit control to validate input	
             WindowsEditProc = (WNDPROC) SetWindowLong (GetDlgItem (hWnd, IDC_CRYPTED_PWD), GWL_WNDPROC, (LONG) MyEditProc);
             // Timers    
             PostMessage (hWnd, WM_FREEMEM, 0, 0);
             PostMessage (hWnd, WM_REFRESH, 0, 0);
             break;



		////////////////////////////////////////////////////
		// Timers wake up
		////////////////////////////////////////////////////
	   case WM_FREEMEM :
		   SetProcessWorkingSetSize (GetCurrentProcess (),-1,-1);
           SetTimer (hWnd, WM_FREEMEM, 60000, NULL);
		   break;

       case WM_REFRESH :
           SetTimer (hWnd, WM_REFRESH, 1000, NULL);
           //PostMessage (hWnd, WM_COMMAND, IDOK, 0);
           break;

		////////////////////////////////////////////////////
		// Gestion des contrôles
		////////////////////////////////////////////////////

       case WM_NOTIFYTASKTRAY :
            switch (lParam)
            {POINT Pt;
                case WM_RBUTTONDOWN:
                    GetCursorPos (& Pt);
                    Rc = TrackPopupMenu (GetSystemMenu (hWnd, FALSE), TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
                                         Pt.x, Pt.y, 0, hWnd, NULL);
                    if (Rc!=0) PostMessage (hWnd, WM_SYSCOMMAND, Rc, 0);
                    break;
                case WM_LBUTTONDBLCLK :        
                    ShowWindow (hWnd, SW_SHOWNORMAL);
                    SetForegroundWindow (hWnd);
                    CheckMenuItem (GetSystemMenu (hWnd, FALSE), IDM_CRACK_HIDE, MF_UNCHECKED);
                    break;
            }
            break;

       // request message
       case WM_SIZE :
            if (wParam==SIZE_MINIMIZED)
                 ShowWindow (hWnd, SW_HIDE);   // Hide Window
            break;



       case WM_QUERYDRAGICON :
            return (LONG) hIcon;

       case WM_CLOSE :
            // otherwise Fall Through
       case WM_DESTROY :
            TrayMessage (hWnd, NIM_DELETE, NULL);
            DestroyIcon (hIcon);
            EndDialog (hWnd, 0);
            break;

       case WM_SYSCOMMAND :
            if (wParam == IDM_CRACK_HIDE)
            {
                 CheckMenuItem (GetSystemMenu (hWnd, FALSE), IDM_CRACK_HIDE,IsWindowVisible(hWnd) ? MF_CHECKED : MF_UNCHECKED);
                 ShowWindow (hWnd, IsWindowVisible(hWnd) ? SW_HIDE  : SW_SHOW);
            } 
            break;

       case WM_COMMAND :
            Handle_VM_Command (hWnd, wParam, lParam);
            break;

       case WM_NOTIFY :
            Handle_VM_Notify (hWnd,  wParam, (LPNMHDR) lParam);
            break;

       case WM_TIMER :
            KillTimer(hWnd, wParam);
            PostMessage (hWnd, wParam, 0, 0);
            break;

  }
return FALSE;
} // CALLBACK Wndproc




/* ----------------------------- */
/* WinMain                       */
/* ----------------------------- */
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
int	 	 Rc;
HWND     hPrevWnd;
HANDLE   myMutEx;

#define  CISCOCRACK_MUTEX			"<CiscoCrack> by Ph. Jounin MutEx"

 // Détecter une instance déjà active
   myMutEx = CreateMutex (NULL, TRUE, CISCOCRACK_MUTEX);
   if (myMutEx!=NULL  &&  GetLastError()==ERROR_ALREADY_EXISTS)
   {
	  if (myMutEx!=NULL)  CloseHandle (myMutEx);
	   // peut-on la rappeler
   	  hPrevWnd = FindWindow (NULL, APP_TITLE);
      if (hPrevWnd != NULL)
	  {
   	     SetForegroundWindow (hPrevWnd);
		 ShowWindow (hPrevWnd, SW_SHOWNORMAL);
	  }
      else
	      MessageBox (NULL, "CiscoCrack is already running", "CiscoCrack", MB_OK | MB_ICONSTOP);
      return 0;
   }
	/* ------------------------------------------ */
	/* app is starting now                        */
	/* ------------------------------------------ */

     DialogBox (hInstance, MAKEINTRESOURCE(IDD_CISCOCRACK), NULL, WndProc);

     ReleaseMutex (myMutEx);
     CloseHandle (myMutEx);

return 0;
} /* WinMain */



/* ------------------------------------------------- */
/* SubClass CallBacks                                */
/* ------------------------------------------------- */
int CALLBACK MyEditProc (HWND hWnd, UINT message, WPARAM wParam, LONG lParam)
{
  switch (message)
  {
     case WM_CHAR : 
            PostMessage (hDbgMainWnd, WM_COMMAND, IDOK, 0);
            break;

  }
return CallWindowProc (WindowsEditProc, hWnd, message, wParam, lParam);
} // CallBack EditProc 

