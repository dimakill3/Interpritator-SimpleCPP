#include "Semantic.h"
#include "vector"
#include <string.h>
#define max(a,b) a<b? b : a

//#define maxShort ((short)32767)
#define maxInt 2147483647
//#define maxLong ((long)2147483647)
#define maxLongLong ((long long)9223372036854775807)

//#define isShort(num) (num >= -maxShort - 1 && num <= maxShort)
#define isInt(num) (num >= -maxInt - 1 && num <= maxInt)
//#define isLong(num) (num >= -maxLong - 1 && num <= maxLong)
#define isLongLong(num) (num >= -maxLongLong - 1 && num <= maxLongLong)


Tree* Tree::cur = (Tree*)NULL;

// ������ ��� ������������� ������� ����� �� Semantic.h DATA_TYPE � �� ����������
std::vector<std::string> types = { "none", /*"short",*/ "int", /*"long",*/ "long long" };

// ����� �������� id ����� �� ������ �� ������� ������� from.
// ����� �������������� �� ����� ������ ����������� �� ����� ������
Tree* Tree::FindUpOneLevel(Tree* from, TypeLex id)
{
	Tree* i = from; // ������� ������� ������
	while ((i->up != NULL) &&
		(i->up->r != i)
		)
	{
		if (memcmp(id, i->n->id, max(strlen(i->n->id), strlen(id))) == 0)
			return i; // �a��� ����������� �������������
		i = i->up; // ����������� ������ �� ������
	}
	return NULL;
}

// ����������� ��� ���������� ������� ����� ���� � ������� ������� � �������
Tree::Tree(void)
{
	n = new Node();
	up = NULL; l = NULL; r = NULL;
	memcpy(n, &("------"), sizeof(Node));
}

// ����������� � ����������� ������� ����� ���� � ��������� ������� � �������
Tree::Tree(Tree* l, Tree* r, Tree* up, Node* d)
{
	n = new Node();
	this->up = up; this->l = l; this->r = r;
	memcpy(n, d, sizeof(Node));
}

void Tree::SetScaner(TScaner* sc)
{
	this->sc = sc;
}

// �������� ������ ������� �� ������� ������� this
void Tree::SetLeft(Node* data)
{
	Tree* a = new Tree(NULL, NULL, this, data); // ����� �������
	l = a; // ��������� this � ����� �������� �����
}

// �������� ������� ������� �� ������� ������� this
void Tree::SetRight(Node* data)
{
	Tree* a = new Tree(NULL, NULL, this, data); // ����� �������
	r = a; // ��������� this � ����� �������� ������
}

// ����� ������ � ������ �� �������� ������� from �� ����� ����� �� ������
Tree* Tree::FindUp(Tree* from, TypeLex id)
{
	Tree* i = from; // ������� ������� ������
	while ((i != NULL) && (memcmp(id, i->n->id, max(strlen(i->n->id), strlen(id))) != 0))
		i = i->up; // ����������� ����� �� ������

	return i;
}

// ����� ������ � ������, ������� �� ������� ������� this �� ����� ����� �� ������
Tree* Tree::FindUp(TypeLex id)
{
	return FindUp(this, id);
}

// ���������� ������� ���� ������
void Tree::SetCur(Tree* a)
{
	cur = a;
}

// �������� �������� �������� ���� ������
Tree* Tree::GetCur()
{
	return cur;
}

// ��������� �������������� a � ������� � ����� t
Tree* Tree::SemInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType)
{
	if (DupControl(cur, a))
		PrintError("��������� �������� ��������������", a);

	Tree *v;
	Node b;

	if (objType != TYPE_FUNC)
	{
		memcpy(b.id, a, strlen(a) + 1); b.objType = objType; b.dataType = dataType; /*b.var.data = NULL;*/ b.var.constFlag = false; b.var.init = false;
		cur->SetLeft(&b); // ������� ������� - ����������
		cur = cur->l;
		return cur;
	}
	else
	{
		memcpy(b.id, a, strlen(a) + 1); b.objType = objType; b.dataType = dataType;
		cur->SetLeft(&b); // ������� ������� - �������
		cur = cur->l;
		v = cur; // ��� ����� �������� ����� ������ �� �������

		b.dataType = TYPE_NONE;
		b.objType = OBJ_NONE;
		memcpy(&b.id, &"<r>", 4);
		cur->SetRight(&b); // ������� ������ �������
		cur = cur->r;
		return v;
	}
}

