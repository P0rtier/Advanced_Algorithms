#pragma once
#include<string>
#include<iostream>
#include<vector>
#include "Matrix.h"
using namespace std;

class TSP_BF {
	vector<int> route;
	vector<vector<int>> grid;
	int nV;
public:
	double calculateTSP_Brute_Force(Matrix matrix);
private:
	int cont_BF(int* permutation);
	int factorial(int n);
};