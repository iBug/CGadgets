// File: sys.h
// Author: iBug

#ifndef _SYS_H
#define _SYS_H

#ifdef __unix__
#define pause Pause
#endif
void pause(void);
void clearKeyBuffer(void);
void clearInputBuffer(void);
void clearScreen(void);

#endif // _SYS_H
