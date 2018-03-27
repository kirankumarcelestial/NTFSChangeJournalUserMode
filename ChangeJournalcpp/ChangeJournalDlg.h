// ChangeJournalDlg.h : header file
//

#if !defined(AFX_CHANGEJOURNALDLG_H__A4251921_8D43_4B5A_B4E7_9304EB029955__INCLUDED_)
#define AFX_CHANGEJOURNALDLG_H__A4251921_8D43_4B5A_B4E7_9304EB029955__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CChangeJournalDlg dialog

class CChangeJournalDlg : public CDialog
{
// Construction
public:
	CChangeJournalDlg(CWnd* pParent = NULL);	// standard constructor

	void	LoadListHead();
	void	AddRecords(const CJRecords& recs);

// Dialog Data
	//{{AFX_DATA(CChangeJournalDlg)
	enum { IDD = IDD_CHANGEJOURNAL_DIALOG };
	CComboBox	m_cmbBlock;
	CComboBox	m_cmbDrive;
	CListCtrl	m_lst;
	CString		m_strDrive;
	CString		m_strDetail;
	CString		m_strQuery;
	UINT		m_nPage;
	CString		m_strBlock;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeJournalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CChangeJournalDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnFilter();
	afx_msg void OnBtnFirstPage();
	afx_msg void OnBtnQuery();
	afx_msg void OnKillfocusCmbBlock();
	afx_msg void OnBtnPrev();
	afx_msg void OnBtnNextPage();
	virtual void OnOK();
	afx_msg void OnCreate();
	afx_msg void OnDisable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CFilterDlg			m_dlgFilter;	
	HCURSOR				m_curBusy;
	HCURSOR				m_curOK;

	CChangeJournal		m_ccj;
	USN_JOURNAL_DATA	m_usnJournalData;
	CJRecords			m_cjrecs;
	USN					m_nextUsn;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEJOURNALDLG_H__A4251921_8D43_4B5A_B4E7_9304EB029955__INCLUDED_)
