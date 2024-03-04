#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <mutex>
using namespace std;

typedef PROCESSENTRY32 PE;

vector<string> Targets = {"哔哩哔哩","PRTS","明日方舟"};
vector<string> ClassNames = {"Chrome_WidgetWin_1","MozillaWindowClass","CefBrowserWindow","Chrome_RenderWidgetHostHWND"};

BOOL CALLBACK EnumProc(HWND handle,LPARAM lparam){
    if (IsWindowVisible(handle)==FALSE||GetParent(handle)!=NULL){
        return TRUE;
    }
    int nameLength = GetWindowTextLengthA(handle)+1;
    char name[nameLength] = {0};
    char className[100] = {0};
    GetWindowTextA(handle,name,nameLength);
    GetClassNameA(handle,className,100);
    if (name==""||(std::count(ClassNames.begin(),ClassNames.end(),((string)className))==0)){
        return TRUE;
    }else{
        auto fd = std::count(Targets.begin(),Targets.end(),((string)name));
        if (fd!=0){
            MessageBoxA(handle,"禁止访问此页面","消息",MB_OK+MB_ICONWARNING);
            if (FAILED(SendMessageA(handle,WM_CLOSE,0,0))){
            DWORD pid; GetWindowThreadProcessId(handle,&pid);
            HANDLE p = OpenProcess(PROCESS_TERMINATE,FALSE,pid);
            TerminateProcess(p,0);
        }
    }
    return TRUE;
}
}

int main(int argc,char* argv[]){

    string name;
    HANDLE hMutex;
    HWND hCMD = GetConsoleWindow();

    ShowWindow(hCMD,SW_HIDE);

    TCHAR * MutexName = (TCHAR*)"RunningMutex";
    hMutex = CreateMutex(NULL,FALSE,MutexName);

    if (GetLastError() == ERROR_ALREADY_EXISTS){
        MessageBox(NULL,"Program Already Running","Fatal Error!",MB_OK+MB_ICONERROR);
        exit(1);
    }
    
    string Local = ((string)argv[0]);
    int idx = Local.find_last_of("\\"); 
    name = Local.substr(idx+1,-1);
    
    if (argc==1){
        while (true)
        {
            EnumWindows(EnumProc,0);
            _sleep(500);
        }
    }else {
        if (argc==2){
            if (access(argv[1],F_OK)!=0){
                MessageBox(NULL,"incorrect path","Fatal Error",MB_OK+MB_ICONERROR);
            }
            try
            {
                if (CopyFileA(argv[0],strcat(argv[1],("\\"+name).c_str()),FALSE)==TRUE){
                    MessageBox(NULL,"Deploy Success","Message",MB_OK+MB_ICONINFORMATION);  
                    exit(0);
                }else{
                    MessageBox(NULL,"Deploy Failed","Message",MB_OK+MB_ICONWARNING);
                }
            }
            catch(const std::exception& e)
            {
                MessageBox(NULL,e.what(),"Fatal Error!",MB_ICONERROR+MB_OK);
            }
        }else{
            MessageBox(NULL,"Unknown Usage!","Fatal Error!",MB_OK);
        }
    }
}