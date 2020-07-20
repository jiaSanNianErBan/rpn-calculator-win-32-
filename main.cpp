#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

//CalcE.h��Calc.cpp����ת�����ʽ��������ʽ���߼�
#include "calc.h" 

//�����ڵĴ��ڹ��̣������߼��ڴ˺�����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// �洢��ʾ��Ϣ
void showText(char *buf, char *rpn, HWND hwnd, int len, int iyChar);

// ��������ֵ
void inputVar(WPARAM wParam);

// �ҳ����ʽ������Щ����
void lookHasVar(char * s, int len); 

// �жϱ��ʽ�����б����Ƿ��Ѿ���ֵ
bool hasAllNum();

void str2num();

// ԭʼ��׺���ʽ�����뻺��
char origBuf[80];
int origBufLen = 0;

// ת���ɵĺ�׺���ʽ�Ļ���
char rpn[80] = "";

HINSTANCE hInst;
HWND hwd;

char assigBtnText[26][2];

// �洢������Ӧ����ֵ
double varTable[26];

// ÿ�����������뻺��
char assigBuf[26][80];
int assigBufLen[26] = { 0 };

// �������ʱ��¼������С�������
int cntPoint[26] = { 0 };

// ��¼���ʽ������Щ����
bool hasVar[26] = { 0 };

// ��¼��ǰ���뽹��
int inTag = -1;

