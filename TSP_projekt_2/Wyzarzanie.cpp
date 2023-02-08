#include<iostream>
#include<stdio.h>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<random>
#include<chrono>

#include "Wyzarzanie.h"
#include "Result.h"

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> distribution;

// Algorytm symulowanego wyzarzania w wersji z dwu elemenetowym wyznacznikiem kolejnego (wejœciowego) oraz najlepszego rozwiazania
// Implementacja sasiedztwa na podstawie przykladowych rozwiazan z wykladu: sasiedztwo SWAP, INSERT, INVERT
// Implementacja malej epoki, ktora z wyznaczen eskperymetnalnych nie wplywa znaczaco na czas calkowity algorytmu, lecz
// polepsza wynik w przypadku duzych grafow -> powyzej ok. 130 wierzcholkow.
// Znaczacymi argumentami funckji jest wybor sasiedztwa, warunek stopu rozumiany jako maksymalny czas realizacji algorytmu,
// oraz wartosc parametru alfa, oznaczonego jako "cooler" sluzacego do zmniejszania temperatury algorytmu wzorem: temperatura = alfa * temperatura_w_chwili_t;

Result Wyzarzanie::sa_algorithm(Matrix matrix, double cooler, double time_max, int neighbourhood_type, int min_restriction_chosen) {
	this->matrix = matrix.getMatrix();						// Przypisanie macierzy do pola klasy
	number_of_vertices = matrix.getNumber_of_vertices();	// Pomocnicze pole przechowujace liczbe wierzcholkow rozpatrywanego grafu
	chosen_neighbourhood = neighbourhood_type;				// Pole pomocnicze przechowujace wybrany typ sasiedztwa
	vector<int> startPoint = calcRandomPath();				// Permutacja poczatkowa otrzymywana za pomoca losowego ulozenia wierzcholkow grafu
	vector<int> currentBestPath = startPoint;				// Wektor przechowujacy najlepszy wynik (scie¿ke TSP) w aktualnej chwili algorytmu
	vector<int> neighbourPathSwap;							// Wektor przechowujacy kadnydata na najlepsza sciezke wzynaczany poprzez wybrany sposob sasiedztwa
	neighbourPathSwap.reserve(number_of_vertices);

	uniform_real_distribution<double> check(0.0, 1.0);		// Struktura pomocnicza bioraca udzial we wzorze odpowiedzialnym za dopuszczenie gorszego wyniku ( Eksploracja rozwiazan problemu )
	uniform_int_distribution<int> distributionToBe(0, number_of_vertices - 1);	// Struktura pomocnicza bioraca udzial w pseudo-losowym wybieraniu kolejnych wierzcholkow do permutacji
																				// wykorzystujac wybrany sposób okreslenia sasiedztwa
	distribution = distributionToBe;

	double temperature = calculateMaxTemperature(currentBestPath);				// Funkcja obliczajaca Temperature startowa algorytmu
	double annealing_prob;														// Prawdopodobienstwo wyznaczane za pomoca wzoru podanego w materialach do projektu: exp(-1* ( roznica kosztow aktualnego rozwiazania z zapamietanym rozwiazaniem wejscia danej iteracji / aktualna_temperatura )
	double standard_prob;														// Standardowe prawdopodobienstwo z zakresu <0.0, 1.0>

	//double n = 0.25;
	chrono::duration<double> total_time;					// Przedzial czasu sluzacy do monitorowania algorytmu ( Kryterium stopu )
	auto timerStart = chrono::steady_clock::now();			// Rozruch zegara
	do {

		for (int i = 0; i < 2; i++) {						// Wprowadzona zostala bardzo mala epoka, ktora z testow eskperymentalnych poprawia wyniki dla duzych grafow
			neighbourPathSwap = startPoint;					
			getNeighbour(neighbourPathSwap);				// Okreslenie sasiada aktualnie rozwazanego rozwiazania uwzgledniaj¹c wybrany sposob sasiedztwa
			int neighbourPathCost = calculate_path(neighbourPathSwap);		// Obliczenie kosztu sasiada
			int cost_difference = neighbourPathCost - calculate_path(startPoint);		// Roznica kosztow sciezki startowej oraz rozwazanego sasiada

			if (cost_difference < 0) {			// Jezeli sasiad posiada lepszy koszt od rozw. poczatkowego nalezy interpetowac go jako punkt wejscia kolejnej iteracji
				startPoint = neighbourPathSwap;
				if (neighbourPathCost < calculate_path(currentBestPath))		// Jezeli dodatkowo koszt sasiada / podmienionego rozw. poczatkowego jest lepszy od najlepszego znanego rozw. -> zamiana
					currentBestPath = startPoint;
			}
			else {
				standard_prob = check(gen);		// W przeciwnym przypadku dopuszczamy z prawdopodobienstwem uzaleznionym od aktualnej temperatury uwzglednienie gorszego rozwiazania
												// tym samym promujac eksploracje wiekszego przedzia³u sciezek, oraz ich pozniejszej eskploatacji -> poszukiwania min lokalnego kolejnego przedzialu rozwazan
				annealing_prob = exp(-1 * (cost_difference / temperature));		// Wzor z instrukcji projektowej oraz wykladu
				if (standard_prob < annealing_prob)			// Bezposrednie dopusczenie gorszego rozw w wypadku gdy prawdopodienstwo std jest mniejsze od prawd. uzaleznionego od Temperatury
					startPoint = neighbourPathSwap;
			}
		}
		temperature *= cooler;					// Zamykajac epoke zmniejszamy temperature algorytmu
		if (min_restriction_chosen == 2 && temperature <= 0.001)				// Jezeli temperatura osi¹gnela punkt minimalny, mo¿emy przerwac algorytm ( Nieznaczace lub zadne poprawy aktualnego rozwiazania TSP )
			break;
		auto timerStop = chrono::steady_clock::now();
		total_time = timerStop - timerStart;

		// Zaimplementowane dla potrzeb testu do sprawozdania
		//if (total_time.count() >= n) {
		//	printf("\nAktualna [s] realizacji wyzarzania: %f || Aktualny wynik: %d", total_time.count(), calculate_path(currentBestPath));
		//	n += 0.25;
		//}

	} while (total_time.count() < time_max);	// Algorytm wykonuje siê do osiagniecia kryterium stopu lub poprzez przerwanie wynikaj¹ce z osiagniêcia przyjetej temp minimalnej

	Result result(currentBestPath, calculate_path(currentBestPath));		// Klasa Result przechowuje œciezke najlepszego rozwiazania oraz jej koszt TSP
	return result;
}


