#include<string>
#include<iostream>
#include<algorithm>
#include<vector>
#include<chrono>

#include "Matrix.h"
#include "TSP_BF.h"

#define INT_MAX 9999;
using namespace std;

int TSP_BF::cont_BF(int* permutation) {
	int temp_result = 0;

	// Dla ka¿dej permutacji liczona jest odleglosc nastepujacych po sobie wierzcholkow tworzacych cykl Hamiltona.
	// W petli wystepuje manipulacja odleglosciami bazujaca na aktualnym stanie tablicy permutacji : int add = grid[permutation[i]][permutation[i + 1]],
	// Na koniec dodawany jest koszt powrotu z ostatniego wierzcholka tablicy permutacji : grid[permutation[nV-1]][ X ] -> wierzcholka startowego danej permutacji : grid[permutation[nV-1]][permutation[0]]
	for (int i = 0; i<nV - 1; i++) {
		int add = grid[permutation[i]][permutation[i + 1]];
		temp_result += add;
	}
	return temp_result + grid[permutation[nV - 1]][permutation[0]];
}

double TSP_BF::calculateTSP_Brute_Force(Matrix matrix) {
	char s;
	nV = matrix.getNumber_of_vertices(); //Liczba wierzcholkow grafu
	grid = matrix.getMatrix(); //grid przechowuje wlasciwa macierz odleglosci
	int final_answer = INT_MAX;
	auto permutation = make_unique<int[]>(nV); //tablica przechowujaca kolejne permutacje zbioru wierzcholkow [a,b,c,d], [a,b,d,c], [a,d,b,c] itp...
	auto start = chrono::steady_clock::now(); //Rozruch zegara

	for (int i = 0; i < nV; i++)
		permutation.get()[i] = i; //poczatkowe ustawienie wierzcholkow, a wiec i kolejnosci przegladan macierzy. Start:  [0, ... , nV-1 ]
	
	do {
		int temp_answer = cont_BF(permutation.get()); //Obliczenie nowej, potencjalnie najlepszej wartoœci TSP
		if (temp_answer < final_answer) { //je¿eli nowa wartoœæ jest lepsza od obecnie najbardziej optymalnej, ustawiamy nowa wartoœæ jako wartoœæ optymaln¹.
			final_answer = temp_answer;
			route.clear();
			for (int i = 0; i < nV; i++)
				route.push_back(permutation.get()[i]); //Œcie¿ka TSP jest równa kolejnym przejœciom do wierzcholkow zapisanych w aktualnie najlepszej permutacji, rozpoczynaj¹c od wierzcholka startowego.
		}

	} while (next_permutation(permutation.get()+1, permutation.get()+nV)); //Wywolanie nastepnej iteracji petli z zaktualizowana permutacja zbioru wierzcholkow.
																								   // Zoptymalizowanie algorytmu przez permutacje (n-1)!, nastepuja tylko permutacje [n-1] elementow, z wierzcholkem 0 na poczatku.

	auto end = chrono::steady_clock::now(); //Stop zegara

	chrono::duration<double> elapsed_seconds = (end - start) * pow(10,3); //Obliczenie calkowitego czasu algorytmu [s]

	cout << "Wynik TPS_Brute_Force to: " << final_answer;
	cout << endl<<"Sciezka: ";
	for (int i = 0; i < route.size(); i++)
		cout << route[i] << " ==> ";
	cout << "0";
	cout << "\nCzas algorytmu: " << elapsed_seconds.count() <<" s";
	return elapsed_seconds.count();
}