#pragma once
#include <vector>
#include <fstream>
#include <cmath>
#include <complex>
#include <algorithm>
#include <random>
#include <numeric>
#include <thread>

typedef std::complex<double> base;

enum type_modulation {
	AM,
	PM,
	FM
};

struct signal {
	int N;
	std::vector<double> I;
	std::vector<double> Q;
	std::vector<base> s;

	void resize(int n) {
		I.resize(n);
		Q.resize(n);
		s.resize(n);
	}

	void erase(int delay, int sample) {
		I = std::vector<double>(I.begin() + delay, I.begin() + delay + sample);
		Q = std::vector<double>(Q.begin() + delay, Q.begin() + delay + sample);
		s = std::vector<base>(s.begin() + delay, s.begin() + delay + sample);
	}

	int size() {
		return I.size();
	}

	std::vector<double> getSreal() {
		std::vector<double> res(s.size());
		for (int i = 0; i < res.size(); i++) {
			res[i] = s[i].real();
		}
		return res;
	}

	std::vector<double> getSimag() {
		std::vector<double> res(s.size());
		for (int i = 0; i < res.size(); i++) {
			res[i] = s[i].imag();
		}
		return res;
	}
};

std::vector<bool> generate_bits(int nbits, int sample);

void fft(std::vector<base>& a, bool invert);

void correlation(signal s1, signal s2, double step, std::vector<double>& corr, std::vector<double>& t);

std::pair<std::vector<double>, std::vector<double>> correlationSumma(signal s1, signal s2);

void normalizeSize(signal& s1, signal& s2);

std::pair<double, double> findMax(std::vector<double> corr, std::vector<double> t, double delay);

void addNoise(signal s, double snr, signal& s_n);

double criteria(std::vector<double> corr);

void one_sdvig(signal s_fully, signal s, int delay, std::vector<double>& res_fft);

std::vector<std::vector<double>> create_f_t(signal s_fully, signal s, double fd, std::vector<double>& ff, std::vector<double>& tau);

std::pair<double, double> find_sdvig(std::vector<std::vector<double>>& image, std::vector<double>& ff, std::vector<double>& tau);

class modulation {
	std::vector<double> t;
	signal s;
	std::vector<bool> bits;
	double fd;
	int nbits;
	int sample_base;
	double bitrate;
	double deltaF;
	int delay;
	double snr;
	type_modulation type;
	double duration;
	int sample;

public:
	void setParam(double _fd, int _nbits, double _bitrate, double _fc, double _delay, double _sample_base, type_modulation _type);

	void generate_t();

	void modAM();

	void modPM2();

	void modFM2();

	signal getS();

	std::vector<double> getT();

	double getDuration();

	std::vector<double> getBits();

	void manipulation();

	void setType(type_modulation _type);

	signal createBaseSignal();
};