#ifndef __SEMAN
#define __SEMAN
#include <string>
#include "defs.h"
#include "TScaner.h"

#pragma once
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

union DataValue
{
	long long llValue;
	int intValue;
};

struct TData
{
	DATA_TYPE dataType;		// Тип значения
	DataValue dataValue;	// Значение
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
	Tree* up, * l, * r;	// Узлы сверху, слева и справа
	TScaner* sc;
public:
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
	void SemSetData(Tree* addr, char* data);	// Установить значение
	void SemSetInit(Tree* addr, bool flag);		// Установить флаг инициализации
	void SemSetValue(Tree* addr, DATA_TYPE dataType, TypeLex val);	// Установить значение
	void SemSetValue(Tree* addr, DataValue val);	// Установить значение
	Tree* CoplexOperator();		// Добавить в дерево составной оператор

	DATA_TYPE GetType(TypeLex l);	// Узнать тип константы
	void SemTypeCastCheck(TypeLex a, TypeLex l);	// Проверка возможности приведения типа
	void SemTypeCastCheck(TypeLex a, DATA_TYPE dt);	// Проверка возможности приведения типа
	void SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2); // Проверка возможности приведения типа
};

#endif

