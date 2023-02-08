#pragma once

#include<string>
#include "Matrix.h"
using namespace std;

class Menu {
	char choice;
	string filepath;
	string number_of_vertices;
	Matrix matrix;
	double chosen_time_max;
	int chosen_neighbourhood_type;

public:

	void mainMenu();
	void preMenu();
	void printPath(vector<int> path);
private:
	void resolveAnnealing(Matrix matrix, double cooler);
	void resolveTabu(Matrix matrix);
	void error_exit(const string& msg);
	int header_get_size(ifstream& file);
	int value_digits(int val);
	int convert();
	void test();
	void saveParameters();
};