// ScreenDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MacroClick.h"
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

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CScreenDlg::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
}

void CScreenDlg::OnBnClickedCancel()
{
	ShowWindow(SW_HIDE);
}

void CScreenDlg::set_bitmap(CRect r)
{
	m_hBitmap = capture_screen_to_bitmap(r);
	MoveWindow(r);
	//Invalidate(false);
	ShowWindow(SW_SHOW);

	m_pt.clear();
	m_delay.clear();
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

	rc.DeflateRect(300, 300);
	dc.FillSolidRect(rc, red);

	SelectObject(hdc, hBmpOld);
}

void CScreenDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pt.push_back(point);

	if (m_pt)

	CDialogEx::OnLButtonUp(nFlags, point);
}
