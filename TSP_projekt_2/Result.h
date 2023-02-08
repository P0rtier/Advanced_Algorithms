#pragma once
#include<iostream>
#include<stdio.h>
#include<vector>
#include<string>

using namespace std;

class Result {
public:
	vector<int> final_path;
	int final_result;
	Result() {};
	Result(vector<int> final_path, int final_result) {
		this->final_path = final_path;
		this->final_result = final_result;
	}
};