#pragma comment(lib,"WtsApi32.lib")

#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <conio.h>
#include <string>
#include <cstring>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <mutex>
#include <shlobj.h>
#include <fstream>
#include <thread>
#include <wtsapi32.h>
#include <WinUser.h>
#include <winreg.h>
#include <winsvc.h>
#include "utils.h"
using namespace std;

typedef PROCESSENTRY32 PE;

extern "C" {
    int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLauguageId, IN DWORD dwMillisenconds);
}

vector<string> Targets = {"哔哩哔哩","PRTS","明日方舟"};
vector<string> ClassNames = {"Chrome_WidgetWin_1","MozillaWindowClass","CefBrowserWindow","Chrome_RenderWidgetHostHWND"};
vector<thread> WindowThreads;
DWORD delay = 1;
DWORD resp;
BYTE back[5];

enum Browsers {
    CHROME,
    EDGE,
    FIREFOX
};

LPSTR title = (LPSTR)"消息";
LPSTR msg = (LPSTR)"禁止访问此页面";

HANDLE WINAPI newOpenProcess(
    _In_ DWORD dwDesiredAccess,
    _In_ BOOL bInheritHandle,
    _In_ DWORD dwProcessId
) {

}

VOID ShowBox(HWND handle){
    MessageBoxA(handle, msg, title, MB_OK);
}
//TODO
/*
BOOL AntiRecov(Browsers brws=CHROME) {
    CHAR c[100] = {0};
    CHAR buf[5000] = { 0 };
    SHGetSpecialFolderPathA(NULL, c, CSIDL_LOCAL_APPDATA, FALSE);
    string path = ((string)c + "\\Google\\User Data\\Default\\Preferences");
    ifstream f(path);
    if (f.is_open()) {

    }

}
*/

BOOL CALLBACK EnumProcW(HWND handle,LPARAM lparam){
    if (IsWindowVisible(handle)==FALSE||GetParent(handle)!=NULL){
        return TRUE;
    }
    // int nameLength = GetWindowTextLengthA(handle)+1;
    char name[120] = {0};
    char className[100] = {0};
    GetWindowTextA(handle,name,120);
    GetClassNameA(handle,className,100);
    if (name==""||(std::count(ClassNames.begin(),ClassNames.end(),((string)className))==0)){
        return TRUE;
    }else{
        for (auto i = Targets.begin(); i != Targets.end(); i++){
            auto fd = ((string)name).find(*i);
            if (fd!=string::npos){
                int a = WTSSendMessageA(WTS_CURRENT_SERVER_HANDLE,WTS_CURRENT_SESSION,(LPSTR)title, sizeof(title), (LPSTR)msg, sizeof(msg), MB_OK + MB_ICONWARNING + MB_SYSTEMMODAL, resp, &delay, FALSE);
                printf("%s %s\r\n",name,className);
                SendMessage(handle, WM_DESTROY, 0, 0);
                Sleep(3000);
                
                if (FindWindowA(className, name) != 0) {
                    DWORD pid; GetWindowThreadProcessId(handle,&pid);
                    /*
                    CHAR cmd[20];sprintf(cmd,"taskkill -pid %d -f -t",(int)pid);
                    printf("%s/r/n",cmd);
                    system(cmd);
                    */
                    ObfuscatedTerminate("C:/Program Files (x86)/Seewo/SeewoService",pid,"SeewoBrowseControl.exe");
                }
                
                /*
                if (DestroyWindow(handle)) {
                    DWORD pid; GetWindowThreadProcessId(handle, &pid);
                    HANDLE p = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
                    TerminateProcess(p, 0);
                }
               */  
            }
        
        }
    return TRUE;
    }
}

