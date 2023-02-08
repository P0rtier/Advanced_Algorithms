#include<iostream>
#include<stdio.h>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<random>
#include<chrono>

#include "TabuSearch.h"
using namespace std;
mt19937 mt;

// Algorytm TabuSearch w najprostszej wersji implementujacy rodzaje sasiedztwa SWAP,INSERT,INVERT.
// Kryterium stopu - czas wykonania algorytmu, list tabu zostala zaimplementowana poprzez macierz, w ktorej numeru indeksu odpowiadaja 
// przejsciu z jednego wierzcholka do kolejnego. tabu_list[zrodlo][cel]. Jezeli na danych indeksach znajduje sie -1, oznacza ze dane przejscie nie jest 
// aktualnie zakazane. Permutacja poczatkowa jest wybierana jednakowo jak w algorytmie symulowanego wyzarzania -> funckja generujaca pseudolosowe ulozenie : calcRandomPath();
// Argumentami funkcji jest wejsciowa macierz sasiedztwa, kryterium stopu -> czas [s], typ_sasiedztwa

Result TabuSearch::tabu_search_alg(Matrix matrix, double time_max, int neighbourhood_type) {
	this->matrix = matrix.getMatrix();		// Przypisanie macierzy do pola klasy
	number_of_vertices = matrix.getNumber_of_vertices();		// Zapamietanie liczby wierczholkow
	this->chosen_neighbourhood = neighbourhood_type;			// Zapamietanie wybranego sposobu generacji sasiadow
	vector<int> x_0 = calcRandomPath();							// Sciezka poczatkowa 
	vector<int> x_opt = x_0;									// Sciezka optymalna (najlepsza)
	//double n = 1.0;
	int div_counter = 0;										// licznik dywersyfikacji dla ma³ych grafów
	bool div_model = number_of_vertices <= 25;

	tabu_list.reserve(number_of_vertices);				// Tworzenie macierzy przechowujacej wartosci listy tabu_search, jezeli na danym indeksie odpowiadajacym przejsciu [zrodlo][cel]
	for (int i = 0; i < number_of_vertices; i++) {		//	znajduje sie -1, oznacza ze dane przejscie nie jest aktualnie zakazane
		vector<int> temp;
		temp.reserve(number_of_vertices);
		for (int j = 0; j < number_of_vertices; j++) {
			temp.push_back(-1);
		}
		tabu_list.push_back(temp);
	}
	chrono::duration<double> total_time;
	auto timerStart = chrono::steady_clock::now();
	do {
		int cost_of_x_opt = calculate_path(x_opt);		// Obliczenie kosztu sciezki optymalnej (przyda siê w przypadku spelnienia kryterium aspiracji)
		x_0 = findBestNeighbourWithAspiration(x_0, cost_of_x_opt);		// generacja najlepszego sasiada danej permutacji
		if (calculate_path(x_0) < cost_of_x_opt) {						// jezeli koszt wygenerowanego sasiada jest lepszy od kosztu sciezki optymalnej -> zamiana
			x_opt = x_0;
		}
		else if (div_model) {											// tylko je¿eli rozpatrujemy ma³a instancjê problemu
			div_counter += 1;											// Inkrementacja licznika dywersyfikacji
		}

		tabu_list[start_index_of_new_tabu_element][end_index_of_new_tabu_element] = number_of_vertices; //ustawienie kadenncji nowego elementu listy tabu

		for (int i = 0; i < number_of_vertices; i++) {		// Dla kazdego elementu na liscie tabu, dekrementujemy jej kadencje
			for (int j = 0; j < number_of_vertices; j++) {
				if (tabu_list[i][j] != -1) {
					tabu_list[i][j] -= 1;
				}
				if (tabu_list[i][j] == 0) {	// W przypadku gdy kadencja elementu wynosi 0, "usuwamy" element z listy tabu -> wstawiamy wartosc -1
					tabu_list[i][j] = -1;
				}
			}
		}
		if (div_model) {
			if (div_counter >= 20000) {		// Uproszczony model dywersyfikacyjny spe³niaj¹cy swoje za³o¿enie dla ma³ych instancji problemu
				div_counter = 0;
				x_0 = calcRandomPath();		// Eksploracja odleg³ych przedzia³ów rozwi¹zañ w przypadku zapêtlenia algorytmu eksploatacji jednego zbioru lokalnego
				if (calculate_path(x_0) < calculate_path(x_opt)) {
					x_opt = x_0;
				}
			}
		}
		auto timerStop = chrono::steady_clock::now();		//obliczenie aktualnego czasu realizacji algorytmu
		total_time = timerStop - timerStart;

		// Zaimplementowane dla potrzeb testu do sprawozdania
		// 
		//if (total_time.count() >= n) {
		//	printf("\nAktualna [s] realizacji tabu: %f || Aktualny wynik: %d", total_time.count(), calculate_path(x_opt));
		//	n += 1.0;
		//}

	} while (total_time.count() < time_max);		// sprawdzenie czy nie zostalo osiagniete kryterium stopu


	return Result(x_opt, calculate_path(x_opt));		// Zwrot wyniku, obiekt Result przechowuje najlepsza sciezke oraz jej koszt
}


