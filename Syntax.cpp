#include "defs.h"
#include "TScaner.h"
#include "TDiagram.h"
#include <iostream>
#include "Semantic.h"


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	TScaner* sc;
	
	// ввести файл с исходным модулем:
	if (argc <= 1) 
		sc = new TScaner(const_cast<char*>("CodeTXT/CodeL2Sem8.txt"));// файл по умолчанию
	else 
		sc = new TScaner(argv[1]); // задан файл
	
	TDiagram* dg = new TDiagram(sc);
	
	// Создаём запись корня в семантическом дереве
	Node n;
	memcpy(n.id, &"<root>", 7);
	n.dataType = TYPE_NONE;
	
	// Создаём корневой узел семантического дерева и делаем его текущим
	Tree* myRoot = new Tree(nullptr, nullptr, nullptr, &n);
	myRoot->SetCur(myRoot);
	myRoot->SetScaner(sc);

	dg->SetRoot(myRoot);

	// Запускаем синтаксический анализ
	dg->TProgram();
	
	// Проверяем последний символ
	int type; TypeLex l;
	type = sc->Scaner(l);
	if (type == TEnd) 
		printf("Синтаксических и семантических ошибок не обнаружено. \n");
	else 
		sc->PrintError(const_cast<char*>("Syntax"), const_cast<char*>("Лишний текст в конце программы."), const_cast<char*>(""), -1, -1);

	// Выводим семантическое дерево
	// myRoot->Print();
	
	return 0;
}