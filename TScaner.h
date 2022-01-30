#ifndef __SCANER
#define __SCANER
#include "defs.h"

class TScaner
{
private:
	char text[MAX_TEXT];	// Текст программы
	int pos;	// Позиция в тексте программы
	int line;	// Текущая строка
	int posNewLine;	// Позиция новой строки
public:
	void SetPos(int i);
	void SetLine(int i);
	void SetPosNewLine(int i);
	int GetPos(void);
	int GetLine(void);
	int GetPosNewLine(void);
	void PrintError(char*, char*, char*, int, int);
	int Scaner(TypeLex l);
	void GetData(char*);
	TScaner(char*);
	~TScaner() {}
};
#endif


