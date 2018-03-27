/* 
	File: CChangeJournal.h
	Description:
		define class 'CChangeJournal' which can be used to read information
		from Change Journal from NTFS volume

	Author: Jason Zhang
	Notice:
		many code here is from article and demo of 
		'Keeping an Eye on Your NTFS Drives: the Windows 2000 Change Journal Explained '
			written by Jeffrey Cooperstein & Jeffrey Richter.
		the articles(2) can be found on msdn
*/

#ifndef CChangeJournal_H
#define CChangeJournal_H

#define _WIN32_WINNT 0x0500
#define _UNICODE
#define UNICODE

#include <windows.h>
#include <winioctl.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include <string>
#include <vector>
using namespace std; 

struct CJRecord
{
	USN				_usn;
	DWORD			_reason;
	DWORDLONG		_ParentFRN;
	LARGE_INTEGER	_timestamp;
#ifdef UNICODE
	wstring			_filename;
#else 
	string			_filename;
#endif
};
typedef vector<CJRecord> CJRecords;

void printOnConsole(const CJRecords * const recs);

class CChangeJournal
{
public:
	CChangeJournal(TCHAR drive='C');
	~CChangeJournal();
	void setDriver(TCHAR drive);

	BOOL create(DWORDLONG MaximumSize=0, DWORDLONG AllocationDelta=0);
	BOOL disable(DWORDLONG UsnJournalID=0);

	BOOL open();	//open file handle
	void close();	//close file handle

	BOOL query(PUSN_JOURNAL_DATA pUsnJournalData);
	BOOL setMemoryBlock(DWORD bytes = 1024 * 1024); //alocate memory
	BOOL getRecords(USN &usn, DWORDLONG UsnJournalID, CJRecords* recs, DWORD filter = 0xFFFFFFFF);

	TCHAR  getDrive(){return m_drive;}
	TCHAR* getError(){return m_error;}

	static BOOL GetReasonString(DWORD dwReason, LPTSTR pszReason, int cchReason);
	static TCHAR* getTimestamp(const LARGE_INTEGER * timestamp);
private:
	
	HANDLE					m_hCJ;
	READ_USN_JOURNAL_DATA	m_rujd;       // parameters for reading records
	PBYTE					m_pbCJData;   // buffer(heap) of records
	
	TCHAR					m_drive;
	TCHAR*					m_error;
};

#endif //CChangeJournal_H
