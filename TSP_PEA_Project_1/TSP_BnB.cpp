#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <cstring>
#include <climits>
#include<chrono>

#include "TSP_BnB.h"
using namespace std;


Node* TSP_BnB::generate_new_node(const vector<vector<int>>& parentReducedMatrix,const vector<int> &routeToPrint, int depth, int start, int destination) {

	Node* node = new Node; // generowanie nowego wezla grafu odpowiadajacego wierzcholkowi j

	node->routeToPrint = routeToPrint; // kopia obecnie najbardziej optymalnej drogi

	node->routeToPrint.push_back(destination);  // dodanie celu drogi do vectora przechowujacego wirzcholki sciezki
	node->matrixReduction.clear(); // przepisanie zredukowanej macierzy odziedziczonej po wezle rodzicu.
	for (int x = 0; x < number_of_vertices; x++) {
		vector<int> temp_row;
		for (int y = 0; y < number_of_vertices; y++) {
			temp_row.push_back(parentReducedMatrix[x][y]);
		}
		node->matrixReduction.push_back(temp_row);
	}

	if (depth != 0) {
		for (int set_inf = 0; set_inf < number_of_vertices; set_inf++) { // Ustawienie nieskonczonosci w kolumnie oraz wierszu odpowiadajacym numerom wierzcholkow analizowanej drogi
																	 // np. droga z wierzcholkow 2 -> 3, INFINITY w wierszu 2 i kolumnie 3, 
			node->matrixReduction[start][set_inf] = 9999;
			node->matrixReduction[set_inf][destination] = 9999;
		}
	}

	node->matrixReduction[destination][0] = 9999; // Dodatkowo ustawiamy wartosc powrotu (destination , start) do wierzch. startowego na INFINITY. np. 2 -> 3, Infinity w matrixReduction[3][0]

	node->depth = depth; // Ustawiamy obecnie analizowana glêbokosc drzewa
	node->vertex = destination; // Ustawiamy wartoœæ pomocnicza - informacje o wierzcholku grafu odpowiadajacego wezlowi drzewa.

	return node;
}

void TSP_BnB::RoutePrintMethod(const vector<int>& routeToPrint) {
	cout << "Sciezka: ";
	for (int i = 0; i < routeToPrint.size(); i++) {
		if (i == routeToPrint.size() - 1)
			cout << routeToPrint[i];
		else
			cout << routeToPrint[i] << " ==> ";
	}
}

vector<int> TSP_BnB::reduceRow(vector<vector<int>>& matrixReduction) {

	vector<int> row;
	for (int i = 0; i < number_of_vertices; i++) {
		row.push_back(9999);
	}


	// Dla ka¿dego wiersza znajdujemy jego wartosc minimaln¹ i przechowujemy j¹ w vectorze row, na opdowiajacym mu indeksie:
		// row[0] = currentMinimumNode( macierz_redukcji[0][] ), row[1] = currentMinimumNode( macierz_redukcji[1][] )
	for (int i = 0; i < number_of_vertices; i++) {
		int temp_min = 9999;
		for (int j = 0; j < number_of_vertices; j++) {
			if (matrixReduction[i][j] < temp_min) {
				temp_min = matrixReduction[i][j];
			}
		}
		row[i] = temp_min;
	}

	// Przystepujemy do redukcji wierszy macierzy, od kazdego elementu rozpatrywanego wierszu odejmujemy minimum wiersza. Jezeli na danym indeksie jest nieskonczonosc,
		// lub dany wiersz jest juz po redukcji ( na ka¿dym indeksie znajduje siê ju¿ nieskonczonosc ), omijamy.
	for (int i = 0; i < number_of_vertices; i++)
	{
		for (int j = 0; j < number_of_vertices; j++)
		{
			if (matrixReduction[i][j] != 9999 && row[i] != 9999) {
				matrixReduction[i][j] -= row[i];
			}
		}
	}

	return row;
}


