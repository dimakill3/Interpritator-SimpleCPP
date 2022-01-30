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
	TData TFuncBody();		// Тело функции
	void TOperators();		// Операторы
	void TOperator();		// Оператор
	TData TFuncRet();		// Возврат из функции
	DATA_TYPE TFuncStart();		// Вызов функции
	void TConstList();		// Список именованных констант
	void TVariableList();	// Список переменных
	void TAssignSD(bool);	// Присваивание
	void TForSD();			// Цикл for
	DATA_TYPE TExpr();		// Выражение
	DATA_TYPE TCompareEl();	// Элемент сравнения
	DATA_TYPE TAddEl();		// Слогаемое
	DATA_TYPE TMulEl();		// Множитель
	DATA_TYPE TPrefEl();	// Элемент префиксной операции
	DATA_TYPE TPostEl();	// Элемент постфиксной операции
	DATA_TYPE TElementExpr();		// Элементарное выражение

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

