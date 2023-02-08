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
	try {
		number_of_vertices = stoi(line);
	}
	catch (exception& err) {
		cout << "\n!Linia przechowujaca ilosc miast jest zle zadeklarowana!\n";
		exit(0);
	}


	int temp = 0;
	int read_int;
	try{
		while (getline(matrix_file, line)) {
			istringstream string_stream(line);
			vector<int> tempv;
			while (string_stream >> read_int) {
				tempv.push_back(read_int);
			}
			if (tempv.size() < number_of_vertices) {
				cout << "\n!Wiersz macierzy ma za malo elementow!\n";
				exit(0);
			}
			matrix.push_back(tempv);
			temp += 1;
			if (temp == number_of_vertices)
				break;
		}
	}
	catch (exception& err) {
		cout << "\n!Definicja macierzy sasiedztwa w pliku jest bledna!";
		exit(0);
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
				temp_row.push_back(9999);
			else
				temp_row.push_back(distrib(gen));
		matrix.push_back(temp_row);
		temp_row.clear();
	}
};

void Matrix::printMatrix() {
	if (number_of_vertices > 30) {
		cout << "Macierz za duza do wypisania na konsole!\n";
	}
	else {
		cout << "Liczba miast: " << number_of_vertices << endl;
		for (int i = 0; i < matrix.size(); i++) {
			for (int j = 0; j < matrix[i].size(); j++) {
				cout << matrix[i][j] << " ";
			}
			cout << endl;
		}
	}
}

vector<vector<int>> Matrix::getMatrix() {
	return this->matrix;
}

int Matrix::getNumber_of_vertices() {
	return this->number_of_vertices;
}