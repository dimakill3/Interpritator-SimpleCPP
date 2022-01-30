#define _CRT_SECURE_NO_WARNINGS
#ifndef __DEFS
#define __DEFS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEXT 10000	// Максимальный размер текста программы
#define MAX_LEX	100 // Максимальная длина лексемы
#define MAX_KEYW	9 // Число ключевых слов
typedef char TypeLex[MAX_LEX];

// Ключевые слова
#define TMain	50
#define TFor	55
#define TKeyConst	56
#define TTypeInt	60
#define TTypeShort	61
#define TTypeLong	62
#define TRet	80
#define TError	100
#define TEnd	200
// Идентификаторы и константы
#define TIdent	5
#define TConsInt	10
#define TConsHex	11
// Знаки операций
#define TEqual	20
#define TNotEqual	21
#define TLess	22
#define TLessOrEqual	23
#define TMore	24
#define TMoreOrEqual	25
#define TAdd	26
#define TSub	27
#define TMul	28
#define TDiv	29
#define TDivPart	30
#define TInc	31
#define TDec	32
#define TAssign	35
// Спец. символы
#define TLeftRB 	40
#define TRightRB 	41
#define TLeftFB 	42
#define TRightFB 	43
#define TComma 	44
#define TEndComma 	45
#endif

