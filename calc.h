#ifndef CALCH
#define CALCH

#define gt ('>')
#define lt ('<')
#define eq ('=')
#define un (' ')


#define isOpt(ch) ( (ch)=='+'||(ch)=='-'||(ch)=='*' \
		||(ch)=='#'||(ch)=='/'||(ch)=='('||(ch)==')' )

bool toRpn(char * rpn, char * src, int len);
double calcRpn(char *rpn, double *vTab);
bool printError();

#endif