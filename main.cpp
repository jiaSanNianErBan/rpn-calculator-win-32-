#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

//CalcE.h和Calc.cpp中是转换表达式、计算表达式的逻辑
#include "calc.h" 

//主窗口的窗口过程，交互逻辑在此函数中
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 存储显示信息
void showText(char *buf, char *rpn, HWND hwnd, int len, int iyChar);

// 给变量赋值
void inputVar(WPARAM wParam);

// 找出表达式中有哪些变量
void lookHasVar(char * s, int len); 

// 判断表达式中所有变量是否都已经赋值
bool hasAllNum();

void str2num();

// 原始中缀表达式的输入缓冲
char origBuf[80];
int origBufLen = 0;

// 转换成的后缀表达式的缓冲
char rpn[80] = "";

HINSTANCE hInst;
HWND hwd;

char assigBtnText[26][2];

// 存储变量对应的数值
double varTable[26];

// 每个变量的输入缓冲
char assigBuf[26][80];
int assigBufLen[26] = { 0 };

// 输入变量时记录已输入小数点个数
int cntPoint[26] = { 0 };

// 记录表达式中有哪些变量
bool hasVar[26] = { 0 };

// 记录当前输入焦点
int inTag = -1;

// 记录程序当前状态
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
		MessageBox(hwnd, TEXT("窗口类注册失败!"), szAppName, MB_ICONERROR);

	for (int i = 0; i < 26; i++) {;
		assigBtnText[i][0] = 'A'+i;
		assigBuf[i][0] = assigBtnText[i][1] = '\0';
	}
	hwnd = CreateWindow(szAppName, TEXT("数据结构课程设计:计算器"), 
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

	static TCHAR* funBtnText[] = { TEXT("转换"), TEXT("计算"), TEXT("复位") };
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

	case WM_COMMAND://按钮消息
		if (wParam == BTTRANS && status != 0)
			MessageBox(hwnd, TEXT("已经转换过了。"), TEXT("转换"), MB_OK);
		else if (wParam == BTTRANS && origBufLen <= 0)
			MessageBox(hwnd, TEXT("还没有输入中缀表达式呢,请先输入"), TEXT("转换"), MB_OK);
		else if (wParam == BTTRANS && toRpn(rpn, origBuf, origBufLen)) {
				status = 1;
				MessageBox(hwnd, TEXT("转换成功!"), TEXT("转换"), MB_OK);
				lookHasVar(origBuf, origBufLen);
		}
		else if (wParam == BTCALC && status == 2 && hasAllNum()) {
			char b1[30];
			str2num();
			double r = calcRpn(rpn, varTable);
			sprintf(b1, "%.6lf", r);
			MessageBoxA(hwnd, b1, "计算完成", MB_OK);
		}
		else if(wParam==BTCALC && status!=2)
			MessageBox(hwnd, TEXT("请先给表达式中的所有变量赋值，再来计算"), TEXT("计算"), MB_OK);
		else if (wParam == BTRESET) {
			status = 0;
			origBufLen = 0;
			rpn[0] = 0;
			inTag = -1;
			for (int i = 0; i < 26; i++)
				varTable[i] = assigBufLen[i] = assigBuf[i][0] = cntPoint[i] = 0;
			MessageBox(hwnd, TEXT("复位成功。"), TEXT("复位"), MB_OK);
		}
		else if (wParam >= 'A' && wParam <= 'Z' && status == 0)
			MessageBox(hwnd, TEXT("还没有转换呢，请先转换。"), TEXT("赋值"), MB_OK);
		else if(wParam >= 'A' && wParam <= 'Z'&& (status==1 || status==2)){
			status = 2;
			TCHAR b[20];
			if (!hasVar[wParam - 'A']) {
				wsprintf(b, TEXT("表达式中没有%c。"), wParam);
				MessageBox(hwnd, b, TEXT("赋值"), MB_OK);
			}
			else {
				inTag = wParam - 'A';
				wsprintf(b, TEXT("给%c赋值。"), wParam);
				MessageBoxW(hwnd, b, TEXT("赋值"), MB_OK);
			}
		}
		showText(origBuf, rpn, hwnd, origBufLen, iyChar);
		return 0;

	case WM_PAINT:
		showText(origBuf, rpn, hwnd, origBufLen, iyChar);
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_CHAR: //键盘消息
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
	static TCHAR * promptArrStr[] = { TEXT("请按键盘上的 字母键、+、-、*、/ 键 输入表达式。"),
		TEXT("如输入错误，可按退格键删除。输完按转换按钮转换。"),
		TEXT("转换后可按左边字母按钮可给对应变量赋值。"),
		TEXT("赋值后可按计算按钮计算") };
	for(int i=0; i<4; i++)
		TextOut(hdc, 10, iyChar * (8+i*2), promptArrStr[i], lstrlen(promptArrStr[i]));

	static TCHAR * promptRpnAndInFix[] = { TEXT("输入的中缀表达式："), TEXT("转成的后缀表达式：") };
	static char fBuf[300]; sprintf(fBuf, "%280s", " ");
	char * pArrRpnAndInfix[] = { buf, rpn };
	int lenRpnAndInfix[] = { len, strlen(rpn) };
	for (int i = 0; i < 2; i++) {
		TextOut(hdc, 20, iyChar * (2+i*2), promptRpnAndInFix[i], lstrlen(promptRpnAndInFix[i]));
		TextOutA(hdc, 155, iyChar * (2 + i * 2), fBuf, strlen(fBuf));
		TextOutA(hdc, 155, iyChar * (2 + i * 2), pArrRpnAndInfix[i], lenRpnAndInfix[i]);
	}

	TCHAR *promptFunBtn = TEXT("按对应按钮完成相应功能。");
	TextOut(hdc, 35, iyChar * 18, promptFunBtn, lstrlen(promptFunBtn));
	TCHAR *promptAIn = TEXT("赋值按钮区：");
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
