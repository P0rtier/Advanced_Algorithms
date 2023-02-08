#include<iostream>
#include<stdio.h>
#include<string>
#include<chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Menu.h"
#include "Matrix.h"
#include "Wyzarzanie.h"
#include "Result.h"
#include "TabuSearch.h"

#define     FILE_IN         "ftv170.atsp"         // Nazwa pliku z danymi do przetworzenia
#define     FILE_OUT        "tsp_"              // Prefiks nazwy pliku z danymi przetworzonymi
// Do prefiksu dodawana jest liczba miast
#define     HEADER_LINES    7

typedef     vector<int>     c_v_i;              // vector of int
typedef     vector<c_v_i>   c_v_v_i;            // vector of vector of int


using namespace std;

void Menu::preMenu() {
	char enter;
	matrix = Matrix();
	int matrix_inserted = 0;
	do {
		chosen_time_max = 0.0;
		chosen_neighbourhood_type = -1;
		system("CLS");
		cout << endl;
		cout << "================= Menu =================\n";
		cout << "1. Menu algorytmow\n";
		cout << "2. Wczytaj plik .txt z danymi\n";
		cout << "3. Generuj dane losowo\n";
		cout << "4. Test\n";
		cout << "5. Wgraj plik TSPLIB\n";
		cout << "0. Wyjdz z programu\n";
		if (matrix_inserted == 0)
			cout << "\n!Wgrano macierz sasiedztwa: NIE!\n";
		else {
			cout << "\n!Wgrano macierz sasiedztwa: TAK!\n";
			cout << "!Liczba wierczholkow grafu: " << matrix.getNumber_of_vertices()<<endl;
		}
		cout << "========================================\n";
		cout << "Wybierz opcje: ";
		cin >> choice;
		switch (choice) {
		case '1':
			if (matrix_inserted == 0) {
				cout << "\n!!! Nalezy najpierw wgrac / wygenerowac macierz sasiedztwa !!!";
				cout << "\nEnter: ";
				cin >> enter;
			}
			else {
				mainMenu();
			}
			break;
		case '2':
			cout << "\nPodaj sciezke docelowa pliku z danymi:";
			cin >> filepath;
			matrix.createMatrixFromTxtFile(filepath);
			matrix.printMatrix();
			cout << "\nEnter: ";
			cin >> enter;
			matrix_inserted = 1;
			break;
		case '3':
			cout << "\nPodaj ilosc miast: ";
			cin >> number_of_vertices;
			matrix.createRandomMatrixFromNumberOfVertices(stoi(number_of_vertices));
			cout << endl;
			matrix.printMatrix();
			cout << "\nEnter: ";
			cin >> enter;
			matrix_inserted = 1;
			break;
		case '4':
			test();
			break;
		case '5':
			convert();
			break;
		case '0':
			exit(0);
		default:
			cout << "\nWybierz wlasciwa opcje!\nNacisnij enter!";
			cin.ignore();
			continue;
		}
	} while (choice != '0');
}

void Menu::mainMenu() {
	char enter;
	while (true) {
		system("CLS");
		cout << "================= Menu algorytmow =================\n";
		cout << "1. Algorytm Symulowanego Wyzarzania\n";
		cout << "2. Tabu search\n";
		cout << "3. Wpisz paramtery algorytmow\n";
		cout << "4. Powrot\n";
		cout << "0. Wyjdz z programu\n";
		if (chosen_neighbourhood_type != -1) {
			cout << "\nZapisane parametry symulacji:\nCzas trwania algorytmu: " << chosen_time_max << " [s]\nWybrane sasiedztwo: ";
			if (chosen_neighbourhood_type == 1)
				cout << "Swap\n";
			if (chosen_neighbourhood_type == 2)
				cout << "Invert\n";
			if (chosen_neighbourhood_type == 3)
				cout << "Insert\n";
		}
		else {
			cout << "\nBrak zapisanych parametrow symulacji!\n";
		}
		cout << "===================================================\n";
		cout << "Wybierz opcje: ";
		cin >> choice;

		switch (choice) {
		case '1':
			resolveAnnealing(matrix, 0.999999);
			break;
		case '2':
			resolveTabu(matrix);
			break;
		case '3':
			saveParameters();
			break;
		case '4':
			return;
		case '0':
			exit(0);
		}
	}
}

