#pragma once
#include <vector>
#include <fstream>

void print_graph_file(std::vector<double> s, std::vector<double> t, std::string filename, std::string title, std::string xlabel, std::string ylabel, std::string label = "", std::vector<double> axvline = {});

void print_points(std::ofstream& file, std::vector<double> s, std::vector<double> t, std::string label = "");

void print_data(std::ofstream& file, std::vector<double> s, std::vector<double> t, std::string title, std::string xlabel, std::string ylabel, std::string label = "", std::vector<double> axvline = {});

void print_add_points(std::vector<double> s, std::vector<double> t, std::string filename, std::string label = "");

void print_add_graph(std::vector<double> s, std::vector<double> t, std::string filename, std::string title, std::string xlabel, std::string ylabel, std::string label = "", std::vector<double> axvline = {});