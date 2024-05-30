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
#include <wtsapi32.h>
#include <winsvc.h>
#include "utils.h"
using namespace std;

#define VERSION 4p
#define STRICT_MODE 1

extern "C" {
    int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLauguageId, IN DWORD dwMillisenconds);
}

vector<string> Targets = {"哔哩哔哩","PRTS","明日方舟","抖音","图吧工具箱"};
vector<string> ClassNames = {"Chrome_WidgetWin_1","MozillaWindowClass","CefBrowserWindow","Chrome_RenderWidgetHostHWND","WTWindow"};

LPSTR title = (LPSTR)"消息提醒";
LPSTR msg = (LPSTR)"禁止访问此页面";
DWORD resp;
INT fails;
INT fatal_fails;

SYSTEMTIME nowTime;
vector<pair<int,int>> EnableTime = {
    {114000,135000},
    {175000,183000}
};

//TODO

VOID simulateClose(){
    keybd_event(VK_MENU,0,0,0);
    keybd_event(VK_F4,0,0,0);
    
    keybd_event(VK_F4,0,KEYEVENTF_KEYUP,0);
    keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);
}

BOOL CALLBACK EnumProcW(HWND handle,LPARAM lparam){
    if (IsWindowVisible(handle)==FALSE||GetParent(handle)!=NULL||IsWindowEnabled==FALSE){
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
        for (auto i = Targets.begin(); i != Targets.end(); i++){
            auto fd = ((string)name).find(*i);
            if (fd!=string::npos){
                CHAR cs[50] = {0};
                BOOL R = WTSSendMessageA(WTS_CURRENT_SERVER_HANDLE,WTS_CURRENT_SESSION,(LPSTR)title, sizeof(title), (LPSTR)msg, sizeof(msg), MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL, 0, &resp, FALSE);
                //SetCapture(handle);
                // sprintf(cs,"%s %s %d %d\r\n",name,className,R,resp);

                //PostMessage(handle, WM_DESTROY, 0, 0);
                //simulateClose() 模拟ALT-F4; 
                SendMessageA(handle,WM_CLOSE,0,0);
                // printf("fin\r\n");
                Sleep(3000);
                // R = EnableWindow(handle,FALSE) 禁用窗口输入;
                // printf("%d %d\r\n",R,GetLastError());
                if (FindWindowA(className, name) != NULL) {
                    DWORD pid; GetWindowThreadProcessId(handle,&pid);
                    /*
                    CHAR cmd[20];sprintf(cmd,"taskkill -pid %d -f -t",(int)pid);
                    printf("%s/r/n",cmd);
                    system(cmd);
                    */
                    
                    BOOL bRet = ObfuscatedTerminate("C:/Program Files (x86)/Seewo/SeewoService",pid,"SeewoBrowseControl.exe");
                    if (!bRet){
                        fails++;
                        printf("warn:%d\r\n",fails);
                        if (fails>=5&&STRICT_MODE){
                            MessageBoxTimeoutA(handle,"请不要违规使用设备","警告",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL,0,3000); 
                            // WTSSendMessageA(WTS_CURRENT_SERVER,WTS_CURRENT_SESSION,"警告",sizeof("警告"),"请不要违规使用设备",sizeof("请不要违规使用设备"),MB_OK|MB_ICONWARNING,0,NULL,FALSE);
                            Sleep(5000);
                            system("shutdown -s -t 0");
                        }
                    }
                }else{
                    
                }
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
    printf("\r\n");
    
    if (argc == 1) {
        ShowWindow(hCMD,SW_HIDE);
    }else{
        printf(argv[1]);
        if (strcmp(argv[1],"debug")==0){
            if (access("debug.key",F_OK)==-1){
                printf("Key Not Found");
                return -2;
            }
        }else{
            printf("Unknown Usage!");
            return -3;
        }
    }
    

    TCHAR * MutexName = (TCHAR*)"RunningMutex";
    hMutex = CreateMutex(NULL,FALSE,MutexName);

    if (GetLastError() == ERROR_ALREADY_EXISTS){
        MessageBoxA(NULL, (LPCSTR)"Program Already Running", (LPCSTR)"Fatal Error!", MB_OK + MB_ICONERROR);
        exit(1);
    }
    
    string Local = ((string)argv[0]);
    int idx = Local.find_last_of("\\"); 
    name = Local.substr(idx+1,-1);
    
    while (true)
    {
        GetLocalTime(&nowTime);
        // printf("%2d%2d%2d",nowTime.wHour,nowTime.wMinute,nowTime.wSecond);
        if (isIntime(&EnableTime,nowTime,1)){
            // printf(" no\r\n");
            continue;
        }
        // printf(" yes\r\n");
        EnumWindows(EnumProcW,0);
        Sleep(1000);
    }
}