vector<int> TabuSearch::findBestNeighbourWithAspiration(vector<int> x_0, int cost_of_x_opt) {		// Zwrot najlepszego sasiada danego elementu uwzgledniajac schemat aspiracji
	int global_best_cost = cost_of_x_opt;		// aktualne najlepsze rozwiazanie algorytmu
	vector<int> currently_best_path = x_0;		// sciezka wejsciowa funkcji
	int min_of_function = INT_MAX;				// minimalny koszt najlepszego sasiada szukanego przez funkcje
	bool start_aspiration_plus = false;			// zmienna pomocnicza okreslajaca czy zaspokoilismy juz kryterium aspiracji plus
	int global_iteration_counter = 0;			// pomocniczy licznik iteracji funkcji
	int aspiration_counter = 0;					// licznik, ktorego rozruch nastepuje po osiagnieciu kryterium aspiracji plus
	int aspiration_plus_value = number_of_vertices / 10;		// eksperymentalna wartosc bezwgledna przedzialu "plus"
	int min_aspiration_iteration = number_of_vertices / 2;		// eksperymentalna wartosc minimalnej liczby iteracji funkcji

	for (int i = 1; i < number_of_vertices - 1; i++) {		// iterujemy po wszystkich polaczeniach i -> j
		for (int j = i + 1; j < number_of_vertices; j++) {

			bool prohibitted_move = false;		// zmienna pomocnicza okreslajaca czy przejscie i -> j znajduje sie na liscie tabu
			if(tabu_list[i][j] != -1){			// sprawdzenie czy przejscie i -> j znajduje na liscie tabu
				vector<int> aspiration_path = x_0;		// Jezeli polaczenie znajduje sie na liscie tabu, sprawdzamy kryt. aspiracji

				getNeighbour(aspiration_path, i, j);	// do funkcji wysylana jest referencja na aspiration_path, przez co aspiration_path staje sie sasiadem x_0 uwzgledniajac wybor sasiedztwa
														// oraz wierzcholki akutalnie rozpatrywanego przejscia i, j
				int asp_cost = calculate_path(aspiration_path);		// koszt rozwazanej sciezki
				if (asp_cost < global_best_cost) {		// jezeli koszt sciezki aspiracji jest mniejszy niz aktualne min_optymalne ->
					if (asp_cost < min_of_function) {	// sprawdzamy jednoczesnie czy w funkcji nie zostalo juz znalezione lepsze rozwiazanie ->
						currently_best_path = aspiration_path;		// jezeli nie, to zostalo spelnione kryt. aspiracji i dopuszczamy dane rozwiazanie pomimo polaczenia zakazanego przez tabu
						min_of_function = asp_cost;
						global_best_cost = asp_cost;
						start_aspiration_plus = true;
					}
				}
				prohibitted_move = true;	// Na koniec ustawiamy zmienna pomocnicza na true, przez co nie kontynuujemy rozwazan nad danym przejsciem
			}
			if (!prohibitted_move) {		// Jezeli polaczenie nie znajduje sie na liscie tabu
				vector<int> min_path_candidate = x_0;		// okreslamy sciezke kandydata jako aktualne min_funkci
				getNeighbour(min_path_candidate, i, j);		// kandydat staje sie bezposrednim sasiadem x_0 przy polaczeniu i - j
				int min_path_candidate_cost = calculate_path(min_path_candidate);		// obliczenie kosztu kandydata
				if (min_path_candidate_cost < min_of_function) {		// jezeli koszt kandydata jest mniejszy od aktualnego najlepszego min funkcji -> zamiana
					currently_best_path = min_path_candidate;
					min_of_function = min_path_candidate_cost;
					start_index_of_new_tabu_element = i;	// zapamietanie wartosci i,j w celu pozniejszego dodania polaczenia do listy ruchow zakazanych - tabu
					end_index_of_new_tabu_element = j;

					if (min_of_function < global_best_cost) {		// za wartosc kryt. aspiracji w podejsciu strategii wyboru rozwiazan -> aspiracja plus przyjeto aktualne najlepsze
																	// rozwiazanie algorytmu
						start_aspiration_plus = true;				// osiagnelismy kryt. aspiracji plus -> algorytm przeglada juz tylko dane plus rozwiazan jezeli przekroczyl juz liczbe minimalnych iteracji
					}
				}
			}
			global_iteration_counter += 1;		// wartosc pomocnicza zliczajaca iteracje algorytmu
			if (start_aspiration_plus) {		// jezeli we wczesniejszych krokach osiagnelsimy kryt. aspriacji
				aspiration_counter += 1;		// zaczynami inrkementowac pomocniczy licznik iteracji wykonanych po osiagnieciu kryterium
				if (global_iteration_counter > min_aspiration_iteration) {		// jezeli program wykonal juz minimalna liczbe iteracji
					if (aspiration_counter >= aspiration_plus_value)			// sprawdzamy czy wykonal rowniez "plus" iteracji okreslonych eksperymentalnymi wzorami u gory funkcji
						return currently_best_path;								// jezeli przegladnelismy juz caly zakres "plus" nie kontynuujemy poszukiwan najlepszego sasiada, zwracamy aktulnie najlepszy ( lokalnie ) wynik
				}
			}
		}
	}

	return currently_best_path;		// zwrocenie najlepszej sciezki ( sasiada ) odnalezionego przez funckje
}

