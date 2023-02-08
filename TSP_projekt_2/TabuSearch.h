#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include "Matrix.h"
#include "Result.h"

class TabuSearch {
	vector<vector<int>> matrix;
	int number_of_vertices;
	//vector<pair<int, int>> tabu_vertices;
	//vector<int> tabu_cadency;
	vector<vector<int>> tabu_list;
	int start_index_of_new_tabu_element;
	int end_index_of_new_tabu_element;
	int chosen_neighbourhood;
public:
	Result tabu_search_alg(Matrix matrix, double timeMax, int neighbourhood_type);
private:
	int calculate_path(vector<int>& temp_path);
	vector<int> calcRandomPath();
	vector<int> findBestNeighbourWithAspiration(vector<int> x_0, int cost_of_x_opt);
	void getNeighbour(vector<int>& currentPath, int pos1, int pos2);
};