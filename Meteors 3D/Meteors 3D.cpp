#include "framework.h"
#include "Meteors 3D.h"
#include "mmsystem.h"
#include <d2d1.h>
#include <dwrite.h>
#include "ErrH.h"
#include "FCheck.h"
#include "D2BMPLOADER.h"
#include "Gameserv.h"
#include "gifresizer.h"
#include <chrono>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "gameserv.lib")
#pragma comment(lib, "gifresizer.lib")

constexpr wchar_t bWinClassName[]{ L"My3DAttempt" };

constexpr char temp_file[](".\\res\\data\\temp.dat");
constexpr wchar_t Ltemp_file[](L".\\res\\data\\temp.dat");
constexpr wchar_t help_file[](L".\\res\\data\\help.dat");
constexpr wchar_t record_file[](L".\\res\\data\\record.dat");
constexpr wchar_t save_file[](L".\\res\\data\\save.dat");
constexpr wchar_t sound_file[](L".\\res\\data\\main.wav");

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int first_record{ 2001 };
constexpr int record{ 2002 };
constexpr int no_record{ 2003 };

WNDCLASS bWin{};
HINSTANCE bIns{ nullptr };
HWND bHwnd{ nullptr };
HCURSOR main_cur{ nullptr };
HCURSOR out_cur{ nullptr };
POINT cur_pos{};
HICON main_icon{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
MSG bMsg{};
BOOL bRet{ 0 };
HDC PaintDC{ nullptr };
PAINTSTRUCT bPaint{};
UINT bTimer{ 0 };

D2D1_RECT_F b1Rect{ 30.0f, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2Rect{ scr_width / 3, 0, scr_width * 2 / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3Rect{ scr_width * 2 / 3, 0, scr_width - 50.0f, 50.0f };

D2D1_RECT_F b1TxtRect{ 50.0f, 10.0f, scr_width / 3 - 70.0f, 50.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3 + 30.0f, 10.0f, scr_width * 2 / 3 - 70.0f, 50.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2 / 3 + 30.0f, 10.0f, scr_width - 70.0f, 50.0f };

wchar_t current_player[16]{ L"ONE CAPITAN" };

bool pause{ false };
bool sound{ true };
bool in_client{ false };
bool show_help{ false };
bool name_set{ false };
bool b1Hglt{ false };
bool b2Hglt{ false };
bool b3Hglt{ false };

int score = 0;
int level = 1;
int mins = 0;
int secs = 0;

dirs field_dir = dirs::stop;
bool hero_alive = true;

dll::RANDIT RandGen{};

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1RadialGradientBrush* FieldBckgBrush{ nullptr };

ID2D1SolidColorBrush* TxtBrush{ nullptr };
ID2D1SolidColorBrush* HgltBrush{ nullptr };
ID2D1SolidColorBrush* InactBrush{ nullptr };
ID2D1SolidColorBrush* StatusBckgBrush{ nullptr };

ID2D1SolidColorBrush* SmallStarBrush{ nullptr };
ID2D1SolidColorBrush* MidStarBrush{ nullptr };
ID2D1SolidColorBrush* BigStarBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmFormat{ nullptr };
IDWriteTextFormat* midFormat{ nullptr };
IDWriteTextFormat* bigFormat{ nullptr };

ID2D1Bitmap* bmpCannonL{ nullptr };
ID2D1Bitmap* bmpCannonR{ nullptr };

ID2D1Bitmap* bmpLaserL{ nullptr };
ID2D1Bitmap* bmpLaserR{ nullptr };

ID2D1Bitmap* bmpIntro[48]{ nullptr };
ID2D1Bitmap* bmpExplosion[24]{ nullptr };

ID2D1Bitmap* bmpAsteroid1[127]{ nullptr };
ID2D1Bitmap* bmpAsteroid2[50]{ nullptr };
ID2D1Bitmap* bmpAsteroid3[20]{ nullptr };

/////////////////////////////////////////////////////////////////

std::vector<dll::Object> vStars;
dll::PROTON* left_laser{ nullptr };
dll::PROTON* right_laser{ nullptr };





////////////////////////////////////////////////////////////////

template<typename T>concept HasRelease = requires(T check)
{
    check.Release();
};
template<HasRelease U> bool ClearMem(U** var)
{
    if ((*var))
    {
        (*var)->Release();
        (*var) = nullptr;
        return true;
    }
    return false;
}
void LogError(LPCWSTR what)
{
    std::wofstream err(L".\\res\\data\\error.log", std::ios::app);
    err << what << L" Error occured at: " << std::chrono::system_clock::now() << std::endl;
    err.close();
}
void ClearResources()
{
    if (!ClearMem(&iFactory))LogError(L"Error releasing iFactory !");
    if (!ClearMem(&Draw))LogError(L"Error releasing Draw !");
    if (!ClearMem(&FieldBckgBrush))LogError(L"Error releasing FieldBckgBrush !");
    if (!ClearMem(&SmallStarBrush))LogError(L"Error releasing SmallStarBrush !");
    if (!ClearMem(&MidStarBrush))LogError(L"Error releasing MidStarBrush !");
    if (!ClearMem(&BigStarBrush))LogError(L"Error releasing BigStarBrush !");
    if (!ClearMem(&b1BckgBrush))LogError(L"Error releasing b1BckgBrush !");
    if (!ClearMem(&b2BckgBrush))LogError(L"Error releasing b2BckgBrush !");
    if (!ClearMem(&b3BckgBrush))LogError(L"Error releasing b3BckgBrush !");
    if (!ClearMem(&TxtBrush))LogError(L"Error releasing TxtBrush !");
    if (!ClearMem(&HgltBrush))LogError(L"Error releasing HgltBrush !");
    if (!ClearMem(&InactBrush))LogError(L"Error releasing InactBrush !");
    if (!ClearMem(&StatusBckgBrush))LogError(L"Error releasing StatusBckgBrush !");
    if (!ClearMem(&iWriteFactory))LogError(L"Error releasing iWriteFactory !");
    if (!ClearMem(&nrmFormat))LogError(L"Error releasing nrmFormat !");
    if (!ClearMem(&midFormat))LogError(L"Error releasing midFormat !");
    if (!ClearMem(&bigFormat))LogError(L"Error releasing bigFormat !");
    if (!ClearMem(&bmpCannonL))LogError(L"Error releasing bmpCannonL !");
    if (!ClearMem(&bmpCannonR))LogError(L"Error releasing bmpCannonR !");
    if (!ClearMem(&bmpLaserL))LogError(L"Error releasing bmpLaserL !");
    if (!ClearMem(&bmpLaserR))LogError(L"Error releasing bmpLaserR !");

    for (int i = 0; i < 48; ++i)if (!ClearMem(&bmpIntro[i]))LogError(L"Error releasing bmpIntro !");
    for (int i = 0; i < 24; ++i)if (!ClearMem(&bmpExplosion[i]))LogError(L"Error releasing bmpExplosion !");
    for (int i = 0; i < 127; ++i)if (!ClearMem(&bmpAsteroid1[i]))LogError(L"Error releasing bmpAsteroid1 !");
    for (int i = 0; i < 50; ++i)if (!ClearMem(&bmpAsteroid2[i]))LogError(L"Error releasing bmpAsteroid2 !");
    for (int i = 0; i < 20; ++i)if (!ClearMem(&bmpAsteroid3[i]))LogError(L"Error releasing bmpAsteroid3 !");
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBoxW(NULL, ErrHandle(what), L"Критична грешка!", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    std::remove(temp_file);
    ClearResources();
    exit(1);
}

void GameOver()
{
    PlaySound(NULL, NULL, NULL);
    KillTimer(bHwnd, bTimer);




    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void InitGame()
{
    level = 1;
    score = 0;
    secs = 180;
    wcscpy_s(current_player, L"ONE CAPITAN");
    name_set = false;
    ////////////////////////////////////

    field_dir = dirs::stop;

    if (!vStars.empty())for (int i = 0; i < vStars.size(); ++i)ClearMem(&vStars[i]);
    vStars.clear();
    for (int i = 0; i < 150; ++i)
    {
        bool ok = false;
        while (!ok)
        {
            switch (RandGen(0, 2))
            {
            case 0:
                {
                    dll::Object aStar = dll::Factory(type_small_star, (float)(RandGen(20, (int)(scr_width))),
                        (float)(RandGen((int)(sky), (int)(ground))));
                    if (vStars.empty())
                {
                    vStars.push_back(aStar);
                    ok = true;
                    break;
                }
                    else
                    {
                        for (int i = 0; i < vStars.size(); ++i)
                    {
                        if (abs(aStar->center.x - vStars[i]->center.x) <= aStar->x_radius + vStars[i]->x_radius
                            && abs(aStar->center.y - vStars[i]->center.y) <= aStar->y_radius + vStars[i]->y_radius)break;
                    }
                        vStars.push_back(aStar);
                        ok = true;
                        break;
                    }
                }
                break;

            case 1:
            {
                dll::Object aStar = dll::Factory(type_mid_star, (float)(RandGen(20, (int)(scr_width))),
                    (float)(RandGen((int)(sky), (int)(ground))));
                if (vStars.empty())
                {
                    vStars.push_back(aStar);
                    ok = true;
                    break;
                }
                else
                {
                    for (int i = 0; i < vStars.size(); ++i)
                    {
                        if (abs(aStar->center.x - vStars[i]->center.x) <= aStar->x_radius + vStars[i]->x_radius
                            && abs(aStar->center.y - vStars[i]->center.y) <= aStar->y_radius + vStars[i]->y_radius)break;
                    }
                    vStars.push_back(aStar);
                    ok = true;
                    break;
                }
            }
            break;

            case 2:
            {
                dll::Object aStar = dll::Factory(type_big_star, (float)(RandGen(20, (int)(scr_width))),
                    (float)(RandGen((int)(sky), (int)(ground))));
                if (vStars.empty())
                {
                    vStars.push_back(aStar);
                    ok = true;
                    break;
                }
                else
                {
                    for (int i = 0; i < vStars.size(); ++i)
                    {
                        if (abs(aStar->center.x - vStars[i]->center.x) <= aStar->x_radius + vStars[i]->x_radius
                            && abs(aStar->center.y - vStars[i]->center.y) <= aStar->y_radius + vStars[i]->y_radius)break;
                    }
                    vStars.push_back(aStar);
                    ok = true;
                    break;
                }
            }
            break;

            }
            
        }
    }

    left_laser = new dll::PROTON(150.0f, ground - 183.0f, 250.0f, 183.0f);
    right_laser = new dll::PROTON(scr_width - 400.0f, ground - 183.0f, 250.0f, 183.0f);


}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (WPARAM)(main_icon));
        return true;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 16) < 1)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
                wcscpy_s(current_player, L"ONE CAPITAN");
                MessageBox(bHwnd, L"Ха, ха, хааа ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
            break;
        }
        break;
    }

    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        if (bIns)
        {
            SetTimer(hwnd, bTimer, 1000, NULL);

            bBar = CreateMenu();
            bMain = CreateMenu();
            bStore = CreateMenu();

            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

            AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
            AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
            AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bMain, MF_STRING, mExit, L"Изход");

            AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
            AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
            AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

            SetMenu(hwnd, bBar);
            InitGame();
        }
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
        if (MessageBox(hwnd, L"Ако продължиш, губиш прогреса по тази игра !\n\nНаистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(10, 10, 10)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        secs--;
        mins = secs / 60;
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(bHwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1TxtRect.left && cur_pos.x <= b1TxtRect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b2TxtRect.left && cur_pos.x <= b2TxtRect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = true;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b3TxtRect.left && cur_pos.x <= b3TxtRect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = false;
                        b3Hglt = true;
                    }
                }

                SetCursor(out_cur);
                return true;
            }
            else if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }

            SetCursor(main_cur);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }

            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако продължиш, губиш прогреса по тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mLvl:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако продължиш, губиш прогреса за това ниво !\n\nНаистина ли го прескачаш ?",
                L"Следващо ниво !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            //LevelUp();
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;



        }
        break;

    case WM_RBUTTONDOWN:
        if (HIWORD(lParam) <= scr_height / 2 - 100.0f)
        {
            if (LOWORD(lParam) <= scr_width / 2 - 150.0f)field_dir = dirs::down_right;
            else if (LOWORD(lParam) >= scr_width / 2 + 150.0f)field_dir = dirs::down_left;
            else field_dir = dirs::down;
        }
        else if (HIWORD(lParam) >= scr_height / 2 + 100.0f)
        {
            if (LOWORD(lParam) <= scr_width / 2 - 150.0f)field_dir = dirs::up_right;
            else if (LOWORD(lParam) >= scr_width / 2 + 150.0f)field_dir = dirs::up_left;
            else field_dir = dirs::up;
        }
        else
        {
            if (LOWORD(lParam) <= scr_width / 2)field_dir = dirs::right;
            else field_dir = dirs::left;
        }
        break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int result = 0;
    CheckFile(Ltemp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream start(Ltemp_file);
        start << L"Game started at: " << std::chrono::system_clock::now();
        start.close();
    }

    int win_x = GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2);
    if (GetSystemMetrics(SM_CXSCREEN) < win_x + (int)(scr_width) || GetSystemMetrics(SM_CYSCREEN) < (int)(scr_height)+10)ErrExit(eScreen);

    main_icon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
    if (!main_icon)ErrExit(eIcon);

    main_cur = LoadCursorFromFileW(L".\\res\\main.ani");
    out_cur = LoadCursorFromFileW(L".\\res\\out.ani");
    if (!main_cur || !out_cur)ErrExit(eCursor);

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &WinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
    bWin.hIcon = main_icon;
    bWin.hCursor = main_cur;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindow(bWinClassName, L"МЕТЕОРИТНА АТАКА !", WS_CAPTION | WS_SYSMENU, win_x, 10, (int)(scr_width),
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
        if (hr != S_OK)
        {
            LogError(L"Error creating base D2D1 iFactory !");
            ErrExit(eD2D);
        }
        if (iFactory)
        {
            hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
                D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
            if (hr != S_OK)
            {
                LogError(L"Error creating base D2D1 HwndRenderTarget !");
                ErrExit(eD2D);
            }

            if (Draw)
            {
                D2D1_GRADIENT_STOP gStops[2]{};
                ID2D1GradientStopCollection* gColl{ nullptr };

                gStops[0].position = 0;
                gStops[0].color = D2D1::ColorF(D2D1::ColorF::Cyan);
                gStops[1].position = 1.0f;
                gStops[1].color = D2D1::ColorF(D2D1::ColorF::Indigo);

                hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1 GradientStopCollection for buttons background !");
                    ErrExit(eD2D);
                }
                if (gColl)
                {
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b1Rect.left
                        + (b1Rect.right - b1Rect.left) / 2, 25.0f), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2, 25.0f),
                        gColl, &b1BckgBrush);
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b2Rect.left
                        + (b2Rect.right - b2Rect.left) / 2, 25.0f), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2, 25.0f),
                        gColl, &b2BckgBrush);
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b3Rect.left
                        + (b3Rect.right - b3Rect.left) / 2, 25.0f), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2, 25.0f),
                        gColl, &b3BckgBrush);
                    if (hr != S_OK)
                    {
                        LogError(L"Error creating D2D1 RadialGradientBrushes for buttons background !");
                        ErrExit(eD2D);
                    }

                    ClearMem(&gColl);
                }

                gStops[0].position = 0;
                gStops[0].color = D2D1::ColorF(D2D1::ColorF::Goldenrod);
                gStops[1].position = 1.0f;
                gStops[1].color = D2D1::ColorF(D2D1::ColorF::Black);

                hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1 GradientStopCollection for field background !");
                    ErrExit(eD2D);
                }
                if (gColl)
                {
                    hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2,
                        scr_height / 2 - 25.0f), D2D1::Point2F(0, 0), 250.0f, 100.0f), gColl, &FieldBckgBrush);
                    if (hr != S_OK)
                    {
                        LogError(L"Error creating D2D1 RadialGradientBrushes for Field background !");
                        ErrExit(eD2D);
                    }

                    ClearMem(&gColl);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkSlateGray), &StatusBckgBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Chartreuse), &TxtBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &HgltBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Sienna), &InactBrush);

                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1 Brushes for buttons Text !");
                    ErrExit(eD2D);
                }

                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold), &SmallStarBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::AntiqueWhite), &MidStarBrush);
                hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Turquoise), &BigStarBrush);

                if (hr != S_OK)
                {
                    LogError(L"Error creating D2D1 Brushes for Stars !");
                    ErrExit(eD2D);
                }


                bmpCannonL = Load(L".\\res\\img\\cannonL.png", Draw);
                if (!bmpCannonL)
                {
                    LogError(L"Error loading bmpCannonL");
                    ErrExit(eD2D);
                }
                bmpCannonR = Load(L".\\res\\img\\cannonr.png", Draw);
                if (!bmpCannonR)
                {
                    LogError(L"Error loading bmpCannonr");
                    ErrExit(eD2D);
                }
                bmpLaserL = Load(L".\\res\\img\\LaserL.png", Draw);
                if (!bmpLaserL)
                {
                    LogError(L"Error loading bmpLaserL");
                    ErrExit(eD2D);
                }
                bmpLaserR = Load(L".\\res\\img\\Laserr.png", Draw);
                if (!bmpLaserR)
                {
                    LogError(L"Error loading bmpLaserr");
                    ErrExit(eD2D);
                }

                for (int i = 0; i < 48; ++i)
                {
                    wchar_t name[100] = L".\\res\\img\\intro\\0";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpIntro[i] = Load(name, Draw);
                    if (!bmpIntro[i])
                    {
                        LogError(L"Error loading bmpIntro");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 24; ++i)
                {
                    wchar_t name[100] = L".\\res\\img\\explosion\\";
                    wchar_t add[10] = L"\0";

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpExplosion[i] = Load(name, Draw);
                    if (!bmpExplosion[i])
                    {
                        LogError(L"Error loading bmpExplosion");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 127; ++i)
                {
                    wchar_t name[100] = L".\\res\\img\\asteroid1\\0";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"00");
                    else if (i < 100)wcscat_s(name, L"0");
                    
                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpAsteroid1[i] = Load(name, Draw);
                    if (!bmpAsteroid1[i])
                    {
                        LogError(L"Error loading bmpAsteroid1");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 50; ++i)
                {
                    wchar_t name[100] = L".\\res\\img\\asteroid2\\0";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpAsteroid2[i] = Load(name, Draw);
                    if (!bmpAsteroid2[i])
                    {
                        LogError(L"Error loading bmpAsteroid2");
                        ErrExit(eD2D);
                    }
                }
                for (int i = 0; i < 20; ++i)
                {
                    wchar_t name[100] = L".\\res\\img\\asteroid3\\0";
                    wchar_t add[10] = L"\0";

                    if (i < 10)wcscat_s(name, L"0");

                    wsprintf(add, L"%d", i);
                    wcscat_s(name, add);
                    wcscat_s(name, L".png");

                    bmpAsteroid3[i] = Load(name, Draw);
                    if (!bmpAsteroid3[i])
                    {
                        LogError(L"Error loading bmpAsteroid3");
                        ErrExit(eD2D);
                    }
                }
            }
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
        if (hr != S_OK)
        {
            LogError(L"Error creating base D2D1 iWriteFactory !");
            ErrExit(eD2D);
        }
        if (iWriteFactory)
        {
            hr = iWriteFactory->CreateTextFormat(L"SEGOE SCRIPT", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 16, L"", &nrmFormat);
            hr = iWriteFactory->CreateTextFormat(L"SEGOE SCRIPT", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 24, L"", &midFormat);
            hr = iWriteFactory->CreateTextFormat(L"SEGOE SCRIPT", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 62, L"", &bigFormat);
            if (hr != S_OK)
            {
                LogError(L"Error creating base D2D1 iWriteFactory Text Formats !");
                ErrExit(eD2D);
            }
        }
    }

    if (Draw && TxtBrush && bigFormat)
    {
        wchar_t start_txt[46]{ L"МЕТЕОРИТНА АТАКА !    \n\n        dev. Daniel !" };
        wchar_t show_txt[46]{ L"" };
        int intro_frame = 0;
        int txt_pos = 0;

        bool start_ready = false;

        mciSendString(L"play .\\res\\snd\\morse.wav", NULL, NULL, NULL);

        while (!start_ready)
        {
            Draw->BeginDraw();
            Draw->DrawBitmap(bmpIntro[intro_frame], D2D1::RectF(0, 0, scr_width, scr_height));
            show_txt[txt_pos] = start_txt[txt_pos];
            intro_frame++;
            if (intro_frame > 46)start_ready = true;
            Draw->DrawTextW(show_txt, txt_pos, bigFormat, D2D1::RectF(100.0f, 300.0f, scr_width, scr_height), TxtBrush);
            if (txt_pos < 46)++txt_pos;
            Draw->EndDraw();
            Sleep(80);
        }

        PlaySound(L".\\res\\snd\\boom.wav", NULL, SND_SYNC);

    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)
    {
        LogError(L"Error obtaining hInstance !");
        ErrExit(eClass);
    }

    
    
    CreateResources();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessageW(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            if (Draw && bigFormat && TxtBrush)
            {
                Draw->BeginDraw();
                Draw->DrawBitmap(bmpIntro[RandGen(0, 47)], D2D1::RectF(0, 0, scr_width, scr_height));
                Draw->DrawTextW(L"ПАУЗА", 6, bigFormat, D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, scr_width, scr_height),
                    TxtBrush);
                Draw->EndDraw();
                continue;
            }
        }
        //////////////////////////////////////////////////////////

        if (!vStars.empty() && field_dir != dirs::stop)
        {
            for (std::vector<dll::Object>::iterator star = vStars.begin(); star < vStars.end(); ++star)
            {
                if (!(*star)->Move((float)(level), true, field_dir))
                {
                    (*star)->Release();
                    vStars.erase(star);
                    break;
                }
            }
        }
        if (vStars.size() < 150)
        {
            for (int i = (int)(vStars.size()); i <= 150; i++)
            {
                bool is_ok = false;
                while (!is_ok)
                {
                    switch (RandGen(0, 2))
                    {
                    case 0:
                    {
                        dll::Object aStar = dll::Factory(type_small_star, (float)(RandGen(0, (int)(scr_width))), 
                            (float)(RandGen((int)(sky), (int)(ground))));
                        is_ok = true;

                        if (!vStars.empty())
                        {
                            for (int i = 0; i < vStars.size(); ++i)
                            {
                                if (abs(aStar->center.x - vStars[i]->center.x) <= aStar->x_radius + vStars[i]->x_radius
                                    && abs(aStar->center.y - vStars[i]->center.y) <= aStar->y_radius + vStars[i]->y_radius)
                                {
                                    is_ok = false;
                                    break;
                                }
                            }

                            if (is_ok)vStars.push_back(aStar);
                        }
                        else vStars.push_back(aStar);
                    }
                    break;

                    case 1:
                    {
                        dll::Object aStar = dll::Factory(type_mid_star, (float)(RandGen(0, (int)(scr_width))),
                            (float)(RandGen((int)(sky), (int)(ground))));
                        is_ok = true;

                        if (!vStars.empty())
                        {
                            for (int i = 0; i < vStars.size(); ++i)
                            {
                                if (abs(aStar->center.x - vStars[i]->center.x) <= aStar->x_radius + vStars[i]->x_radius
                                    && abs(aStar->center.y - vStars[i]->center.y) <= aStar->y_radius + vStars[i]->y_radius)
                                {
                                    is_ok = false;
                                    break;
                                }
                            }

                            if (is_ok)vStars.push_back(aStar);
                        }
                        else vStars.push_back(aStar);
                    }
                    break;

                    case 2:
                    {
                        dll::Object aStar = dll::Factory(type_big_star, (float)(RandGen(0, (int)(scr_width))),
                            (float)(RandGen((int)(sky), (int)(ground))));
                        is_ok = true;

                        if (!vStars.empty())
                        {
                            for (int i = 0; i < vStars.size(); ++i)
                            {
                                if (abs(aStar->center.x - vStars[i]->center.x) <= aStar->x_radius + vStars[i]->x_radius
                                    && abs(aStar->center.y - vStars[i]->center.y) <= aStar->y_radius + vStars[i]->y_radius)
                                {
                                    is_ok = false;
                                    break;
                                }
                            }

                            if (is_ok)vStars.push_back(aStar);
                        }
                        else vStars.push_back(aStar);
                    }
                    break;
                    }
                   
                }
            }
        }







        // DRAW THINGS ***************************************

        Draw->BeginDraw();

        if (StatusBckgBrush && b1BckgBrush && b2BckgBrush && b3BckgBrush && TxtBrush && HgltBrush && InactBrush && nrmFormat)
        {
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), StatusBckgBrush);
            Draw->FillRectangle(b1Rect, b1BckgBrush);
            Draw->FillRectangle(b2Rect, b2BckgBrush);
            Draw->FillRectangle(b3Rect, b3BckgBrush);

            if (name_set)Draw->DrawTextW(L"ИМЕ НА КАПИТАН", 15, nrmFormat, b1TxtRect, InactBrush);
            else
            {
                if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА КАПИТАН", 15, nrmFormat, b1TxtRect, TxtBrush);
                else Draw->DrawTextW(L"ИМЕ НА КАПИТАН", 15, nrmFormat, b1TxtRect, HgltBrush);
            }
            if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2TxtRect, TxtBrush);
            else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2TxtRect, HgltBrush);
            if (!b3Hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3TxtRect, TxtBrush);
            else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3TxtRect, HgltBrush);
        }

        if (FieldBckgBrush)Draw->FillRectangle(D2D1::RectF(0, sky, scr_width, scr_height), FieldBckgBrush);

        //////////////////////////////////////////////////////

        if (!vStars.empty())
        {
            for (int i = 0; i < vStars.size(); ++i)
            {
                switch (vStars[i]->type)
                {
                case type_small_star:
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vStars[i]->center.x, vStars[i]->center.y), vStars[i]->x_radius,
                        vStars[i]->y_radius), SmallStarBrush);
                    break;

                case type_mid_star:
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vStars[i]->center.x, vStars[i]->center.y), vStars[i]->x_radius,
                        vStars[i]->y_radius), MidStarBrush);
                    break;

                case type_big_star:
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vStars[i]->center.x, vStars[i]->center.y), vStars[i]->x_radius,
                        vStars[i]->y_radius), BigStarBrush);
                    break;
                }
            }
        }
        if (left_laser)Draw->DrawBitmap(bmpCannonL, D2D1::RectF(left_laser->start.x, left_laser->start.y,
            left_laser->end.x, left_laser->end.y));
        if (right_laser)Draw->DrawBitmap(bmpCannonR, D2D1::RectF(right_laser->start.x, right_laser->start.y,
            right_laser->end.x, right_laser->end.y));

        /////////////////////////////////////////////////////
        Draw->EndDraw();
    }

    std::remove(temp_file);
    ClearResources();

    return (int) bMsg.wParam;
}