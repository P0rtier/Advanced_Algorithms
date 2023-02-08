#pragma once
#include<string>
#include<iostream>
#include<vector>
#include<deque>
#include "Matrix.h"
using namespace std;

class TSP_DP {
	int final_condition;
	Matrix matrix;
	vector<vector<int>> dynamic_search;
	vector<int> route;
public:
	double calculateTSP_Dynammic_Programming(Matrix matrix);
private:
	int iterate_tsp(int mask, int position, const vector<vector<int>>& grid, int number_of_vertices);
	void preStart(int nV);
	void printRoute(int nV, const vector<vector<int>>& tspMatrix);
};
