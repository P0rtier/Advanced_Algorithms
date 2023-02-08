#include<iostream>
#include<stdio.h>
#include<string>

#include "Menu.h"
#include "Matrix.h"
#include "TSP_DP.h"
#include "TSP_BF.h"
#include "TSP_BnB.h"

using namespace std;

void Menu::preMenu() {
	char enter;
	matrix = Matrix();
	do {
		system("CLS");
		cout << endl;
		cout << "================= Menu =================\n";
		cout << "1. Wczytaj plik .txt z danymi\n";
		cout << "2. Generuj dane losowo\n";
		cout << "3. Test\n";
		cout << "0. Wyjdz z programu\n";
		cout << "========================================\n";
		cout << "Wybierz opcje: ";
		cin >> choice;
		switch (choice) {
			case '1':
				cout << "\nPodaj sciezke docelowa pliku z danymi:";
				cin >> filepath;
				matrix.createMatrixFromTxtFile(filepath);
				matrix.printMatrix();
				cout << "\nEnter: ";
				cin >> enter;
				mainMenu();
				break;
			case '2':
				cout << "\nPodaj ilosc miast: ";
				cin >> number_of_vertices;
				matrix.createRandomMatrixFromNumberOfVertices(stoi(number_of_vertices));
				cout << endl;
				matrix.printMatrix();
				cout << "\nEnter: ";
				cin >> enter;
				mainMenu();
				break;
			case '3':
				test();
				break;
			case '0':
				exit(0);
			default:
				cout << "\nWybierz wlasciwa opcje!\nNacisnij enter!";
				cin.ignore();
				continue;
		}
	} while (choice!='0');
}

void Menu::mainMenu() {
	char enter;
	TSP_DP data;
	TSP_BnB data2;
	TSP_BF data3;
	while(true){
		system("CLS");
		cout << "================= Menu algorytmow =================\n";
		cout << "1. Programowanie Dynamiczne\n";
		cout << "2. Metoda Podzialu i Ograniczen\n";
		cout << "3. Przeglad zupelny\n";
		cout << "4. Powrot\n";
		cout << "5. Wyjdz z programu\n";
		cout << "Wybierz opcje: ";
		cin >> choice;
	
		switch (choice) {
		case '1':
			data = TSP_DP();
			system("CLS");
			cout << "\n===========Traveling Salesman Algorithm Programowanie Dynamiczne============\n";
			cout << "Macierz wejsciowa: \n";
			matrix.printMatrix();
			cout << "\nWyniki:\n";
			data.calculateTSP_Dynammic_Programming(matrix);
			cout << "\n===========================================================================";
			cout << "\nEnter: ";
			cin >> enter;
			break;
		case '2':
			data2 = TSP_BnB();
			system("CLS");
			cout << "\n===========Traveling Salesman Algorithm Metoda Podzialu i Ograniczen============\n";
			cout << "Macierz Wejsciowa: \n";
			matrix.printMatrix();
			cout << "\nWyniki:\n";
			data2.calculateTSP_Brounch_And_Bound(matrix);
			cout << "\n===========================================================================";
			cout << "\nEnter: ";
			cin >> enter;
			break;
		case '3':
			data3 = TSP_BF();
			system("CLS");
			cout << "\n===========Traveling Salesman Algorithm Przeglad Zupelny============\n";
			cout << "Macierz wejsciowa: \n";
			matrix.printMatrix();
			cout << "\nWyniki:\n";
			data3.calculateTSP_Brute_Force(matrix);
			cout << "\n======================================================================";
			cout << "\nEnter: ";
			cin >> enter;
			break;
		case '4':
			return;
		case '5':
			exit(0);
		}
	}
}

void Menu::test() {
	Matrix matrix = Matrix();
	TSP_BF brute_force = TSP_BF();
	TSP_DP dynamic_programming;
	TSP_BnB brunch_and_bound = TSP_BnB();
	double total_seconds_dp = 0.0;
	double total_seconds_bnb = 0.0;
	double total_seconds_bf = 0.0;
	system("CLS");
	int x;
	int y;
	cout << "\nWprowadz liczbe wierzcholkow (DP, BnB): ";
	cin >> x;
	cout << "\nWprowadz liczbe wierzcholkow (Brute Force): ";
	cin >> y;
	system("CLS");
	cout << "\n =============Programowanie dynamiczne, 100 prob: ==================\n";
	for(int i = 0; i < 100; i++)
	{
		dynamic_programming = TSP_DP();
		matrix.createRandomMatrixFromNumberOfVertices(x);
		double add = dynamic_programming.calculateTSP_Dynammic_Programming(matrix);
		cout << "\nProba " << i << ": " << add << " [s]";
		total_seconds_dp += add;
	}

	cout << "\n============ Brounch and Bound, 100 prob: ============";
	for (int i = 0; i < 100; i++)
	{
		matrix.createRandomMatrixFromNumberOfVertices(x);
		double add = brunch_and_bound.calculateTSP_Brounch_And_Bound(matrix);
		cout << "\nProba " << i << ": " << add << " [s]";
		total_seconds_bnb += add;
	}

	cout << "\n============ Brute force, 100 prob: ============";
	for (int i = 0; i < 100; i++)
	{
		matrix.createRandomMatrixFromNumberOfVertices(y);
		double add = brute_force.calculateTSP_Brute_Force(matrix);
		cout << "\nProba " << i << ": " << add << " [s]";
		total_seconds_bf += add;
	}
	cout << "\n\n=================Usrednione czasy eksperymentu: ====================\n";
	cout << "Programowanie dynamiczne "<<"(" << x <<" wierzcholkow)" << " : " << total_seconds_dp / 100.0 << " [ms]";
	cout << "\nBrounch and Bound " << "(" << x << " wierzcholkow)" << " : " << total_seconds_bnb / 100.0 << " [ms]";
	cout << "\nBrute Force " << "(" << y << " wierzcholkow)" << " : " << total_seconds_bf / 100.0 << " [ms]";
	cout << "\n====================================================================";
	char s;
	cout << "\n\nEnter: ";
	cin >> s;
}