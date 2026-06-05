// XServices.cpp : Defines the entry point for the DLL application.
//
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "XServices.h"
#include "resource.h"

static char *jours[7] = {"dimanche", "lundi","mardi","mercredi","jeudi","vendredi","samedi"};
static char *mois[12] = {"janvier", "février","mars", "avril", "mai", "juin","juillet","août","septembre", "octobre", "novembre", "décembre"};
SYSTEMTIME st;
SC_HANDLE   schService;
SC_HANDLE   schSCManager;
SERVICE_STATUS          ssStatus;
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
TCHAR                   szErr[256];
HINSTANCE hInst;

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	hInst=hModule;
    return TRUE;
}
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize ){
	DWORD dwRet;
	LPTSTR lpszTemp = NULL;
	dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR)&lpszTemp,0,NULL );
	if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )lpszBuf[0] = TEXT('\0');
	else	{		lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');
	_stprintf( lpszBuf, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
	}
	if ( lpszTemp )LocalFree((HLOCAL) lpszTemp );
	return lpszBuf;
}

int RemoveService(char* NomDuService){
	WNDCLASS	wc;	
	wc.cbClsExtra		= 0; 
	wc.cbWndExtra		= 0; 
	wc.hbrBackground	= (HBRUSH)CreateSolidBrush(RGB(64,64,64)); 
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW); 
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION); 
	wc.hInstance		= hInst; 
	wc.lpfnWndProc		= 0; 
	wc.lpszClassName	= "XServices"; 
	wc.lpszMenuName		= NULL; 
	wc.style			= CS_HREDRAW | CS_VREDRAW; 
  	if( !RegisterClass(&wc)) {
		MessageBox(NULL, TEXT("Erreur lors de la création de la Classe XService/Liste"), TEXT("Erreur XService"), MB_OK);
		return 0;
	HWND hList=CreateWindow("LISTBOX","Suppression de Service",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME||LBS_STANDARD|LBS_SORT|LBS_NOTIFY|WS_BORDER|WS_VSCROLL|WS_CAPTION|WS_VISIBLE, ,0,0,320,240,0,0,hInst,0);
	GetLocalTime(&st);
	char buf[255];
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)"Version: 7.0.015.213 © Patrice Wächter-Ebling 1995-2015");
	wsprintf(buf,"Il est %.2d:%.2d:%.2d Nous sommes %s, le %2d %.3s %4d",st.wHour,st.wMinute,st.wSecond,jours[st.wDayOfWeek],st.wDay,mois[st.wMonth-1],st.wYear);
	SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
	wsprintf(buf,"Nom du service à tuer : %s",NomDuService);
	SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);
	if ( schSCManager ) {
		  schService = OpenService(schSCManager, NomDuService, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);
		  if (schService) {
			  if ( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )  {
				  wsprintf(buf,"Tentative d'arrêt du service: %s.",NomDuService);
				  SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
				  Sleep( 1000 );
				  while ( QueryServiceStatus( schService, &ssStatus ) ){
					  if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING ) { 
						  SendMessage(hList,LB_ADDSTRING,0,(LPARAM)".");
						  Sleep( 1000 );
					  } else {  
						  break;
					  }
					  if ( ssStatus.dwCurrentState == SERVICE_STOPPED ){ 
						  wsprintf(buf,"%s a été arrtêté",NomDuService);
						  SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
					}else{
						  wsprintf(buf,"%s n'a pas été arrtêté",NomDuService);
						  SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
					}
					if ( DeleteService(schService) ){
						wsprintf(buf,"%s a été déinstallé",NomDuService);
						SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
					}else{
						wsprintf(buf,"Echec lors de la déinsatallation de %s [%s]",NomDuService, GetLastErrorText(szErr,256));
						SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
					}
					CloseServiceHandle(schService);
				  }
			  }else{
				  wsprintf(buf,"Echec à l'ouverture du Service [%s]",GetLastErrorText(szErr,256));
				  SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
				 }
				CloseServiceHandle(schSCManager);
		  }else{
			  wsprintf(buf,"Echec à l'ouverture du gestionnaire de Services [%s]",GetLastErrorText(szErr,256));
			  SendMessage(hList,LB_ADDSTRING,0,(LPARAM)buf);
		  }
		  return 0;
	}
}