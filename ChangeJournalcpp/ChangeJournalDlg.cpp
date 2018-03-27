// ChangeJournalDlg.cpp : implementation file
/*
	Note: much code copied from Jeffrey Cooperstein & Jeffrey Richter'
	demo of the article 'Keeping an Eye on Your NTFS Drives, Part II: Building a Change Journal Application  '
	if copy, please note
*/
//

#include "stdafx.h"
#include "ChangeJournal.h"
#include "ChangeJournalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString get64String(LONGLONG number)
{
	CString str;
	str.Format("%016I64X", number);
	return str;
}

char* get_c_string(const WCHAR* wstring)
{
	static char dest[256];
	memset(dest, 0, 256);
    WideCharToMultiByte( CP_ACP, 0, wstring, -1,
        dest, 256, NULL, NULL );
	
	return dest;
}

void RecursePathFind(LPCSTR path, DWORDLONG frn, CString &filename, BOOL &found)
{
	CString szPath2;
	szPath2.Format("%s\\", path);
	HANDLE hDir = CreateFile(szPath2, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	
	if (INVALID_HANDLE_VALUE == hDir) return;

	BY_HANDLE_FILE_INFORMATION fi;
	GetFileInformationByHandle(hDir, &fi);
	CloseHandle(hDir);

	DWORDLONG index = ((((DWORDLONG) fi.nFileIndexHigh) << 32) | fi.nFileIndexLow);
	if(frn == index ) {
		filename = szPath2;
		found = TRUE;
		return;
	}

	TCHAR szTempPath[_MAX_PATH];
    lstrcpy(szTempPath, szPath2);
    lstrcat(szTempPath, TEXT("*.*"));
	WIN32_FIND_DATA fd;
	HANDLE hSearch = FindFirstFile(szTempPath, &fd);
	if (INVALID_HANDLE_VALUE == hSearch)   return;

	do {
	  TCHAR szTempRecursePath[_MAX_PATH];
	  lstrcpy(szTempRecursePath, szPath2);
	  lstrcat(szTempRecursePath, fd.cFileName);
	  if (   (0 != (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		  && (0 != lstrcmp(fd.cFileName, TEXT(".")))
		  && (0 != lstrcmp(fd.cFileName, TEXT("..")))
		  && (0 == (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) ) {

		  if(frn == ((((DWORDLONG) fi.nFileIndexHigh) << 32) | fi.nFileIndexLow)) {
				filename = szTempRecursePath;
				found = TRUE;
				break;
		  }
		  else{
				RecursePathFind(szTempRecursePath, frn, filename, found);
		  }
		  
	  }
	} while (FindNextFile(hSearch, &fd) != FALSE);

	FindClose(hSearch);
	
}

CString getFullFileName(TCHAR drive, DWORDLONG fileReferenceNumber)
{
	CString filename;
	
	TCHAR path[MAX_PATH];
	memset(path, 0, MAX_PATH);
	sprintf(path, "%c:", drive);

	BOOL found = FALSE;
	RecursePathFind(path, fileReferenceNumber, filename, found);
	
	return filename;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeJournalDlg dialog

CChangeJournalDlg::CChangeJournalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeJournalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeJournalDlg)
	m_strDrive = _T("C");
	m_strDetail = _T("");
	m_strQuery = _T("");
	m_nPage = 0;
	m_strBlock = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChangeJournalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeJournalDlg)
	DDX_Control(pDX, IDC_CMB_BLOCK, m_cmbBlock);
	DDX_Control(pDX, IDC_CMB_DRIVE, m_cmbDrive);
	DDX_Control(pDX, IDC_LIST, m_lst);
	DDX_CBString(pDX, IDC_CMB_DRIVE, m_strDrive);
	DDX_Text(pDX, IDC_DETAIL, m_strDetail);
	DDX_Text(pDX, IDC_QUERY, m_strQuery);
	DDX_Text(pDX, IDC_PAGE, m_nPage);
	DDV_MinMaxUInt(pDX, m_nPage, 0, 99999999);
	DDX_CBString(pDX, IDC_CMB_BLOCK, m_strBlock);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeJournalDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeJournalDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_FILTER, OnBtnFilter)
	ON_BN_CLICKED(IDC_BTN_FIRST_PAGE, OnBtnFirstPage)
	ON_BN_CLICKED(IDC_BTN_QUERY, OnBtnQuery)
	ON_CBN_KILLFOCUS(IDC_CMB_BLOCK, OnKillfocusCmbBlock)
	ON_BN_CLICKED(IDC_BTN_NEXT_PAGE, OnBtnNextPage)
	ON_COMMAND(IDM_CREATE, OnCreate)
	ON_COMMAND(IDM_DISABLE, OnDisable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeJournalDlg message handlers

BOOL CChangeJournalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	LoadListHead();
	m_curBusy = LoadCursor(::AfxGetInstanceHandle(), IDC_WAIT);
	m_curOK = LoadCursor(::AfxGetInstanceHandle(), IDC_ARROW);	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChangeJournalDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChangeJournalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChangeJournalDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CChangeJournalDlg::OnBtnFilter()
{
	m_dlgFilter.DoModal();
}

void CChangeJournalDlg::OnBtnFirstPage() 
{
	UpdateData(TRUE);

	if(m_strQuery.IsEmpty()) {
		AfxMessageBox("query first");
		return;
	}

	m_cjrecs.clear();
	USN usn = m_usnJournalData.FirstUsn;
	if(!m_ccj.getRecords(usn, m_usnJournalData.UsnJournalID,
					&m_cjrecs,m_dlgFilter.m_dwFilter)){
	
		AfxMessageBox(m_ccj.getError());
	}
	else{
		m_nPage = 1;
		m_nextUsn = m_cjrecs[m_cjrecs.size()-1]._usn;
		AddRecords(m_cjrecs);
	}

	UpdateData(FALSE);
}

void CChangeJournalDlg::LoadListHead()
{
	m_lst.InsertColumn( 0, "USN", LVCFMT_LEFT, 140 );
	m_lst.InsertColumn( 1, "Reason", LVCFMT_LEFT, 160 );
	m_lst.InsertColumn( 2, "fileName", LVCFMT_LEFT, 140 );
	m_lst.InsertColumn( 3, "Time", LVCFMT_LEFT, 200 );
}

void CChangeJournalDlg::AddRecords(const CJRecords& recs)
{
	m_lst.DeleteAllItems();
	TCHAR reason[MAX_PATH];
	for(int i=0; i<recs.size(); ++i){
		m_lst.InsertItem(i, "");

		ZeroMemory(reason, MAX_PATH);
		CChangeJournal::GetReasonString(recs[i]._reason, reason, MAX_PATH);

		m_lst.SetItem(i, 0, LVIF_TEXT, get64String(recs[i]._usn), 0, 0, 0, 0);
		m_lst.SetItem(i, 1, LVIF_TEXT, reason, 0, 0, 0, 0);
		m_lst.SetItem(i, 2, LVIF_TEXT, get_c_string(recs[i]._filename.c_str()), 0, 0, 0, 0);
		m_lst.SetItem(i, 3, LVIF_TEXT, CChangeJournal::getTimestamp(&(recs[i]._timestamp)), 0, 0, 0, 0);
	}
}


void CChangeJournalDlg::OnBtnQuery() 
{
	UpdateData(TRUE);
	
	m_ccj.setDriver(m_strDrive.GetAt(0));
	if(!m_ccj.open()){
		AfxMessageBox(m_ccj.getError());
		return;
	}

	if(m_ccj.query(&m_usnJournalData)){
		m_strQuery.Format("UsnJournalID = %lu, Maximunsize = %lu",
			m_usnJournalData.UsnJournalID , m_usnJournalData.MaximumSize );
		
	}
	else{		
		m_strQuery = "";
		AfxMessageBox(m_ccj.getError());
	}

	UpdateData(FALSE);
}


void CChangeJournalDlg::OnKillfocusCmbBlock() 
{
	UpdateData(TRUE);
	if(atol((LPCSTR)m_strBlock)>0)
		m_ccj.setMemoryBlock(atol((LPCSTR)m_strBlock));
	UpdateData(FALSE);	
}


void CChangeJournalDlg::OnBtnNextPage() 
{
	UpdateData(TRUE);

	if(m_nPage == 0) {
		AfxMessageBox("first page first");
		return;
	}

	m_cjrecs.clear();
	USN usn = m_nextUsn;
	if(!m_ccj.getRecords(usn, m_usnJournalData.UsnJournalID,
					&m_cjrecs,m_dlgFilter.m_dwFilter)){
	
		AfxMessageBox(m_ccj.getError());
	}
	else{
		if(m_cjrecs.size()>1){
			m_nPage++;
			m_nextUsn = m_cjrecs[m_cjrecs.size()-1]._usn;
			AddRecords(m_cjrecs);
		}
		else{
			AfxMessageBox("no more data");
		}
	}

	UpdateData(FALSE);
}

void CChangeJournalDlg::OnOK() 
{
	POSITION pos = m_lst.GetFirstSelectedItemPosition();
	if (pos == NULL){
		AfxMessageBox("choose a record first");
		return;
	}

	int nItem = m_lst.GetNextSelectedItem(pos);

	UpdateData(TRUE);
	SetCursor(m_curBusy);
	m_strDetail = getFullFileName(m_ccj.getDrive(), m_cjrecs[nItem]._ParentFRN);
	SetCursor(m_curOK);
	UpdateData(FALSE);
}

void CChangeJournalDlg::OnCreate() 
{
	m_ccj.setDriver(m_strDrive.GetAt(0));
	m_ccj.open();

	if(m_ccj.create()) AfxMessageBox("create change journal o.k.");
	else AfxMessageBox(m_ccj.getError());
}

void CChangeJournalDlg::OnDisable() 
{
	if(m_strQuery.IsEmpty()){
		AfxMessageBox("query first");
		return;
	}

	if(m_ccj.disable(m_usnJournalData.UsnJournalID)) AfxMessageBox("disable change journal o.k.");
	else AfxMessageBox(m_ccj.getError());
}
