#pragma once

#include <windows.h>
#include <memory>
# include <string>
class NTFSChangesWatcher
{
public:
	NTFSChangesWatcher(char drive_letter, TCHAR* IoPath);
	~NTFSChangesWatcher() = default;

	// Method which runs an infinite loop and waits for new update sequence number in a journal.
	// The thread is blocked till the new USN record created in the journal.
	void WatchChanges();

private:

	TCHAR *m_IoPath;
	FILE_ID_DESCRIPTOR getFileIdDescriptor(const DWORDLONG fileId);
	
	//FILE_ID_DESCRIPTOR getFileIdDescriptor(const FILE_ID_128 fileId);


	HANDLE OpenVolume(char drive_letter);

	bool CreateJournal(HANDLE volume);

	bool LoadJournal(HANDLE volume, USN_JOURNAL_DATA* journal_data);

	bool WaitForNextUsn(PREAD_USN_JOURNAL_DATA read_journal_data) const;

	std::unique_ptr<READ_USN_JOURNAL_DATA> GetWaitForNextUsnQuery(USN start_usn);

	bool ReadJournalRecords(PREAD_USN_JOURNAL_DATA journal_query, LPVOID buffer,
		DWORD& byte_count) const;

	std::unique_ptr<READ_USN_JOURNAL_DATA> GetReadJournalQuery(USN low_usn);


	char drive_letter_;

	HANDLE volume_;

	std::unique_ptr<USN_JOURNAL_DATA> journal_;

	DWORDLONG journal_id_;

	USN last_usn_;

	// Flags, which indicate which types of changes you want to listen.
	static const int FILE_CHANGE_BITMASK;

	static const int kBufferSize;
};
