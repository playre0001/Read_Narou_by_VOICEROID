#ifndef _LOADNAROU_HEADER_ 2017_
#define _LOADNAROU_HEADER_ 2017_

#pragma warning(disable:4996)

#pragma comment(lib, "wininet.lib")

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wchar.h>
#include<Windows.h>
#include<WinInet.h>

#ifndef BUFFERSIZE
#define BUFFERSIZE 1000
#endif
#ifndef OPENFILENAME
#define OPENFILENAME L"novel.txt"
#endif

typedef struct {
	HWND hwnd;
	char windowname[256];
} param;

int LoadNarou(char *url);
int Say(char *targetname);

#endif