int main(int argc,char* argv[]){

    // HOOK32(TerminateProcess,newTerminateProcess,)

    string name;
    HANDLE hMutex;
    HWND hCMD = GetConsoleWindow();
    printf(argv[0]);

    if (argc == 1) {
        ShowWindow(hCMD,SW_HIDE);
    }
    

    TCHAR * MutexName = (TCHAR*)"RunningMutexN";
    hMutex = CreateMutex(NULL,FALSE,MutexName);

    if (GetLastError() == ERROR_ALREADY_EXISTS){
        MessageBoxA(NULL, (LPCSTR)"Program Already Running", (LPCSTR)"Fatal Error!", MB_OK + MB_ICONERROR);
        exit(1);
    }
    
    string Local = ((string)argv[0]);
    int idx = Local.find_last_of("\\"); 
    name = Local.substr(idx+1,-1);
    
    if (argc==1){
        while (true)
        {
            EnumWindows(EnumProcW,0);
            Sleep(500);
            if (_kbhit()){
                CHAR a = getch();
                printf("%d\r\n",a);
            }
        }
    }else {
        if (argc==2){
            printf(argv[1]);
            if (argv[1][0]!='-'){
                if (_access(argv[1],0)!=0){
                    MessageBoxA(NULL,"incorrect path","Fatal Error",MB_OK+MB_ICONERROR);
                }
                try
                {
                    if (CopyFileA(argv[0],strcat(argv[1],("\\"+name).c_str()),FALSE)==TRUE){
                        MessageBoxA(NULL,"Deploy Success","Message",MB_OK+MB_ICONINFORMATION);  
                        exit(0);
                    }else{
                        MessageBoxA(NULL,"Deploy Failed", "Message", MB_OK + MB_ICONWARNING);
                    }
                }
                catch(const std::exception& e)
                {
                    MessageBoxA(NULL,(e.what()),"Fatal Error!",MB_ICONERROR+MB_OK);
                }
            }else{
                switch (argv[1][1])
                {
                case 'c':
                    LPSTR a[MAX_PATH];
                    if (SHGetSpecialFolderPathA(0, *a, CSIDL_STARTUP, FALSE)) {
                        // printf(a);
                        if (CopyFileA(argv[0], strcat(*a, ("\\" + name).c_str()), FALSE) == TRUE) {
                            MessageBoxA(NULL, "Deploy Success", "Message", MB_OK + MB_ICONINFORMATION);
                            exit(0);
                        }
                        else {
                            // printf("%d",GetLastError());
                            MessageBoxA(NULL, "Deploy Failed At CopyFile", "Message", MB_OK + MB_ICONERROR);
                            exit(1);
                        }
                    }
                    else {
                        // printf("%d",GetLastError());
                        MessageBoxA(NULL, "Deploy Failed At GetDirectory", "Fatal Error!", MB_OK + MB_ICONERROR);
                        exit(1);
                    }
                    break;
                case 'a':
                    {
                        /*
                    HKEY hKey;

                    if (RegGetValue(hKey, 0, (LPCWSTR)"Seewo Ant Service", REG_SZ, 0, NULL, NULL) == ERROR_SUCCESS) {
                        RegDeleteKeyA(hKey, 0);
                        printf("d");
                    }


                    int r = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                        (LPCWSTR)"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                        0,
                        NULL,
                        REG_OPTION_NON_VOLATILE,
                        KEY_WOW64_64KEY | KEY_ALL_ACCESS,
                        NULL,
                        &hKey,
                        NULL);
                    if (r != ERROR_SUCCESS) {
                        printf("c");
                        return -1;
                    }
                    r = RegSetValueEx(hKey, (LPCWSTR)"MSBSS", 0, REG_SZ, (const BYTE*)argv[0], sizeof((BYTE)argv[0]));
                    if (r != ERROR_SUCCESS) {
                        printf("s");
                        return -1;
                    }

                    RegCloseKey(hKey);
                    break;
                    */
                }
                    

                default:
                    break;
                }
            }
        }else{
            MessageBoxA(NULL,"Unknown Usage!","Fatal Error!",MB_OK+MB_ICONERROR);
            exit(1);
        }
    }
}