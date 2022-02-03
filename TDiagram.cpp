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

	Root->PrintWithTag("Вывод дерева перед определением функции: ");
	
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
		
		TData* thisFuncData = new TData();
		TFuncBody(thisFuncData);
		Root->SemTypeCastCheck(funcRetType, thisFuncData->dataType);
		Root->SemSetValue(v, thisFuncData->dataValue);
	}
	else
		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось имя функции"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	Root->PrintWithTag("Вывод дерева перед освобождением памяти после функции: ");
	Root->DelSubTreeForFunc(v);
	Root->PrintWithTag("Вывод дерева после освобождения памяти после функции: ");
	
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

void TDiagram::TFuncBody(TData* data)
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

	TFuncRet(data);
	
	type = sc->Scaner(lex);
	if (type != TRightFB)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ }"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
}

void TDiagram::TFuncRet(TData* data)
{
	int type;

	type = sc->Scaner(lex);
	if (type != TRet)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидалось ключевое слово return"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	TExpr(data);
	
	Root->SemTypeCastCheck(Root->SemGetFirstFunc(), data->dataType);
	
	type = sc->Scaner(lex);
	if (type != TEndComma)
		sc->PrintError(syntaxError, const_cast < char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	if (data->dataType == TYPE_INTEGER)
		data->dataValue.intValue = 0;
	else if (data->dataType == TYPE_LONG_LONG)
		data->dataValue.llValue = 0;
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
	
	TData* data = new TData();
	TExpr(data);

	Root->SemTypeCastCheck(ident, data->dataType);
	// Присваиваем значение
	Root->SemSetValue(v, data);
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

void TDiagram::TOperator(bool inFor)
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
		{
			TData* data = new TData();
			TExpr(data);
		}

		type = sc->Scaner(lex);
		if (type != TEndComma)
			sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	}
	else if (type == TEndComma)
		type = sc->Scaner(lex);
	else if ((type == TConsInt) || (type == TConsHex) || (type == TLeftRB) || (type == TAdd) || (type == TSub) || (type == TInc) || (type == TDec))
	{
		TData* data = new TData();
		TExpr(data);
		
		type = sc->Scaner(lex);
		if (type != TEndComma)
			sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
	}
	else if (type == TLeftFB)
	{
		Tree* l = NULL;

		if (!inFor)
		{
			Root->PrintWithTag("Вывод дерева перед началом блока: ");
			l = Root->CoplexOperator();
		}
		
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

		if (!inFor)
		{
			Root->PrintWithTag("Вывод дерева перед концом блока: ");
			Tree* ret = Root->DelSubTreeForBlock(l);
			Root->PrintWithTag("Вывод дерева после конца блока: ");
			Root->SetCur(ret);
		}
		sc->Scaner(lex);
	}
	else
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался оператор"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

}

void TDiagram::TForSD()
{
	int type;

	Root->PrintWithTag("Вывод дерева перед оператором for: ");
	
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

	TData* expr = new TData();
	TExpr(expr);

	type = sc->Scaner(lex);
	if (type != TEndComma)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ ;"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	TData* post = new TData();
	TPostEl(post);
	
	type = sc->Scaner(lex);
	if (type != TRightRB)
		sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ )"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());

	TOperator(true);

	Root->PrintWithTag("Вывод дерева перед очисткой оператора for: ");
	Tree* retForSet = Root->DelSubTreeForBlock(ret);
	Root->PrintWithTag("Вывод дерева после очистки оператора for: ");
	Root->SetCur(retForSet);
}

void TDiagram::TExpr(TData* data)
{
	int type;

	TCompareEl(data);

	type = lookForvard(1);
	
	while ((type == TEqual) || (type == TNotEqual))
	{
		sc->Scaner(lex);
		TData* compareData = new TData();
		TCompareEl(compareData);
		// dt = max(dt1, dt);
		type = lookForvard(1);
	}
}

void TDiagram::TCompareEl(TData* data)
{
	int type;

	TAddEl(data);

	type = lookForvard(1);

	while ((type == TLess) || (type == TMore) || (type == TLessOrEqual) || (type == TMoreOrEqual))
	{
		sc->Scaner(lex);
		TData* addData = new TData();
		TAddEl(addData);
		//dt = max(dt1, dt);
		type = lookForvard(1);
	}
}

void TDiagram::TAddEl(TData* data)
{
	int type;

	TMulEl(data);

	type = lookForvard(1);

	while ((type == TAdd) || (type == TSub))
	{
		sc->Scaner(lex);
		TData* mulData = new TData();
		TMulEl(mulData);
		//dt = max(dt1, dt);
		type = lookForvard(1);
	}
}

void TDiagram::TMulEl(TData* data)
{
	int type;

	TPrefEl(data);

	type = lookForvard(1);

	while ((type == TMul) || (type == TDiv) || (type == TDivPart))
	{
		sc->Scaner(lex);
		TData* prefData = new TData();
		TPrefEl(prefData);
		//dt = max(dt1, dt);
		type = lookForvard(1);
	}
}

void TDiagram::TPrefEl(TData* data)
{
	int type;

	type = lookForvard(1);
	
	if ((type == TInc) || (type == TDec) || (type == TAdd) || (type == TSub))
	{
		sc->Scaner(lex);
		isPref = true;
	}

	TPostEl(data);
}

void TDiagram::TPostEl(TData* data)
{
	int type;
	
	TElementExpr(data);

	type = lookForvard(1);

	if ((type == TInc) || (type == TDec))
	{
		Root->SemCheckFuncOrConst(lastIdentInExpr);
		sc->Scaner(lex);
	}
}

void TDiagram::TElementExpr(TData* data)
{
	int type;

	type = lookForvard(1);

	if (type == TLeftRB)
	{
		sc->Scaner(lex);
		TExpr(data);
		
		type = sc->Scaner(lex);
		if (type != TRightRB)
			sc->PrintError(syntaxError, const_cast<char*>("Ожидался символ )"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
		return;
	}

	if (type == TIdent)
	{
		type = lookForvard(2);

		if (type == TLeftRB)
		{
			//return TFuncStart();
			TFuncStart();
		}
		else
		{
			sc->Scaner(lex);
			//dt = Root->GetType(lex);

			// Взять тип и значение
			Root->SemGetData(Root->SemGetVar(lex), data);
			
			Root->CheckInit(lex);
			// Нельзя ++ -- для константы
			if (isPref)
			{
				Root->CheckConst(lex);
				isPref = false;
			}
			memcpy(lastIdentInExpr, lex, sizeof(lex));
		}
		return;
	}
	
	if ((type == TConsInt) || (type == TConsHex))
	{
		sc->Scaner(lex);
		// !!!Semantic!!! //
		if (isPref)
		{
			printf("Semantic error (строка %d): Нельзя применить префиксный оператор к константе", sc->GetLine());
		}

		Root->SemReadStringValue(lex, data);

		return;
	}

		sc->PrintError(syntaxError, const_cast<char*>("Ожидалось элементарное выражение"), lex, sc->GetLine(), sc->GetPos() - sc->GetPosNewLine());
}
