#include "stdafx.h"
#include <Windows.h>
#include <WinIoCtl.h>
#include "NTFSChangeWatcher.h"
#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR path[MAX_PATH] = TEXT("C:\\Users\\kshivakumar\\Documents");
	NTFSChangesWatcher *cw = new NTFSChangesWatcher('C', path);
	cw->WatchChanges();
}