#include<iostream>
#include<stdio.h>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<random>

#include "Matrix.h"
using namespace std;

void Matrix::createMatrixFromTxtFile(string filePath) {
	matrix.clear();
	ifstream matrix_file(filePath);

	matrix_file.exceptions(ifstream::failbit | ifstream::badbit);

	string line;

	getline(matrix_file, line);
	number_of_vertices = stoi(line);

	int temp = 0;
	int read_int;
	while (getline(matrix_file, line)) {
		istringstream string_stream(line);
		vector<int> tempv;
		while (string_stream >> read_int) {
			tempv.push_back(read_int);
		}
		matrix.push_back(tempv);
		temp += 1;
		if (temp == number_of_vertices)
			break;
	}
}

void Matrix::createRandomMatrixFromNumberOfVertices(int number) {
	matrix.clear();
	number_of_vertices = number;
	vector<int> temp_row;

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, 100);

	for (int i = 0; i < number; i++) {
		for (int j = 0; j < number; j++)
			if (i == j)
				temp_row.push_back(0);
			else
				temp_row.push_back(distrib(gen));
		matrix.push_back(temp_row);
		temp_row.clear();
	}
};

void Matrix::printMatrix() {
	cout << "Liczba miast: " << number_of_vertices << endl;
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
}

vector<vector<int>> Matrix::getMatrix() {
	return this->matrix;
}

int Matrix::getNumber_of_vertices() {
	return this->number_of_vertices;
}