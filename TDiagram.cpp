#include "defs.h"
#include "TScaner.h"
#include "TDiagram.h"
#include "Semantic.h"

#define max(a,b) a<b? b : a
// Проверка, является ли лексема типом
#define isType(type) (/*(type == TTypeShort) || */(type == TTypeInt) || (type == TTypeLong))
// Проверка, является ли лексема константой
#define isConst(type) ((type == TConsInt) || (type == TConsHex))
// Проверка, является ли лексема оператором
#define isOperator(type) ((type == TFor) || (type == TLeftFB) || (type == TEndComma) || (type == TIdent))
// Проверка, является ли лексема выражением
#define isExpr(type) ((type == TLeftRB) || (type == TAdd) || (type == TSub) || (type == TInc) || (type == TDec))

DATA_TYPE dataType;		// Последний просканированный тип
bool isPref = false;
TypeLex lastIdentInExpr;

char* syntaxError = const_cast<char*>("Syntax");

// Задать корень семантического дерева
void TDiagram::SetRoot(Tree* r)
{
	this->Root = r;
}

// Посмотреть тип k-ой лексемы спереди
int TDiagram::lookForvard(int k = 1)
{
	int pos = sc->GetPos();
	int line = sc->GetLine();
	int nextPos = sc->GetPosNewLine();

	int nextType;

	for (int i = 0; i < k; i++)
		nextType = sc->Scaner(lex);

	sc->SetPos(pos);
	sc->SetLine(line);
	sc->SetPosNewLine(nextPos);

	return nextType;
}

