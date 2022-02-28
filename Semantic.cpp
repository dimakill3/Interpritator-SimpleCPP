#include "Semantic.h"

#include <iomanip>
#include <iostream>

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
std::vector<std::string> types = { "none", "int", "long long" };

// ������ ��� ������������� ������� ����� �� Semantic.h DATA_TYPE � �� ����������
std::vector<std::string> objTypes = { "none", "var", "func" };

// ����� �������� id ����� �� ������ �� ������� ������� from.
// ����� �������������� �� ����� ������ ����������� �� ����� ������
Tree* Tree::FindUpOneLevel(Tree* from, TypeLex id)
{
	if (!isInterpret)
		return (Tree*)NULL;
	
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
	if (!isInterpret)
		return;
	this->sc = sc;
}

// �������� ������ ������� �� ������� ������� this
void Tree::SetLeft(Node* data)
{
	if (!isInterpret)
		return;
	
	Tree* a = new Tree(NULL, NULL, this, data); // ����� �������
	l = a; // ��������� this � ����� �������� �����
}

void Tree::SetLeft(Tree* data)
{
	if (!isInterpret)
		return;

	Tree* a = new Tree(data->l, data->r, this, data->n); // ����� �������
	l = a; // ��������� this � ����� �������� �����
}

// �������� ������� ������� �� ������� ������� this
void Tree::SetRight(Node* data)
{
	if (!isInterpret)
		return;
	
	Tree* a = new Tree(NULL, NULL, this, data); // ����� �������
	r = a; // ��������� this � ����� �������� ������
}

void Tree::SetRight(Tree* data)
{
	if (!isInterpret)
		return;

	Tree* a = new Tree(data->l, data->r, this, data->n); // ����� �������
	r = a; // ��������� this � ����� �������� ������
}

// ����� ������ � ������ �� �������� ������� from �� ����� ����� �� ������
Tree* Tree::FindUp(Tree* from, TypeLex id)
{
	if (!isInterpret)
		return (Tree*)NULL;
	
	Tree* i = from; // ������� ������� ������
	while ((i != NULL) && (memcmp(id, i->n->id, max(strlen(i->n->id), strlen(id))) != 0))
		i = i->up; // ����������� ����� �� ������

	return i;
}

// ����� ������ � ������, ������� �� ������� ������� this �� ����� ����� �� ������
Tree* Tree::FindUp(TypeLex id)
{
	if (!isInterpret)
		return (Tree*)NULL;
	
	return FindUp(this, id);
}

// ���������� ������� ���� ������
void Tree::SetCur(Tree* a)
{
	if (!isInterpret)
		return;
	cur = a;
}

// �������� �������� �������� ���� ������
Tree* Tree::GetCur()
{
	if (!isInterpret)
		return (Tree*)NULL;
	
	return cur;
}

