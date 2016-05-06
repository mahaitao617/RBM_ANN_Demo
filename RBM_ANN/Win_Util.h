#ifndef _WIN_UTIL_H_
#define _WIN_UTIL_H_

///�����ǿ���̨�����ʽ���Ʋ��ֺ궨��
#ifdef _GNU_SOURCE ///in linux OS
#   define SetText(attr)
#else
#   include <windows.h>
//����ǰ��
#   define FG_R  FOREGROUND_RED
#   define FG_G  FOREGROUND_GREEN
#   define FG_B  FOREGROUND_BLUE
#   define FG_HL FOREGROUND_INTENSITY    //������ʾ
//���ñ���
#   define BG_R  BACKGROUND_RED
#   define BG_G  BACKGROUND_GREEN
#   define BG_B  BACKGROUND_BLUE
#   define BG_HL BACKGROUND_INTENSITY    //������ʾ
//�����ı�
#   define SetText(attr) \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),attr)

#endif // _GNU_SOURCE

#define setBlue() SetText(FG_HL | FG_B)
#define setGreen() SetText(FG_HL | FG_G)
#define setWhite() SetText(FG_HL | FG_R | FG_G | FG_B)
#define setDefault() SetText(FG_R | FG_G | FG_B)

#ifdef _GNU_SOURCE ///in linux OS
#include <stdlib.h> //system()
#include <stdio.h>  //sprintf()
#include <unistd.h> //STDIN_FILENO, sleep()
#define MB_YESNO 4
#define IDYES    6
#define ShellExecute(...)
#ifndef CLK_TCK
#   define CLK_TCK 1000000
#endif // CLK_TCK
inline char checkKeyDown()
{
    system("stty cbreak -echo");
    struct timeval tv = {0};
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);
    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
    char key = 0;
    if(FD_ISSET(STDIN_FILENO, &rdfs)!=0) {
		key = getchar();
        putchar('\r');
    }
    system("stty cbreak echo");
    return key;
}
inline int MessageBox(int, const char* content, const char* title, int)
{
    char str[128];
    sprintf(str, "gdialog --title \"%s\" --yesno \"%s\"", title, content);
    return system(str)==0 ? IDYES : 7;
}
inline void createFolder(const char* folder)
{
    char str[128];
    sprintf(str, "mkdir %s", folder);
    system(str);   //�����ļ���
}

#else ///in windows OS

#include <conio.h>  //getch(), kbhit()
#include <direct.h> //_mkdir()
inline char checkKeyDown()
{
    return _kbhit() ? _getch() : 0;
}
inline void createFolder(const char* folder)
{
    _mkdir(folder);
}
///�������̺߳���ʱ��Ҫ�õ���һЩ������߳�
#ifdef __AFXWIN_H__
#   define HAND AfxGetMainWnd()->m_hWnd
#   define RETURN_TYPE UINT
#   define RETURN_VALUE 0
#else
#   define HAND 0
#   define RETURN_TYPE void
#   define RETURN_VALUE
#endif

#endif //linux

///��vs�е�һЩ�ļ���д��������
#ifndef FILE_VS
#   define FILE_VS
#   ifdef _MSC_VER
#       define FILEOPEN(fp,filename,mode) fopen_s(&fp,filename,mode)
#       define FPRINTF fprintf_s
#       define SPRINTF sprintf_s
#       define FSCANF  fscanf_s
#   else
#       define FILEOPEN(fp,filename,mode) fp = fopen(filename,mode)
#       define FPRINTF fprintf
#       define SPRINTF sprintf
#       define FSCANF  fscanf
#   endif // _MSC_VER
#endif // !FILE_VS

#ifdef _MSC_VER
#   include <tchar.h>
#   define LOCALTIME(_Tm,_Time) localtime_s(_Tm,_Time)
#else
#   define _T(x) x
#   define LOCALTIME(_Tm,_Time) *_Tm = *localtime(_Time)
#endif

#endif // _WIN_UTIL_H_
