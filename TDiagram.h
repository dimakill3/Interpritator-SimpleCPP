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
	void TFuncBody(TData* data);		// ���� �������
	void TOperators();		// ���������
	void TOperator(bool inFor = false);		// ��������
	void TFuncRet(TData* data);		// ������� �� �������
	DATA_TYPE TFuncStart();		// ����� �������
	void TConstList();		// ������ ����������� ��������
	void TVariableList();	// ������ ����������
	void TAssignSD(bool);	// ������������
	void TForSD();			// ���� for
	void TExpr(TData* data);		// ���������
	void TCompareEl(TData* data);	// ������� ���������
	void TAddEl(TData* data);		// ���������
	void TMulEl(TData* data);		// ���������
	void TPrefEl(TData* data);	// ������� ���������� ��������
	void TPostEl(TData* data);	// ������� ����������� ��������
	void TElementExpr(TData* data);		// ������������ ���������

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