// ��������� �������������� a � ������� � ����� t
Tree* Tree::SemInclude(TypeLex a, OBJECT_TYPE objType, DATA_TYPE dataType)
{
	if (!isInterpret)
		return (Tree*)NULL;
	
	if (DupControl(cur, a))
		PrintError("��������� �������� ��������������", a);

	Tree *v;
	Node b;

	if (objType != TYPE_FUNC)
	{
		memcpy(b.id, a, strlen(a) + 1); b.objType = objType; b.dataType = dataType; b.var.constFlag = false; b.var.init = false;
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
	if (!isInterpret)
		return (Tree*)NULL;
	
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
	if (!isInterpret)
		return (Tree*)NULL;
	
	Tree* v = FindUp(cur, a);
	if (v == NULL)
		PrintError("����������� �������� �������", a);
	if (v->n->objType != TYPE_FUNC)
		PrintError("�� �������� ��������", a);
	return v;
}

Tree* Tree::SemGetFirstFunc()
{
	if (!isInterpret)
		return (Tree*)NULL;
	
	Tree* i = cur; // ������� ������� ������
	while ((i != NULL) && i->n->objType != TYPE_FUNC)
		i = i->up; // ����������� ����� �� ������

	return i;
}

DATA_TYPE Tree::SemGetDataType(Tree* addr)
{
	if (!isInterpret)
		return (DATA_TYPE)NULL;

	return addr->n->dataType;
}


// ��������� ����������� ���������� ������������ ���������
void Tree::SemCheckFuncOrConst(TypeLex a)
{
	if (!isInterpret)
		return;
	
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
	if (!isInterpret)
		return;
	
	addr->n->var.constFlag = flag;
}

// ���������� ���� �������������
void Tree::SemSetInit(Tree* addr, bool flag)
{
	if (!isInterpret)
		return;
	
	addr->n->var.init = flag;
}

void Tree::SemSetValue(Tree* addr, DATA_TYPE dataType, TypeLex val)
{
	if (!isInterpret)
		return;
	
	if (dataType == TYPE_INTEGER)
		addr->n->dataValue.intValue = atoll(val);
	
	if (dataType == TYPE_LONG_LONG)
		addr->n->dataValue.llValue = atoll(val);
}

void Tree::SemSetValue(Tree* addr, DataValue val)
{
	if (!isInterpret)
		return;
	
	addr->n->dataValue = val;
}

void Tree::SemSetValue(Tree* addr, TData* val)
{
	if (!isInterpret)
		return;
	
	switch (val->dataType)
	{
	case TYPE_INTEGER:
		{
			switch (addr->n->dataType)
			{
			case TYPE_INTEGER:
				{
					addr->n->dataValue.intValue = val->dataValue.intValue;
					break;
				}
			case TYPE_LONG_LONG:
				{
					addr->n->dataValue.llValue = val->dataValue.intValue;
					break;
				}
			default: break;
			}
			break;
		}
	case TYPE_LONG_LONG:
		{
			switch (addr->n->dataType)
			{
			case TYPE_INTEGER:
				{
					addr->n->dataValue.intValue = val->dataValue.llValue;
					break;
				}
			case TYPE_LONG_LONG:
				{
					addr->n->dataValue.llValue = val->dataValue.llValue;
					break;
				}
			default: break;
			}
			break;
		}
	default: break;
	}
}

void Tree::SemGetData(Tree* addr, TData* data)
{
	if (!isInterpret)
		return;
	
	data->dataType = addr->n->dataType;
	data->dataValue = addr->n->dataValue;
}

void Tree::SemInvertValue(TData* data)
{
	if (!isInterpret)
		return;
	
	switch (data->dataType)
	{
	case TYPE_INTEGER:
		{
			data->dataValue.intValue = -data->dataValue.intValue;
			break;
		}
	case TYPE_LONG_LONG:
		{
			data->dataValue.llValue = -data->dataValue.llValue;
			break;
		}
	}
}

void Tree::SemSetFuncBodyPos(Tree* addr, int pos)
{
	if (!isInterpret)
		return;
	
	addr->n->func.bodyPos = pos;
}

int Tree::SemGetFuncBodyPos(TypeLex func)
{
	if (!isInterpret)
		return NULL;
	
	Tree* f = SemGetFunc(func);

	return f->n->func.bodyPos;
}

void Tree::SemReadStringValue(TypeLex val, TData* data)
{
	if (!isInterpret)
		return;
	
	long long value = atoll(val);
	if (isInt(value))
	{
		data->dataValue.intValue = (int) value;
		data->dataType = TYPE_INTEGER;
	}
	else if (isLongLong(value))
	{
		data->dataValue.llValue = value;
		data->dataType = TYPE_LONG_LONG;
	}
}

void Tree::SemDoBiOperation(TData* val1, TData* val2, int type)
{
	if (!isInterpret)
		return;
	
	DATA_TYPE maxType = max(val1->dataType, val2->dataType);
	
	DataValue realValue1, realValue2;

	if (maxType == val1->dataType)
	{
		realValue1 = val1->dataValue;
	}
	else
	{
		switch (val1->dataType)
		{
		case TYPE_INTEGER:
		{
			realValue1.llValue = val1->dataValue.intValue;
			break;
		}
		case TYPE_LONG_LONG:
		{
			realValue1.llValue = val1->dataValue.llValue;
			break;
		}
		}
	}
	
	if (maxType == val2->dataType)
	{
		realValue2 = val2->dataValue;
	}
	else
	{
		switch (val2->dataType)
		{
		case TYPE_INTEGER:
		{
			realValue2.llValue = val2->dataValue.intValue;
			break;
		}
		case TYPE_LONG_LONG:
		{
			realValue2.llValue = val2->dataValue.llValue;
			break;
		}
		}
	}
	
	/*switch (maxType)
	{
	case TYPE_INTEGER:
		{
			realValue1.intValue = atoll(value1.c_str());
			realValue2.intValue = atoll(value2.c_str());
			break;
		}
	case TYPE_LONG_LONG:
		{
			realValue1.llValue = atoll(value1.c_str());
			realValue2.llValue = atoll(value2.c_str());
			break;
		}
	default: break;
	}*/
	
	switch (type)
	{
	case TEqual:
	{
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				if (realValue1.intValue == realValue2.intValue)
					val1->dataValue.intValue = 1;
				else
					val1->dataValue.intValue = 0;
				break;
			}
		case TYPE_LONG_LONG:
			{
				if (realValue1.llValue == realValue2.llValue)
					val1->dataValue.llValue = 1;
				else
					val1->dataValue.llValue = 0;
				break;
			}
		default: break;
		}
	break;
	}
	case TNotEqual:
	{
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				if (realValue1.intValue != realValue2.intValue)
					val1->dataValue.intValue = 1;
				else
					val1->dataValue.intValue = 0;
				break;
			}
		case TYPE_LONG_LONG:
			{
				if (realValue1.llValue != realValue2.llValue)
					val1->dataValue.llValue = 1;
				else
					val1->dataValue.llValue = 0;
				break;
			}
		default: break;
		}
		break;
	}
	case TLess:
	{
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				if (realValue1.intValue < realValue2.intValue)
					val1->dataValue.intValue = 1;
				else
					val1->dataValue.intValue = 0;
				break;
			}
		case TYPE_LONG_LONG:
			{
				if (realValue1.llValue < realValue2.llValue)
					val1->dataValue.llValue = 1;
				else
					val1->dataValue.llValue = 0;
				break;
			}
		default: break;
		}
		break;
	}
	case TMore:
	{
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				if (realValue1.intValue > realValue2.intValue)
					val1->dataValue.intValue = 1;
				else
					val1->dataValue.intValue = 0;
				break;
			}
		case TYPE_LONG_LONG:
			{
				if (realValue1.llValue > realValue2.llValue)
					val1->dataValue.llValue = 1;
				else
					val1->dataValue.llValue = 0;
				break;
			}
		default: break;
		}
		break;
	}
	case TLessOrEqual:
	{
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				if (realValue1.intValue <= realValue2.intValue)
					val1->dataValue.intValue = 1;
				else
					val1->dataValue.intValue = 0;
				break;
			}
		case TYPE_LONG_LONG:
			{
				if (realValue1.llValue <= realValue2.llValue)
					val1->dataValue.llValue = 1;
				else
					val1->dataValue.llValue = 0;
				break;
			}
		default: break;
		}
		break;
	}
	case TMoreOrEqual:
	{
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				if (realValue1.intValue >= realValue2.intValue)
					val1->dataValue.intValue = 1;
				else
					val1->dataValue.intValue = 0;
				break;
			}
		case TYPE_LONG_LONG:
			{
				if (realValue1.llValue >= realValue2.llValue)
					val1->dataValue.llValue = 1;
				else
					val1->dataValue.llValue = 0;
				break;
			}
		default: break;
		}
		break;
	}
	case TAdd:
		{
			switch (maxType)
			{
			case TYPE_INTEGER:
				{
					long long tmp = long long (realValue1.intValue) + long long(realValue2.intValue);
					if (!isInt(tmp))
					{
						val1->dataValue.llValue = tmp;
						val1->dataType = TYPE_LONG_LONG;
					}
					else
					{
						val1->dataValue.intValue = realValue1.intValue + realValue2.intValue;
					}
					break;
				}
			case TYPE_LONG_LONG:
				{
					val1->dataValue.llValue = realValue1.llValue + realValue2.llValue;
					break;
				}
			default: break;
			}
			break;
		}
	case TSub:
		{
			switch (maxType)
			{
			case TYPE_INTEGER:
				{
					long long tmp = long long(realValue1.intValue) - long long(realValue2.intValue);
					if (!isInt(tmp))
					{
						val1->dataValue.llValue = tmp;
						val1->dataType = TYPE_LONG_LONG;
					}
					else
					{
						val1->dataValue.intValue = realValue1.intValue - realValue2.intValue;
					}
					break;
				}
			case TYPE_LONG_LONG:
				{
					val1->dataValue.llValue = realValue1.llValue - realValue2.llValue;
					break;
				}
			default: break;
			}
			break;
		}
	case TMul:
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				long long tmp = long long(realValue1.intValue) * long long(realValue2.intValue);
				if (!isInt(tmp))
				{
					val1->dataValue.llValue = tmp;
					val1->dataType = TYPE_LONG_LONG;
				}
				else
				{
					val1->dataValue.intValue = realValue1.intValue * realValue2.intValue;
				}
				break;
			}
		case TYPE_LONG_LONG:
			{
				val1->dataValue.llValue = realValue1.llValue * realValue2.llValue;
				break;
			}
		default: break;
		}
		break;
	case TDiv:
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				long long tmp = long long(realValue1.intValue) / long long(realValue2.intValue);
				if (!isInt(tmp))
				{
					val1->dataValue.llValue = tmp;
					val1->dataType = TYPE_LONG_LONG;
				}
				else
				{
					val1->dataValue.intValue = realValue1.intValue / realValue2.intValue;
				}
				break;
			}
		case TYPE_LONG_LONG:
			{
				val1->dataValue.llValue = realValue1.llValue / realValue2.llValue;
				break;
			}
		default: break;
		}
		break;
	case TDivPart:
		switch (maxType)
		{
		case TYPE_INTEGER:
			{
				long long tmp = long long(realValue1.intValue) % long long(realValue2.intValue);
				if (!isInt(tmp))
				{
					val1->dataValue.llValue = tmp;
					val1->dataType = TYPE_LONG_LONG;
				}
				else
				{
					val1->dataValue.intValue = realValue1.intValue % realValue2.intValue;
				}
				break;
			}
		case TYPE_LONG_LONG:
			{
				val1->dataValue.llValue = realValue1.llValue % realValue2.llValue;
				break;
			}
		default: break;
		}
		break;
	}
}

