#ifndef __DIAGRAM
#define __DIAGRAM
#include "TScaner.h"
#include "Semantic.h"

#pragma once
class TDiagram
{
private:
	TScaner* sc;
	TypeLex lex;

	Tree* Root;

	//int type;
	
	// Функции СД
	void TFunc();			// Функция
	void TConstData();		// Именованные константы
	void TVariableData();	// Переменные
	DATA_TYPE TType();		// Тип
	void TFuncBody();		// Тело функции
	void TOperators();		// Операторы
	void TOperator(bool inFor = false);		// Оператор
	void TFuncRet();		// Возврат из функции
	void TFuncStart(TData* data);		// Вызов функции
	void TConstList();		// Список именованных констант
	void TVariableList();	// Список переменных
	void TAssignSD(bool);	// Присваивание
	void TForSD();			// Цикл for
	void TExpr(TData* data);		// Выражение
	void TCompareEl(TData* data);	// Элемент сравнения
	void TAddEl(TData* data);		// Слогаемое
	void TMulEl(TData* data);		// Множитель
	void TPrefEl(TData* data);	// Элемент префиксной операции
	void TPostEl(TData* data);	// Элемент постфиксной операции
	void TElementExpr(TData* data);		// Элементарное выражение

	int lookForvard(int k);	// Посмотреть тип k-ой лексемы спереди

public:

	void TProgram();		// Программа
	
	int GetType();
	void SetRoot(Tree* r);	// Задать корень семантического дерева

	TDiagram(TScaner* s)
	{
		sc = s;
	}
	
	~TDiagram();
};
#endif

