#include "stdafx.h"
#include "NTFSChangeWatcher.h"
#include <iostream>

using namespace std;

const int NTFSChangesWatcher::kBufferSize = 1024 * 1024 / 2;

const int NTFSChangesWatcher::FILE_CHANGE_BITMASK =
USN_REASON_RENAME_NEW_NAME | USN_REASON_SECURITY_CHANGE | USN_REASON_BASIC_INFO_CHANGE | USN_REASON_DATA_OVERWRITE |
USN_REASON_DATA_TRUNCATION | USN_REASON_DATA_EXTEND | USN_REASON_CLOSE;


NTFSChangesWatcher::NTFSChangesWatcher(char drive_letter,  TCHAR* ioPath) :
	drive_letter_(drive_letter),
	m_IoPath(ioPath)
{

	HANDLE h = CreateFile(m_IoPath,
		GENERIC_READ,              // dwDesiredAccess
		FILE_SHARE_READ | FILE_SHARE_WRITE,  // share mode
		NULL,                                                    // default security attributes
		OPEN_EXISTING,                                           // disposition
																 // It is always set, no matter whether you explicitly specify it or not. This means, that access
																 // must be aligned with sector size so we can only read a number of bytes that is a multiple of the sector size.
		FILE_FLAG_BACKUP_SEMANTICS,  // file attributes
		NULL);

	TCHAR filePath[MAX_PATH];
	GetFinalPathNameByHandle(h, m_IoPath, MAX_PATH, 0);
	
	volume_ = OpenVolume(drive_letter_);

	journal_ = make_unique<USN_JOURNAL_DATA>();

	bool res = LoadJournal(volume_, journal_.get());

	if (!res) {
		cout << "Failed to load journal" << endl;
		return;
	}

	journal_id_ = journal_->UsnJournalID;
	last_usn_ = journal_->NextUsn;
}

FILE_ID_DESCRIPTOR NTFSChangesWatcher::getFileIdDescriptor(const DWORDLONG fileId)
{
	FILE_ID_DESCRIPTOR fileDescriptor;
	fileDescriptor.Type = FileIdType;
	fileDescriptor.FileId.QuadPart = fileId;
	fileDescriptor.dwSize = sizeof(fileDescriptor);

	return fileDescriptor;
}

HANDLE NTFSChangesWatcher::OpenVolume(char drive_letter) {

	wchar_t pattern[10] = L"\\\\.\\C:";

	
	HANDLE volume = nullptr;

	
	volume = CreateFile(
		pattern,  // lpFileName
				  // also could be | FILE_READ_DATA | FILE_READ_ATTRIBUTES | SYNCHRONIZE
		GENERIC_READ ,              // dwDesiredAccess
		FILE_SHARE_READ | FILE_SHARE_WRITE,  // share mode
		NULL,                                                    // default security attributes
		OPEN_EXISTING,                                           // disposition
																 // It is always set, no matter whether you explicitly specify it or not. This means, that access
																 // must be aligned with sector size so we can only read a number of bytes that is a multiple of the sector size.
		0,  // file attributes
		NULL                     // do not copy file attributes
	);

	

	if (volume == INVALID_HANDLE_VALUE) {
		// An error occurred!
		cout << "Failed to open volume" << endl;
		return nullptr;
	}

	return volume;
}


bool NTFSChangesWatcher::CreateJournal(HANDLE volume) {

	DWORD byte_count;
	CREATE_USN_JOURNAL_DATA create_journal_data;

	bool ok = DeviceIoControl(volume, // handle to volume
		FSCTL_CREATE_USN_JOURNAL,     // dwIoControlCode
		&create_journal_data,         // input buffer
		sizeof(create_journal_data),  // size of input buffer
		NULL,                         // lpOutBuffer
		0,                            // nOutBufferSize
		&byte_count,                  // number of bytes returned
		NULL) != 0;                   // OVERLAPPED structure

	if (!ok) {
		// An error occurred!
	}

	return ok;
}


