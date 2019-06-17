#include <Windows.h>
#include <stdio.h>
#include <time.h>

#define BitsPerPixel 24

int main(int argc, char* argv[]) {
	HWND hWnd = GetConsoleWindow(); // ���� ���� â�� �ڵ� ��������
	ShowWindow(hWnd, SW_HIDE); // ���� ����â�� hide
	Sleep(300); //to wait while hide console application
	
	int x1, y1, x2, y2, w, h;

	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN); // ���ʳ�
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN); // ���ʳ�
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);// ������
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);// ������

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


	HDC hScreen = GetDC(NULL); // DC = Device Context, HDC = Handle DC, ��ü ��ũ��(NULL)�� ���� DC �ڵ��� ������

						//��üȭ��� ������ ������ �޸�DC(hDC)�� ������Ʈ(hBitmap)�����ϴ� ����
	HDC hDC = CreateCompatibleDC(hScreen); // �ڵ�� ������ �޸�DC�� ����
					//HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h); // ��ü ��ũ�� DC�� ȣȯ�Ǵ� ��Ʈ��(����w,����h) ����
					//HGDIOBJ old_obj = SelectObject(hDC, hBitmap); // �޸�DC�� �� ������Ʈ(hBitmap,��ü��ũ���� ������ ũ��) ����
	HBITMAP Section = CreateDIBSection(hScreen, &Info, DIB_RGB_COLORS, (void**)&pixels, 0, 0);//DIB(��ġ ���� ��Ʈ��)����
	SelectObject(hDC, Section);

	// �޸�DC(Section)�� ��üȭ��(hScreen)�� ����,��üȭ�� ũ�� �״��(srccopy) 
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
	OpenClipboard(NULL);//�̹� �����ִ�(NULL) Ŭ�����带 ����
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap); //ǥ��Ŭ����������(CF_BITMAP,��Ʈ���ڵ�), �޸�DC�� ������Ʈ�� ���� �ڵ�

	CloseClipboard();
	*/
	CloseHandle(hWnd);
	return 0;
}