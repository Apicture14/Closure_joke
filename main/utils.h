#define UTILS_H_

#include <iostream>
#include <tlhelp32.h>
#include <cstdio>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <unistd.h>
#include <vector>
using namespace std;

namespace std{
    WINBOOL ObfuscatedTerminate(string ExecutePath,DWORD pid,string ObfuscatedName = "", string ExecutorPath = "mods/modk.exe"){
        // printf("ObfuscatedK\r\n");
        WINBOOL _Ret = FALSE;
        if (access(ExecutePath.c_str(),F_OK)==-1 || access(ExecutePath.c_str(),F_OK)==-1){
            printf("Executor Not Found");
            return FALSE;
        }
        string exeName = ObfuscatedName == "" ? ExecutorPath.substr(ExecutorPath.find_last_of("/")+1,-1) : ObfuscatedName; 
        string ExecutePathN = (ExecutePath+"/"+exeName);
        // printf("%s %s",exeName.c_str(),ExecutePathN.c_str());
        CHAR pCmd[100]={0};sprintf(pCmd,"\"%s\" %d",ExecutePathN.c_str(),pid);
        if (CopyFileA(ExecutorPath.c_str(),ExecutePathN.c_str(),FALSE)){
            FILE* fp;
            CHAR bRet[10] = {0};
            fp = _popen(pCmd,"r");
            int c = fread(bRet,1,10,fp);
            while (c!=0)
            {
                c = fread(bRet,1,10,fp);
            }
            pclose(fp);
            printf("ExecutorReturn:%s\r\n",bRet);
            if (((string)bRet)=="success"){
                printf("Close Success");
                if (!DeleteFileA(ExecutePathN.c_str())){
                printf("Delete TempExecutor Failed With %d",GetLastError());
            }
                return TRUE;
            }else{
                printf("Close Failed\r\n");
                if (!DeleteFileA(ExecutePathN.c_str())){
                printf("Delete TempExecutor Failed With %d",GetLastError());
            }
                return FALSE;
            }

        }else{
            printf("Failed to Copy Executor With %d",GetLastError());
            return FALSE;
        }

    }

    struct MyProc{
        wstring ExeName;
        DWORD pid;
        DWORD exitCode;
    };
    /*
    VOID Terminate Tree(wstring ImageName){
        std::vector<MyProc> tags;
        PROCESSENTRY32 pi;
        pi.dwSize = sizeof(pi);
        HANDLE p = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
        if (p==INVALID_HANDLE_VALUE) return;
        BOOL bRet = Process32FirstW(p,&pi);
        while (bRet)
        {
            if(((wstring)pi.szExeFile).find(ImageName)!=wstring::npos){
                MyProc pr;
                pr.ExeName = pi.szExeFile;
                pr.pid = pi.th32ProcessID;
                tags.push_back(pr);
            }
            bRet = Process32NextW(p,&pi);
        }
        if ()
    }*/

    VOID RequestAdministrtor(){
        CHAR n[100] = {0};
        GetModuleFileNameA(NULL,n,sizeof(n));
        SHELLEXECUTEINFOA sei = {sizeof(sei)};
        sei.lpVerb = "runas";
        sei.lpFile = n;
        sei.nShow = SW_HIDE;

        if (ShellExecuteExA(&sei)){
            printf("admin failed");
            return;
        }
    }

    BOOL isIntime(vector<pair<int,int>>* times,SYSTEMTIME nowTime,BOOL reverse=FALSE){
        char nowTimeS;sprintf(&nowTimeS,"%2d%2d%2d",nowTime.wHour,nowTime.wMinute,nowTime.wSecond);
        int nowTimeI = atoi(&nowTimeS);
        if (nowTime.wSecond < 10){
            nowTimeI *= 100;
        }
        for (auto i=times->begin();i!=times->end();i++){
            if (nowTimeI>=(*i).first && nowTimeI <(*i).second){
                return reverse?false:true;
            }
        }
        return reverse?true:false;
    }
}