#include "defs.h"
#include "TScaner.h"

#define isNumber (text[pos] <= '9') && (text[pos] >= '0')
#define isHexCase ((text[pos] >= 'a') && (text[pos] <= 'f')) || ((text[pos] >= 'A') && (text[pos] <= 'F'))
#define lowerCase (text[pos] >= 'a') && (text[pos] <= 'z')
#define upperCase (text[pos] >= 'A') && (text[pos] <= 'Z')

TypeLex Keyword[MAX_KEYW] = { "main", "for", "const",
	"int", /*"short",*/ "long", "return"};

int IndexKeyword[MAX_KEYW] = {TMain, TFor, TKeyConst,
	TTypeInt, /*TTypeShort,*/ TTypeLong, TRet};

char* lexicalError = const_cast<char*>("Lexical");

TScaner::TScaner(char* fileName)
{
	GetData(fileName);
	SetPos(0);
	posNewLine = 0;
	line = 0;
}

void TScaner::SetPos(int i) { pos = i; } // ���������� ���������

void TScaner::SetLine(int i) { line = i; } // ���������� ������

void TScaner::SetPosNewLine(int i) { posNewLine = i; } // ���������� ������� ������

int TScaner::GetPos(void) { return pos; } // �������� ���������

int TScaner::GetLine(void) { return line; } // �������� ����� ������

int TScaner::GetPosNewLine(void) { return posNewLine; } // �������� ������� ��������� ������

void TScaner::PrintError(char* errorType, char* err, char* lex, int line, int pos)
{
	if ((line == -1) && (pos == -1))
	{
		if (lex[0] == '\0')
			printf("Error: %s\n", errorType, err);
		else
			printf("Error: %s. �������� ������ %s\n", errorType, err, lex);
	}
	else
	{
		if (lex[0] == '\0')
			printf("%s error: %s, ������ %s � ������ %d �� ������� %d\n", errorType, err, lex, line + 1, pos + 1);
		else
			printf("%s error: %s. �������� ������ %s � ������ %d �� ������� %d\n", errorType, err, lex, line + 1, pos + 1);
	}
	exit(-10);
}

