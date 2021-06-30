// WindowAPI2.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowAPI2.h"

//c++ header


#pragma comment(lib,"msimg32.lib");


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// >> : image
POINT AniPos = { 0,0 };
HBITMAP hBackImage;
BITMAP bitBack;

HBITMAP hTransparentImage;
BITMAP bitTransparent;
// : animation
HBITMAP hAniImage;
BITMAP bitAni;
const int SPRITE_SIZE_X = 32;
const int SPRITE_SIZE_Y = 32; 
int xStart = 0;
int yStart = 0;

// : run
int RUN_FRAME_MAX = 6;
int RUN_FRAME_MIN = 0;
int curframe = RUN_FRAME_MIN;
RECT rectView;




void CreateBitmap();

void DrawBitmap(HWND hWnd, HDC hdc);
void DrawTransBitmap(HWND hWnd, HDC hdc);
void DrawAnimation(HWND hWnd, HDC hdc);
void Update();
void UpdateFrame();
void UpdateFrame(HWND hWnd);
void DeleteBitmap();

VOID CALLBACK AniProc(HWND hWnd, UINT uMsg,
	UINT idEvent, DWORD dwTime);

void DrawRectText(HDC hdc);


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWAPI2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWAPI2));

	MSG msg;

	// 기본 메시지 루프입니다:
	//while (GetMessage(&msg, nullptr, 0, 0))
	//{
	//    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//    {
	//        TranslateMessage(&msg);
	//        DispatchMessage(&msg);
	//    }
	//}

	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			Update();
		}
	}

	return (int)msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWAPI2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWAPI2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bselectd = false;


	// >>  : image

    switch (message)
    {
	case WM_CREATE:
	{
		GetClientRect(hWnd, &rectView);
		CreateBitmap();
	//	SetTimer(hWnd, 123, 100, NULL);
		SetTimer(hWnd, 123, 100, AniProc);
	}
		break;
	case WM_TIMER:
	{
		UpdateFrame(hWnd);
		InvalidateRect(hWnd, NULL, false);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_FUNC1:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

			DrawBitmap(hWnd, hdc);
//			DrawTransBitmap(hWnd, hdc);
			DrawAnimation(hWnd, hdc);
//			DrawRectText(hdc);



            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteBitmap();
		KillTimer(hWnd, 123);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;

}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void CreateBitmap()
{
	hBackImage = (HBITMAP)LoadImage(NULL, TEXT("images/수지.bmp"),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hBackImage, sizeof(BITMAP), &bitBack);

	hTransparentImage = (HBITMAP)LoadImage(NULL, TEXT("images/sigong.bmp"),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hTransparentImage, sizeof(BITMAP), &bitTransparent);

	hAniImage = (HBITMAP)LoadImage(NULL, TEXT("images/zero_run2.bmp"),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);	
	GetObject(hAniImage, sizeof(BITMAP), &bitAni);

	RUN_FRAME_MAX = 5;
	RUN_FRAME_MIN = 0;
	curframe = RUN_FRAME_MIN;
}

void DrawBitmap(HWND hWnd, HDC hdc)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;
	int bx = 1000, by = 1000;

	hMemDC = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackImage);


	BitBlt(hdc, 0, 0, bx, by, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);

	DeleteDC(hMemDC);
}


void DrawTransBitmap(HWND hWnd, HDC hdc)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;
	int bx, by;

	hMemDC = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hTransparentImage);
	bx = bitTransparent.bmWidth;
	by = bitTransparent.bmHeight;


	BitBlt(hdc, 100, 200, bx, by, hMemDC, 0, 0, SRCCOPY);
	TransparentBlt(hdc, 300, 300, bx, by, hMemDC, 0, 0,
		bx,by,RGB(255, 0, 255));


	SelectObject(hMemDC, hOldBitmap);

	DeleteDC(hMemDC);
}

void DrawAnimation(HWND hWnd, HDC hdc)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;
	int bx, by;

	hMemDC = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hAniImage);
	bx = bitAni.bmWidth / 12;
	by = bitAni.bmHeight / 4;

	TransparentBlt(hdc, AniPos.x, AniPos.y, bx, by, hMemDC, (curframe + xStart) * SPRITE_SIZE_X, yStart, bx, by, RGB(255, 0, 255));

	SelectObject(hMemDC, hOldBitmap);

	DeleteDC(hMemDC);

}

void UpdateFrame(HWND hWnd)
{
	curframe++;
	if (curframe > RUN_FRAME_MAX)
		curframe = RUN_FRAME_MIN;

}

void DeleteBitmap()
{
	DeleteObject(hBackImage);
	DeleteObject(hTransparentImage);
	DeleteObject(hAniImage);
}
VOID CALLBACK AniProc(HWND hWnd, UINT uMsg,
	UINT idEvent, DWORD dwTime)
{
	UpdateFrame(hWnd);
	InvalidateRect(hWnd, NULL, false);
}

void DrawRectText(HDC hdc)
{
	static int yPos = 0;
	TCHAR strTest[] = _T("이미지 출력");
	TextOut(hdc, 0, yPos, strTest, _tcslen(strTest));
	yPos += 5;
	if (yPos > rectView.bottom) yPos = 0;
}

void Update() {
	DWORD newTime = GetTickCount();
	static DWORD oldTime = newTime;
	if (newTime - oldTime < 100) return;
	oldTime = newTime;

	// : to do something
	UpdateFrame();
}

void UpdateFrame() {
	if (GetKeyState(VK_RIGHT) & 0x8000) {
		AniPos.x += 10;
		xStart = 0;
		yStart = 2;
		yStart *= SPRITE_SIZE_Y;
	}
	if (GetKeyState(VK_LEFT) & 0x8000) {
		AniPos.x -= 10;
		xStart = 0;
		yStart = 1;
		yStart *= SPRITE_SIZE_Y;
	}
	if (GetKeyState(VK_UP) & 0x8000) {
		AniPos.y -= 10;
		xStart = 0;
		yStart = 3;
		yStart *= SPRITE_SIZE_Y;
	}
	if (GetKeyState(VK_DOWN) & 0x8000) {
		AniPos.y += 10;
		xStart = 0;
		yStart = 0;
		yStart *= SPRITE_SIZE_Y;
	}
	if ((GetKeyState(VK_RIGHT) & 0x8000) && (GetKeyState(VK_DOWN) & 0x8000))
	{
		AniPos.x += 10;
		AniPos.y += 10;
		xStart = 6;
		yStart = 0;
		yStart *= SPRITE_SIZE_Y;
	}
	if ((GetKeyState(VK_LEFT) & 0x8000) && (GetKeyState(VK_DOWN) & 0x8000))
	{
		AniPos.x -= 10;
		AniPos.y += 10;
		xStart = 6;
		yStart = 1;
		yStart *= SPRITE_SIZE_Y;
	}
	if ((GetKeyState(VK_RIGHT) & 0x8000) && (GetKeyState(VK_UP) & 0x8000))
	{
		AniPos.x += 10;
		AniPos.y -= 10;
		xStart = 6;
		yStart = 3;
		yStart *= SPRITE_SIZE_Y;
	}
	if ((GetKeyState(VK_LEFT) & 0x8000) && (GetKeyState(VK_UP) & 0x8000))
	{
		AniPos.x -= 10;
		AniPos.y -= 10;
		xStart = 6;
		yStart = 2;
		yStart *= SPRITE_SIZE_Y;
	}

}