double Wyzarzanie::calculateMaxTemperature(vector<int> randomPath) {		// Funkcja oblicza dynamicznie temperature maksymalna dla danego grafu
	double maxTemp = 0.0;
	vector<int> next_path = randomPath;
	for (int i = 0; i < 120; i++) {		// Wartosc iteracji wybrana z esperymentalnego zakresu
		getNeighbour(next_path);		// Otrzymanie sasiada danej permutacji
		double temp_result = abs(calculate_path(randomPath) - calculate_path(next_path));		// Wyznaczenie wartosci bezwzglednej roznicy kosztu sciezki wejsciowej ( Rowniez pseudo-losowej ) oraz aktualnie rozwazanej
		if (temp_result > maxTemp)		// W przypadku gdy obliczona temperatura jest wieksza od aktualnie maxTemp zamieniamy
			maxTemp = temp_result;
		randomPath = next_path;			// W kolejnych iteracjach porownujemy z wczesniejsza permutacja
	}
	return maxTemp;						// Koncowo zwracamy najwieksza znaleziona roznice kosztow sasiadow ( Nie sa to tylko bezposredni sasiedzi 1-ego stopnia sciezki wejsciowej! )
}

vector<int> Wyzarzanie::calcRandomPath() {		// Obliczenie pseudo-losowej sciezki poczatkowej 
	vector<int> newPath;
	newPath.reserve(number_of_vertices);
	for (int i = 0; i < number_of_vertices; i++) {
		newPath.push_back(i);
	}

	mt19937 mt;
	shuffle(newPath.begin()+1, newPath.end(), mt);		// Wykorzystuje tutaj funkcje std::shuffle która zwraca pseduo-losowe ustawienie wierzcholkow bazujac na ustawieniu wejsciowym oraz
														// pseudo-losowym generatorze mt19937.
	return newPath;
}

int Wyzarzanie::calculate_path(vector<int>& temp_path) {	// Obliczenie kosztu rozwazanej sciezki
	int result = 0;
	for (int i = 0; i < number_of_vertices - 1; i++)
		result += matrix[temp_path[i]][temp_path[i + 1]]; // Wyliczanie kosztu wierzcholkow przy aktualnej konfiguracji (temp_path) oraz dodawanie do tymczasowego wyniku
	return result + matrix[temp_path[number_of_vertices - 1]][temp_path[0]]; // Dodanie kosztu powrotu do miasta poczatkowego
}

void Wyzarzanie::getNeighbour(vector<int>& currentPath) {	// Funckja zwracajaca sasiada danej permutacji bazujac na wybranym sposobie doboru sasiada (sasiedztwa x)

	int pos1 = 0;
	int pos2 = 0;
	if (chosen_neighbourhood == 1) { //Sasiedztwo typu Swap
		do {
			pos1 = distribution(gen);
			pos2 = distribution(gen);
		} while (pos1 == pos2 || pos2 < pos1); // generacja wierzcholkow grafu tak dlugo aby ominac malo sensownych sasiadow
		swap(currentPath[pos1], currentPath[pos2]);
	}
	else if (chosen_neighbourhood == 2) { //Sasiedztwo typu Invert
		do {
			pos1 = distribution(gen);
			pos2 = distribution(gen);
		} while (pos1 == pos2 || pos2 < pos1); // generacja indeksow grafu tak dlugo aby ominac malo sensownych sasiadow
		reverse(currentPath.begin() + pos1, currentPath.begin() + pos2 + 1);
	}
	else if (chosen_neighbourhood == 3) {//Sasiedztwo typu Insert
		do {
			pos1 = distribution(gen);
			pos2 = distribution(gen);
		} while (pos1 == pos2 || pos1 == pos2 - 1 || pos1 == pos2 + 1); // generacja indeksow grafu tak dlugo aby ominac malo sensownych sasiadow
		currentPath.insert(currentPath.begin() + pos2, currentPath[pos1]);
		if (pos2 > pos1)	// Usuniecie zdublowanych wartosci
			currentPath.erase(currentPath.begin() + pos1);
		else
			currentPath.erase(currentPath.begin() + pos1 + 1);
	}
}