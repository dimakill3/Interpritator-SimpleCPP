#ifndef __SEMAN
#define __SEMAN
#include <string>
#include "defs.h"
#include "TScaner.h"

#pragma once

#define maxInt 2147483647
#define maxLongLong ((long long)9223372036854775807)

// ���� ������
enum DATA_TYPE {
	TYPE_NONE = 0, TYPE_INTEGER,
	TYPE_LONG_LONG
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
	int bodyPos;
};

// ���� ��� �������� ��������
union DataValue
{
	long long llValue;
	int intValue;
};

// ����� ������
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
	bool isLink;	// ���������������� ��� ������
	int number;		// ����� ������, ���� �������� ������� (isLink = true)
	TypeLex lex;	// ������� ����������������� ��������
};

class Triada
{
	int triadeNum;	// ����� ������
	int TOper;		// ��� ��������
	Operand operands[2];	// ��������
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
	bool isInterpret = true;
	
	//////////////////////////////
	// ������� ������ � ������� //
	//////////////////////////////
	Tree();
	Tree(Tree* l, Tree* r, Tree* up, Node* data);
	void SetScaner(TScaner* sc);	// ���������� ������
	
	void SetLeft(Node* data);	// ���������� ���� �����
	void SetRight(Node* data);	// ���������� ���� ������

	void SetLeft(Tree* data);	// ���������� ���� �����
	void SetRight(Tree* data);	// ���������� ���� ������
	
	Tree* FindUp(Tree* from, TypeLex id);	// ����� ������ �� ���������� �������
	Tree* FindUp(TypeLex id);				// ����� ������ �� ������� �������

	Tree* FindUpOneLevel(Tree* from, TypeLex id);	// ����� ������ �� ������� from �� ������ ������ �����������

	void DelSubTree(Tree* addr); // �������� ���������
	void DelSubTreeForFunc(Tree* addr);	// �������� ���� �������
	Tree* DelSubTreeForBlock(Tree* addr); // �������� �����

	void Print(void);	// ����� ������
	void PrintWithTag(std::string tag);	// ����� ������ � ��������
	void PrintError(std::string error, TypeLex a);	// ����� �������������� ������
	
	////////////////////////////////
	// ������������� ������������ //
	////////////////////////////////
	void SetCur(Tree* a);	// ������ ������� ����
	Tree* GetCur(void);		// �������� ������� ����
	Tree* SemInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType);	// �������� ������ � ������
	Tree* SemGetVar(TypeLex a);		// ����� � ������� ���������� � ������ a
	Tree* SemGetFunc(TypeLex a);	// ����� � ������� ������� � ������ a
	Tree* SemGetFirstFunc();	// ����� � ������� ������ ���������� �������
	DATA_TYPE SemGetDataType(Tree* addr);	// �������� ��� ����
	int DupControl(Tree* addr, TypeLex a);	// �������� �� ����������

	void CheckConst(TypeLex a);		// �������� �� ���������
	void CheckInit(TypeLex a);		// �������� �������������

	void SemCheckFuncOrConst(TypeLex a);	// �������� �� ����������� �������� ����� ������� ��� ���������
	
	void SemSetConst(Tree* addr, bool flag);	// ���������� ���� ���������
	void SemSetInit(Tree* addr, bool flag);		// ���������� ���� �������������
	
	void SemSetValue(Tree* addr, DATA_TYPE dataType, TypeLex val);	// ���������� ��������
	void SemSetValue(Tree* addr, DataValue val);	// ���������� ��������
	void SemSetValue(Tree* addr, TData* val);	// ���������� ��������
	void SemSetValue(TypeLex a, TData* val);		// ���������� ��������
	
	void SemReadStringValue(TypeLex val, TData* data);	// �������������� ���������� ��������
	void SemGetData(Tree* addr, TData* data);	// �������� �������� ����
	void SemInvertValue(TData* data);	// �������� ���� ��������
	void SemSetFuncBodyPos(Tree* addr, int pos);	// ���������� ������� ���� �������
	int SemGetFuncBodyPos(TypeLex func);	// �������� ������� ���� �������
	Tree* SemStartFunction(TypeLex func);	// ������ �������

	Tree* CopyFunc(Tree* addr);
	Tree* SemSetFuncCall(TypeLex func);
	Tree* CoplexOperator();		// �������� � ������ ��������� ��������

	void SemDoBiOperation(TData* val1, TData* val2, int type);	// ��������� �������� �������� �� ����
	void SemDoUnoOperation(TData* val, TypeLex ident, int type);	// ��������� ������� �������� �� ����
	void SemPostPrefIdent(TypeLex ident, int type);	// ��������� ��������� ��� ��������� ����������
	
	DATA_TYPE GetType(TypeLex l);	// ������ ��� ���������
	void SemTypeCastCheck(TypeLex a, TypeLex l);	// �������� ����������� ���������� ����
	void SemTypeCastCheck(TypeLex a, DATA_TYPE dt);	// �������� ����������� ���������� ����
	void SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2); // �������� ����������� ���������� ����
};

#endif

