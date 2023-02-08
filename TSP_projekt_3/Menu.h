#pragma once

#include<string>
#include "Matrix.h"
using namespace std;

class Menu {
	char choice;
	string filepath;
	string number_of_vertices;
	Matrix matrix;
	bool parameters_have_been_chosen;
	double chosen_time_max;
	int mutation_type;
	double mutation_coeff;
	double cross_coeff;
	int population_size;

public:

	void mainMenu();
	void preMenu();
	void printPath(vector<int> path);
private:
	void error_exit(const string& msg);
	int header_get_size(ifstream& file);
	int value_digits(int val);
	int convert();
	void test();
	void test2();
	void saveParameters();
	void resolveGeneticAlgorithm(Matrix matrix);
	void printParameters();
};