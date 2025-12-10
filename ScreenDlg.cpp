// ScreenDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MacroClick.h"
#include "MacroClickDlg.h"
#include "afxdialogex.h"
#include "ScreenDlg.h"

#include "Common/Functions.h"
#include "Common/MemoryDC.h"

// CScreenDlg 대화 상자

IMPLEMENT_DYNAMIC(CScreenDlg, CDialogEx)

CScreenDlg::CScreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCREEN, pParent)
{

}

CScreenDlg::~CScreenDlg()
{
}

void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CScreenDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CScreenDlg::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CScreenDlg 메시지 처리기

BOOL CScreenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_message.set_text(this, _T(""), 28, Gdiplus::FontStyleBold, 4.0f, 2.4f);
	m_message.set_text_color(Gdiplus::Color::Beige);
	m_message.set_stroke_color(Gdiplus::Color::Black);
	m_message.set_alpha(192);
	m_message.use_control(false);

	m_click_effect.load(this, _T("GIF"), (UINT)IDR_GIF_NHQV_05);
	m_click_effect.use_control(false);
	m_click_effect.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CScreenDlg::OnBnClickedOk()
{
	OnBnClickedCancel();
}

void CScreenDlg::OnBnClickedCancel()
{
	m_message.ShowWindow(SW_HIDE);
	m_message.set_text(_T(""));
	m_click_effect.gif_stop();
	m_click_effect.ShowWindow(SW_HIDE);

	ShowWindow(SW_HIDE);

	//레코딩이 끝나면 맨 마지막 딜레이를 1초로 설정한다.
	if (m_pt.size() > 0)
	{
		m_delay.push_back(1000);
		m_action.resize(m_pt.size());
	}

	for (int i = 0; i < m_pt.size(); i++)
	{
		TRACE(_T("pt = %d, %d, delay = %ld\n"), m_pt[i].x, m_pt[i].y, m_delay[i]);
	}

	((CMacroClickDlg*)(AfxGetApp()->GetMainWnd()))->macro_recording_finished();
}

void CScreenDlg::set_bitmap(CRect r)
{
	m_hBitmap = capture_screen_to_bitmap(r);
	MoveWindow(r);
	ShowWindow(SW_SHOW);
	SetForegroundWindowForce(m_hWnd, false);

	m_pt.clear();
	m_delay.clear();

	m_message.set_text(_T("원하는 위치를 모두 클릭한 후\nEsc키를 눌러 위치 저장을 중단할 수 있습니다."));
	m_message.CenterWindow();// GetDesktopWindow());
	m_message.fade_in(0, 1000, true);

	SetFocus();
}
BOOL CScreenDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialogEx::OnEraseBkgnd(pDC);
}

void CScreenDlg::OnPaint()
{
	CPaintDC dc1(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	CRect rc;
	GetClientRect(rc);
	CMemoryDC dc(&dc1, &rc);

	HDC hdc = CreateCompatibleDC(dc.GetSafeHdc());
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdc, m_hBitmap);
	::BitBlt(dc.GetSafeHdc(), 0, 0, rc.Width(), rc.Height(), hdc, 0, 0, SRCCOPY);

	SelectObject(hdc, hBmpOld);
}

void CScreenDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_click_effect.SetWindowPos(NULL, point.x - m_click_effect.m_img.width / 2, point.y - m_click_effect.m_img.height / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_click_effect.fade_in(10, 1000, true);

	m_pt.push_back(point);

	if (m_pt.size() == 1)
	{
		m_time_clicked = clock();
	}
	else
	{
		m_delay.push_back(clock() - m_time_clicked);
		m_time_clicked = clock();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

BOOL CScreenDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_ESCAPE:
				OnBnClickedCancel();
				break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
