#pragma once
#include<iostream>
#include<stdio.h>
#include<vector>
#include<string>

#include "Matrix.h"
#include "Result.h"
using namespace std;

class Wyzarzanie {
	vector<vector<int>> matrix;
	int number_of_vertices;
	int chosen_neighbourhood;

public:
	Result sa_algorithm(Matrix matrix, double cooler, double time_max, int neighbourhood_type, int min_restriction_chosen);
private:
	int calculate_path(vector<int>& temp_path);
	vector<int> calcRandomPath();
	double calculateMaxTemperature(vector<int> randomPath);
	void getNeighbour(vector<int>& currentPath);
};