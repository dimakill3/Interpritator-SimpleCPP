#include "defs.h"
#include "TScaner.h"
#include "TDiagram.h"
#include <iostream>
#include "Semantic.h"


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	TScaner* sc;
	
	// ������ ���� � �������� �������:
	if (argc <= 1) 
		sc = new TScaner(const_cast<char*>("CodeTXT/CodeL2Sem8.txt"));// ���� �� ���������
	else 
		sc = new TScaner(argv[1]); // ����� ����
	
	TDiagram* dg = new TDiagram(sc);
	
	// ������ ������ ����� � ������������� ������
	Node n;
	memcpy(n.id, &"<root>", 7);
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
		printf("�������������� � ������������� ������ �� ����������. \n");
	else 
		sc->PrintError(const_cast<char*>("Syntax"), const_cast<char*>("������ ����� � ����� ���������."), const_cast<char*>(""), -1, -1);

	// ������� ������������� ������
	// myRoot->Print();
	
	return 0;
}