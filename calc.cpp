#include <stack>
using namespace std;

#include <stdlib.h>
#include <windows.h>

#include "calc.h"

int opt2rank(char c) {
	switch (c) {
	case '+': return 0;
	case '-': return 1;
	case '*': return 2;
	case '/': return 3;
	case '(': return 4;
	case ')': return 5;
	case '#': return 6;
	}
	exit(-1);
}

char opt2pri(char top, char cur) {
	char priTab[7][7] = {
		gt,gt,lt,lt,lt,gt,gt,
		gt,gt,lt,lt,lt,gt,gt,
		gt,gt,gt,gt,lt,gt,gt,
		gt,gt,gt,gt,lt,gt,gt,
		lt,lt,lt,lt,lt,eq,un,
		gt,gt,gt,gt,un,gt,gt,
		lt,lt,lt,lt,lt,un,eq
	};
	return priTab[opt2rank(top)][opt2rank(cur)];
}

bool toRpn(char * rpnb, char * src, int len) {
	stack<char> optr;
	char rpn[80];
	int ir = 0, is = 0, cntOpnd = 0;
	optr.push('#');
	src[len] = '#';
	while (is <= len) {
		if (src[is] >= 'A' && src[is] <= 'Z') {
			rpn[ir++] = src[is++];
			cntOpnd++;
			if (optr.size()<2 && cntOpnd>1)
				return printError();
		}
		else if (isOpt(src[is])) {
			switch (opt2pri(optr.top(), src[is])) {
			case lt:
				optr.push(src[is++]);
				break;
			case gt:
				rpn[ir++] = optr.top();
				optr.pop();
				cntOpnd--;
				if (cntOpnd<1)
					return printError();
				break;
			case eq:
				optr.pop();
				is++;
				break;
			case un:
				return printError();
			}
		}
	}
	if (!optr.empty() || cntOpnd>1)
		return printError();
	for (int i = 0; i<ir; i++)
		rpnb[i] = rpn[i];
	rpnb[ir] = '\0';
	return true;
}

double calcRpn(char *rpn, double *vTab) {
	stack<double> s;
	double a, b;
	for (int i = 0; i<strlen(rpn); i++) {
		if (rpn[i] >= 'A' && rpn[i] <= 'Z')
			s.push(vTab[rpn[i] - 'A']);
		else {
			b = s.top();
			s.pop();
			a = s.top();
			s.pop();
			switch (rpn[i]) {
			case '+':
				s.push(a + b);
				break;
			case '-':
				s.push(a - b);
				break;
			case '*':
				s.push(a*b);
				break;
			case '/':
				s.push(a / b);
				break;
			default:
				exit(1);
			}
		}
	}
	if (s.size() != 1)
		exit(1);
	return s.top();
}

bool printError() {
	MessageBox(NULL, TEXT("表达式语法错误"), TEXT("转换失败"), MB_ICONERROR);
	return false;
}
