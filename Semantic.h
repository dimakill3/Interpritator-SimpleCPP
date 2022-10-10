#ifndef __SEMAN
#define __SEMAN
#include <string>
#include "defs.h"
#include "TScaner.h"

#pragma once

#define maxInt 2147483647
#define maxLongLong ((long long)9223372036854775807)

// Типы данных
enum DATA_TYPE {
	TYPE_NONE = 0, TYPE_INTEGER,
	TYPE_LONG_LONG
};

// Типы объектов
enum OBJECT_TYPE {
	OBJ_NONE = 0, TYPE_VAR, TYPE_FUNC
};

// Структура для параметров переменных (или констант)
struct VarData
{
	bool constFlag;			// Флаг константы
	char* data;				// Cсылка на значение или NULL
	bool init;				// Флаг инициализации
};

// Структура для параметров функции
struct FuncData
{
	int bodyPos;
};

// Поля для хранения значений
union DataValue
{
	long long llValue;
	int intValue;
};

// Класс данных
class TData
{
public:
	DATA_TYPE dataType;		// Тип значения
	DataValue dataValue;	// Значение
	
	TData()
	{
		dataType = TYPE_INTEGER;
		dataValue.intValue = -maxInt - 1;
	}

	long long GetRealValue()
	{
		switch (dataType)
		{
		case TYPE_INTEGER:
			{
			return  long long (dataValue.intValue);
			}
		case TYPE_LONG_LONG:
			{
			return  dataValue.llValue;
			}
		default: return -maxInt - 1;
		}
	}
};

class Operand
{
	bool isLink;	// Непосредственный или ссылка
	int number;		// Номер триады, если является ссылкой (isLink = true)
	TypeLex lex;	// Лексема непосредственного операнда
};

class Triada
{
	int triadeNum;	// Номер триады
	int TOper;		// Код операции
	Operand operands[2];	// Операнды
};

// Структура данных узла (переменной или функции) в семантическом дереве
struct Node
{
	OBJECT_TYPE objType;	// Тип объекта
	
	TypeLex id;				// Идентификатор
	DATA_TYPE dataType;		// Тип значения
	DataValue dataValue;	// Значение

	union 
	{
		VarData var;
		FuncData func;
	};
};

// Класс семантического дерева
class Tree
{
	Node* n;
	TScaner* sc;

public:
	Tree* up, * l, * r;	// Узлы сверху, слева и справа
	static Tree* cur;
	bool isInterpret = true;
	
	//////////////////////////////
	// Функции работы с деревом //
	//////////////////////////////
	Tree();
	Tree(Tree* l, Tree* r, Tree* up, Node* data);
	void SetScaner(TScaner* sc);	// Установить сканер
	
	void SetLeft(Node* data);	// Установить узел слева
	void SetRight(Node* data);	// Установить узел справа

	void SetLeft(Tree* data);	// Установить узел слева
	void SetRight(Tree* data);	// Установить узел справа
	
	Tree* FindUp(Tree* from, TypeLex id);	// Поиск сверху от конкретной вершины
	Tree* FindUp(TypeLex id);				// Поиск сверху от текущей вершины

	Tree* FindUpOneLevel(Tree* from, TypeLex id);	// Поиск сверху от вершины from по одному уровню вложенности

	void DelSubTree(Tree* addr); // Удаление поддерева
	void DelSubTreeForFunc(Tree* addr);	// Удаление тела функции
	Tree* DelSubTreeForBlock(Tree* addr); // Удаление блока

	void Print(void);	// Вывод дерева
	void PrintWithTag(std::string tag);	// Вывод дерева с надписью
	void PrintError(std::string error, TypeLex a);	// Вывод ссемантической ошибки
	
	////////////////////////////////
	// Семантические подпрограммы //
	////////////////////////////////
	void SetCur(Tree* a);	// Задать текущий узел
	Tree* GetCur(void);		// Получить текущий узел
	Tree* SemInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType);	// Добавить объект в дерево
	Tree* SemGetVar(TypeLex a);		// Найти и вернуть переменную с именем a
	Tree* SemGetFunc(TypeLex a);	// Найти и вернуть функцию с именем a
	Tree* SemGetFirstFunc();	// Найти и вернуть первую попавшуюся функцию
	DATA_TYPE SemGetDataType(Tree* addr);	// Получить тип узла
	int DupControl(Tree* addr, TypeLex a);	// Проверка на повторение

	void CheckConst(TypeLex a);		// Проверка на константу
	void CheckInit(TypeLex a);		// Проверка инициализации

	void SemCheckFuncOrConst(TypeLex a);	// Проверка на постфиксный оператор после функции или константы
	
	void SemSetConst(Tree* addr, bool flag);	// Установить флаг константы
	void SemSetInit(Tree* addr, bool flag);		// Установить флаг инициализации
	
	void SemSetValue(Tree* addr, DATA_TYPE dataType, TypeLex val);	// Установить значение
	void SemSetValue(Tree* addr, DataValue val);	// Установить значение
	void SemSetValue(Tree* addr, TData* val);	// Установить значение
	void SemSetValue(TypeLex a, TData* val);		// Установить значение
	
	void SemReadStringValue(TypeLex val, TData* data);	// Преобразование строкового значения
	void SemGetData(Tree* addr, TData* data);	// Получить значение узла
	void SemInvertValue(TData* data);	// Изменить знак значения
	void SemSetFuncBodyPos(Tree* addr, int pos);	// Установить позицию тела функции
	int SemGetFuncBodyPos(TypeLex func);	// Получить позицию тела функции
	Tree* SemStartFunction(TypeLex func);	// Запуск функции

	Tree* CopyFunc(Tree* addr);
	Tree* SemSetFuncCall(TypeLex func);
	Tree* CoplexOperator();		// Добавить в дерево составной оператор

	void SemDoBiOperation(TData* val1, TData* val2, int type);	// Выполнить бинарную операцию по типу
	void SemDoUnoOperation(TData* val, TypeLex ident, int type);	// Выполнить унарную операцию по типу
	void SemPostPrefIdent(TypeLex ident, int type);	// Выполнить инкремент или декремент переменной
	
	DATA_TYPE GetType(TypeLex l);	// Узнать тип константы
	void SemTypeCastCheck(TypeLex a, TypeLex l);	// Проверка возможности приведения типа
	void SemTypeCastCheck(TypeLex a, DATA_TYPE dt);	// Проверка возможности приведения типа
	void SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2); // Проверка возможности приведения типа
};

#endif

