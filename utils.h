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
        if (access(ExecutePath.c_str(),F_OK)!=0 || access(ExecutePath.c_str(),F_OK)!=0){
            // printf("et nf");
            return FALSE;
        }
        string exeName = ObfuscatedName != "" ? ExecutorPath.substr(ExecutorPath.find_last_of("/")+1,-1) : ObfuscatedName; 
        string ExecutePathN = (ExecutePath+"/"+exeName);
        // printf("%s %s",exeName.c_str(),ExecutePathN.c_str());
        CHAR pCmd[100]={0};sprintf(pCmd,"%s %d",ExecutePathN.c_str(),pid);
        if (CopyFileA(ExecutorPath.c_str(),ExecutePathN.c_str(),FALSE)){
            FILE* fp;
            CHAR bRet[10] = {0};
            fp = _popen(pCmd,"r");
            int c = fread(bRet,1,10,fp);
            while (c==0)
            {
                c = fread(bRet,1,10,fp);
            }
            pclose(fp);
            printf(bRet);
            if (((string)bRet)=="success"){
                // printf("y");
                return TRUE;
            }else{
                // printf("n");
                return FALSE;
            }
        }else{
            // printf("cp nf");
            return FALSE;
        }

    }



}