// WindowAPI2.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowAPI2.h"

//c++ header
#include <ObjIdl.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

#pragma comment(lib,"msimg32.lib")


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

//double buffering
HBITMAP hDoubleBufferImage;


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

//double buffering
void DrawBitmapDoubleBuffering(HWND hWnd, HDC hdc);

//GDI+
ULONG_PTR g_GdiToken;

void GDI_Init();
void GDI_Draw(HDC hdc);
void OnGDI_Paint(HDC hdc);
void GDI_End();

// Dialog
BOOL CALLBACK MyDlg_Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MyDlg_Proc2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static HWND hModallessDlg;
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
	HACCEL hAcc;
	hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(ID_FUNC1));

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

	GDI_Init();

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

	GDI_End();

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
		SetTimer(hWnd, 123, 1, AniProc);
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
				DialogBox(hInst, MAKEINTRESOURCE(IDD_MYDIALOG), hWnd, MyDlg_Proc);
				break;
			case ID_FUNC2:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_MYDIALOG2), hWnd, MyDlg_Proc2);

				//if (!IsWindow(hModallessDlg))
				//{
				//	hModallessDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MYDIALOG2), hWnd, MyDlg_Proc2);
				//	ShowWindow(hModallessDlg, SW_SHOW);
				//}
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
//			DrawAnimation(hWnd, hdc);
//			DrawRectText(hdc);
//			DrawBitmapDoubleBuffering(hWnd, hdc);


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

void DrawBitmapDoubleBuffering(HWND hWnd, HDC hdc)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;
	int bx, by;

	HDC hMemDC2;
	HBITMAP hOldBitmap2;

	hMemDC = CreateCompatibleDC(hdc); // hdc와 호환가능한 새로운 DC생성

	if (!hMemDC)
	{
		MessageBox(hWnd, _T("CreateCompatibleDC failed")
			, _T("Error"), MB_OK);
		return;
	}

	if (hDoubleBufferImage == NULL)
	{
		hDoubleBufferImage = CreateCompatibleBitmap(hdc,
			rectView.right, rectView.bottom);
	}

	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hDoubleBufferImage);

	// Back
	{
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, hBackImage);
		bx = bitBack.bmWidth;
		by = bitBack.bmHeight;

		BitBlt(hMemDC, 0, 0, bx, by, hMemDC2, 0, 0, SRCCOPY);

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}
	//sigong
	{
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, hTransparentImage);
		bx = bitTransparent.bmWidth;
		by = bitTransparent.bmHeight;

		BitBlt(hMemDC, 100, 200, bx, by, hMemDC2, 0, 0, SRCCOPY);
		TransparentBlt(hMemDC, 200, 200, bx, by, hMemDC2, 0, 0,
			bx, by, RGB(255, 0, 255));

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);

	}

	//animation
	{
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, hAniImage);
		bx = SPRITE_SIZE_X;
		by = SPRITE_SIZE_Y;

		TransparentBlt(hMemDC, AniPos.x, AniPos.y, bx * 3, by * 3, hMemDC2, (curframe + xStart) * bx, yStart * by, bx, by, RGB(255, 255, 255));

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}
	//GDI
	GDI_Draw(hMemDC);

	//
	BitBlt(hdc, 0, 0, rectView.right, rectView.bottom,
		hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
}

void Update() {
	DWORD newTime = GetTickCount();
	static DWORD oldTime = newTime;
	if (newTime - oldTime < 2) return;
	oldTime = newTime;

	// : to do something
	UpdateFrame();
}

