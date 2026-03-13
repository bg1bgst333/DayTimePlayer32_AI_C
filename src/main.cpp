#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <string>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comdlg32.lib")

#define IDC_BTN_OPEN   101
#define IDC_BTN_PLAY   102
#define IDC_BTN_STOP   103
#define IDC_LABEL_FILE 104

static std::string g_wavPath;
static bool        g_playing = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ---------------------------------------------------------------------------

static void UpdateButtons(HWND hwnd)
{
    EnableWindow(GetDlgItem(hwnd, IDC_BTN_PLAY), !g_wavPath.empty() && !g_playing);
    EnableWindow(GetDlgItem(hwnd, IDC_BTN_STOP), g_playing);
}

static void StopFile()
{
    mciSendStringA("stop wav",  NULL, 0, NULL);
    mciSendStringA("close wav", NULL, 0, NULL);
    g_playing = false;
}

static void OpenFile(HWND hwnd)
{
    char buf[MAX_PATH] = {0};
    OPENFILENAMEA ofn  = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hwnd;
    ofn.lpstrFilter = "WAV Files\0*.wav\0All Files\0*.*\0";
    ofn.lpstrFile   = buf;
    ofn.nMaxFile    = MAX_PATH;
    ofn.Flags       = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (!GetOpenFileNameA(&ofn))
        return;

    StopFile();
    g_wavPath = buf;
    SetDlgItemTextA(hwnd, IDC_LABEL_FILE, buf);
    UpdateButtons(hwnd);
}

static void PlayFile(HWND hwnd)
{
    if (g_wavPath.empty())
        return;

    StopFile();

    std::string openCmd = "open \"" + g_wavPath + "\" type waveaudio alias wav";
    MCIERROR err = mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
    if (err != 0) {
        char errBuf[256] = {0};
        mciGetErrorStringA(err, errBuf, sizeof(errBuf));
        MessageBoxA(hwnd, errBuf, "再生エラー", MB_ICONERROR);
        return;
    }

    mciSendStringA("play wav notify", NULL, 0, hwnd);
    g_playing = true;
    UpdateButtons(hwnd);
}

// ---------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;

        CreateWindowExA(0, "STATIC", "ファイルを選択してください",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 10, 360, 20,
            hwnd, (HMENU)IDC_LABEL_FILE, hInst, NULL);

        CreateWindowExA(0, "BUTTON", "開く",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, 40, 80, 30,
            hwnd, (HMENU)IDC_BTN_OPEN, hInst, NULL);

        CreateWindowExA(0, "BUTTON", "再生",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
            100, 40, 80, 30,
            hwnd, (HMENU)IDC_BTN_PLAY, hInst, NULL);

        CreateWindowExA(0, "BUTTON", "停止",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
            190, 40, 80, 30,
            hwnd, (HMENU)IDC_BTN_STOP, hInst, NULL);

        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BTN_OPEN: OpenFile(hwnd); break;
        case IDC_BTN_PLAY: PlayFile(hwnd); break;
        case IDC_BTN_STOP:
            StopFile();
            UpdateButtons(hwnd);
            break;
        }
        return 0;

    case MM_MCINOTIFY:
        if (wParam == MCI_NOTIFY_SUCCESSFUL) {
            mciSendStringA("close wav", NULL, 0, NULL);
            g_playing = false;
            UpdateButtons(hwnd);
        }
        return 0;

    case WM_DESTROY:
        StopFile();
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}

// ---------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSA wc    = {0};
    wc.lpfnWndProc  = WndProc;
    wc.hInstance    = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = "WavPlayer";
    wc.hCursor      = LoadCursorA(NULL, IDC_ARROW);
    wc.hIcon        = LoadIconA(NULL, IDI_APPLICATION);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0, "WavPlayer", "WAV Player",
        (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 130,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}