void Tree::SemDoUnoOperation(TData* val, TypeLex ident, int type)
{
	if (!isInterpret)
		return;
	
	Tree* TreeIdent = SemGetVar(ident);
	
	switch (type)
	{
	case TAdd:
		{
			break;
		}
	case TSub:
		{
			switch (val->dataType)
			{
			case TYPE_INTEGER:
				{
					switch (TreeIdent->n->dataType)
					{
					case TYPE_INTEGER:
						{
							val->dataValue.intValue = -TreeIdent->n->dataValue.intValue;
							break;
							return;
						}
					case  TYPE_LONG_LONG:
						{
							val->dataValue.intValue = -TreeIdent->n->dataValue.llValue;
							break;
							return;
						}
					}
				}
			case TYPE_LONG_LONG:
				{
				switch (TreeIdent->n->dataType)
				{
				case TYPE_INTEGER:
					{
						val->dataValue.llValue = -TreeIdent->n->dataValue.intValue;
						break;
					}
				case  TYPE_LONG_LONG:
					{
						val->dataValue.llValue = -TreeIdent->n->dataValue.llValue;
						break;
					}
				}
				}
			default: break;
			}
		break;
		}
	}
}

void Tree::SemPostPrefIdent(TypeLex ident, int type)
{
	if (!isInterpret)
		return;
	
	Tree* TreeIdent = SemGetVar(ident);

	switch (type)
	{
	case TInc:
	{
		switch (TreeIdent->n->dataType)
		{
		case TYPE_INTEGER:
			{
				TreeIdent->n->dataValue.intValue++;
				break;
			}
		case TYPE_LONG_LONG:
			{
				TreeIdent->n->dataValue.llValue++;
				break;
			}
		default:break;
		}
		break;
	}
	case TDec:
	{
		switch (TreeIdent->n->dataType)
		{
		case TYPE_INTEGER:
			{
				TreeIdent->n->dataValue.intValue--;
				break;
			}
		case TYPE_LONG_LONG:
			{
				TreeIdent->n->dataValue.llValue--;
				break;
			}
		default:break;
		}
		break;
	}
	}
}

