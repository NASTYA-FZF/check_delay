
// check_delayDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "check_delay.h"
#include "check_delayDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CcheckdelayDlg



CcheckdelayDlg::CcheckdelayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHECK_DELAY_DIALOG, pParent)
	, fd(1.1)
	, nbits(150)
	, bitrate(150)
	, fc(0.5)
	, delay(100)
	, snr(3)
	, result_delay(_T(""))
	, sample_base(400)
	, snr_fully(10)
	, min_snr(-30)
	, max_snr(10)
	, step_snr(1)
	, N_generate(300)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcheckdelayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FD, fd);
	DDX_Text(pDX, IDC_EDIT_NBITS, nbits);
	DDX_Text(pDX, IDC_EDIT_BITRATE, bitrate);
	DDX_Text(pDX, IDC_EDIT_FC, fc);
	DDX_Text(pDX, IDC_EDIT_DELAY, delay);
	DDX_Text(pDX, IDC_EDIT_SNR, snr);
	DDX_Text(pDX, IDC_STATIC_EST_DELAY, result_delay);
	DDX_Text(pDX, IDC_EDIT_SAMPLE_BASE, sample_base);
	DDX_Text(pDX, IDC_EDIT_SNR2, snr_fully);
	DDX_Text(pDX, IDC_EDIT_DELAY2, min_snr);
	DDX_Text(pDX, IDC_EDIT_SAMPLE_BASE2, max_snr);
	DDX_Text(pDX, IDC_EDIT_SNR3, step_snr);
	DDX_Text(pDX, IDC_EDIT_SNR4, N_generate);
}

BEGIN_MESSAGE_MAP(CcheckdelayDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTCREATE, &CcheckdelayDlg::OnBnClickedButcreate)
	ON_BN_CLICKED(IDC_RADIO_AM, &CcheckdelayDlg::OnBnClickedRadioAm)
	ON_BN_CLICKED(IDC_RADIO_PM, &CcheckdelayDlg::OnBnClickedRadioPm)
	ON_BN_CLICKED(IDC_RADIO_FRM, &CcheckdelayDlg::OnBnClickedRadioFrm)
	ON_BN_CLICKED(IDC_BUTTON_ESTIMATE, &CcheckdelayDlg::OnBnClickedButtonEstimate)
END_MESSAGE_MAP()


// Обработчики сообщений CcheckdelayDlg

BOOL CcheckdelayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	srand(time(NULL));
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CcheckdelayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CcheckdelayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int mainThread(double _fd, int _nbits, double _bitrate, double _fc, double _delay, double _snr, double _snr_fully, double duration_base, type_modulation _type, double& result, int N_generate)
{
	int N_exp = N_generate;
	result = 0;
	for (int k = 0; k < N_exp; k++) {
		modulation manipulated;
		manipulated.setParam(_fd, _nbits, _bitrate, _fc, _delay, duration_base, _type);
		manipulated.manipulation();
		auto base_signal = manipulated.createBaseSignal();
		auto fully_signal = manipulated.getS();

		signal base_signal_noise;
		signal fully_signal_noise;
		addNoise(base_signal, _snr, base_signal_noise);
		addNoise(fully_signal, _snr_fully, fully_signal_noise);

		std::vector<double> corr;
		std::vector<double> t_corr;
		correlation(fully_signal_noise, base_signal_noise, 1. / (_fd * 1000), corr, t_corr);
		auto a = findMax(corr, t_corr, _delay);
		if (abs(a.first - _delay / 1000) <= 0.5 / _bitrate) {
			result += 1;
		}
	}
	result /= N_exp;
	return 0;
}

