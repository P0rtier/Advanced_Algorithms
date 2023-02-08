#pragma once
#include<iostream>
#include<stdio.h>
#include<vector>
#include<string>
#include "Result.h"
#include "Matrix.h"

using namespace std;

class GeneticAlgorithm {
	vector<vector<int>> matrix;
	int number_of_vertices;
    vector<vector<int>> population;
    int populationSize;
    double mutRate;
    double crossRate;
    double time;
    int numberOfMutations;
    int mutationType;
public:
    Result runGA(Matrix matrix, int population_size, int mutation_type, double mutation_rate, double cross_rate, double time);
private:
    void generateStartPopulation();
    int calculate_path(const vector<int>& temp_path);
    void swapMutation(vector<int>& offspring);
    void insertMutation(vector<int>& offspring);
    void calculateFitnessOfOffspring();
    vector<int> singlePointCrossover(vector<int>& parentCandidate1, vector<int>& parentCandidate2);
    vector<int> selectParent();
};