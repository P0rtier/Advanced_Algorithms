#include "GeneticAlgorithm.h"
#include <random>
#include <cmath>
#include <chrono>
#include <algorithm>
#include "Result.h"
#include "Matrix.h"

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> distribution;
uniform_real_distribution<double> probMutation;


//Glowna funkcja realizujaca Algorytm Genetyczny 
Result GeneticAlgorithm::runGA(Matrix matrix, int population_size, int mutation_type, double mutation_rate, double cross_rate, double time) {
	// Przygotowania
	this->matrix = matrix.getMatrix();
	number_of_vertices = matrix.getNumber_of_vertices();
	populationSize = population_size;
	mutRate = mutation_rate;
	mutationType = mutation_type;
	crossRate = cross_rate;
	this->time = time;
	population.reserve(populationSize);

	uniform_real_distribution<double> probMutationToBe(0, 1);	// prawdobodobienstwo pomocnicze, wykorzystywane pozniej w determinowaniu, czy zajdzie mechanizm krzyzowania i mutacji
	uniform_int_distribution<int> probSelection(0, populationSize - 1);	// prawodopodobienstwo selekcji z przedzialu <0, rozmiar_populacji-1> okreslajace, ktore elementy beda brane pod uwage 
																		// przy tworzeniu nowych populacji-dzieci
	uniform_int_distribution<int> distributionToBe(0, number_of_vertices - 1);	// Struktura pomocnicza bioraca udzial w pseudo-losowym wybieraniu kolejnych wierzcholkow do permutacji
	distribution = distributionToBe;
	probMutation = probMutationToBe;
	chrono::duration<double> elapsed_seconds;	// czas calkowity symulacji wykorzystywany przy sprawdzaniu kryterium stopu

	auto gaStartTime = chrono::steady_clock::now();	//rozruch algorytmu

	generateStartPopulation();	// Funkcja wypelniajaca pole "population" wartosciami startowymi

	// Ponizej znajduja sie struktury czesto wykorzystywane w trakcie zycia programu, zamiast instancjonowac ich podczas dzialania programu zdecydowano sie to zrobic raz, a pozniej dzialac
	// na funkcjach wspomagajacyh ich realokacje czy czyszcenie.
	vector<vector<int>> selectionPopulation;
	vector<vector<int>> offspringPopulation;
	vector<int> parentCandidate1;
	vector<int> parentCandidate2;
	selectionPopulation.reserve(populationSize);
	offspringPopulation.reserve(populationSize);
	parentCandidate1.reserve(number_of_vertices);
	parentCandidate2.reserve(number_of_vertices);

	do {
		// Przy kazdej nowej iteracji petli zewnetrznej czyscimy struktury ktore beda na nowo wykorzystywane w programie, a moga zawierac w sobie juz jakies dane
		selectionPopulation.clear();
		offspringPopulation.clear();
		parentCandidate1.clear();
		parentCandidate2.clear();

		// Petla odpowiadajaca za wypelenienie struktury selectioPopulation, wystepuja tutaj prosty mechanizm seleckji turniejowej, gdzie z posrod dwoch losowo wybranych elmentow rodzica, 
		// do dalszych rozwazan wybierana jest ta ktora posiada lepszy koszt. Petla w ten sposob ma wypelnic strukture kadnydatow do rozmiaru populacji.
		while (selectionPopulation.size() < populationSize) {

			// Przy kazdym rozruchu petli czyszczone sa struktury odpowiadajace za przechowanie kandydatow do dalszych rozwazan
			parentCandidate1.clear();
			parentCandidate2.clear();

			int index1 = probSelection(gen); // Z prawdopodobienstwem "probSelection" wybierany jest index <0, rozmiar_populacji-1>
			int index2 = probSelection(gen); // ^^
			if (index1 == index2) continue; // Jezeli wylosowane indeksy sa jednakowe, przechodzimy do kolejnej iteracji petli

			parentCandidate1 = population[index1]; // Kandydatoci sa odpowiednio przypisywani
			parentCandidate2 = population[index2]; // ^^
			// Do potencjalnej przestrzeni rozwazan brany jest ten z kandydatow, ktory cechuje sie lepszym kosztem
			selectionPopulation.push_back(calculate_path(parentCandidate1) < calculate_path(parentCandidate2) ? parentCandidate1 : parentCandidate2);
		}

		// Petla odpowiadajaca za wypelnienie wlasciwej struktury dziecka odpowiednimi wartosciami. 
		for (int i = 0; i < populationSize - 1; i += 2) {
			// Na poczatku kazdej iteracji struktury wykorzystywane sa czyszcone
			parentCandidate1.clear();
			parentCandidate2.clear();

			// Selekcja z tablicy kandydatow odbywa sie na podstawie iteracji po tablicy oraz tworzeniu par kandydatow sasiadujacych ze soba
			parentCandidate1 = selectionPopulation[i];
			parentCandidate2 = selectionPopulation[i + 1];

			// Okreslenie prawdopodobienstwa z przedzialu <0,1>, jezeli prawodobodienstwo "takenProb" jest mniejsze od prawdopodobienstwa krzyzowania nastepuje wywolanie metody krzyzowania
			double takenProb = probMutation(gen);
			if (takenProb < crossRate) {
				// Wlasciwe krzyzowanie OX ze stalym pivotem
				parentCandidate1 = singlePointCrossover(parentCandidate1, parentCandidate2);
				parentCandidate2 = singlePointCrossover(parentCandidate2, parentCandidate1);
			}
			// W zaleznosci od wybranego sposobu mutacji wykonywana jest metoda mutacji Swap lub Insert
			// Metody sa jednakowe jak w przypadku poprzedniego projektu TSP_projekt2
			if (mutationType == 1) { //mutacja typu swap
				swapMutation(parentCandidate1);
				swapMutation(parentCandidate2);
			}
			else if (mutationType == 2) { //mutacja typu insert
				insertMutation(parentCandidate1);
				insertMutation(parentCandidate2);
			}
			// Po dokonaniu ewentualnej mutacji lub krzyzowania elementy dzieci sa dodawane do wektora populacji dzieci
			offspringPopulation.push_back(parentCandidate1);
			offspringPopulation.push_back(parentCandidate2);
		}

		// Populacja dzieci staje sie glowna populacja rozwazan
		population = offspringPopulation;

		// Nastepuje uszeregowanie elementow poprzez obliczenie kondycji elementow (porownanie ich kosztow), najlepsze rozwiazania zawsze sa na poczatku tablicy
		calculateFitnessOfOffspring();

		// Sprawdzenie calkowitego czasu algorytmu, w celu kontroli kryterium stopu
		auto endSimTime = chrono::steady_clock::now();
		elapsed_seconds = endSimTime - gaStartTime;

	} while (elapsed_seconds.count() < time);

	// Na koniec jeszcze raz sprawdzamy kondycje populacji, w celu zapewnienia jak najkorzystniejszego wyniku
	calculateFitnessOfOffspring();

	// Koncowo zwracany jest element z poczatku tablicy po wyliczeniu kondycji populacji
	Result result(population[0], calculate_path(population[0]));		// Klasa Result przechowuje œciezke najlepszego rozwiazania oraz jej koszt TSP
	return result;
}