void UpdateFrame() {
	if (GetKeyState(VK_RIGHT) & 0x8000) {
		AniPos.x += 1;
		xStart = 0;
		yStart = 2;
	}
	if (GetKeyState(VK_LEFT) & 0x8000) {
		AniPos.x -= 1;
		xStart = 0;
		yStart = 1;
	}
	if (GetKeyState(VK_UP) & 0x8000) {
		AniPos.y -= 1;
		xStart = 0;
		yStart = 3;
	}
	if (GetKeyState(VK_DOWN) & 0x8000) {
		AniPos.y += 1;
		xStart = 0;
		yStart = 0;
	}
	if ((GetKeyState(VK_RIGHT) & 0x8000) && (GetKeyState(VK_DOWN) & 0x8000))
	{
		AniPos.x += 1;
		AniPos.y += 1;
		xStart = 6;
		yStart = 0;
	}
	if ((GetKeyState(VK_LEFT) & 0x8000) && (GetKeyState(VK_DOWN) & 0x8000))
	{
		AniPos.x -= 1;
		AniPos.y += 1;
		xStart = 6;
		yStart = 1;
	}
	if ((GetKeyState(VK_RIGHT) & 0x8000) && (GetKeyState(VK_UP) & 0x8000))
	{
		AniPos.x += 1;
		AniPos.y -= 1;
		xStart = 6;
		yStart = 3;
	}
	if ((GetKeyState(VK_LEFT) & 0x8000) && (GetKeyState(VK_UP) & 0x8000))
	{
		AniPos.x -= 1;
		AniPos.y -= 1;
		xStart = 6;
		yStart = 2;
	}

}


