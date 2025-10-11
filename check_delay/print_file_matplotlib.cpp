#include "pch.h"
#include "print_file_matplotlib.h"

void print_graph_file(std::vector<double> s, std::vector<double> t, std::string filename, std::string title, std::string xlabel, std::string ylabel, std::string label, std::vector<double> axvline)
{
	std::ofstream file;          // поток для записи
	file.open("graphs/" + filename, std::ios::binary); // окрываем файл для записи
	print_data(file, s, t, title, xlabel, ylabel, label, axvline);
	file.close();
}

void print_points(std::ofstream& file, std::vector<double> s, std::vector<double> t, std::string label)
{
	file << "pointX:";
	for (int i = 0; i < t.size(); i++) {
		file << t[i] << " ";
	}
	file << "\npointY:";
	for (int i = 0; i < s.size(); i++) {
		file << s[i] << " ";
	}
	if (label.size() > 0) {
		file << "\nlabel:" + label;
	}
}

void print_data(std::ofstream& file, std::vector<double> s, std::vector<double> t, std::string title, std::string xlabel, std::string ylabel, std::string label, std::vector<double> axvline)
{
	file << "title:" + title << std::endl;
	file << "xlabel:" + xlabel << std::endl;
	file << "ylabel:" + ylabel << std::endl;
	print_points(file, s, t, label);
	if (axvline.size() > 0) {
		file << "\naxvline:";
		for (int i = 0; i < axvline.size(); i++) {
			file << axvline[i] << " ";
		}
	}
}

void print_add_points(std::vector<double> s, std::vector<double> t, std::string filename, std::string label)
{
	std::ofstream file;          // поток для записи
	file.open("graphs/" + filename, std::ios::binary | std::ios::app); // окрываем файл для записи
	file << "\n";
	print_points(file, s, t, label);
	file.close();
}

void print_add_graph(std::vector<double> s, std::vector<double> t, std::string filename, std::string title, std::string xlabel, std::string ylabel, std::string label, std::vector<double> axvline)
{
	std::ofstream file;          // поток для записи
	file.open("graphs/" + filename, std::ios::binary | std::ios::app); // окрываем файл для записи
	file << "separation ";
	print_data(file, s, t, title, xlabel, ylabel, label, axvline);
	file.close();
}
