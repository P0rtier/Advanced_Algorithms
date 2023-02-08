#pragma once
#include<iostream>
#include<stdio.h>
#include<vector>
#include<string>

using namespace std;

class Node {
public:
	//vector<pair<int, int>> routeToPrint;
	vector<int> routeToPrint;
	vector<vector<int>> matrixReduction;
	int node_result;
	int vertex;
	int depth;
	int number_of_vertices;
};

//class Node {
//public:
//	vector<pair<int, int>> path;
//	vector<vector<int>> reducedMatrix;
//	int cost;
//	int vertex;
//	int level;
//	int number_of_vertices;
//	int getCost();
//};