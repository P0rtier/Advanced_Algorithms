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
#include "Result.h"
#include "GeneticAlgorithm.h"

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
	parameters_have_been_chosen = false;
	do {
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
			test2();
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
		cout << "1. Uruchom Algorytm Genetyczny\n";
		cout << "2. Wpisz paramtery algorytmu\n";
		cout << "3. Powrot\n";
		cout << "0. Wyjdz z programu\n";
		if (!parameters_have_been_chosen) {
			cout << "\nBrak zapisanych parametrow symulacji!\n";
		}
		else {
			cout << "\nParametry symulacji:\n";
			printParameters();
		}
		cout << "===================================================\n";
		cout << "Wybierz opcje: ";
		cin >> choice;

		switch (choice) {
		case '1':
			resolveGeneticAlgorithm(matrix);
			break;
		case '2':
			saveParameters();
			break;
		case '3':
			return;
		case '0':
			exit(0);
		}
	}
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

	Result wynik;
	GeneticAlgorithm data;
	int result = 0;
	char enter;

	 //br17-pop10-swap
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 10, 1, 0.01, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-pop10-swap: " << result / 5;
	result = 0;
	// br17-pop50-swap
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 1, 0.01, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-pop50-swap: " << result / 5;
	result = 0;
	// br17-pop100-swap
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 100, 1, 0.01, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-pop100-swap: " << result / 5;
	result = 0;

	//br17-pop10-insert
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 10, 2, 0.01, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-pop10-insert: " << result / 5;
	result = 0;
	// br17-pop50-insert
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 2, 0.01, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-pop50-insert: " << result / 5;
	result = 0;
	// br17-pop100-insert
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 100, 2, 0.01, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-pop100-insert: " << result / 5;
	result = 0;

	cout << "\nEnter: ";
	cin >> enter;

}

void Menu::test2() {

	Result wynik;
	GeneticAlgorithm data;
	int result = 0;
	char enter;

	//br17-mut0,02-swap
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 1, 0.02, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-0.02-swap: " << result / 5;
	result = 0;
	// br17-pop50-swap
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 1, 0.05, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-0.05-swap: " << result / 5;
	result = 0;
	// br17-pop100-swap
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 1, 0.1, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-0.1-swap: " << result / 5;
	result = 0;

	//br17-pop10-insert
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 2, 0.02, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-0.02-insert: " << result / 5;
	result = 0;
	// br17-pop50-insert
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 2, 0.05, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-0.05-insert: " << result / 5;
	result = 0;
	// br17-pop100-insert
	for (int i = 0; i < 5; i++) {
		wynik = data.runGA(matrix, 50, 2, 0.1, 0.8, 60);
		cout << "\nWynik[" << i << "]: " << wynik.final_result << endl;
		result += wynik.final_result;
	}
	cout << "\nUsrednione z tsp171-0.1-insert: " << result / 5;
	result = 0;

	cout << "\nEnter: ";
	cin >> enter;

}

void Menu::saveParameters() {
	char enter;
	double time_to_execute = 0.0;
	int rodzaj_mutacji = -1;
	int wielkosc_populacji = -1;
	double mut_coeff = 0.01;
	double crosso_coeff = 0.8;
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
	cout << "\nWybierz rodzaj mutacji (Dla lepszych wynikow zalecana mutacja Insert):\n1.Mutacja Swap\n2.Mutacja Insert\nWybor: ";
	try {
		cin >> rodzaj_mutacji;
	}
	catch (exception& err) {
		cout << "\nWybierz poprawny rodzaj mutacji!\nEnter: ";
		cin >> enter;
		return;
	}
	if (rodzaj_mutacji <= 0 || rodzaj_mutacji > 2) {
		cout << "\nWybierz poprawny rodzaj mutacji!\nEnter: ";
		cin >> enter;
		return;
	}

	cout << "\nPodaj rozmiar populacji (domyslnie 100): ";
	try {
		cin >> wielkosc_populacji;
	}
	catch (exception& err) {
		cout << "Podano bledny rozmiar!";
		population_size = 100;
		cin >> enter;
		return;
	}
	if (wielkosc_populacji <= 0) {
		cout << "Podano bledny rozmiar!";
		population_size = 100;
		cin >> enter;
		return;
	}

	cout << "\nPodaj wspolczynnik kryzowania (domyslnie 0.8), musi sie miescic w przedziale <0.0, 1.0>: ";
	try {
		cin >> crosso_coeff;
	}
	catch (exception& err) {
		cout << "Podano bledna wartosc!";
		crosso_coeff = 0.8;
		cin >> enter;
		return;
	}
	if (crosso_coeff < 0.0 || crosso_coeff > 1.0) {
		cout << "Podano bledna wartosc!";
		crosso_coeff = 0.8;
		cin >> enter;
		return;
	}
	cout << "\nPodaj wspolczynnik mutacji (domyslnie 0.01), musi sie miescic w przedziale <0.0, 1.0>: ";
	try {
		cin >> mut_coeff;
	}
	catch (exception& err) {
		cout << "Podano bledna wartosc!";
		mut_coeff = 0.01;
		cin >> enter;
		return;
	}
	if (mut_coeff < 0.0 || mut_coeff > 1.0) {
		cout << "Podano bledna wartosc!";
		mut_coeff = 0.01;
		cin >> enter;
		return;
	}

	chosen_time_max = time_to_execute;
	mutation_type = rodzaj_mutacji;
	population_size = wielkosc_populacji;
	this->cross_coeff = crosso_coeff;
	mutation_coeff = mut_coeff;

	parameters_have_been_chosen = true;
}

void Menu::resolveGeneticAlgorithm(Matrix matrix) {
	Result wynik;
	GeneticAlgorithm data;
	char enter;
	int choice = 2;
	while (!parameters_have_been_chosen) {
		saveParameters();
	}
	system("CLS");
	cout << "\n===========Traveling Salesman Algorithm Algorytm Genetyczny============\n";
	cout << "Macierz wejsciowa: \n";
	matrix.printMatrix();
	cout << "Obliczanie w toku!\n";
	auto timerStart = chrono::steady_clock::now();
	wynik = data.runGA(matrix, population_size, mutation_type, mutation_coeff, cross_coeff, chosen_time_max);
	auto timerStop = chrono::steady_clock::now();
	cout << "\nWyniki:\n";
	printParameters();
	cout << "Koszt: " << wynik.final_result;
	printPath(wynik.final_path);
	chrono::duration<double> total_time = (timerStop - timerStart) * pow(10, 3);
	cout << "\nCzas algorytmu: " << total_time.count() << " [ms]";
	cout << "\n===========================================================================";
	cout << "\nEnter: ";
	cin >> enter;

}

void Menu::printParameters() {
	string mutation_type_string = mutation_type == 1 ? "Mutacja Swap" : "Mutacja Insert";
	cout << "Kryterium stopu: " << chosen_time_max << " [s]\nTyp mutacji: " << mutation_type_string << "\nRozmiar populacji: " << population_size << "\nWspolczynnik krzyzowania: " << cross_coeff << "\nWspolczynnik mutacji: " << mutation_coeff << "\n";
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