void TabuSearch::getNeighbour(vector<int>& currentPath, int pos1, int pos2) {	// Zwrot bezposredniego sasiada currentPath, analogicznie jak w SA.
	if (chosen_neighbourhood == 1) { //Sasiedztwo typu Swap
		swap(currentPath[pos1], currentPath[pos2]);
	}
	else if (chosen_neighbourhood == 2) { //Sasiedztwo typu Invert
		reverse(currentPath.begin() + pos1, currentPath.begin() + pos2 + 1);
	}
	else if (chosen_neighbourhood == 3) {//Sasiedztwo typu Insert
		if(pos1 == pos2 - 1){
			swap(currentPath[pos1], currentPath[pos2]); // w przypadku gdy sa to nastepujace po sobie indeksy, jest to zwykly swap
		}
		else {
			currentPath.insert(currentPath.begin() + pos2, currentPath[pos1]);
			currentPath.erase(currentPath.begin() + pos1);
		}

	}
}

vector<int> TabuSearch::calcRandomPath() {	// Szerszy opis funckji w algorytmie SA
	vector<int> newPath;
	newPath.reserve(number_of_vertices);
	for (int i = 0; i < number_of_vertices; i++) {
		newPath.push_back(i);
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(newPath.begin() + 1, newPath.end(), default_random_engine(seed));
	return newPath;
}

int TabuSearch::calculate_path(vector<int>& temp_path) { // Szerszy opis funckji w algorytmie SA
	int result = 0;
	for (int i = 0; i < number_of_vertices - 1; i++)
		result += matrix[temp_path[i]][temp_path[i + 1]]; // Wyliczanie kosztu wierzcho³ków przy aktualnej konfiguracji (temp_path) oraz dodawanie do tymczasowego wyniku
	return result + matrix[temp_path[number_of_vertices - 1]][temp_path[0]]; // Dodanie kosztu powrotu do miasta poczatkowego
}
