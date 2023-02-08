#pragma once
#include<iostream>
#include<stdio.h>
#include<vector>
#include<string>

using namespace std;

class Matrix {
	vector<vector<int>> matrix;
	int number_of_vertices;
public:
	void createMatrixFromTxtFile(string filePath);

	void printMatrix();
	void createRandomMatrixFromNumberOfVertices(int number);
	vector<vector<int>> getMatrix();
	int getNumber_of_vertices();
};