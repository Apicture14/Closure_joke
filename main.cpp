#include "Utils.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <mutex>
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

using namespace types;
typedef PROCESSENTRY32 PE;

vector<string> t = {"哔哩哔哩","PRTS","明日方舟"};
string flag = "chrome.exe";
vector<string> classn = {"Chrome_WidgetWin_1","MozillaWindowClass","CefBrowserWindow","Chrome_RenderWidgetHostHWND"};

BOOL CALLBACK EnumProc(HWND h,LPARAM l){
    if (IsWindowVisible(h)==FALSE||GetParent(h)!=NULL){
        return TRUE;
    }
    int s = GetWindowTextLengthA(h)+1;
    char c[s] = {0};
    char cl[100] = {0};
    GetWindowTextA(h,c,s);
    GetClassNameA(h,cl,100);
    if (c==""||(std::count(classn.begin(),classn.end(),((string)cl))==0)){
        return TRUE;
    }else{
        bool f;
        string a = c;
        auto fd = std::count(t.begin(),t.end(),((string)c));
        if (fd!=0){
            MessageBoxA(h,"禁止访问此页面","消息",MB_OK+MB_ICONWARNING);
            if (FAILED(SendMessageA(h,WM_CLOSE,0,0))){
            DWORD pid; GetWindowThreadProcessId(h,&pid);
            HANDLE p = OpenProcess(PROCESS_TERMINATE,FALSE,pid);
            TerminateProcess(p,0);
        }
        
        
    }
    return TRUE;
}
}

int main(int argc,char* argv[]){
    HANDLE hMutex;
    string name = (argv[0]);
    HWND h = GetConsoleWindow();
    DWORD xw;

    
    GetWindowThreadProcessId(h,&xw);
    ShowWindow(h,SW_HIDE);
    /*

    HANDLE p = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    PROCESSENTRY32 pi;

    pi.dwSize = sizeof(PROCESSENTRY32);

    if (INVALID_HANDLE_VALUE == p){
        return -1;
    }

    bool bRet;
    bRet = (p,&pi);
    while (bRet)
    {
        bRet = Process32Next(p,&pi);
    }
    */

    TCHAR * MutexName = (TCHAR*)"RunningMutex";
    hMutex = CreateMutex(NULL,FALSE,MutexName);

    if (GetLastError() == ERROR_ALREADY_EXISTS){
        MessageBox(NULL,"Program Already Running","Fatal Error!",MB_OK+MB_ICONERROR);
        exit(1);
    }
    

    while (true)
    {
        EnumWindows(EnumProc,0);
        _sleep(500);
    }
    
}