// ����� � ������� ���������� � ������ a � ������� ������ �� ��������������� ������� ������
Tree* Tree::SemGetVar(TypeLex a)
{
	Tree* v = FindUp(cur, a);
	if (v == NULL)
		PrintError("����������� ��������", a);
	if (v->n->objType == TYPE_FUNC)
		PrintError("�������� ������������� ������ �������", a);
	return v;
}

// ����� � ������� ������� � ������ a � ������� ������ �� ��������������� ������� ������
Tree* Tree::SemGetFunc(TypeLex a)
{
	Tree* v = FindUp(cur, a);
	if (v == NULL)
		PrintError("����������� �������� �������", a);
	if (v->n->objType != TYPE_FUNC)
		PrintError("�� �������� ��������", a);
	return v;
}

DATA_TYPE Tree::SemGetFirstFunc()
{
	Tree* i = cur; // ������� ������� ������
	while ((i != NULL) && i->n->objType != TYPE_FUNC)
		i = i->up; // ����������� ����� �� ������

	return i->n->dataType;
}

// ��������� ����������� ���������� ������������ ���������
void Tree::SemCheckFuncOrConst(TypeLex a)
{
	Tree* v = FindUp(cur, a);

	if (v == NULL)
		printf("Semantic error (������ %d): ������ ��������� ����������� �������� � ���������", sc->GetLine() + 1);
	
	if (v->n->objType == TYPE_FUNC)
		PrintError("������ ��������� ����������� �������� � ������ �������", a);
	else if (v->n->var.constFlag)
		PrintError("���������� �������� ���������", a);

}

// ���������� ���� ���������
void Tree::SemSetConst(Tree *addr, bool flag)
{
	addr->n->var.constFlag = flag;
}

// ���������� ������
void Tree::SemSetData(Tree *addr, char* data)
{
	addr->n->var.data = data;
}

// ���������� ���� �������������
void Tree::SemSetInit(Tree* addr, bool flag)
{
	addr->n->var.init = flag;
}

void Tree::SemSetValue(Tree* addr, DATA_TYPE dataType, TypeLex val)
{
	if (dataType == TYPE_INTEGER)
		addr->n->dataValue.intValue = atoi(val);
	
	if (dataType == TYPE_LONG_LONG)
		addr->n->dataValue.llValue = atoll(val);
}

void Tree::SemSetValue(Tree* addr, DataValue val)
{
	addr->n->dataValue = val;
}


// �������� �������������� � �� ��������� �������� ������ �����.
// ����� �������������� ����� �� ������� addr.
int Tree::DupControl(Tree* addr, TypeLex a)
{
	if (FindUpOneLevel(addr, a) == NULL) return 0;
	return 1;
}

// ��������� ���� ���������
void Tree::CheckConst(TypeLex a)
{
	Tree* v = SemGetVar(a);
	if (v->n->var.constFlag)
		PrintError("���������� �������� ���������", a);
}

// ��������� ������
void Tree::CheckData(Tree* addr)
{
	
}

// ��������� �������������
void Tree::CheckInit(TypeLex a)
{
	Tree* v = SemGetVar(a);
	if (v->n->var.init == false)
		PrintError("������������ �������������������� ����������", a);
}

// ����� ������
void Tree::PrintError(std::string error, TypeLex a)
{
	printf("Semantic error (������ %d): %s, ������������� %s", sc->GetLine() + 1, error.c_str(), a);
	exit(-2);
}