void Menu::resolveAnnealing(Matrix matrix, double cooler) {
	Result wynik;
	Wyzarzanie data;
	char enter;
	int choice=2;
	while (chosen_time_max == 0.0 || chosen_neighbourhood_type == -1) {
		saveParameters();
	}
	cout << "\nCzy wylaczyc opcje zatrzymania algorytmu przy osiagnieciu temperatury minimalnej? (Domyslnie => NIE):\n1.Tak\n2.Nie\nWybor: ";
	try {
		cin >> choice;
	}
	catch (exception& err) {
		choice = 2;
	}if (choice != 1 && choice != 2) {
		choice = 2;
	}
	system("CLS");
	cout << "\n===========Traveling Salesman Algorithm Symulowane Wyzarzanie============\n";
	cout << "Macierz wejsciowa: \n";
	matrix.printMatrix();
	cout << "Obliczanie w toku!\n";
	auto timerStart = chrono::steady_clock::now();
	wynik = data.sa_algorithm(matrix, cooler, chosen_time_max, chosen_neighbourhood_type, choice);
	auto timerStop = chrono::steady_clock::now();
	cout << "\nWyniki:\n";
	if(chosen_neighbourhood_type == 1)
		cout << "Sasiedztwo: Swap\n";
	if(chosen_neighbourhood_type == 2)
		cout << "Sasiedztwo: Invert\n";
	if (chosen_neighbourhood_type == 3)
		cout << "Sasiedztwo: Insert\n";
	cout << "Koszt: " << wynik.final_result;
	printPath(wynik.final_path);
	chrono::duration<double> total_time = (timerStop - timerStart) * pow(10, 3);
	cout << "\nCzas algorytmu: " << total_time.count() << " [ms]";
	cout << "\n===========================================================================";
	cout << "\nEnter: ";
	cin >> enter;
}

void Menu::resolveTabu(Matrix matrix) {
	Result wynik;
	TabuSearch data;
	char enter;
	while (chosen_time_max == 0.0 || chosen_neighbourhood_type == -1) {
		saveParameters();
	}
	system("CLS");
	cout << "\n===========Traveling Salesman Algorithm Metoda Tabu Search============\n";
	cout << "Macierz wejsciowa: \n";
	matrix.printMatrix();
	cout << "Obliczanie w toku!\n";
	auto timerStart = chrono::steady_clock::now();
	wynik = data.tabu_search_alg(matrix, chosen_time_max, chosen_neighbourhood_type);
	auto timerStop = chrono::steady_clock::now();
	cout << "\nWyniki:\n";
		if(chosen_neighbourhood_type == 1)
		cout << "Sasiedztwo: Swap\n";
	if(chosen_neighbourhood_type == 2)
		cout << "Sasiedztwo: Invert\n";
	if (chosen_neighbourhood_type == 3)
		cout << "Sasiedztwo: Insert\n";
	cout << "Koszt: " << wynik.final_result;
	printPath(wynik.final_path);
	chrono::duration<double> total_time = (timerStop - timerStart) * pow(10, 3);
	cout << "\nCzas algorytmu: " << total_time.count() << " [ms]";
	cout << "\n===========================================================================";
	cout << "\nEnter: ";
	cin >> enter;
}

void Menu::printPath(vector<int> path) {

	int id_0 = 0;
	for (int i = 0; i < path.size(); i++) {
		if (path[i] == 0)
			id_0 = i;
	}

	for (int i = 0; i < id_0; i++) {
		rotate(path.begin(), path.begin() + 1, path.end());
	}

	cout << "\nSciezka: ";
	for (int i = 0; i < path.size(); i++) {
		cout << path[i] << " ==> " ;
	}
	cout << "0";
}

void Menu::test() {
	Wyzarzanie data;
	TabuSearch data2;
	Matrix matrix;
	char enter;
	double wyniki = 0;
	string nazwa_pliku;
	Result wynik;
	system("CLS");
	cout << "\nPodaj plik: ";
	cin >> nazwa_pliku;
	matrix.createMatrixFromTxtFile(nazwa_pliku);


	cout << "\nTabuSearch";
	cout << "\nSwap";
	wynik = data2.tabu_search_alg(matrix, 60, 1);
	cout << "\n\nInvert";
	wynik = data2.tabu_search_alg(matrix, 60, 2);
	cout << "\n\nInsert";
	wynik = data2.tabu_search_alg(matrix, 60, 3);

	cout << "\n\nKontynuowac do Wyzarzania?\nEnter:";
	cin >> enter;

	cout << "\nStart:\nSimulatedAnnealing";
	cout << "\nSwap";
	wynik = data.sa_algorithm(matrix, 0.999999, 60, 1, 2);
	cout << "\n\nInvert";
	wynik = data.sa_algorithm(matrix, 0.999999, 60, 2, 2);
	cout << "\n\nInsert";
	wynik = data.sa_algorithm(matrix, 0.999999, 60, 3, 2);

	cout << "\n\nZakonczyc test??\nEnter:";
	cin >> enter;

}

