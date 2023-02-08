#include<stdio.h>
#include<algorithm>
#include<string>
#include<chrono>
#include "TSP_DP.h"
using namespace std;
#define INT_MAX 9999;



double TSP_DP::calculateTSP_Dynammic_Programming(Matrix matrix) {
	final_condition = (1 << matrix.getNumber_of_vertices()) - 1;
	preStart(matrix.getNumber_of_vertices());
	int nV = matrix.getNumber_of_vertices(); // Liczba wierzcholkow danego grafu
	vector<vector<int>> tspMatrix = matrix.getMatrix(); // tspMatrix przechowuje zadan¹ macierz odleg³oœci

	auto start = chrono::steady_clock::now();
	int wynik = iterate_tsp(1, 0, tspMatrix, nV); // Maska pocz¹tkowa 0001 -> bo wierzcholek startowy jest ju¿ rozpatrzony, pozycja 0 -> konkretny numer wierzcholka aktualnie rozpatrywanego ( start ).
	auto end = chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = (end - start) * pow(10,3);

	cout << "Wynik TSP_Dynammic_programming to: " << wynik;
	printRoute(nV, tspMatrix);
	cout << "\nCzas algorytmu: " << elapsed_seconds.count() << " [ms]";
	return elapsed_seconds.count();
}

void TSP_DP::preStart(int nV) {
	for (int i = 0; i < (1 << nV); i++) {
		vector<int> temp_row;
		for (int j = 0; j < nV; j++) {
			temp_row.push_back(-1);
		}
		dynamic_search.push_back(temp_row); // W celu sprowadzenia zlolonoœci czasowej problemu do ( (2^n) * n^2 ), wypelniamy pomocnicza macierz dynamic_search wartoœciami pocz¹tkowymi -1, które docelowo
											// bedziemy wypelniac obliczonymi wartosciami sciezek w celu eliminacji nadmiernych obliczeñ jednakowych pod-problemow. Dodatkowo posluzy sie ona do wyswietlenia wlasciwej drogi
	}										// problemu poprzez przechodzenie po najlepszych wynikach i zapisywanie ich numeru indeksu ( jednoznacznego z numerem danego wierzcholka ).
}

void TSP_DP::printRoute(int nV, const vector<vector<int>>& tspMatrix) { // Sposob wyswietlania sciezki opiera sie na przegladaniu najlepszych rozwiazan macierzy pomocniczej dynamic_search
																		// z uwzglednieniem poprzedajacych sciezek z wlasciwej macierzy problemu tspMatrix
	int mask = 1;
	int temp_mask = 1;
	int previous_pos = 0;
	int previous_sum = 0;
	for (int i = 0; i < nV; i++) {
		int max = 9999;
		int pos = 0;
		int temp_previous_sum = 0;
		for (int j = 0; j < nV; j++) {
			if ((mask & (1 << j)) == 0) {
				int temp = dynamic_search[mask | (1 << j)][j];
				temp += tspMatrix[previous_pos][j] + previous_sum;
				if (temp < max){
					temp_mask = mask | (1 << j);
					max = temp;
					pos = j;
					temp_previous_sum = 0;
					temp_previous_sum += tspMatrix[previous_pos][j];
				}
			}
		}
		route.push_back(pos);
		previous_pos = pos;
		mask = temp_mask;
		previous_sum += temp_previous_sum;
	}

	cout << "\nSciezka: 0";
	for (int i = 0; i < route.size(); i++)
	{
		cout << " ==> " <<route[i];
	}
}

int TSP_DP::iterate_tsp(int mask, int pos, const vector<vector<int>>& grid, int nV) {

	if (mask == final_condition) { // Jezeli w rekurencji doszlismy do otrzymania maksymalnej maski danego problemu : 2^(n) - 1, dodajemy do wyniku odleglosc aktualnej pozycji (wierzcholka) do wierzcholka poczatkowego 
		return grid[pos][0];
	}

	if (dynamic_search[mask][pos] != -1) {
		return dynamic_search[mask][pos];// Jezeli macierz dynamic_search o indeksach [mask][pos] ma wartoœæ rozna od poczatkowej, oznacza to, ¿e rozwiazalismy ju¿ problem zmierzenia odleglosci dla zadanych wierzcholkow
										  // i mo¿emy zwrocic wartosc zapisana w macierzy pomocniczej dynamic_search;
	}
	int final_answer = 9999;
	for (int i = 0; i < nV; i++) {

		if ( (mask & (1 << i)) == 0) { // Sprawdzamy czy dany wierzcho³ek zostal juz sprawdzony, poprzez iloczyn logiczny aktualnej maski z maska o przesunieciu bitowym odpowiadajacym obecnie rozpatrywanemu wierzcholkowi
									   // np. Maska poczatkowa dla nV = 4 : 0001, pierwszy wierzcholek nie jest rozpatrywany ponownie, poniewaz 0001 & (1 << 0) => 0001 & 0001 != 0. Podobne porownania s¹ rozpatrywane dla kolejnych wierzcholkow.

			int temp_answer = grid[pos][i] + iterate_tsp( (mask | (1 << i)), i, grid, nV); // wartosc TSP dla problemu Dynamicznego programowania rozwiazywana jest poprzez implementacje TOP-DOWN DP, dla ka¿dej rekurencji aktualnie
																					  // rozwazana wartosc bedzie odlegloscia aktualnej pozycji do kolejnych wierzcholkow, tspMatrix[pos][i] oraz calkowitego wyniku TSP dla kolejnych rozgalezien
																					  // wierzcholka i. Przy kazdym kolejnym wywolaniu funkcji modyfikowana jest maska, ktora informuje algorytm, ktore wierzcho³ki zostaly ju¿ sprawdzone.
																					  // np. przy rozwayaniu wierzcholka 2, masce poczatkowej np. 0001 -> (0001 | 0100) => (0101) co informuje nas o kolejnych przebytych wierzcholkach.

			final_answer = min(final_answer, temp_answer); // Porownujemy aktualnie najlepsza wartosc z otrzymanym wynikiem.
		}
	}

	dynamic_search[mask][pos] = final_answer; // Aktualizujemy macierz dynamic_search[][] w celu eliminacji powtorzen obliczania tych samych sciezek.
	return final_answer; // Zwracamy aktualnie najlepsza wartosc TSP.
}