void GDI_Init()
{
	GdiplusStartupInput gpsi;
	GdiplusStartup(&g_GdiToken, &gpsi, NULL);
}
void GDI_Draw(HDC hdc)
{
	OnGDI_Paint(hdc);
}
void OnGDI_Paint(HDC hdc)
{
	Graphics graphics(hdc);

	// text
	SolidBrush brush(Color(255, 255, 0, 0));
	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	PointF pointf(10.0f, 20.0f);
	graphics.DrawString(L"HelloGDI+", -1, &font, pointf, &brush);

	//line
	Pen pen(Color(120, 0, 255, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);

	//image
	Image img((WCHAR *)L"images/sigong.png");
	int w = img.GetWidth();
	int h = img.GetHeight();
	graphics.DrawImage(&img, 100, 100, w, h);

	//image rotation
	Image *pImage = nullptr;
	pImage = Image::FromFile((WCHAR *)L"images/sigong.png");

	Gdiplus::Matrix mat;
	static int rot = 0;
	int xpos = 200;
	int ypos = 100;

	rot += 15;
	mat.RotateAt(rot % 360,
		Gdiplus::PointF(xpos + (float)w/2,
		ypos + (float)(h / 2)));
	graphics.SetTransform(&mat);
	graphics.DrawImage(pImage, 200, 100, w, h);

	//
	ImageAttributes imgAttr;
	imgAttr.SetColorKey(Color(240, 0, 240), (Color(255, 10, 255)));

	mat.RotateAt(-(rot % 360),
		Gdiplus::PointF(xpos + (float)w / 2,
			ypos + (float)(h / 2)));
	graphics.SetTransform(&mat);
	rot += 15;
	xpos = 300;

	graphics.DrawImage(pImage,
		Rect(xpos - (float)w / 2, ypos - (float)h / 2, w, h),
		0, 0, w, h,
		UnitPixel, &imgAttr);

	//
	brush.SetColor(Color(128, 255, 0, 0));
	graphics.FillRectangle(&brush, 400, 250, 200, 300);

	//


	static REAL transparency;
	transparency += 0.1f;
	if (transparency > 1.0f)
		transparency = 0.0f;
	ColorMatrix colorMatrix =
	{
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, transparency, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};
	imgAttr.SetColorMatrix(&colorMatrix);
	xpos = 400;
	graphics.DrawImage(pImage,
		Rect(xpos - (float)w / 2, ypos - (float)h / 2, w, h),
		0, 0, w, h,
		UnitPixel, &imgAttr);

	//
	colorMatrix =
	{
		0.3f, 0.3f, 0.3f, 0.0f, 0.0f,
		0.6f, 0.6f, 0.6f, 0.0f, 0.0f,
		0.1f, 0.1f, 0.1f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};
	imgAttr.SetColorMatrix(&colorMatrix);
	xpos = 500;
	graphics.DrawImage(pImage,
		Rect(xpos - (float)w / 2, ypos - (float)h / 2, w, h),
		0, 0, w, h,
		UnitPixel, &imgAttr);

	if (pImage) delete pImage;

}
void GDI_End()
{
	GdiplusShutdown(g_GdiToken);
}

BOOL CALLBACK MyDlg_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static int Check[3], Radio;
	TCHAR hobby[][30] = { _T("독서"), _T("음악감상"), _T("웹서핑") };
	TCHAR sex[][30] = { _T("여자"), _T("남자") };
	TCHAR output[128];

	switch (iMsg)
	{
	case WM_INITDIALOG:
	{
//		HWND hWndBtn = GetDlgitem(hDlg, IDC_PAUSE_BTN);
//		EnableWindow()
		return 1;
	}
	break;
	case WM_COMMAND:
	{
		
		TCHAR text[128];

		switch (LOWORD(wParam))
		{
		case IDC_CHECK_READING:
			Check[0] = 1 - Check[0];
			break;
		case IDC_CHECK_MUSIC:
			Check[1] = 1 - Check[1];
			break;
		case IDC_CHECK_WEB:
			Check[2] = 1 - Check[2];
			break;
		case IDC_RADIO_MALE:
			Radio = 1;
			break;
		case IDC_RADIO_FEMALE:
			Radio = 0;
			break;

		case IDC_BUTTON_OUTPUT:
			_stprintf_s(output, _T("선택한 취미는 %s %s %s 입니다.\r\n")
				_T("선택한 성별은 %s 입니다."),
				Check[0] ? hobby[0] : _T(""),
				Check[1] ? hobby[1] : _T(""),
				Check[2] ? hobby[2] : _T(""),
				sex[Radio]);
			SetDlgItemText(hDlg, IDC_EDIT_OP, output);

			break;
		case IDC_START_BTN:
			SetDlgItemText(hDlg, IDC_MYDLG_STATIC, _T("시작"));
			{
				HWND hWndBtn = GetDlgItem(hDlg, IDC_START_BTN);
				EnableWindow(hWndBtn, false);

				hWndBtn = GetDlgItem(hDlg, IDC_PAUSE_BTN);

				EnableWindow(hWndBtn, true);
			}
			break;
		case IDC_PAUSE_BTN:
			SetDlgItemText(hDlg, IDC_MYDLG_STATIC, _T("정지"));
			{
				HWND hWndBtn = GetDlgItem(hDlg, IDC_START_BTN);
				EnableWindow(hWndBtn, true);

				hWndBtn = GetDlgItem(hDlg, IDC_PAUSE_BTN);
				EnableWindow(hWndBtn, false);
			}
			break;
		case IDC_BUTTON_COPY:

			GetDlgItemText(hDlg, IDC_INPUT_EDIT, text, 128);
			SetDlgItemText(hDlg, IDC_OUTPUT_EDIT, text);
			break;
		case IDC_BUTTON_DELETE:
			text[0] = NULL;
			SetDlgItemText(hDlg, IDC_OUTPUT_EDIT, text);
			break;
		case IDC_EXIT_BTN:
			EndDialog(hDlg, 0);
			break;

		}
	}
	break;
	}


	return 0;
}


BOOL CALLBACK MyDlg_Proc2(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hcombo;
	static int selection;
	TCHAR name[20];


	switch (iMsg)
	{
	case WM_INITDIALOG:
		hcombo = GetDlgItem(hDlg, IDC_COMBO_LIST);
		return 1;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_INSERT:
			GetDlgItemText(hDlg, IDC_EDIT_NAME, name, 20);
			if (_tcscmp(name, _T("")))
				SendMessage(hcombo, CB_ADDSTRING, 0, (LPARAM)name);
				return 0;
				break;
		case IDC_BUTTON_DELETE:
			SendMessage(hcombo, CB_DELETESTRING, selection, 0);
			return 0;
		case IDC_COMBO_LIST:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				selection = SendMessage(hcombo, CB_GETCURSEL, 0, 0);
			break;
		case IDCLOSE:
			EndDialog(hDlg, 0);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return 0;

		}
		break;
	}
	return 0;
}