// �������� ���
DATA_TYPE Tree::GetType(TypeLex l)
{
	Tree* v = FindUp(cur, l);
	if (v != NULL)
		return v->n->dataType;
	
	errno = 0;
	char* lexEnd;
	long long num = strtoll(l, &lexEnd, 10);

	bool range_error = errno == ERANGE;

	//std::string extracted(l, lexEnd - l);

	if (range_error)
		printf("������ %d: ������� ������� ����� %s\n", sc->GetLine() + 1, l);

	/*if (isShort(num))
		return TYPE_SHORT;
	else */if (isInt(num))
		return TYPE_INTEGER;
	//else if (isLong(num))
	//	return TYPE_LONG;
	else if (isLongLong(num))
		return TYPE_LONG_LONG;
}

// ��������� ����������� ���������� �����
void Tree::SemTypeCastCheck(TypeLex a, TypeLex l)
{
	Tree* v = SemGetVar(a);

	DATA_TYPE constDataType = GetType(l);
	
	if (v->n->dataType < constDataType)
	{
		printf("Semantic error (������ %d): ���������� �������� ��� %s � %s, ������������� %s", sc->GetLine() + 1,
			types[constDataType].c_str(), types[v->n->dataType].c_str(), a);
		exit(-2);
	}
	
}

// ��������� ����������� ���������� �����
void Tree::SemTypeCastCheck(TypeLex a, DATA_TYPE dt)
{
	Tree* v = SemGetVar(a);

	if (v->n->dataType < dt)
	{
		printf("Semantic error (������ %d): ���������� �������� ��� %s � %s, ������������� %s", sc->GetLine() + 1,
			types[dt].c_str(), types[v->n->dataType].c_str(), a);
		exit(-2);
	}
}

// ��������� ����������� ���������� �����
void Tree::SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2)
{
	if (dt1 < dt2)
	{
		printf("Semantic error (������ %d): ���������� ������� ��� %s, �������� ��� %s", sc->GetLine() + 1,
			types[dt2].c_str(), types[dt1].c_str());
		exit(-2);
	}
}

// ����� ������
void Tree::Print()
{
	std::string nType, lType, rType;	// ���� ������ ��� �����
	std::string nVal, lVal, rVal;		// �������� ��� �����
	
	nType = types[n->dataType];
	nVal = n->dataType == TYPE_INTEGER ? std::to_string(n->dataValue.intValue) : n->dataType == TYPE_LONG_LONG ? std::to_string(n->dataValue.llValue) : "none";

	
	printf("������� � ������� %s <type %s> <value %s> ----->", n->id, nType.c_str(), nVal.c_str());
	if (l != NULL)
	{
		lType = types[l->n->dataType];

		lVal = l->n->dataType == TYPE_INTEGER ? std::to_string(l->n->dataValue.intValue) : l->n->dataType == TYPE_LONG_LONG ? std::to_string(l->n->dataValue.llValue) : "none";
		
		printf(" ����� ������ %s <type %s> <value %s>", l->n->id, lType.c_str(), lVal.c_str());
	}
	if (r != NULL)
	{
		rType = types[r->n->dataType];

		rVal = r->n->dataType == TYPE_INTEGER ? std::to_string(r->n->dataValue.intValue) : r->n->dataType == TYPE_LONG_LONG ? std::to_string(r->n->dataValue.llValue) : "none";
		
		printf(" ������ ������ %s <type %s> <value %s>", r->n->id, rType.c_str(), rVal.c_str());
	}
	printf("\n");
	if (r != NULL) r->Print();
	if (l != NULL) l->Print();
}

// ���������� � ������������� ������ ���������� ��������� (������ ���� �����, �� ���� ������ ���� ������)
Tree* Tree::CoplexOperator()
{
	Tree* v;
	Node b;

	// ������ ������ ��� ������� ���� �����
	memcpy(b.id, &"<l>", 4);
	b.dataType = TYPE_NONE;
	b.objType = OBJ_NONE;
	// ������ ����� ������ ����
	cur->SetLeft(&b);
	cur = cur->l;
	v = cur;	// ���������� ������ ��� ��������� ����� ����, ��� ������ �� ���������� ���������

	// ������ ������ ��� ������� ���� ������
	memcpy(&b.id, &"<r>", 4);
	// ������ ������ ������ ����
	cur->SetRight(&b);
	cur = cur->r; // ���� �� �� ������ �� ���������� ���������, ����� �������� �� ������� ����
	return v;
}