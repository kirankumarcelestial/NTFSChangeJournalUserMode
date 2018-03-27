#if !defined(AFX_FILTERDLG_H__54C30E65_D4F9_4FBF_90AB_CB462C76FFDA__INCLUDED_)
#define AFX_FILTERDLG_H__54C30E65_D4F9_4FBF_90AB_CB462C76FFDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

class CFilterDlg : public CDialog
{
// Construction
public:
	CFilterDlg(CWnd* pParent = NULL);   // standard constructor

	DWORD	getFilter();
	DWORD	m_dwFilter;
// Dialog Data
	//{{AFX_DATA(CFilterDlg)
	enum { IDD = IDD_DLG_FILTER };
	BOOL	m_BASIC_INFO_CHANGE;
	BOOL	m_CLOSE;
	BOOL	m_COMPRESSION_CHANGE;
	BOOL	m_DATA_EXTEND;
	BOOL	m_DATA_OVERWRITE;
	BOOL	m_DATA_TRUNCATION;
	BOOL	m_EA_CHANGE;
	BOOL	m_ENCRYPTION_CHANGE;
	BOOL	m_FILE_CREATE;
	BOOL	m_FILE_DELETE;
	BOOL	m_HARD_LINK_CHANGE;
	BOOL	m_INDEXABLE_CHANGE;
	BOOL	m_NAMED_DATA_EXTEND;
	BOOL	m_NAMED_DATA_OVERWRITE;
	BOOL	m_NAMED_DATA_TRUNCATION;
	BOOL	m_OBJECT_ID_CHANGE;
	BOOL	m_RENAME_NEW_NAME;
	BOOL	m_RENAME_OLD_NAME;
	BOOL	m_REPARSE_POINT_CHANGE;
	BOOL	m_SECURITY_CHANGE;
	BOOL	m_STREAM_CHANGE;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDLG_H__54C30E65_D4F9_4FBF_90AB_CB462C76FFDA__INCLUDED_)
