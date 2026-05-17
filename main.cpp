#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <commdlg.h>
#include <tlhelp32.h>
#include <thread>
#include <chrono>

using namespace std;

// Configuration Structure
struct Config {
    string targetExe;
    string dllPath;
    int delayMs;
    bool autoClose;
};

// Function Declarations
bool LoadConfig(Config& config, const string& filename);
void SaveConfig(const Config& config, const string& filename);
void SetupConfig(Config& config, const string& filename);
DWORD GetProcessIdByName(const wstring& processName);
bool InjectDLL(DWORD processID, const string& dllPath);
wstring StringToWString(const string& s);
string WStringToString(const wstring& ws);


int main() {
    SetConsoleTitleA("XASAC Injector Engine");
    
    // Set console colors for a slightly better look
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    
    cout << "=======================================" << endl;
    cout << "        XASAC Auto-Injector v1.0       " << endl;
    cout << "        https://xasac.com.tr           " << endl;
    cout << "=======================================" << endl << endl;

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    // Get the absolute directory where the injector executable is located
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    string exeDir = string(exePath);
    size_t lastSlash = exeDir.find_last_of("\\/");
    if (lastSlash != string::npos) {
        exeDir = exeDir.substr(0, lastSlash + 1); // keep the trailing slash
    } else {
        exeDir = ".\\";
    }

    const string configFileName = exeDir + "config.cfg";
    Config config;

    // Try to load config, if it fails, run setup
    if (!LoadConfig(config, configFileName)) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "[!] Config file not found or corrupted at " << configFileName << endl;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        SetupConfig(config, configFileName);
    }

    cout << "[+] Configuration Loaded:" << endl;
    cout << "    Target EXE: " << config.targetExe << endl;
    cout << "    DLL Path:   " << config.dllPath << endl;
    cout << "    Delay:      " << config.delayMs << " ms" << endl;
    cout << "    Auto Close: " << (config.autoClose ? "Yes" : "No") << endl << endl;

    cout << "[*] Waiting for process '" << config.targetExe << "' to start..." << endl;

    wstring wTargetExe = StringToWString(config.targetExe);
    DWORD processID = 0;

    // Loop until process is found
    while ((processID = GetProcessIdByName(wTargetExe)) == 0) {
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "[+] Process found! PID: " << processID << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    cout << "[*] Waiting " << config.delayMs << "ms before injecting..." << endl;
    this_thread::sleep_for(chrono::milliseconds(config.delayMs));

    cout << "[*] Attempting to inject '" << config.dllPath << "'..." << endl;
    
    if (InjectDLL(processID, config.dllPath)) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout << "[+] Injection Successful!" << endl;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    } else {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "[-] Injection Failed." << endl;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        system("pause");
        return 1;
    }

    if (config.autoClose) {
        cout << "[*] Auto closing in 3 seconds..." << endl;
        this_thread::sleep_for(chrono::seconds(3));
    } else {
        cout << endl << "[*] Operation complete. Press any key to exit." << endl;
        system("pause");
    }

    return 0;
}

// Utility function to convert std::string to std::wstring
wstring StringToWString(const string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring r(buf);
    delete[] buf;
    return r;
}

// Utility function to convert std::wstring to std::string
string WStringToString(const wstring& ws) {
    int len;
    int slength = (int)ws.length() + 1;
    len = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), slength, 0, 0, 0, 0);
    char* buf = new char[len];
    WideCharToMultiByte(CP_ACP, 0, ws.c_str(), slength, buf, len, 0, 0);
    string r(buf);
    delete[] buf;
    return r;
}

bool LoadConfig(Config& config, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    bool foundTarget = false;
    bool foundDll = false;
    
    // Default values if not specified
    config.delayMs = 100;
    config.autoClose = true;

    while (getline(file, line)) {
        size_t delimiterPos = line.find('=');
        if (delimiterPos == string::npos) continue;

        string key = line.substr(0, delimiterPos);
        string value = line.substr(delimiterPos + 1);

        if (key == "TargetExe") { config.targetExe = value; foundTarget = true; }
        else if (key == "DllPath") { config.dllPath = value; foundDll = true; }
        else if (key == "DelayMs") { config.delayMs = stoi(value); }
        else if (key == "AutoClose") { config.autoClose = (value == "1" || value == "true"); }
    }

    return foundTarget && foundDll;
}

void SaveConfig(const Config& config, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "TargetExe=" << config.targetExe << endl;
        file << "DllPath=" << config.dllPath << endl;
        file << "DelayMs=" << config.delayMs << endl;
        file << "AutoClose=" << (config.autoClose ? "1" : "0") << endl;
        cout << "[+] Configuration saved to " << filename << endl;
    } else {
        cout << "[-] Failed to save configuration." << endl;
    }
}

