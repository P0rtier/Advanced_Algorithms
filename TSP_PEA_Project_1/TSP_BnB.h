#pragma once
#include<string>
#include<iostream>
#include<vector>
#include "Matrix.h"
#include "Node.h"
using namespace std;

class TSP_BnB {
	Node node;
	int number_of_vertices;
public:
	double calculateTSP_Brounch_And_Bound(Matrix matrix);
private:
	Node* generate_new_node(const vector<vector<int>> &parentReducedMatrix,const vector<int> &routeToPrint,
		int depth, int start, int destination);
	vector<int> reduceRow(vector<vector<int>>& matrixReduction);
	vector<int> reduceColumn(vector<vector<int>>& matrixReduction);
	int calculateNodeLowerBound(vector<vector<int>>& matrixReduction);
	void RoutePrintMethod(const vector<int>& routeToPrint);
};