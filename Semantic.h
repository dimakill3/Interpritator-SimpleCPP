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
	TYPE_NONE = 0, /*TYPE_SHORT,*/ TYPE_INTEGER,
	/*TYPE_LONG,*/ TYPE_LONG_LONG
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
	
};

// Поля для хранения значений
union DataValue
{
	long long llValue;
	int intValue;
};

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

	std::string GetValue()
	{
		switch (dataType)
		{
		case TYPE_INTEGER:
			{
			return  std::to_string(dataValue.intValue);
			}
		case TYPE_LONG_LONG:
			{
			return  std::to_string(dataValue.llValue);
			}
		default: return "";
		}
	}
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
	// Функции работы с деревом //
	Tree();
	Tree(Tree* l, Tree* r, Tree* up, Node* data);
	void SetScaner(TScaner* sc);	// Установить сканер
	
	void SetLeft(Node* data);	// Установить узел слева
	void SetRight(Node* data);	// Установить узел справа

	Tree* FindUp(Tree* from, TypeLex id);	// Поиск сверху от конкретной вершины
	Tree* FindUp(TypeLex id);				// Поиск сверху от текущей вершины

	Tree* FindUpOneLevel(Tree* from, TypeLex id);	// Поиск сверху от вершины from по одному уровню вложенности

	Tree* FindRightLeft(Tree* from, TypeLex id);
	Tree* FindRightLeft(TypeLex id);
	
	void Print(void);	// Вывод дерева
	void PrintWithTag(std::string tag);	// Вывод дерева с надписью
	void PrintError(std::string error, TypeLex a);	// Вывод ссемантической ошибки

	// Семантические подпрограммы //
	void SetCur(Tree* a);	// Задать текущий узел
	Tree* GetCur(void);		// Получить текущий узел
	Tree* SemInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType);	// Добавить объект в дерево
	Tree* SemGetVar(TypeLex a);		// Найти и вернуть переменную с именем a
	Tree* SemGetFunc(TypeLex a);	// Найти и вернуть функцию с именем a
	DATA_TYPE SemGetFirstFunc();	// Найти и вернуть первую попавшуюся функцию
	int DupControl(Tree* addr, TypeLex a);	// Проверка на повторение

	void CheckConst(TypeLex a);		// Проверка на константу
	void CheckData(Tree* addr);		// Проверка на значение
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
	Tree* CoplexOperator();		// Добавить в дерево составной оператор

	void SemDoBiOperation(TData* val1, TData* val2, int type);	// 
	void SemDoUnoOperation(TData* val, int type);	// 
	
	DATA_TYPE GetType(TypeLex l);	// Узнать тип константы
	void SemTypeCastCheck(TypeLex a, TypeLex l);	// Проверка возможности приведения типа
	void SemTypeCastCheck(TypeLex a, DATA_TYPE dt);	// Проверка возможности приведения типа
	void SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2); // Проверка возможности приведения типа

	void DelSubTree(Tree* addr); // Удаление поддерева

	void DelSubTreeForFunc(Tree* addr);	// Удаление тела функции
	Tree* DelSubTreeForBlock(Tree* addr); // Удаление блока
};

#endif

