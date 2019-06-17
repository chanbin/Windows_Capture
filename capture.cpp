#include <Windows.h>
#include <stdio.h>
#include <time.h>

#define BitsPerPixel 24

int main(int argc, char* argv[]) {
	HWND hWnd = GetConsoleWindow(); // 현재 열린 창의 핸들 가져오기
	ShowWindow(hWnd, SW_HIDE); // 현재 열린창을 hide
	Sleep(300); //to wait while hide console application
	
	int x1, y1, x2, y2, w, h;

	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN); // 왼쪽끝
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN); // 위쪽끝
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);// 가로폭
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);// 세로폭

	w = x2 - x1;
	h = y2 - y1;

	BITMAPINFO Info;
	BITMAPFILEHEADER Header;
	memset(&Info, 0, sizeof(Info));
	memset(&Header, 0, sizeof(Header));
	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = w;
	Info.bmiHeader.biHeight = h;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = 24;
	Info.bmiHeader.biCompression = BI_RGB;
	//Info.bmiHeader.biSizeImage = w * h * (BitsPerPixel > 24 ? 4 : 3);
	Info.bmiHeader.biSizeImage = w * h;
	Header.bfType = 0x4D42;
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	char *pixels = NULL;


	HDC hScreen = GetDC(NULL); // DC = Device Context, HDC = Handle DC, 전체 스크린(NULL)에 대한 DC 핸들을 가져옴

						//전체화면과 동일한 설정의 메모리DC(hDC)에 오브젝트(hBitmap)설정하는 과정
	HDC hDC = CreateCompatibleDC(hScreen); // 핸들과 동일한 메모리DC를 생성
					//HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h); // 전체 스크린 DC와 호환되는 비트맵(가로w,세로h) 생성
					//HGDIOBJ old_obj = SelectObject(hDC, hBitmap); // 메모리DC에 들어갈 오브젝트(hBitmap,전체스크린과 동일한 크기) 설정
	HBITMAP Section = CreateDIBSection(hScreen, &Info, DIB_RGB_COLORS, (void**)&pixels, 0, 0);//DIB(장치 독립 비트맵)설정
	SelectObject(hDC, Section);

	// 메모리DC(Section)에 전체화면(hScreen)을 복사,전체화면 크기 그대로(srccopy) 
	BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);
	DeleteDC(hDC);

	time_t timer;
	time(&timer);
	char fname[32];
	sprintf_s(fname, 32, "screenshot_%lld.png", timer);
	FILE *shot;
	if (fopen_s(&shot, fname, "wb") != 0) {
		DeleteObject(Section);
		return 1;
	}
	else {
		fwrite((char*)&Header,1,sizeof(Header),shot);
		fwrite((char*)&Info.bmiHeader, 1, sizeof(Info.bmiHeader), shot);
		fwrite(pixels, 1, (((BitsPerPixel * w + 31) & ~31) / 8) * h, shot);
		fclose(shot);
		DeleteObject(Section);
	}

	/*
	OpenClipboard(NULL);//이미 열려있는(NULL) 클립보드를 연결
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap); //표준클립보드포맷(CF_BITMAP,비트맵핸들), 메모리DC의 오브젝트에 대한 핸들

	CloseClipboard();
	*/
	CloseHandle(hWnd);
	return 0;
}