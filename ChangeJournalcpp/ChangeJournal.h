// ChangeJournal.h : main header file for the CHANGEJOURNAL application
//

#if !defined(AFX_CHANGEJOURNAL_H__19288F1D_46A9_4D4B_BDCA_BA51E3956557__INCLUDED_)
#define AFX_CHANGEJOURNAL_H__19288F1D_46A9_4D4B_BDCA_BA51E3956557__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "CChangeJournal.h"

/////////////////////////////////////////////////////////////////////////////
// CChangeJournalApp:
// See ChangeJournal.cpp for the implementation of this class
//

class CChangeJournalApp : public CWinApp
{
public:
	CChangeJournalApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeJournalApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CChangeJournalApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEJOURNAL_H__19288F1D_46A9_4D4B_BDCA_BA51E3956557__INCLUDED_)
