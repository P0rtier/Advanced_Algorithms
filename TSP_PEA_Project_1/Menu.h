#pragma once

#include<string>
#include "Matrix.h"
using namespace std;

class Menu {
	char choice;
	string filepath;
	string number_of_vertices;
	Matrix matrix;

public:

	void mainMenu();
	void preMenu();
	void test();
};