// �������� �������������� � �� ��������� �������� ������ �����.
// ����� �������������� ����� �� ������� addr.
int Tree::DupControl(Tree* addr, TypeLex a)
{
	if (!isInterpret)
		return 0;
	
	if (FindUpOneLevel(addr, a) == NULL) return 0;
	return 1;
}

// ��������� ���� ���������
void Tree::CheckConst(TypeLex a)
{
	if (!isInterpret)
		return;
	
	Tree* v = SemGetVar(a);
	if (v->n->var.constFlag)
		PrintError("���������� �������� ���������", a);
}

// ��������� �������������
void Tree::CheckInit(TypeLex a)
{
	if (!isInterpret)
		return;
	
	Tree* v = SemGetVar(a);
	if (v->n->var.init == false)
		PrintError("������������ �������������������� ����������", a);
}

// ����� ������
void Tree::PrintError(std::string error, TypeLex a)
{
	if (!isInterpret)
		return;
	
	printf("\n!!!Semantic error (������ %d): %s, ������������� %s\n", sc->GetLine() + 1, error.c_str(), a);
	exit(-2);
}

// �������� ���
DATA_TYPE Tree::GetType(TypeLex l)
{
	if (!isInterpret)
		return (DATA_TYPE)NULL;
	
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
	if (!isInterpret)
		return;
	
	Tree* v = SemGetVar(a);

	DATA_TYPE constDataType = GetType(l);
	
	if (v->n->dataType < constDataType)
	{
		printf("\n***Semantic warning (������ %d): ������� ���������� ����� ��� %s � %s, ������������� %s\n", sc->GetLine() + 1,
			types[constDataType].c_str(), types[v->n->dataType].c_str(), a);
	}
	
}

