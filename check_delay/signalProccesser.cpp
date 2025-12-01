#include "pch.h"
#define _USE_MATH_DEFINES
#include "signalProccesser.h"
using namespace std;

std::vector<bool> generate_bits(int nbits, int sample)
{
	vector<bool> result(sample);
	vector<bool> bits(nbits);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution dist(0.5);

	// Генерация последовательности
	for (int i = 0; i < nbits; i++) {
		bits[i] = dist(gen);
	}
	int sam_bit = ceil((double)sample / nbits); 
	for (int i = 0; i < sample; i++) {
		result[i] = bits[floor((double)i / sam_bit)];
	}
	return result;
}

void fft(std::vector<base>& a, bool invert)
{
	int n = (int)a.size();
	if (n == 1)  return;

	vector<base> a0(n / 2), a1(n / 2);
	for (int i = 0, j = 0; i < n; i += 2, ++j) {
		a0[j] = a[i];
		a1[j] = a[i + 1];
	}
	fft(a0, invert);
	fft(a1, invert);

	double ang = 2 * M_PI / n * (invert ? -1 : 1);
	base w(1), wn(cos(ang), sin(ang));
	for (int i = 0; i < n / 2; ++i) {
		a[i] = a0[i] + w * a1[i];
		a[i + n / 2] = a0[i] - w * a1[i];
		if (invert)
			a[i] /= 2, a[i + n / 2] /= 2;
		w *= wn;
	}
}

void correlation(signal s1, signal s2, double step, std::vector<double>& corr, std::vector<double>& t)
{
	fft(s1.s, true);
	fft(s2.s, true);
	std::vector<base> result(s1.size());
	for (int i = 0; i < result.size(); i++) {
		result[i] = s1.s[i] * std::conj(s2.s[i]);
	}
	fft(result, false);
	corr.resize(s1.N);
	t.resize(s1.N);
	for (int i = 0; i < corr.size(); i++) {
		corr[i] = (result[i] * std::conj(result[i])).real();
		t[i] = i * step;
	}
}

std::pair<std::vector<double>, std::vector<double>> correlationSumma(signal s1, signal s2)
{
	if (s1.size() > s2.size()) {
		auto s = s1;
		s1 = s2;
		s2 = s;
	}
	int size_shift = abs(s1.size() - s2.size()) + 1;
	std::vector<double> shift(size_shift);
	base temp;
	std::vector<double> res(size_shift);

	int size = s1.size();
	for (int i = 0; i < size_shift; i++) {
		shift[i] = i;
		temp = base(0, 0);
		for (int j = 0; j < size; j++) {
			temp += s1.s[j] * std::conj(s2.s[i + j]);
		}
		res[i] = temp.real() * temp.real() + temp.imag() * temp.imag();
	}

	return std::pair<std::vector<double>, std::vector<double>>(res, shift);
}

void normalizeSize(signal& s1, signal& s2)
{
	int size = 0;
	if (s1.size() >= s2.size()) {
		size = s1.size();
	}
	else {
		size = s2.size();
	}
	int pw2 = 1;
	while (size > pow(2, pw2)) {
		pw2++;
	}
	size = pow(2, pw2);
	s1.resize(size);
	s2.resize(size);
}

std::pair<double, double> findMax(std::vector<double> corr, std::vector<double> t, double delay)
{
	delay /= 1000;
	int indexMax = std::max_element(corr.begin(), corr.end()) - corr.begin();
	double d_delay = 0.;
	if (delay == 0) {
		if (t[indexMax] != 0) {
			d_delay = abs(delay - t[indexMax]) / t[indexMax];
		}
	}
	else {
		d_delay = abs(delay - t[indexMax]) * 100. / delay;
	}
	return std::pair<double, double>(t[indexMax], d_delay);
}

void addNoise(signal s, double snr, signal& s_n)
{
	s_n = s;
	if (s_n.s.empty()) return;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> dist(0.0, 1.0);

	// Вычисление мощности сигнала с использованием std::accumulate
	double signal_power = 0.;
	for (auto sample : s_n.s) {
		signal_power += std::norm(sample);
	}
	signal_power /= s_n.N;

	double snr_linear = std::pow(10.0, snr / 10.0);
	double noise_power = signal_power / snr_linear;
	double noise_stddev = std::sqrt(noise_power / 2.0);

	// Добавление шума
	for (int i = 0; i < s_n.N; i++) {
		s_n.s[i] += base(dist(gen) * noise_stddev,
			dist(gen) * noise_stddev);
	}
}

double criteria(std::vector<double> corr)
{
	int indexMax = std::max_element(corr.begin(), corr.end()) - corr.begin();
	double c_max = corr[indexMax];
	double avr = 0;
	for (int i = 0; i < corr.size(); i++) {
		avr += corr[i];
	}
	avr /= corr.size();
	double sigma = 0;
	for (int i = 0; i < corr.size(); i++) {
		sigma += (corr[i] - avr) * (corr[i] - avr);
	}
	sigma = sqrt(sigma) / corr.size();
	return c_max / sigma;
}

