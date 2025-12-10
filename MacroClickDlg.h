
// MacroClickDlg.h: 헤더 파일
//

#pragma once

#include "ScreenDlg.h"
#include "Common/ResizeCtrl.h"
#include "Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"

// CMacroClickDlg 대화 상자
class CMacroClickDlg : public CDialogEx
{
// 생성입니다.
public:
	CMacroClickDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CResizeCtrl				m_resize;
	CScreenDlg				m_screenDlg;

	enum LIST_COLUMN
	{
		col_index = 0,
		col_point_x,
		col_point_y,
		col_delay,
		col_post_action,
	};
	void					init_list();
	void					load_registry();
	void					save_registry();

	void					macro_recording_finished();

	enum TIMER_ID
	{
		timer_play,
	};
	int						m_play_index = 0;
	int						m_play_loop_max = 1;
	int						m_play_loop_count = 0;
	bool					m_play_cancelled = false;
	LRESULT					OnMessageKeyboardScanCode(WPARAM, LPARAM);

	//특정 순번의 액션 및 딜레이가 경과된 후 정해진 액션을 수행한다.
	//현재는 x1, y1, x2, y2, folder 순서를 주면 해당 영역을 캡처하여 folder에 저장하는 액션만 구현됨.
	void					post_action();


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MACROCLICK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CVtListCtrlEx m_list;
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CSCEdit m_edit_repeat;
	afx_msg void OnLvnEndLabelEditList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