// ��������� ����������� ���������� �����
void Tree::SemTypeCastCheck(TypeLex a, DATA_TYPE dt)
{
	if (!isInterpret)
		return;
	
	Tree* v = SemGetVar(a);

	if (v->n->dataType < dt)
	{
		printf("\n***Semantic warning (������ %d): ������� ���������� ����� ��� %s � %s, ������������� %s\n", sc->GetLine() + 1,
			types[dt].c_str(), types[v->n->dataType].c_str(), a);
	}
}

// ��������� ����������� ���������� �����
void Tree::SemTypeCastCheck(DATA_TYPE dt1, DATA_TYPE dt2)
{
	if (!isInterpret)
		return;
	
	if (dt1 < dt2)
	{
		printf("\n***Semantic warning (������ %d): ������� ���������� ����� ��� %s, �������� ��� %s\n", sc->GetLine() + 1,
			types[dt2].c_str(), types[dt1].c_str());
	}
}

// ����� ������
void Tree::Print()
{
	if (!isInterpret)
		return;

	std::string nObjType;	// ���� �������� ��� �����
	std::string nType;	// ���� ������ ��� �����
	std::string nVal;		// �������� ��� �����

	nObjType = objTypes[n->objType];
	nType = types[n->dataType];
	nVal = n->dataType == TYPE_INTEGER ? std::to_string(n->dataValue.intValue) : n->dataType == TYPE_LONG_LONG ? std::to_string(n->dataValue.llValue) : "none";

	std::cout << std::setw(15) << n->id
		<< std::setw(15) << std::left << nObjType.c_str()
		<< std::setw(15) << std::left << nType.c_str()
		<< std::setw(30) << std::left << nVal.c_str();
	
	std::cout << std::setw(15) << std::left;
	// printf("������� � ������� %s <type %s> <value %s> ----->", n->id, nType.c_str(), nVal.c_str());
	if (l != NULL)
	{
		std::cout << l->n->id;
	}
	else
	{
		std::cout << "-";
	}
	std::cout << std::setw(15) << std::left;
	if (r != NULL)
	{
		std::cout << r->n->id;
	}
	else
	{
		std::cout << "-";
	}
	
	printf("\n");
	if (r != NULL) r->Print();
	if (l != NULL) l->Print();
}

