
// check_delayDlg.h: файл заголовка
//

#pragma once
#include "signalProccesser.h"
#include "print_file_matplotlib.h"


// Диалоговое окно CcheckdelayDlg
class CcheckdelayDlg : public CDialogEx
{
// Создание
public:
	CcheckdelayDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHECK_DELAY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButcreate();
	double fd;
	int nbits;
	double bitrate;
	double fc;
	double delay;
	double snr;
	modulation modul_signal;
	CString result_delay;
	type_modulation type;
	afx_msg void OnBnClickedRadioAm();
	afx_msg void OnBnClickedRadioPm();
	afx_msg void OnBnClickedRadioFrm();
	double sample_base;
	double snr_fully;

	std::vector<std::vector<double>> vec_p = std::vector<std::vector<double>>(3);
	std::vector<double> vec_snr;

	afx_msg void OnBnClickedButtonEstimate();
	double min_snr;
	double max_snr;
	double step_snr;
	double N_generate;
	CProgressCtrl progress_experement;
	afx_msg void OnBnClickedButtonDrawOne();
	afx_msg void OnBnClickedButtonDrawMany();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonDrawOne2();
};

int mainThread(double _fd, int _nbits, double _bitrate, double _fc, double _delay, double _snr, double _snr_fully, double duration_base, type_modulation _type, double& result, int N_generate);

std::pair<double, double> mainProcess(double _fd, int _nbits, double _bitrate, double _fc, double _delay, double _snr, double _snr_fully, double duration_base, type_modulation _type);std::pair<double, double> mainProcess(double _fd, int _nbits, double _bitrate, double _fc, double _delay, double _snr, double _snr_fully, double duration_base, type_modulation _type);

void StartThread(CcheckdelayDlg* dlg);