// Helper function to open file dialog and return path
string OpenFileDialog(const string& title, const string& filter) {
    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetConsoleWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    
    wstring wFilter = StringToWString(filter);
    // Replace pipe with null characters in wide string filter
    for (size_t i = 0; i < wFilter.length(); ++i) {
        if (wFilter[i] == '|') {
            wFilter[i] = '\0';
        }
    }
    ofn.lpstrFilter = wFilter.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    
    wstring wTitle = StringToWString(title);
    ofn.lpstrTitle = wTitle.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameW(&ofn) == TRUE) {
        return WStringToString(szFile);
    }
    return "";
}

// Helper to extract file name from full path
string GetFileNameFromPath(const string& path) {
    size_t found = path.find_last_of("/\\");
    if (found != string::npos) {
        return path.substr(found + 1);
    }
    return path;
}

void SetupConfig(Config& config, const string& filename) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "=== INITIAL SETUP MODE ===" << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    cout << "Please select the target executable and DLL in the open dialogs." << endl << endl;

    // Select Executable
    cout << "[*] Opening file dialog to select target executable (FC26.exe / FC26 Showcase.exe)..." << endl;
    string selectedExePath = OpenFileDialog("Select FC26 Target Executable", "Executable Files (*.exe)|*.exe|All Files (*.*)|*.*");
    if (selectedExePath.empty()) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "[!] No executable selected. Exiting." << endl;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        system("pause");
        exit(1);
    }
    config.targetExe = GetFileNameFromPath(selectedExePath);
    cout << "[+] Selected Executable Name: " << config.targetExe << endl << endl;

    // Select DLL
    cout << "[*] Opening file dialog to select FCLiveEditor.DLL..." << endl;
    config.dllPath = OpenFileDialog("Select FCLiveEditor DLL", "DLL Files (*.dll)|*.dll|All Files (*.*)|*.*");
    if (config.dllPath.empty()) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "[!] No DLL selected. Exiting." << endl;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        system("pause");
        exit(1);
    }
    cout << "[+] Selected DLL Path: " << config.dllPath << endl << endl;

    string input;
    cout << "3. Injection Delay in milliseconds [Default: 100]:" << endl << "> ";
    getline(cin, input);
    config.delayMs = input.empty() ? 100 : stoi(input);

    cout << "4. Auto-close injector after success? (1=Yes, 0=No) [Default: 1]:" << endl << "> ";
    getline(cin, input);
    config.autoClose = (input.empty() || input == "1" || input == "true" || input == "yes");

    cout << endl;
    SaveConfig(config, filename);
    cout << "[+] Setup complete! Starting monitoring..." << endl;
    cout << "==========================" << endl << endl;
}

DWORD GetProcessIdByName(const wstring& processName) {
    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    if (Process32FirstW(snapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, processName.c_str()) == 0) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return 0;
}

bool InjectDLL(DWORD processID, const string& dllPath) {
    // Get full path of the DLL
    char fullDllPath[MAX_PATH];
    if (GetFullPathNameA(dllPath.c_str(), MAX_PATH, fullDllPath, NULL) == 0) {
        cout << "[-] Failed to resolve absolute path for DLL." << endl;
        return false;
    }

    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        cout << "[-] Failed to open target process. Make sure you have administrator privileges." << endl;
        return false;
    }

    // Allocate memory in target process for the DLL path
    LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(fullDllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (pDllPath == NULL) {
        cout << "[-] Failed to allocate memory in target process." << endl;
        CloseHandle(hProcess);
        return false;
    }

    // Write DLL path into target process
    if (!WriteProcessMemory(hProcess, pDllPath, (LPVOID)fullDllPath, strlen(fullDllPath) + 1, 0)) {
        cout << "[-] Failed to write to target process memory." << endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get address of LoadLibraryA
    HMODULE hKernel32 = GetModuleHandleA("Kernel32.dll");
    if (hKernel32 == NULL) {
        cout << "[-] Failed to get handle to Kernel32.dll." << endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    FARPROC pLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");
    if (pLoadLibrary == NULL) {
         cout << "[-] Failed to get address of LoadLibraryA." << endl;
         VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
         CloseHandle(hProcess);
         return false;
    }

    // Create remote thread to execute LoadLibraryA with the DLL path
    HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pDllPath, 0, 0);
    if (hThread == NULL) {
        cout << "[-] Failed to create remote thread." << endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for the thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Clean up
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}
