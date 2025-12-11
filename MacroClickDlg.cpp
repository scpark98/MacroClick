
// MacroClickDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MacroClick.h"
#include "MacroClickDlg.h"
#include "afxdialogex.h"

#include "Common/Functions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//출처: http://pooppoop365.tistory.com/entry/초보자용-키보드-후킹
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
HHOOK m_hook = NULL;
HWND g_hMainWnd;

#define MSG_KEYBOARD_SCAN_CODE	WM_USER + 0x7FFF - 565


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMacroClickDlg 대화 상자



CMacroClickDlg::CMacroClickDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MACROCLICK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMacroClickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_REPEAT, m_edit_repeat);
}

BEGIN_MESSAGE_MAP(CMacroClickDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMacroClickDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMacroClickDlg::OnBnClickedCancel)
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CMacroClickDlg::OnBnClickedButtonPlay)
	ON_MESSAGE(MSG_KEYBOARD_SCAN_CODE, &CMacroClickDlg::OnMessageKeyboardScanCode)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, &CMacroClickDlg::OnLvnEndLabelEditList)
END_MESSAGE_MAP()


// CMacroClickDlg 메시지 처리기

BOOL CMacroClickDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_screenDlg.Create(IDD_SCREEN, this);
	m_screenDlg.SetWindowPos(NULL, 0, 0, 1920, 1080, SWP_NOACTIVATE);

	init_list();

	CString caption;
	caption.Format(_T("MacroClick (ver %s)"), get_file_property());
	SetWindowText(caption);

	m_resize.Create(this);
	m_resize.Add(IDC_LIST, 0, 0, 100, 100);
	m_resize.Add(IDC_BUTTON_PLAY, 0, 100, 0, 0);
	m_resize.Add(IDC_STATIC_REPEAT, 0, 100, 0, 0);
	m_resize.Add(IDC_EDIT_REPEAT, 0, 100, 0, 0);
	m_resize.Add(IDOK, 100, 100, 0, 0);

	RestoreWindowPosition(&theApp, this);

	g_hMainWnd = m_hWnd;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMacroClickDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMacroClickDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMacroClickDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMacroClickDlg::OnBnClickedOk()
{
	if (m_list.size() > 0)
	{
		int res = AfxMessageBox(_T("새로 기록을 시작할 경우 기존 기록이 모두 초기화됩니다.\n계속 진행할까요?"), MB_ICONQUESTION | MB_YESNO);
		if (res == IDNO)
			return;
	}

	ShowWindow(SW_MINIMIZE);

	Wait(500);

	std::deque<CRect> vtMonitors;
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&vtMonitors);

	for (int i = 1; i < vtMonitors.size(); i++)
		vtMonitors[0].UnionRect(vtMonitors[0], vtMonitors[i]);

	m_screenDlg.set_bitmap(vtMonitors[0]);

	m_list.delete_all_items();
}

void CMacroClickDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UnhookWindowsHookEx(m_hook);

	save_registry();

	CDialogEx::OnCancel();
}

void CMacroClickDlg::macro_recording_finished()
{
	save_registry();

	for (int i = 0; i < m_screenDlg.m_pt.size(); i++)
	{
		m_list.insert_item(-1, i2S(i));
		m_list.set_text(i, col_point_x, i2S(m_screenDlg.m_pt[i].x));
		m_list.set_text(i, col_point_y, i2S(m_screenDlg.m_pt[i].y));
		m_list.set_text(i, col_delay, i2S(m_screenDlg.m_delay[i]));
	}

	if (IsIconic())
		ShowWindow(SW_RESTORE);
}

void CMacroClickDlg::init_list()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_GRIDLINES);

	m_list.set_headings(_T("순번,50;x,50;y,50;후 딜레이,100;수행 후 액션,200"));
	m_list.set_header_height(24);
	m_list.set_line_height(22);
	m_list.set_font_size(9);

	m_list.allow_edit();
	m_list.allow_sort(false);

	m_list.allow_edit_column(col_index, false);

	m_list.restore_column_width(&theApp, _T("list"));

	load_registry();
}

void CMacroClickDlg::load_registry()
{
	int count = theApp.GetProfileInt(_T("setting\\list"), _T("count"), 0);
	CString str;

	for (int i = 0; i < count; i++)
	{
		str = theApp.GetProfileString(_T("setting\\list"), i2S(i), _T(""));
		std::deque<CString> token;

		get_token_string(str, token, _T("|"));

		if (token.size() < 3)
			continue;

		m_screenDlg.m_pt.push_back(CPoint(_ttoi(token[0]), _ttoi(token[1])));
		m_screenDlg.m_delay.push_back(_ttoi(token[2]));
		m_screenDlg.m_action.push_back((token.size() == 4) ? token[3] : _T(""));

		m_list.insert_item(-1, i2S(i));
		m_list.set_text(i, col_point_x, i2S(m_screenDlg.m_pt[i].x));
		m_list.set_text(i, col_point_y, i2S(m_screenDlg.m_pt[i].y));
		m_list.set_text(i, col_delay, i2S(m_screenDlg.m_delay[i]));
		m_list.set_text(i, col_post_action, m_screenDlg.m_action[i]);
	}

	int repeat = theApp.GetProfileInt(_T("setting"), _T("repeat"), 1);
	m_edit_repeat.set_text(repeat);
}