void one_sdvig(signal s_fully, signal s, int delay, std::vector<double>& res_fft)
{
	std::vector<base> temp;
	int size_2 = s.s.size();
	s.resize(s.N);
	int size = s.N;
	temp.resize(size);
	for (int i = 0; i < size; i++) {
		temp[i] = s.s[i] * std::conj(s_fully.s[i + delay]);
	}
	temp.resize(size_2);
	fft(temp, true);
	res_fft.resize(size_2);
	int center = size_2 / 2;
	for (int i = 0; i < size_2; i++) {
		if (i < center) {
			res_fft[i + center] = sqrt(temp[i].real() * temp[i].real() + temp[i].imag() * temp[i].imag());
		}
		else {
			res_fft[i - center] = sqrt(temp[i].real() * temp[i].real() + temp[i].imag() * temp[i].imag());
		}
	}
}

std::vector<std::vector<double>> create_f_t(signal s_fully, signal s, double fd, std::vector<double>& ff, std::vector<double>& tau)
{
	int max_delay = s_fully.N - s.N;
	std::vector<std::vector<double>> res(max_delay);
	int max_threads = 8;
	thread* thrs = new thread[max_threads];
	for (int i = 0; i < max_delay; i += max_threads) {
		for (int j = 0; j < max_threads; j++) {
			if (i + j >= max_delay) {
				continue;
			}
			thrs[j] = thread(one_sdvig,
				s_fully,
				s,
				i + j,
				std::ref(res[i + j])
			);
		}
		for (int j = 0; j < max_threads; j++) {
			if (thrs[j].joinable()) {
				thrs[j].join();
			}
		}
	}

	ff.resize(res[0].size());
	tau.resize(res.size());
	double step_ff = fd / ff.size();
	double step_tau = 1. / fd;
	int center = ff.size() / 2;
	for (int i = 0; i < ff.size(); i++) {
		if (i < center) {
			ff[i + center] = i * step_ff;
		}
		else {
			ff[i - center] = -fd + i * step_ff;
		}
		//ff[i] = i * step_ff;
	}
	for (int i = 0; i < tau.size(); i++) {
		tau[i] = i * step_tau;
	}

	return res;
}

std::pair<double, double> find_sdvig(std::vector<std::vector<double>>& image, std::vector<double>& ff, std::vector<double>& tau)
{
	std::vector<int> max_ff(tau.size());
	std::vector<double> max_value_ff(tau.size());
	for (int i = 0; i < image.size(); i++) {
		max_ff[i] = std::max_element(image[i].begin(), image[i].end()) - image[i].begin();
		max_value_ff[i] = image[i][max_ff[i]];
	}
	int indexMax = std::max_element(max_value_ff.begin(), max_value_ff.end()) - max_value_ff.begin();
	return std::pair<double, double>(ff[max_ff[indexMax]], tau[indexMax]);
}

void modulation::setParam(double _fd, int _nbits, double _bitrate, double _fc, double _delay, double _sample_base, type_modulation _type)
{
	fd = _fd;
	nbits = _nbits;
	bitrate = _bitrate;
	deltaF = _fc;
	delay = ceil(_delay * fd);
	sample_base = (double)_sample_base * fd;
	type = _type;
	duration = nbits / bitrate;
	if (sample != duration * fd * 1000) {
		sample = duration * fd * 1000;
		generate_t();
		s.resize(sample);
	}
}

void modulation::generate_t()
{
	t.resize(sample);
	for (int i = 0; i < sample; i++) {
		t[i] = i / (fd * 1000);
	}
}

void modulation::modAM()
{
	for (int i = 0; i < sample; i++) {
		s.I[i] = (1 + (bits[i] ? 1 : 0)) / 2.;
		//s.I[i] = bits[i] ? 1 : -1;
		s.Q[i] = 0.0;
	}
}

void modulation::modPM2()
{
	for (int i = 0; i < sample; i++) {
		s.I[i] = (bits[i] == 1) ? 1.0 : -1.0;
		s.Q[i] = 0.0;
	}
}

void modulation::modFM2()
{
	double accumulated_phase = 0.;
	double df = bitrate / 4;
	for (int i = 0; i < sample; i++) {
		accumulated_phase += (bits[i] == 1 ? df : -df) / (fd * 1000);
		s.I[i] = cos(accumulated_phase);
		s.Q[i] = sin(accumulated_phase);
	}
}

signal modulation::getS()
{
	return s;
}

std::vector<double> modulation::getT()
{
	return t;
}

double modulation::getDuration()
{
	return duration;
}

std::vector<double> modulation::getBits()
{
	std::vector<double> double_vec(bits.size());

	for (size_t i = 0; i < bits.size(); ++i) {
		double_vec[i] = bits[i] ? 1.0 : 0.0;
	}
	return double_vec;
}

void modulation::manipulation()
{
	bits = generate_bits(nbits, sample);
	switch (type)
	{
	case AM:
		modAM();
		break;
	case PM:
		modPM2();
		break;
	case FM:
		modFM2();
		break;
	default:
		break;
	}
	for (int i = 0; i < sample; i++) {
		s.s[i] = base(s.I[i], s.Q[i]);
	}
}

void modulation::setType(type_modulation _type)
{
	type = _type;
}

signal modulation::createBaseSignal()
{
	signal result = s;
	result.erase(delay, sample_base);
	s.N = sample;
	result.N = sample_base;

	double phase = 0;
	base temp;
	for (int i = 0; i < s.N; i++) {
		phase += 2 * M_PI * deltaF / (fd * 1000);
		temp = base(cos(phase), sin(phase));
		s.s[i] *= temp;
	}
	normalizeSize(result, s);
	return result;
}
