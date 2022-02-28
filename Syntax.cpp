#include "defs.h"
#include "TScaner.h"
#include "TDiagram.h"
#include <iostream>
#include "Semantic.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	TScaner* sc;

	TData* hh = new TData();

	hh->dataValue.intValue = 1;
	
	// ������ ���� � �������� �������:
	if (argc <= 1) 
		sc = new TScaner(const_cast<char*>("CodeTXT/re-Ivankov.txt"));// ���� �� ���������
	else 
		sc = new TScaner(argv[1]); // ����� ����
	
	TDiagram* dg = new TDiagram(sc);
	
	// ������ ������ ����� � ������������� ������
	Node n;
	memcpy(n.id, &"<root>", 7);
	n.objType = OBJ_NONE;
	n.dataType = TYPE_NONE;
	
	// ������ �������� ���� �������������� ������ � ������ ��� �������
	Tree* myRoot = new Tree(nullptr, nullptr, nullptr, &n);
	myRoot->SetCur(myRoot);
	myRoot->SetScaner(sc);

	dg->SetRoot(myRoot);

	// ��������� �������������� ������
	dg->TProgram();
	
	// ��������� ��������� ������
	int type; TypeLex l;
	type = sc->Scaner(l);
	if (type == TEnd) 
		printf("\n�������������� � ������������� ������ �� ����������. \n");
	else 
		sc->PrintError(const_cast<char*>("Syntax"), const_cast<char*>("\n������ ����� � ����� ���������.\n"), const_cast<char*>(""), -1, -1);

	// ������� ������������� ������
	myRoot->PrintWithTag("����� ������ ����� ���������� ���������: ");
	
	return 0;
}