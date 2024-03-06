#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <mutex>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
using namespace std;

typedef PROCESSENTRY32 PE;
typedef int (WINAPI *MESSAGEBOXTIMEOUTA)(HWND, LPCSTR, LPCSTR, UINT, WORD, DWORD);

vector<string> Targets = {"干杯","PRTS","明日"};
vector<string> ClassNames = {"Chrome_WidgetWin_1","MozillaWindowClass","CefBrowserWindow","Chrome_RenderWidgetHostHWND"};

HMODULE hUser32 = LoadLibraryA("user32.dll");
MESSAGEBOXTIMEOUTA MessageBoxTimeoutA = (MESSAGEBOXTIMEOUTA)GetProcAddress(hUser32, "MessageBoxTimeoutA");

BOOL CALLBACK EnumProc(HWND handle, LPARAM lparam) {
    if (IsWindowVisible(handle) == FALSE || GetParent(handle) != NULL) {
        return TRUE;
    }
    int nameLength = GetWindowTextLengthA(handle) + 1;
    char name[nameLength] = {0};
    char className[100] = {0};
    GetWindowTextA(handle, name, nameLength);
    GetClassNameA(handle, className, 100);
    if (string(name) == "" || (std::count(ClassNames.begin(), ClassNames.end(), string(className)) == 0)) {
        return TRUE;
    } else {
        auto fd = std::count(Targets.begin(), Targets.end(), string(name));
        if (fd != 0) {
            MessageBoxTimeoutA(handle, "禁止访问此页面", "警告", MB_OK + MB_ICONWARNING, 0, 3000);
            DWORD pid; 
            GetWindowThreadProcessId(handle, &pid);
            HANDLE p = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            TerminateProcess(p, 0);
        }
    }
    return TRUE;
}

int main(int argc, char* argv[]) {
    string name;
    HANDLE hMutex;
    HWND hCMD = GetConsoleWindow();

    ShowWindow(hCMD, SW_HIDE);

    TCHAR* MutexName = (TCHAR*)"RunningMutex";
    hMutex = CreateMutex(NULL, FALSE, MutexName);

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxTimeoutA(NULL, "Program Already Running", "Fatal Error!", MB_OK + MB_ICONERROR, 0, 3000);
        exit(1);
    }

    string Local = string(argv[0]);
    int idx = Local.find_last_of("\\"); 
    name = Local.substr(idx + 1);

    if (argc == 1) {
        while (true) {
            EnumWindows(EnumProc, 0);
            _sleep(500);
        }
    } else if (argc == 2) {
        if (access(argv[1], F_OK) != 0) {
            MessageBoxTimeoutA(NULL, "Incorrect Path", "Fatal Error", MB_OK + MB_ICONERROR, 0, 3000);
            exit(1);
        }
        try {
            string newPath = string(argv[1]) + "\\" + name;
            if (CopyFileA(argv[0], newPath.c_str(), FALSE) == TRUE) {
                MessageBoxTimeoutA(NULL, "Deploy Success", "Message", MB_OK + MB_ICONINFORMATION, 0, 3000);
                exit(0);
            } else {
                MessageBoxTimeoutA(NULL, "Deploy Failed", "Message", MB_OK + MB_ICONWARNING, 0, 3000);
            }
        } catch(const std::exception& e) {
            MessageBoxTimeoutA(NULL, e.what(), "Fatal Error!", MB_ICONERROR + MB_OK, 0, 3000);
        }
    } else {
        MessageBoxTimeoutA(NULL, "Unknown Usage!", "Fatal Error!", MB_OK, 0, 3000);
    }
    return 0;
}