vector<int> TSP_BnB::reduceColumn(vector<vector<int>>& matrixReduction) {

	vector<int> column;
	for (int i = 0; i < number_of_vertices; i++) {
		column.push_back(9999);
	}

	// Dla kazdej kolumny znajdujemy jej wartosc minimalna i przechowujemy ja w vectorze column, na opdowiajacym mu indeksie:
		// column[0] = currentMinimumNode( macierz_redukcji[][0] ), column[1] = currentMinimumNode( macierz_redukcji[][1] )
	for (int i = 0; i < number_of_vertices; i++) {
		for (int j = 0; j < number_of_vertices; j++) {
			if (matrixReduction[i][j] < column[j]) {
				column[j] = matrixReduction[i][j];
			}
		}
	}

	// Przystepujemy do redukcji kolumn macierzy, od ka¿dego elementu rozpatrywanej kolmny odejmujemy minimum kolumny. Je¿eli na danym indeksie jest nieskonczonosc,
		// lub dana kolumna jest ju¿ po redukcji ( na ka¿dym indeksie znajduje sie ju¿ nieskonczonosc ), omijamy.
	for (int i = 0; i < number_of_vertices; i++)
	{
		for (int j = 0; j < number_of_vertices; j++)
		{
			if (matrixReduction[i][j] != 9999 && column[j] != 9999)
				matrixReduction[i][j] -= column[j];
		}
	}
	return column;
}

int TSP_BnB::calculateNodeLowerBound(vector<vector<int>>& matrixReduction) {

	int lower_bound_step_product = 0; // Element skladowy dolnej granicy rozpatrywanego wezla 

	vector<int> row = reduceRow(matrixReduction); // otrzymujemy tablice minimow wierszy

	vector<int> col = reduceColumn(matrixReduction); // otrzymujemy tablice minimow kolumn

	for (int i = 0; i < number_of_vertices; i++) { // Wyliczamy wartosc danego wezla, poprzez zsumowanie minimow, w przypadku zredykowanych 
												  //  wierszy / kolumn, dodajemy 0.
		if (row[i] != 9999)
			lower_bound_step_product += row[i];
		else
			lower_bound_step_product += 0;

		if (col[i] != 9999)
			lower_bound_step_product += col[i];
		else
			lower_bound_step_product += 0;
	}

	return lower_bound_step_product;
}