// ����� ������ � ��������
void Tree::PrintWithTag(std::string tag)
{
	if (!isInterpret)
		return;
	
	printf("\n==========================================================\n");
	printf("%s\n", tag.c_str());
	printf("==========================================================\n");
	std::cout << std::setw(15) << std::left << "�������"
		<< std::setw(15) << std::left << "��� �������"
		<< std::setw(15) << std::left << "��� ������"
		<< std::setw(30) << std::left << "��������"
		<< std::setw(15) << std::left << "�����"
		<< std::setw(15) << std::left << "������" << std::endl;
	printf("--------------------------------------------------------------------------------------------------------------\n");
	Print();
}

// ���������� � ������������� ������ ���������� ��������� (������ ���� �����, �� ���� ������ ���� ������)
Tree* Tree::CoplexOperator()
{
	if (!isInterpret)
		return (Tree*)NULL;
	
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

// ������� ���������
void Tree::DelSubTree(Tree* addr)
{
	if (!isInterpret)
		return;
	
	if (addr->r != NULL)
		DelSubTree(addr->r);

	if (addr->l != NULL)
		DelSubTree(addr->l);

	delete addr;
}

// �������� ��� ������� �������
Tree* Tree::SemStartFunction(TypeLex func)
{
	if (!isInterpret)
		return (Tree*)NULL;
	
	Tree* funcNode = SemGetFunc(func);

	Tree* treeCopy = new Tree(nullptr, nullptr, nullptr, funcNode->n);
	
	treeCopy->up = funcNode;
	
	treeCopy->r = new Tree(nullptr, nullptr, nullptr, funcNode->r->n);
	treeCopy->r->up = treeCopy;
	
	treeCopy->l = funcNode->l;
	treeCopy->l->up = treeCopy;
	
	funcNode->l = treeCopy;

	SetCur(treeCopy->r);

	return treeCopy;
}

// ������� ��������� (�������)
void Tree::DelSubTreeForFunc(Tree* addr)
{
	if (!isInterpret)
		return;
	
	DelSubTree(addr->r);

	Tree* up = addr->up;
	
	addr->up->l = addr->l;
	addr->l->up = addr->up;
	
	delete addr;
}
// ������� ��������� (����)
Tree* Tree::DelSubTreeForBlock(Tree* addr)
{
	if (!isInterpret)
		return (Tree*)NULL;
	
	DelSubTree(addr->r);

	Tree* ret = addr->up;
	
	delete addr;

	ret->l = NULL;
	
	return ret;
}
