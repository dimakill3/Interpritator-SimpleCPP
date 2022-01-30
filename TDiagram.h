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
	
	// ������� ��
	void TFunc();			// �������
	void TConstData();		// ����������� ���������
	void TVariableData();	// ����������
	DATA_TYPE TType();		// ���
	TData TFuncBody();		// ���� �������
	void TOperators();		// ���������
	void TOperator();		// ��������
	TData TFuncRet();		// ������� �� �������
	DATA_TYPE TFuncStart();		// ����� �������
	void TConstList();		// ������ ����������� ��������
	void TVariableList();	// ������ ����������
	void TAssignSD(bool);	// ������������
	void TForSD();			// ���� for
	DATA_TYPE TExpr();		// ���������
	DATA_TYPE TCompareEl();	// ������� ���������
	DATA_TYPE TAddEl();		// ���������
	DATA_TYPE TMulEl();		// ���������
	DATA_TYPE TPrefEl();	// ������� ���������� ��������
	DATA_TYPE TPostEl();	// ������� ����������� ��������
	DATA_TYPE TElementExpr();		// ������������ ���������

	int lookForvard(int k);	// ���������� ��� k-�� ������� �������

public:

	void TProgram();		// ���������
	
	int GetType();
	void SetRoot(Tree* r);	// ������ ������ �������������� ������

	TDiagram(TScaner* s)
	{
		sc = s;
	}
	
	~TDiagram();
};
#endif