bool NTFSChangesWatcher::LoadJournal(HANDLE volume, USN_JOURNAL_DATA* journal_data) {

	DWORD byte_count;

	// Try to open journal.
	if (!DeviceIoControl(volume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, journal_data, sizeof(*journal_data), &byte_count,
		NULL)) {

		// If failed (for example, in case journaling is disabled), create journal and retry.
		cout << GetLastError();
		if (CreateJournal(volume)) {
			return LoadJournal(volume, journal_data);
		}

		return false;
	}

	return true;
}

void NTFSChangesWatcher::WatchChanges() {

	auto u_buffer = make_unique<char[]>(kBufferSize);

	auto read_journal_query = GetWaitForNextUsnQuery(last_usn_);

	while (true) {

		// This function does not return until new USN record created.
		WaitForNextUsn(read_journal_query.get());

		
		cout << "New entry created in the journal!" << endl;

		auto journal_query = GetReadJournalQuery(read_journal_query->StartUsn);

		DWORD byte_count;
		if (!ReadJournalRecords(journal_query.get(), u_buffer.get(), byte_count)) {
			// An error occurred.
			cout << "Failed to read journal records" << endl;
		}

		PUSN_RECORD UsnRecord = (PUSN_RECORD)(((PUCHAR)u_buffer.get()) + sizeof(USN));

		while (byte_count > 8)
		{

			HANDLE hh= OpenFileById(volume_, &(getFileIdDescriptor(UsnRecord->FileReferenceNumber)), 0, 0, 0, 0);
			TCHAR filePath[MAX_PATH];

			GetFinalPathNameByHandle(hh,filePath, MAX_PATH, 0);


			printf("File name: %ws\n",filePath);
			printf("Reason: %x\n", UsnRecord->Reason);
			printf("\n");
			byte_count -= UsnRecord->RecordLength;
			UsnRecord = (PUSN_RECORD)(((PCHAR)UsnRecord) +
				UsnRecord->RecordLength);
		}
		last_usn_ = *(USN*)u_buffer.get();
		read_journal_query->StartUsn = last_usn_;

		
		// If you need here you can:
		// Read and parse Journal records from the buffer.
		// Notify an NTFSChangeObservers about journal changes.
	}
}

bool NTFSChangesWatcher::WaitForNextUsn(PREAD_USN_JOURNAL_DATA read_journal_data) const {

	DWORD bytes_read;
	bool ok = true;

	// This function does not return until new USN record created.
	ok = DeviceIoControl(volume_, FSCTL_READ_USN_JOURNAL, read_journal_data, sizeof(*read_journal_data),
		&read_journal_data->StartUsn, sizeof(read_journal_data->StartUsn), &bytes_read,
		nullptr) != 0;



	return ok;
}

unique_ptr<READ_USN_JOURNAL_DATA> NTFSChangesWatcher::GetWaitForNextUsnQuery(USN start_usn) {

	auto query = make_unique<READ_USN_JOURNAL_DATA>();

	query->StartUsn = start_usn;
	query->ReasonMask = 0xFFFFFFFF;     // All bits.
	query->ReturnOnlyOnClose = FALSE;   // All entries.
	query->Timeout = 0;                 // No timeout.
	query->BytesToWaitFor = 1;          // Wait for this.
	query->UsnJournalID = journal_id_;  // The journal.
	query->MinMajorVersion = 2;
	query->MaxMajorVersion = 2;

	return query;
}


bool NTFSChangesWatcher::ReadJournalRecords(PREAD_USN_JOURNAL_DATA journal_query, LPVOID buffer,
	DWORD& byte_count) const {

	return DeviceIoControl(volume_, FSCTL_READ_USN_JOURNAL, journal_query, sizeof(*journal_query), buffer, kBufferSize,
		&byte_count, nullptr) != 0;
}

unique_ptr<READ_USN_JOURNAL_DATA> NTFSChangesWatcher::GetReadJournalQuery(USN low_usn) {

	auto query = make_unique<READ_USN_JOURNAL_DATA>();

	query->StartUsn = low_usn;
	query->ReasonMask = 0xFFFFFFFF;  // All bits.
	query->ReturnOnlyOnClose = FALSE;
	query->Timeout = 0;  // No timeout.
	query->BytesToWaitFor = 0;
	query->UsnJournalID = journal_id_;
	query->MinMajorVersion = 2;
	query->MaxMajorVersion = 2;

	return query;
}