// Синтаксическая проверка программы
void TDiagram::TProgram()
{
	int type, forvard;

	type = lookForvard(1);
	
	while (isType(type) || (type == TKeyConst))
	{
		if (type == TKeyConst)
			TConstData();
		else if (isType(type))
		{
			forvard = 2;
			if (type == TTypeLong)
			{
				type = lookForvard(forvard);
				if (type = TTypeLong)
				{
					forvard++;
					type = lookForvard(forvard);
				}
			}

			type = lookForvard(forvard);

			if (type == TMain)
			{
				TFunc();
			}
			else if (type == TIdent)
			{
				type = lookForvard(forvard + 1);

				if (type == TLeftRB)
					TFunc();
				else
					TVariableData();
			}
			else
				sc->PrintError(syntaxError, const_cast<char*>("Ожидался идентификатор или ключевое слово main"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
		}

		type = lookForvard(1);
	}

	if (type != TEnd)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось описание функции, данных или именованных констант"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
}

void TDiagram::TFunc()
{
	int type;

	DATA_TYPE funcRetType = TType();
	
	type = sc->Scaner(lex);

	Tree* v = Root->SemInclude(lex, TYPE_FUNC, funcRetType);

	if ((type == TMain) || (type == TIdent))
	{	
		type = sc->Scaner(lex);
		if (type != TLeftRB)
			sc->PrintError(syntaxError, const_cast <char*>("Ожидался символ ("), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

		type = sc->Scaner(lex);
		if (type != TRightRB)
		{
			sc->PrintError(syntaxError, const_cast <char*>("Ожидался символ )"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
		}
		
		TData thisFuncData = TFuncBody();
		Root->SemTypeCastCheck(funcRetType, thisFuncData.dataType);
		Root->SemSetValue(v, thisFuncData.dataValue);
	}
	else
		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось имя функции"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	Root->SetCur(v);
}

DATA_TYPE TDiagram::TType()
{
	int type;

	type = sc->Scaner(lex);

	if (!isType(type))
	{
		//sc->PrintError(syntaxError, const_cast < char*>("Ожидался тип (short, int, long, long long)"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
		sc->PrintError(syntaxError, const_cast <char*>("Ожидался тип (int, long long)"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	}
	else if (type == TTypeLong)
	{
		type = lookForvard(1);

		if (type == TTypeLong)
		{
			sc->Scaner(lex);
			return TYPE_LONG_LONG;
		}
		//else
		//	return TYPE_LONG;
		else
			sc->PrintError(syntaxError, const_cast <char*>("Ожидался тип long long"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	}

	//if (type == TTypeShort)
	//	return TYPE_SHORT;
	else if (type == TTypeInt)
		return TYPE_INTEGER;
}

void TDiagram::TConstData()
{
	int type;

	type = sc->Scaner(lex);

	if (type != TKeyConst)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось ключевое слово const"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	dataType = TType();

	TConstList();
}

void TDiagram::TVariableData()
{
	dataType = TType();

	TVariableList();
}

TData TDiagram::TFuncBody()
{
	int type;

	type = sc->Scaner(lex);
	if (type != TLeftFB)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ {"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	type = lookForvard(1);
	
	while (type != TRet)
	{	
		if (isOperator(type) || isConst(type) || isExpr(type))
			TOperators();
		else if ((type == TKeyConst))
			TConstData();
		else if (isType(type))
			TVariableData();
		else
			sc->PrintError(syntaxError, const_cast<char*>("Ожидался выход из функции"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

		type = lookForvard(1);
	}

	TData thisFuncData = TFuncRet();
	
	type = sc->Scaner(lex);
	if (type != TRightFB)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ }"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	return thisFuncData;
}

TData TDiagram::TFuncRet()
{
	int type;

	type = sc->Scaner(lex);
	if (type != TRet)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидалось ключевое слово return"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	DATA_TYPE dt = TExpr();

	Root->SemTypeCastCheck(Root->SemGetFirstFunc(), dt);
	
	type = sc->Scaner(lex);
	if (type != TEndComma)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	TData thisFuncData;
	thisFuncData.dataType = dt;
	if (dt == TYPE_INTEGER)
		thisFuncData.dataValue.intValue = 0;
	else if (dt == TYPE_LONG_LONG)
		thisFuncData.dataValue.llValue = 0;

	return thisFuncData;
}

DATA_TYPE TDiagram::TFuncStart()
{
	int type;
	DATA_TYPE dt;

	type = sc->Scaner(lex);
	if (type != TIdent && type != TMain)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался идентификатор или main"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	Root->SemGetFunc(lex);
	if (isPref)
	{
		Root->PrintError("Нельзя применить префиксный оператор к вызову функции", lex);
		isPref = false;
	}
	memcpy(lastIdentInExpr, lex, sizeof(lex));

	dt = Root->GetType(lex);
	
	type = sc->Scaner(lex);
	if (type != TLeftRB)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ ("), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	type = sc->Scaner(lex);
	if (type != TRightRB)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ )"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	return dt;
}

void TDiagram::TConstList()
{
	int type;
	TypeLex ident;

	do
	{
		type = sc->Scaner(lex);
		if (type != TIdent) sc->PrintError(syntaxError, const_cast < char*>("Ожидался идентификатор"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

		memcpy(ident, lex, sizeof(lex));
		// Добавляем переменную в дерево
		Tree *v = Root->SemInclude(lex, TYPE_VAR, dataType);
		
		type = sc->Scaner(lex);
		if (type != TAssign) sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ ="), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

		type = sc->Scaner(lex);
		if (!isConst(type)) sc->PrintError(syntaxError, const_cast < char*>("Ожидалась константа"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

		// Указываем, что это константа и указываем ссылку на значение
		Root->SemSetConst(v, true);
		Root->SemSetInit(v, true);
		Root->SemTypeCastCheck(ident, lex);
		Root->SemSetValue(v, dataType, lex);
	
		type = sc->Scaner(lex);
	} while (type == TComma);

	if (type != TEndComma) sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
}

void TDiagram::TVariableList()
{
	int type;

	do
	{
		type = lookForvard(2);
		if (type == TAssign)
			TAssignSD(false);
		else
		{
			type = sc->Scaner(lex);
			if (type != TIdent) sc->PrintError(syntaxError, const_cast <char*>("Ожидался идентификатор"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

			// Добавляем переменную в дерево
			Tree* v = Root->SemInclude(lex, TYPE_VAR, dataType);
		}

		type = sc->Scaner(lex);

	} while (type == TComma);

	if (type != TEndComma) sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
}

void TDiagram::TAssignSD(bool checkSem)
{
	int type;
	TypeLex ident;

	type = sc->Scaner(lex);
	if (type != TIdent) sc->PrintError(syntaxError, const_cast < char*>("Ожидался идентификатор"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	memcpy(ident, lex, sizeof(lex));
	// !!!Semantic!!! //
	if (checkSem)
	{
		Root->CheckConst(lex);
	}			

	// Добавляем переменную в дерево
	Tree *v = Root->SemInclude(lex, TYPE_VAR, dataType);
	
	type = sc->Scaner(lex);
	if (type != TAssign) sc->PrintError(syntaxError, const_cast < char*>("Ожидался знак ="), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	DATA_TYPE dt = TExpr();

	Root->SemTypeCastCheck(ident, dt);
	// Присваиваем значение
	Root->SemSetValue(v, dataType, const_cast < char*>("0"));
	// Указываем, что переменная стала инициализированной
	Root->SemSetInit(v, true);
}

void TDiagram::TOperators()
{
	int type;

	type = lookForvard(1);
	
	while(isOperator(type) || isConst(type) || isExpr(type))
	{
		TOperator();
		type = lookForvard(1);
	}
}

void TDiagram::TOperator()
{
	int type;

	type = lookForvard(1);

	if (type == TFor)
		TForSD();
	else if (type == TIdent)
	{
		type = lookForvard(2);
		
		if (type == TAssign)
			TAssignSD(true);
		else
			TExpr();

		type = sc->Scaner(lex);
		if (type != TEndComma)
			sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	}
	else if (type == TEndComma)
		type = sc->Scaner(lex);
	else if ((type == TConsInt) || (type == TConsHex) || (type == TLeftRB) || (type == TAdd) || (type == TSub) || (type == TInc) || (type == TDec))
	{
		TExpr();
		
		type = sc->Scaner(lex);
		if (type != TEndComma)
			sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	}
	else if (type == TLeftFB)
	{
		Tree* l = Root->CoplexOperator();
		Tree* r = Root->GetCur();
		
		type = sc->Scaner(lex);
		do
		{
			type = lookForvard(1);

			if (type == TKeyConst)
			{
				TConstData();
			}
			else if (isType(type))
			{
				TVariableData();
			}
			else if (isOperator(type) || isConst(type) || isExpr(type))
			{
				TOperator();
			}
			else
				sc->PrintError(syntaxError, const_cast<char*>("Ожидался оператор или описание данных"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

			type = lookForvard(1);
			
		} while (type != TRightFB);

		Root->SetCur(l);
		sc->Scaner(lex);
	}
	else
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался оператор"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

}

void TDiagram::TForSD()
{
	int type;

	type = sc->Scaner(lex);
	if (type != TFor)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось ключевое слово for"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	type = sc->Scaner(lex);
	if (type != TLeftRB)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ("), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	Tree* ret = Root->CoplexOperator();
	
	type = sc->Scaner(lex);
	if (type != TTypeInt)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось ключевое слово int"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	dataType = TYPE_INTEGER;
	TAssignSD(false);
	
	type = sc->Scaner(lex);
	if (type != TEndComma)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	TExpr();

	type = sc->Scaner(lex);
	if (type != TEndComma)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	TPostEl();
	
	type = sc->Scaner(lex);
	if (type != TRightRB)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ )"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	TOperator();

	Root->SetCur(ret);
}

DATA_TYPE TDiagram::TExpr()
{
	int type;

	DATA_TYPE dt = TCompareEl();

	type = lookForvard(1);
	
	while ((type == TEqual) || (type == TNotEqual))
	{
		sc->Scaner(lex);
		DATA_TYPE dt1 = TCompareEl();
		dt = max(dt1, dt);
		type = lookForvard(1);
	}

	return dt;
}

DATA_TYPE TDiagram::TCompareEl()
{
	int type;

	DATA_TYPE dt = TAddEl();

	type = lookForvard(1);

	while ((type == TLess) || (type == TMore) || (type == TLessOrEqual) || (type == TMoreOrEqual))
	{
		sc->Scaner(lex);
		DATA_TYPE dt1 = TAddEl();
		dt = max(dt1, dt);
		type = lookForvard(1);
	}

	return dt;
}

DATA_TYPE TDiagram::TAddEl()
{
	int type;

	DATA_TYPE dt = TMulEl();

	type = lookForvard(1);

	while ((type == TAdd) || (type == TSub))
	{
		sc->Scaner(lex);
		DATA_TYPE dt1 = TMulEl();
		dt = max(dt1, dt);
		type = lookForvard(1);
	}

	return dt;
}

DATA_TYPE TDiagram::TMulEl()
{
	int type;

	DATA_TYPE dt = TPrefEl();

	type = lookForvard(1);

	while ((type == TMul) || (type == TDiv) || (type == TDivPart))
	{
		sc->Scaner(lex);
		DATA_TYPE dt1 = TPrefEl();
		dt = max(dt1, dt);
		type = lookForvard(1);
	}

	return dt;
}

DATA_TYPE TDiagram::TPrefEl()
{
	int type;

	type = lookForvard(1);
	
	if ((type == TInc) || (type == TDec) || (type == TAdd) || (type == TSub))
	{
		sc->Scaner(lex);
		isPref = true;
	}

	return TPostEl();
}

DATA_TYPE TDiagram::TPostEl()
{
	int type;
	
	DATA_TYPE dt = TElementExpr();

	type = lookForvard(1);

	if ((type == TInc) || (type == TDec))
	{
		Root->SemCheckFuncOrConst(lastIdentInExpr);
		sc->Scaner(lex);
	}
	return dt;
}

DATA_TYPE TDiagram::TElementExpr()
{
	int type;
	DATA_TYPE dt;

	type = lookForvard(1);

	if (type == TLeftRB)
	{
		sc->Scaner(lex);
		dt = TExpr();
		type = sc->Scaner(lex);
		if (type != TRightRB)
			sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ )"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
		return dt;
	}
	else if (type == TIdent)
	{
		type = lookForvard(2);

		if (type == TLeftRB)
		{
			return TFuncStart();
		}
		else
		{
			sc->Scaner(lex);
			dt = Root->GetType(lex);
			Root->CheckInit(lex);
			// Нельзя ++ -- для константы
			if (isPref)
			{
				Root->CheckConst(lex);
				isPref = false;
			}
			memcpy(lastIdentInExpr, lex, sizeof(lex));

			return dt;
		}
	}
	else if ((type == TConsInt) || (type == TConsHex))
	{
		sc->Scaner(lex);
		// !!!Semantic!!! //
		if (isPref)
		{
			printf("Semantic error (строка %d): Нельзя применить префиксный оператор к константе", sc->GetLine());
		}
		return Root->GetType(lex);
	}
	else
	{
		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось элементарное выражение"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	}
	
}
