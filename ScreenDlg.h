#pragma once
#include "afxdialogex.h"

#include <deque>
#include "Common/CDialog/SCShapeDlg/SCShapeDlg.h"

// CScreenDlg 대화 상자

class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CScreenDlg();

	CSCShapeDlg			m_message;

	CSCShapeDlg			m_click_effect;
	//void				set_click_effect_gif

	HBITMAP				m_hBitmap;
	void				set_bitmap(CRect r);

	std::deque<CPoint>	m_pt;		//클릭된 위치 저장
	std::deque<int>		m_delay;	//마지막 클릭 후 다음 클릭까지 걸린 시간 ms
	std::deque<CString> m_action;	//클릭 후 딜레이 경과 후 특정 액션 수행
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