// Funkcja odpowiedzialna za utworzenie populacji startowej algorytmu
void GeneticAlgorithm::generateStartPopulation() {
	population.clear();

	// Do kazdego wektora randomRoute dodawane sa kolejno wierzcholki drzewa w kolejnosci <0, wierzcholek_o_najwiekszym_numerze>, nastepnie wsytepuja pseudo-randomowe
	// przetasowanie elementow, tak otrzymany wektor jest dodawany do pola "population" koncowo generujac pseudo-losowa generacje poczatkowa.
	for (int i = 0; i < populationSize; i++) {
		vector<int> randomRoute;
		randomRoute.reserve(number_of_vertices);
		for (int j = 0; j < number_of_vertices; j++) {
			randomRoute.push_back(j);
		}
		shuffle(randomRoute.begin(), randomRoute.end(),gen);
		population.push_back(randomRoute);
	}
}

int GeneticAlgorithm::calculate_path(const vector<int>& temp_path) {	// Obliczenie kosztu rozwazanej sciezki
	int result = 0;
	for (int i = 0; i < number_of_vertices - 1; i++)
		result += matrix[temp_path[i]][temp_path[i + 1]]; // Wyliczanie kosztu wierzcholkow przy aktualnej konfiguracji (temp_path) oraz dodawanie do tymczasowego wyniku
	return result + matrix[temp_path[number_of_vertices - 1]][temp_path[0]]; // Dodanie kosztu powrotu do miasta poczatkowego
}