// ��¼����ǰ״̬
int status = 0; 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Data structure course design (1)");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	hInst = hInstance;
	hwd = hwnd;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
		MessageBox(hwnd, TEXT("������ע��ʧ��!"), szAppName, MB_ICONERROR);

	for (int i = 0; i < 26; i++) {;
		assigBtnText[i][0] = 'A'+i;
		assigBuf[i][0] = assigBtnText[i][1] = '\0';
	}
	hwnd = CreateWindow(szAppName, TEXT("���ݽṹ�γ����:������"), 
	                        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
	                        CW_USEDEFAULT, 
	                        CW_USEDEFAULT, CW_USEDEFAULT, 
	                        CW_USEDEFAULT, NULL, NULL, hInstance, NULL);


	MoveWindow(hwnd, 300,100,650,400,1);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	SendMessage(hwnd, WM_PAINT, NULL, NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

#define BTTRANS 21
#define BTCALC 22
#define BTRESET 23

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int iyChar;
	int yc;
	TEXTMETRIC tm;

	static TCHAR* funBtnText[] = { TEXT("ת��"), TEXT("����"), TEXT("��λ") };
	static int funHBtn[] = { BTTRANS, BTCALC, BTRESET };

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		iyChar = tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hwnd, hdc);
		yc = iyChar + 3;
		for (int z = 400; z <= 500; z += 100) {
			for (int i = 0; i < 13; i++) {
				CreateWindowA("Button", assigBtnText[i + (z - 400) / 100*13],
					WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
					z, yc * 6 + yc*i, 25, 15,
					hwnd, (HMENU)('A' + i+(z-400)/100*13), hInst, NULL);
			}
		}
		for(int i=0; i<3; i++)
			CreateWindow(TEXT("Button"), funBtnText[i],
				WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
				35+60*i, iyChar * 20, 50, 25,
				hwnd, (HMENU)funHBtn[i], hInst, NULL);
		return 0;

	case WM_COMMAND://��ť��Ϣ
		if (wParam == BTTRANS && status != 0)
			MessageBox(hwnd, TEXT("�Ѿ�ת�����ˡ�"), TEXT("ת��"), MB_OK);
		else if (wParam == BTTRANS && origBufLen <= 0)
			MessageBox(hwnd, TEXT("��û��������׺���ʽ��,��������"), TEXT("ת��"), MB_OK);
		else if (wParam == BTTRANS && toRpn(rpn, origBuf, origBufLen)) {
				status = 1;
				MessageBox(hwnd, TEXT("ת���ɹ�!"), TEXT("ת��"), MB_OK);
				lookHasVar(origBuf, origBufLen);
		}
		else if (wParam == BTCALC && status == 2 && hasAllNum()) {
			char b1[30];
			str2num();
			double r = calcRpn(rpn, varTable);
			sprintf(b1, "%.6lf", r);
			MessageBoxA(hwnd, b1, "�������", MB_OK);
		}
		else if(wParam==BTCALC && status!=2)
			MessageBox(hwnd, TEXT("���ȸ����ʽ�е����б�����ֵ����������"), TEXT("����"), MB_OK);
		else if (wParam == BTRESET) {
			status = 0;
			origBufLen = 0;
			rpn[0] = 0;
			inTag = -1;
			for (int i = 0; i < 26; i++)
				varTable[i] = assigBufLen[i] = assigBuf[i][0] = cntPoint[i] = 0;
			MessageBox(hwnd, TEXT("��λ�ɹ���"), TEXT("��λ"), MB_OK);
		}
		else if (wParam >= 'A' && wParam <= 'Z' && status == 0)
			MessageBox(hwnd, TEXT("��û��ת���أ�����ת����"), TEXT("��ֵ"), MB_OK);
		else if(wParam >= 'A' && wParam <= 'Z'&& (status==1 || status==2)){
			status = 2;
			TCHAR b[20];
			if (!hasVar[wParam - 'A']) {
				wsprintf(b, TEXT("���ʽ��û��%c��"), wParam);
				MessageBox(hwnd, b, TEXT("��ֵ"), MB_OK);
			}
			else {
				inTag = wParam - 'A';
				wsprintf(b, TEXT("��%c��ֵ��"), wParam);
				MessageBoxW(hwnd, b, TEXT("��ֵ"), MB_OK);
			}
		}
		showText(origBuf, rpn, hwnd, origBufLen, iyChar);
		return 0;

	case WM_PAINT:
		showText(origBuf, rpn, hwnd, origBufLen, iyChar);
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_CHAR: //������Ϣ
		if ((wParam >= 'a' && wParam <= 'z' || wParam >= 'A' && wParam <= 'Z') && status == 0)
			origBuf[origBufLen++] = (((char)wParam)&~0x20);
		else if ((wParam == '+' || wParam == '-' || wParam == '*' || wParam == '/' || wParam == '(' || wParam == ')') && status == 0)
			origBuf[origBufLen++] = (((char)wParam));
		else if (wParam == 8 && status == 0 && origBufLen > 0)
			--origBufLen;
		else if (wParam == 8 && status == 2 && assigBufLen[inTag] > 0)
			assigBufLen[inTag]-- && wParam == '.' && (--cntPoint[inTag]);
		else if (wParam >= '0' && wParam <= '9' || wParam == '.')
			inputVar(wParam);
		showText(origBuf, rpn, hwnd, origBufLen, iyChar);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void showText(char *buf, char *rpn, HWND hwnd, int len, int iyChar) {
	static HDC hdc;
	static char b[80];
	int yc = iyChar + 3;
	hdc = GetDC(hwnd);
	static TCHAR * promptArrStr[] = { TEXT("�밴�����ϵ� ��ĸ����+��-��*��/ �� ������ʽ��"),
		TEXT("��������󣬿ɰ��˸��ɾ�������갴ת����ťת����"),
		TEXT("ת����ɰ������ĸ��ť�ɸ���Ӧ������ֵ��"),
		TEXT("��ֵ��ɰ����㰴ť����") };
	for(int i=0; i<4; i++)
		TextOut(hdc, 10, iyChar * (8+i*2), promptArrStr[i], lstrlen(promptArrStr[i]));

	static TCHAR * promptRpnAndInFix[] = { TEXT("�������׺���ʽ��"), TEXT("ת�ɵĺ�׺���ʽ��") };
	static char fBuf[300]; sprintf(fBuf, "%280s", " ");
	char * pArrRpnAndInfix[] = { buf, rpn };
	int lenRpnAndInfix[] = { len, strlen(rpn) };
	for (int i = 0; i < 2; i++) {
		TextOut(hdc, 20, iyChar * (2+i*2), promptRpnAndInFix[i], lstrlen(promptRpnAndInFix[i]));
		TextOutA(hdc, 155, iyChar * (2 + i * 2), fBuf, strlen(fBuf));
		TextOutA(hdc, 155, iyChar * (2 + i * 2), pArrRpnAndInfix[i], lenRpnAndInfix[i]);
	}

	TCHAR *promptFunBtn = TEXT("����Ӧ��ť�����Ӧ���ܡ�");
	TextOut(hdc, 35, iyChar * 18, promptFunBtn, lstrlen(promptFunBtn));
	TCHAR *promptAIn = TEXT("��ֵ��ť����");
	TextOut(hdc, 405, iyChar * 6 - 5, promptAIn, lstrlen(promptAIn));

	for (int z = 0; z < 2; z++) {
		for (int i = 0; i < 13; i++) {
			sprintf(b, "%16c", ' ');
			TextOutA(hdc, 430 + z * 100, yc * 6 + yc*i, b, strlen(b));
			for (int j = 0; j < assigBufLen[i + z * 13]; j++)
				b[j] = assigBuf[i + z * 13][j];
			b[assigBufLen[i + z * 13]] = '\0';
			if (i + z * 13 == inTag) {
				b[assigBufLen[i + z * 13]] = '_';
				b[assigBufLen[i + z * 13] + 1] = '\0';
			}
			if (inTag == i + z * 13 || assigBufLen[i + z * 13] != 0)
				TextOutA(hdc, 430 + z * 100, yc * 6 + yc*i, b, strlen(b));
		}
	}

	ReleaseDC(hwnd, hdc);
}

void inputVar(WPARAM wParam) {
	if (cntPoint[inTag] > 0 && wParam == '.')
		return;
	if (wParam == '.')
		++cntPoint[inTag];
	char c;
	if (wParam == '.')
		c = '.';
	else
		c = '0' + (wParam - 48);
	assigBuf[inTag][assigBufLen[inTag]++] = c;
}

bool hasAllNum() {
	for (int i = 0; i < 26; i++) {
		if (hasVar[i] && !assigBufLen[i])
			return false;
	}
	return true;
}

void lookHasVar(char * s, int len) {
	for (int i = 0; i < 26; i++)
		hasVar[i] = false;
	for (int i = 0; i < len; i++)
		if (s[i] >= 'A' && s[i] <= 'Z')
			hasVar[s[i] - 'A'] = true;
}

void str2num() {
	for (int i = 0; i < 26; i++) {
		if (hasVar[i]) {
			assigBuf[i][assigBufLen[i]] = '\0';
			sscanf(assigBuf[i], "%lf" , varTable+i);
		}
	}
}