void CMacroClickDlg::save_registry()
{
	CString str;

	int count = m_screenDlg.m_pt.size();
	theApp.WriteProfileInt(_T("setting\\list"), _T("count"), count);

	for (int i = 0; i < count; i++)
	{
		str.Format(_T("%d|%d|%d|%s"), m_screenDlg.m_pt[i].x, m_screenDlg.m_pt[i].y, m_screenDlg.m_delay[i], m_screenDlg.m_action[i]);
		theApp.WriteProfileString(_T("setting\\list"), i2S(i), str);
	}

	int repeat = _ttoi(m_edit_repeat.get_text());
	theApp.WriteProfileInt(_T("setting"), _T("repeat"), repeat);
}

void CMacroClickDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this);
}

void CMacroClickDlg::OnBnClickedButtonPlay()
{
	if (m_list.size() == 0)
		return;

	save_registry();

	CString text = m_edit_repeat.get_text();
	m_play_loop_max = _ttoi(text);

	m_hook = SetWindowsHookEx(WH_KEYBOARD_LL, GetMsgProc, AfxGetApp()->m_hInstance, NULL);
	if (m_hook == NULL)
	{
		AfxMessageBox(_T("Keyboard hooking start error."));
		return;
	}

	m_play_index = 0;
	m_play_loop_count = 0;
	m_play_cancelled = false;

	SetTimer(timer_play, 1000, NULL);
}

void CMacroClickDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_play)
	{
		KillTimer(timer_play);

		CPoint pt;
		CString str;
		str = m_list.get_text(m_play_index, col_point_x);
		pt.x = _ttoi(str);
		str = m_list.get_text(m_play_index, col_point_y);
		pt.y = _ttoi(str);

		m_list.select_item(m_play_index, true, true);

		SetCursorPos(pt.x, pt.y);
		sc_mouse_event(mouse_event_lclick);
		Wait(m_screenDlg.m_delay[m_play_index]);

		post_action();

		if (m_play_cancelled)
		{
			TRACE(_T("cancelled.\n"));
			UnhookWindowsHookEx(m_hook);
			SetForegroundWindowForce(m_hWnd);
			return;
		}

		trace(m_play_index);
		m_play_index++;

		if (m_play_index == m_list.size())
		{
			m_play_index = 0;
			m_play_loop_count++;
			trace(m_play_loop_count);

			if (m_play_loop_max > 0 && (m_play_loop_count >= m_play_loop_max))
			{
				TRACE(_T("play finished.\n"));
				UnhookWindowsHookEx(m_hook);
				SetForegroundWindowForce(m_hWnd);
				return;
			}
		}

		SetTimer(timer_play, 1, NULL);
	}

	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	static int c = 0;
	CString str;

	KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

	if (nCode < 0)  // do not process message
		return CallNextHookEx(m_hook, nCode, wParam, lParam);

	if (wParam == WM_KEYDOWN)
	{
		trace(pKey->vkCode);
		::SendMessage(g_hMainWnd, MSG_KEYBOARD_SCAN_CODE, (WPARAM)pKey, (LPARAM)0);
	}

	return CallNextHookEx(m_hook, nCode, wParam, lParam);
}

LRESULT CMacroClickDlg::OnMessageKeyboardScanCode(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("OnMessageKeyboardScanCode\n"));

	KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)wParam;
	CString str;
	str.Format(_T("scanCode = %d, vkCode = %d, alt = %d"), pKey->scanCode, pKey->vkCode, (bool)lParam);
	TRACE(_T("%s\n"), str);

	if (pKey->vkCode == VK_ESCAPE)
	{
		KillTimer(timer_play);
		m_play_cancelled = true;
	}

	return 0;
}

void CMacroClickDlg::OnLvnEndLabelEditList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = m_list.get_recent_edit_item();
	int subitem = m_list.get_recent_edit_subitem();
	CString text = m_list.get_text(item, subitem);

	switch (subitem)
	{
		case col_point_x:
			m_screenDlg.m_pt[item].x = _ttoi(text);
			break;
		case col_point_y:
			m_screenDlg.m_pt[item].y = _ttoi(text);
			break;
		case col_delay:
			m_screenDlg.m_delay[item] = _ttoi(text);
			break;
		case col_post_action:
			m_screenDlg.m_action[item] = text;
			break;
	}

	save_registry();

	*pResult = 0;
}

BOOL CMacroClickDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_DELETE:
				if (GetFocus() == &m_list)
				{
					std::deque<int> dq;
					
					m_list.get_selected_items(&dq);

					for (int i = dq.size() - 1; i >= 0; i--)
					{
						m_list.delete_item(dq[i]);
						m_screenDlg.m_pt.erase(m_screenDlg.m_pt.begin() + dq[i]);
						m_screenDlg.m_delay.erase(m_screenDlg.m_delay.begin() + dq[i]);
					}

					save_registry();
					return TRUE;
				}
				break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//특정 순번의 액션 및 딜레이가 경과된 후 정해진 액션을 수행한다.
//현재는 x1, y1, x2, y2, folder 순서를 주면 해당 영역을 캡처하여 folder에 저장하는 액션만 구현됨.
void CMacroClickDlg::post_action()
{
	CString str_action;

	str_action = m_list.get_text(m_play_index, col_post_action);

	if (str_action.IsEmpty())
		return;

	std::deque<CString> token;
	get_token_string(str_action, token, _T(","), false);
	if (token.size() != 5)
		return;

	//for (int i = 0; i < 4; i++)

	CRect r(_ttoi(token[0]), _ttoi(token[1]), _ttoi(token[2]), _ttoi(token[3]));
	CString folder = token[4];
	CString file;

	folder.Trim();

	make_full_directory(folder);
	CString str = get_error_str(GetLastError());

	file.Format(_T("%s\\%s.jpg"), folder, get_cur_datetime_str(2, false));
	capture_window(r, file);
}