void GeneticAlgorithm::swapMutation(vector<int>& offspring) {
	if (probMutation(gen) < mutRate) {
		int pos1;
		int pos2;
		do {
			pos1 = distribution(gen);
			pos2 = distribution(gen);
		} while (pos1 == pos2); // generacja wierzcholkow grafu tak dlugo aby ominac malo sensownych sasiadow
		swap(offspring[pos1], offspring[pos2]);
	}
}

void GeneticAlgorithm::insertMutation(vector<int>& offspring) {
	if (probMutation(gen) < mutRate) {
		int pos1;
		int pos2;
		do {
			pos1 = distribution(gen);
			pos2 = distribution(gen);
		} while (pos1 == pos2 || pos1 == pos2 - 1 || pos1 == pos2 + 1); // generacja indeksow grafu tak dlugo aby ominac malo sensownych sasiadow
		offspring.insert(offspring.begin() + pos2, offspring[pos1]);
		if (pos2 > pos1)	// Usuniecie zdublowanych wartosci
			offspring.erase(offspring.begin() + pos1);
		else
			offspring.erase(offspring.begin() + pos1 + 1);
	}
}

// Funkcja obliczajaca kondycje aktualnej populacji polegajaca na uszeregowaniu elementow wzgledem ich kosztow
void GeneticAlgorithm::calculateFitnessOfOffspring() {
	sort(population.begin(), population.end(), [this](const vector<int>& a, const vector<int>& b) { return calculate_path(a) < calculate_path(b); });
}

// Funckja odpowiadajaca za mechanizm krzyzowania wystepuje tutaj prosty mechanizm krzyzowania OX ze stalym pivotem (indeksem do ktorego przepisywane sa elementy z pierwszego argumentu funckji)
vector<int> GeneticAlgorithm::singlePointCrossover(vector<int>& parentCandidate1, vector<int>& parentCandidate2) {

	vector<int> visitedVertice;
	vector<int> crossedOffspring;

	visitedVertice.resize(number_of_vertices);
	crossedOffspring.reserve(number_of_vertices);

	// Struktura pomocnicza jest wypelniana wartosciami -1, polega to na sprawdzaniu indeksow struktury "visitedVertice" w celu upewnienia sie, czy dany wierzcholek zostal juz dodany
	// do tablicy wynikowej crossedOffspring, przydatne w drugim etapie algorytmu, gdzie odwiedzamy kolejno wszystkie elementy argumentu drugiego w celu dodania wierzcholkow nieprzebytych
	fill(visitedVertice.begin(), visitedVertice.end(), -1);

	// Przepisanie elementow z kandydata1 do talbicy wynikowej do pivota
	for (int i = 0; i < number_of_vertices / 2; i++) {
		crossedOffspring.push_back(parentCandidate1[i]);
		visitedVertice[parentCandidate1[i]] = 1;
	}

	// Iteracja po kandydacie2 i dodanie elementow ktore jeszcze nie wsytepuja w tablicy wynikowej crossedOffspring
	for (int j = 0; j < number_of_vertices; j++) {
		if (visitedVertice[parentCandidate2[j]] == -1) {
			crossedOffspring.push_back(parentCandidate2[j]);
			visitedVertice[parentCandidate2[j]] = 1;
		}
	}

	return crossedOffspring;
}