std::pair<double, double> mainProcess(double _fd, int _nbits, double _bitrate, double _fc, double _delay, double _snr, double _snr_fully, double duration_base, type_modulation _type)
{
	modulation manipulated;
	manipulated.setParam(_fd, _nbits, _bitrate, _fc, _delay, duration_base, _type);
	manipulated.manipulation();
	auto base_signal = manipulated.createBaseSignal();
	auto fully_signal = manipulated.getS();
	auto t = manipulated.getT();

	signal base_signal_noise;
	signal fully_signal_noise;
	addNoise(base_signal, _snr, base_signal_noise);
	addNoise(fully_signal, _snr_fully, fully_signal_noise);

	std::vector<double> corr;
	std::vector<double> t_corr;
	correlation(fully_signal_noise, base_signal_noise, 1. / (_fd * 1000), corr, t_corr);
	int sample = fully_signal_noise.N;
	fully_signal_noise.resize(sample);
	base_signal_noise.resize(sample);
	print_graph_file(fully_signal_noise.getSreal(), t, "fully_signal s.txt", "Сигнал полного", "время, с", "");
	print_graph_file(base_signal_noise.getSreal(), t, "base_signal s.txt", "Сигнал опорного", "время, с", "");
	print_graph_file(corr, t_corr, "correlation.txt", "Корреляции", "время, с", "");
	auto a = findMax(corr, t_corr, _delay);
	return a;
}

void CcheckdelayDlg::OnBnClickedButcreate()
{
	// TODO: добавьте свой код обработчика уведомлений 
	UpdateData(TRUE);
	auto mark_delay = mainProcess(fd, nbits, bitrate, fc, delay, snr, snr_fully, sample_base, type);
	if (abs(mark_delay.first - delay / 1000) > 0.5 / bitrate) {
		result_delay.Format(_T("Оцененный сдвиг: %.4f мс; Отличие от заданного: %.3f %%; Некорректно: больше, чем один бит"), mark_delay.first * 1000, mark_delay.second);
	}
	else {
		result_delay.Format(_T("Оцененный сдвиг: %.4f мс; Отличие от заданного: %.3f %%; Корректно: в пределах одного символа"), mark_delay.first * 1000, mark_delay.second);
	}
	UpdateData(FALSE);
	WinExec("python drawing.py", SW_HIDE);
}


void CcheckdelayDlg::OnBnClickedRadioAm()
{
	// TODO: добавьте свой код обработчика уведомлений
	type = AM;
}


void CcheckdelayDlg::OnBnClickedRadioPm()
{
	// TODO: добавьте свой код обработчика уведомлений
	type = PM;
}


void CcheckdelayDlg::OnBnClickedRadioFrm()
{
	// TODO: добавьте свой код обработчика уведомлений
	type = FM;
}

void StartThread(CcheckdelayDlg* dlg)
{
	double min_snr = dlg->min_snr;
	double max_snr = dlg->max_snr;
	double step_snr = dlg->step_snr;
	int num_thread = 6;
	int point = (max_snr - min_snr) / step_snr;
	dlg->vec_p.resize(point);
	dlg->vec_snr.resize(point);
	std::vector<std::thread> thr(num_thread);
	for (int i = 0; i < point; i++) {
		for (int j = 0; j < num_thread; j++) {
			if (i + j >= point) {
				break;
			}
			// Проверяем, можно ли присвоить новый поток
			if (thr[j].joinable()) {
				thr[j].join(); // Ждем завершения предыдущего потока
			}
			dlg->vec_snr[i + j] = min_snr + step_snr * (i + j);
			thr[j] = std::thread(
				mainThread, 
				dlg->fd, 
				dlg->nbits, 
				dlg->bitrate, 
				dlg->fc, 
				dlg->delay, 
				dlg->vec_snr[i + j],
				dlg->snr_fully, 
				dlg->sample_base,
				dlg->type, 
				std::ref(dlg->vec_p[i + j]),
				dlg->N_generate);
		}

		// Ждем завершения всех потоков перед следующей итерацией num_thread * i + j
		for (int j = 0; j < num_thread; j++) {
			if (thr[j].joinable()) {
				thr[j].join();
			}
		}
		i += num_thread - 1;
	}
	print_graph_file(dlg->vec_p, dlg->vec_snr, "experience.txt", "Корректность", "SNR", "Вероятность");
	WinExec("python drawing.py experience.txt", SW_HIDE);
}


void CcheckdelayDlg::OnBnClickedButtonEstimate()
{
	// TODO: добавьте свой код обработчика уведомлений
	std::thread t1(StartThread, this);
	t1.detach();
}