void Menu::saveParameters() {
	char enter;
	double time_to_execute = 0.0;
	int rodzaj_sasiedztwa = -1;
	system("CLS");
	cout << "\n===========Menu wyboru parametrow rozruchu algorytmow============\n";
	cout << "\nPodaj warunek koncowy (czas w sekundach): ";
	try {
		cin >> time_to_execute;
	}
	catch (exception& err) {
		cout << "Podano bledny czas!";
		cin >> enter;
		return;
	}
	if (time_to_execute <= 0.0) {
		cout << "Podano bledny czas!";
		cin >> enter;
		return;
	}
	cout << "\nWybierz rodzaj sasiedztwa:\n1.Sasiedztwo Swap\n2.Sasiedztwo Invert\n3.Sasiedztwo Insert\nWybor: ";
	try {
		cin >> rodzaj_sasiedztwa;
	}
	catch (exception& err) {
		cout << "\nWybierz poprawny rodzaj sasiedztwa!\nEnter: ";
		cin >> enter;
		return;
	}
	if (rodzaj_sasiedztwa <= 0 || rodzaj_sasiedztwa > 3) {
		cout << "\nWybierz poprawny rodzaj sasiedztwa!\nEnter: ";
		cin >> enter;
		return;
	}
	chosen_time_max = time_to_execute;
	chosen_neighbourhood_type = rodzaj_sasiedztwa;
}

//---------------------------------------------------------------------
// Wyjscie w przypadku bledu z komunikatem msg
//---------------------------------------------------------------------
void Menu::error_exit(const string& msg)
{
	cout << msg << endl;
	system("PAUSE");
	exit(1);
}

//---------------------------------------------------------------------
// Odczyt liczby miast z naglowka pliku TSPLIB
// Odczytywane sa wszystkie linie naglowka 
//---------------------------------------------------------------------
int Menu::header_get_size(ifstream& file)
{
	int     header_lines = 0;
	int     size = 0;
	string  line;

	for (;;)
	{
		getline(file, line);
		if (file.fail() || line.empty())
			error_exit("File READ error - HEADER");

		if (line.find("DIMENSION") != string::npos)
		{
			istringstream in_ss(line);
			string tmp;
			in_ss >> tmp;
			in_ss >> size;
		}
		else
			if (line.find("EDGE_WEIGHT_SECTION") != string::npos)
				if (size <= 0)
					error_exit("File READ error - SIZE");
				else
					return(size);

		if (++header_lines >= 7)
			error_exit("File READ error - HEADER");
	}
}

//---------------------------------------------------------------------
// Okreslenie liczby cyfr liczby nieujemnej
// Dla wartosci 0 jedna cyfra
//---------------------------------------------------------------------
int Menu::value_digits(int val)
{
	int width = 0;

	if (val == 0)
		return(1);

	while (val)
	{
		val /= 10;
		width++;
	}

	return(width);
}

//---------------------------------------------------------------------
int Menu::convert()
{
	ifstream    file_in;
	ofstream    file_out;
	int         size;
	int         width;
	int         diagonal;
	int         dist, dist_max;
	c_v_v_i     city_distance;

	file_in.open(FILE_IN);
	if (!file_in.is_open())
		error_exit("File OPEN error - FILE_IN");

	size = header_get_size(file_in);

	city_distance.resize(size);
	for (int i = 0; i < size; i++)
		city_distance[i].resize(size);

	dist_max = 0;
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			file_in >> dist;
			if (file_in.fail())
				error_exit("File READ error - DATA");

			if (i == j)
			{
				if (i == 0)
					diagonal = dist;
				else
					if (dist != diagonal)
						cout << "WARNING: Diagonal data inconsistent (" << i << "," << j << ")" << endl << endl;
				dist = 0;
			}

			city_distance[i][j] = dist;

			if (dist > dist_max)
				dist_max = dist;
		}

	file_in.close();

	file_out.open(FILE_OUT + to_string(size) + ".txt");
	if (!file_out.is_open())
		error_exit("File OPEN error - FILE_OUT");

	width = value_digits(dist_max);

	file_out << size << endl;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
			if (j == 0)
				file_out << setw(width) << city_distance[i][j];
			else
				file_out << setw(width + 1) << city_distance[i][j];

		file_out << endl;
	}

	file_out.close();

	cout << "Size = " << size << endl;
	cout << "Conversion OK" << endl << endl;

	system("PAUSE");
	return(0);
}