#ifndef __SEMAN
#define __SEMAN
#include <string>
#include "defs.h"
#include "TScaner.h"

#pragma once
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

union DataValue
{
	long long llValue;
	int intValue;
};

struct TData
{
	DATA_TYPE dataType;		// ��� ��������
	DataValue dataValue;	// ��������
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
	Tree* up, * l, * r;	// ���� ������, ����� � ������
	TScaner* sc;
public:
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
	void SemSetData(Tree* addr, char* data);	// ���������� ��������
	void SemSetInit(Tree* addr, bool flag);		// ���������� ���� �������������
	void SemSetValue(Tree* addr, DATA_TYPE dataType, TypeLex val);	// ���������� ��������
	void SemSetValue(Tree* addr, DataValue val);	// ���������� ��������
	Tree* CoplexOperator();		// �������� � ������ ��������� ��������

	DATA_TYPE GetType(TypeLex l);	// ������ ��� ���������
	void SemTypeCastCheck(TypeLex a, TypeLex l);	// �������� ����������� ���������� ����
	void SemTypeCastCheck(TypeLex a, DATA_TYPE dt);	// �������� ����������� ���������� ����
	void SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2); // �������� ����������� ���������� ����
};

#endif

