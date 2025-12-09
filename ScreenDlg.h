#pragma once
#include "afxdialogex.h"

#include <deque>

// CScreenDlg 대화 상자

class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CScreenDlg();

	HBITMAP		m_hBitmap;
	void		set_bitmap(CRect r);

	std::deque<CPoint>	m_pt;
	std::deque<int>		m_delay;
	long				m_time_clicked = 0;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCREEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