int TScaner::Scaner(TypeLex l)
{
	int i;	// ������� ����� �������
	char* errMessage;	// ��������� �� ������

	// ������� �������
	for (i = 0; i < MAX_LEX; i++)
		l[i] = 0;

	i = 0;
	
start:
	
	// ������������ �������
	while ((text[pos] == ' ') || (text[pos] == '\n') || (text[pos] == '\t'))
	{
		if (text[pos] == '\n')
		{
			line++;
			posNewLine = pos + 1;
		}
		
		pos++;
	}
	
	// �����������
	if (text[pos] == '/')
	{
		if (text[pos + 1] == '/') // ������������
		{
			pos += 2;
			while (text[pos] != '\n')
				pos++;
			goto start;
		} else if(text[pos + 1] == '*') // �������
		{
			pos += 2;
			while (!((text[pos] == '*') && (text[pos + 1] == '/')))
				pos++;
			pos += 2;
			goto start;
		} else // ���� ��� �� �����������, ������ ��� ���� �������
		{
			l[i++] = text[pos++];
			return TDiv;
		}
	}

	// ����� ���������
	if (text[pos] == '\0')
	{
		l[0] = '#';
		return TEnd;
	}
	
	// ���������
	if (isNumber)
	{
		l[i++] = text[pos++];
		if ((text[pos - 1] == '0') && (text[pos] == 'x')) // 16 �/�
		{
			l[i++] = text[pos++];
			if (!(isNumber || isHexCase))
			{
				errMessage = const_cast<char*>("������������ ��������� � 16 �/�");
				PrintError(lexicalError, errMessage, l, line, pos - posNewLine);
				pos++;
				return TError;
			}
			while (isNumber || isHexCase)
				if (i < MAX_LEX - 1)
					l[i++] = text[pos++];
				else
					pos++;
			return TConsHex;
		}
		else // int
		{
			while(isNumber)
				if (i < MAX_LEX - 1)
					l[i++] = text[pos++];
				else
					pos++;
			return TConsInt;
		}
	}
	else if (lowerCase || upperCase || text[pos] == '_') // ������������� ��� �������� �����
	{
		l[i++] = text[pos++];
		while (isNumber || lowerCase || upperCase || text[pos] == '_')
			if (i < MAX_LEX - 1)
				l[i++] = text[pos++];
			else
				pos++;

		// ��������� �������� �����
		int j;
		for (j = 0; j < MAX_KEYW; j++)
			if (strcmp(l, Keyword[j]) == 0) 
				return IndexKeyword[j];
		return TIdent;
	}
	else if (text[pos] == '!' && text[pos + 1] == '=')
	{
		l[i++] = text[pos++]; l[i++] = text[pos++];
		return TNotEqual;
	}
	else if (text[pos] == '=')
	{
		l[i++] = text[pos++];
		if (text[pos] == '=')
		{
			l[i++] = text[pos++];
			return TEqual;
		}
		else
			return TAssign;
	}
	else if (text[pos] == '<')
	{
		l[i++] = text[pos++];
		if (text[pos] == '=')
		{
			l[i++] = text[pos++];
			return TLessOrEqual;
		}
		else
			return TLess;
	}
	else if (text[pos] == '>')
	{
		l[i++] = text[pos++];
		if (text[pos] == '=')
		{
			l[i++] = text[pos++];
			return TMoreOrEqual;
		}
		else
			return TMore;
	}
	else if (text[pos] == '+')
	{
		l[i++] = text[pos++];
		if (text[pos] == '+')
		{
			l[i++] = text[pos++];
			return TInc;
		}
		else
			return TAdd;
	}
	else if (text[pos] == '-')
	{
		l[i++] = text[pos++];
		if (text[pos] == '-')
		{
			l[i++] = text[pos++];
			return TDec;
		}
		else
			return TSub;
	}
	else if (text[pos] == '*')
	{
		l[i++] = text[pos++];
		return TMul;
	}
	else if (text[pos] == '%')
	{
		l[i++] = text[pos++];
		return TDivPart;
	}
	else if (text[pos] == ',')
	{
		l[i++] = text[pos++]; return TComma;
	}
	else if (text[pos] == ';')
	{
		l[i++] = text[pos++]; return TEndComma;
	}
	else if (text[pos] == '(')
	{
		l[i++] = text[pos++]; return TLeftRB;
	}
	else if (text[pos] == ')')
	{
		l[i++] = text[pos++]; return TRightRB;
	}
	else if (text[pos] == '{')
	{
		l[i++] = text[pos++]; return TLeftFB;
	}
	else if (text[pos] == '}')
	{
		l[i++] = text[pos++]; return TRightFB;
	}
	else {
		errMessage = const_cast<char*>("�������� ������");
		l[i] = text[pos];
		PrintError(lexicalError, errMessage, l, line, pos - posNewLine);
		pos++;
		return TError;
	}
}

void TScaner::GetData(char* fileName) {
	// ���� �������� �����, �� �������� ����� ������ ������������� �����
	char sym;
	char* empty = const_cast<char*>("");
	char* errMessage;
	
	FILE* in = fopen(fileName, "r");
	
	if (in == NULL) 
	{
		errMessage = const_cast<char*>("���������� ������� ����");
		PrintError(lexicalError, errMessage, empty, -1, -1);
		exit(1);
	}
	
	int i = 0;
	
	while (!feof(in))
	{
		fscanf(in, "%c", &sym);
		if (!feof(in)) 
			text[i++] = sym;
		
		if (i >= MAX_TEXT - 1)
		{
			errMessage = const_cast<char*>("���������� ������� ������ ���������");
			PrintError(lexicalError, errMessage, empty, -1, -1);
			break;
		}
	}
	text[i] = '\0'; // ���������� '\0' � ����� ������
	fclose(in);
}