#ifndef __SEMAN
#define __SEMAN
#include <string>
#include "defs.h"
#include "TScaner.h"

#pragma once

#define maxInt 2147483647

// ���� ������
enum DATA_TYPE {
	TYPE_NONE = 0, /*TYPE_SHORT,*/ TYPE_INTEGER,
	/*TYPE_LONG,*/ TYPE_LONG_LONG
};

// ���� ��������
enum OBJECT_TYPE {
	OBJ_NONE = 0, TYPE_VAR, TYPE_FUNC
};

// ��������� ��� ���������� ���������� (��� ��������)
struct VarData
{
	bool constFlag;			// ���� ���������
	char* data;				// C����� �� �������� ��� NULL
	bool init;				// ���� �������������
};

// ��������� ��� ���������� �������
struct FuncData
{
	
};

// ���� ��� �������� ��������
union DataValue
{
	long long llValue;
	int intValue;
};

class TData
{
public:
	DATA_TYPE dataType;		// ��� ��������
	DataValue dataValue;	// ��������
	
	TData()
	{
		dataType = TYPE_INTEGER;
		dataValue.intValue = -maxInt - 1;
	}
};

// ��������� ������ ���� (���������� ��� �������) � ������������� ������
struct Node
{
	OBJECT_TYPE objType;	// ��� �������
	
	TypeLex id;				// �������������
	DATA_TYPE dataType;		// ��� ��������
	DataValue dataValue;	// ��������

	union 
	{
		VarData var;
		FuncData func;
	};
};

// ����� �������������� ������
class Tree
{
	Node* n;
	TScaner* sc;

public:
	Tree* up, * l, * r;	// ���� ������, ����� � ������
	static Tree* cur;
	// ������� ������ � ������� //
	Tree();
	Tree(Tree* l, Tree* r, Tree* up, Node* data);
	void SetScaner(TScaner* sc);	// ���������� ������
	
	void SetLeft(Node* data);	// ���������� ���� �����
	void SetRight(Node* data);	// ���������� ���� ������

	Tree* FindUp(Tree* from, TypeLex id);	// ����� ������ �� ���������� �������
	Tree* FindUp(TypeLex id);				// ����� ������ �� ������� �������

	Tree* FindUpOneLevel(Tree* from, TypeLex id);	// ����� ������ �� ������� from �� ������ ������ �����������

	Tree* FindRightLeft(Tree* from, TypeLex id);
	Tree* FindRightLeft(TypeLex id);
	
	void Print(void);	// ����� ������
	void PrintWithTag(std::string tag);	// ����� ������ � ��������
	void PrintError(std::string error, TypeLex a);	// ����� �������������� ������

	// ������������� ������������ //
	void SetCur(Tree* a);	// ������ ������� ����
	Tree* GetCur(void);		// �������� ������� ����
	Tree* SemInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType);	// �������� ������ � ������
	Tree* SemGetVar(TypeLex a);		// ����� � ������� ���������� � ������ a
	Tree* SemGetFunc(TypeLex a);	// ����� � ������� ������� � ������ a
	DATA_TYPE SemGetFirstFunc();	// ����� � ������� ������ ���������� �������
	int DupControl(Tree* addr, TypeLex a);	// �������� �� ����������

	void CheckConst(TypeLex a);		// �������� �� ���������
	void CheckData(Tree* addr);		// �������� �� ��������
	void CheckInit(TypeLex a);		// �������� �������������

	void SemCheckFuncOrConst(TypeLex a);	// �������� �� ����������� �������� ����� ������� ��� ���������
	
	void SemSetConst(Tree* addr, bool flag);	// ���������� ���� ���������
	void SemSetInit(Tree* addr, bool flag);		// ���������� ���� �������������
	void SemSetValue(Tree* addr, DATA_TYPE dataType, TypeLex val);	// ���������� ��������
	void SemSetValue(Tree* addr, DataValue val);	// ���������� ��������
	void SemSetValue(Tree* addr, TData* val);	// ���������� ��������
	void SemReadStringValue(TypeLex val, TData* data);	// �������������� ���������� ��������
	void SemGetData(Tree* addr, TData* data);	// �������� �������� ����
	Tree* CoplexOperator();		// �������� � ������ ��������� ��������

	DATA_TYPE GetType(TypeLex l);	// ������ ��� ���������
	void SemTypeCastCheck(TypeLex a, TypeLex l);	// �������� ����������� ���������� ����
	void SemTypeCastCheck(TypeLex a, DATA_TYPE dt);	// �������� ����������� ���������� ����
	void SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2); // �������� ����������� ���������� ����

	void DelSubTree(Tree* addr); // �������� ���������

	void DelSubTreeForFunc(Tree* addr);	// �������� ���� �������
	Tree* DelSubTreeForBlock(Tree* addr); // �������� �����
};

#endif