// Funkcja wyjsciowa algorytmy Brounch and Bound dla problemu Komiwojazera (TSP), przy zalozeniu Best-first.
double TSP_BnB::calculateTSP_Brounch_And_Bound(Matrix matrix) {

	struct compare_nodes												// Struktura pomocnicza porownuj¹ca Wezly przy dodawaniu ich do kolejki priorytetowej, gdzie																	//
	{																	// na pocz¹tku kolejki zawsze znajduje siê aktualnie najepszy wezel ( posiadajacy najmniejszy koszt - node_result )
		bool operator()(const Node* leftNode, const Node* rightNode) const {
			return leftNode->node_result > rightNode->node_result;
		}
	};


	number_of_vertices = matrix.getNumber_of_vertices(); // Zmienna przechowujaca rozpatrywana liczbe wierzcholkow dla problemu TSP.

	vector<vector<int>> matrixFun;										// Ustawienie przekatnej macierzy na INF(9999) w celu usprawnienia algorytmu
	for (int i = 0; i < number_of_vertices; i++) {						
		vector<int> tempRow;
		for (int j = 0; j < number_of_vertices; j++) {
			if (i == j)
				tempRow.push_back(9999);
			else
				tempRow.push_back(matrix.getMatrix()[i][j]);
		}
		matrixFun.push_back(tempRow);
	}

	auto start = chrono::steady_clock::now(); // Rozruch zegara

	priority_queue<Node*, vector<Node*>, compare_nodes> nodePrimaryQueue; // Kolejka priorytetowa umozliwiajaca wyciaganie aktualnie najlepszych elementow algorymtu ( Wezlow grafu o najmniejszym koszcie)

	vector<int> route; // vector przechowujacy pary wierzcholkow (i,j) odpowiadaj¹ce sciezce i -> j. Wykorzystywane przy podawaniu dokladnej drogi algorytmu.



	Node* nodeStart = generate_new_node(matrixFun, route, 0, -1, 0); // Generowanie korzenia drzewa rozumianego jako wierzcholek startowy rozpatrywanego grafu problemu TSP. 
															// Zgodnie z za³ozeniami projektowymi punktem startowym algorytmu jest zawsze wierzcholek 0 (4 arugment)
															// W przypadku korzenia drzewa za po³¹czenie podajemy (-1, 0), ograniczenia nalozone w generate_new_node() przepisz¹
															// do korzenia tylko istotne informacje, takie elementy jak minimalizacja macierzy sa pomijane.

	nodeStart->node_result = calculateNodeLowerBound(nodeStart->matrixReduction); // Wyliczamy dolna granice wezla

	nodePrimaryQueue.push(nodeStart); // Zapis korzenia na szczycie kolejki priorytetowej

	while (!nodePrimaryQueue.empty()) // Petla wyliczajaca wartosc TSP dla wczytanego problemu (macierzy)
	{
		Node* currentMinimumNode = nodePrimaryQueue.top(); // Rozruch poprzez sczytanie aktualnie najlepszego wezla ( ktory zawsze znajduje sie na szczycie kolejki )

		nodePrimaryQueue.pop(); // Usuniecie wezla z kolejki, gdyz jest on juz zapisany w referencji 

		int startNode = currentMinimumNode->vertex; // Numer wierzcholka aktualnie rozpatrywanego powiazanego z wezlem (numeracja 0 -> X ), gdzie 0 - korzen / Wierzcholek startowy

		if (currentMinimumNode->depth == number_of_vertices - 1) // Sprawdzamy czy doszlismy do maksymalnej glebokosci drzewa, co oznacza, ¿e dotarlismy do liscia rozpatrywanego drzewa
		{														 // - koniec algorytmu.

			currentMinimumNode->routeToPrint.push_back(0); // Do zapisywanej drogi dodajemy polaczenie koncowe z liscia do wierzcholka startowego ( korzenia )
			auto end = chrono::steady_clock::now();
			chrono::duration<double> elapsed_seconds = (end - start) * pow(10,3);

			cout << "Wynik TSP_Brounch_And_Bound to: " << currentMinimumNode->node_result << endl;
			RoutePrintMethod(currentMinimumNode->routeToPrint);
			cout << "\nCzas algorytmu: " << elapsed_seconds.count() << " [ms]";
			return elapsed_seconds.count();
		}

		for (int destinationNode = 0; destinationNode < number_of_vertices; destinationNode++) // Jezeli nie znajdujemy siê jeszcze w lisciu drzewa, przechodzimy do obliczania kolejnych wartoœci wezlow:
		{
			if (currentMinimumNode->matrixReduction[startNode][destinationNode] != 9999) // Dla kazdej potencjalnej sciezki z i -> j, sprawdzamy czy nie zosta³a ju¿ wykonana redukcja macierzy  <==> Polaczenie zosta³o ju¿ rozpatrzone / brak mozliwosci polaczenia
			{
				Node* child = generate_new_node(currentMinimumNode->matrixReduction, currentMinimumNode->routeToPrint,		// Okreslamy dzieci ( wezly ) rozpatrywanego rodzica, poprzez redukcje macierzy oraz
					currentMinimumNode->depth + 1, startNode, destinationNode);																	// zapis parametrow wezla (glebokosc, rozpatrywana sciezka (i,j), macierz poddawana redukcji bedaca macierza zredukowana dla rodzica danego wezla)

				child->node_result = currentMinimumNode->node_result + currentMinimumNode->matrixReduction[startNode][destinationNode]			// Okreslamy koszt wezla poprzez sumê (Koszt rodzica) + (Wartosc œcie¿ki (i,j) + (Dolna granica sciezki rozpoczynajacej siê z wierzcholka j))
					+ calculateNodeLowerBound(child->matrixReduction);

				nodePrimaryQueue.push(child); // Zapisujemy dziecko w kolejce priorytetowej
			}
		}
		delete currentMinimumNode; // Usuwamy rodzica z pamieci.
	}
}