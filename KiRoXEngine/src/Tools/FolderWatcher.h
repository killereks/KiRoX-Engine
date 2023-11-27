#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>

struct FolderWatcher
{
	enum class Action
	{
		Error, Created, Deleted, OldName, NewName, Modified,
	};
	using Callback = void(std::wstring_view filename, Action action);

	bool watchFolder(const TCHAR* path);
	void update();

	HANDLE dir_handle;
	OVERLAPPED overlapped;
	uint8_t change_buf[1024];

	std::function<Callback> OnFileChanged;
};

inline bool FolderWatcher::watchFolder(const TCHAR* path)
{
	dir_handle = CreateFile(
		path,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		nullptr
	);

	if (dir_handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	overlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	BOOL success = ReadDirectoryChangesW(
		dir_handle,
		change_buf, sizeof(change_buf),
		TRUE,                           // watch subtree
		FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME,
		nullptr,
		&overlapped,
		nullptr
	);

	return success;
}

inline void FolderWatcher::update()
{
	if (!dir_handle || dir_handle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD wait_status = WaitForMultipleObjects(1, &overlapped.hEvent, FALSE, 0);

	// nothing updated
	if (wait_status == WAIT_TIMEOUT)
	{
		return;
	}
	else if (wait_status != WAIT_OBJECT_0)
	{
		//printf("unhandles wait_status value: %u\n", wait_status);
		return;
	}

	DWORD bytes_transferred;
	GetOverlappedResult(dir_handle, &overlapped, &bytes_transferred, FALSE);

	FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;

	while (true)
	{
		Action action = Action::Error;
		std::wstring_view name = { event->FileName, event->FileNameLength / sizeof(WCHAR) };

		switch (event->Action)
		{
			case FILE_ACTION_ADDED:
				action = Action::Created;
				break;
			case FILE_ACTION_REMOVED:
				action = Action::Deleted;
				break;
			case FILE_ACTION_MODIFIED:
				action = Action::Modified;
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				action = Action::OldName;
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				action = Action::NewName;
				break;
			default:
				printf("> Action %d: %.*S\n", event->Action, event->FileNameLength / sizeof(WCHAR), event->FileName);
		}

		OnFileChanged(name, action);

		if (event->NextEntryOffset)
		{
			*((uint8_t**)&event) += event->NextEntryOffset;
		}
		else
		{
			break;
		}
	}
	BOOL success = ReadDirectoryChangesW(
		dir_handle,
		change_buf, sizeof(change_buf),
		FALSE,
		FILE_NOTIFY_CHANGE_LAST_WRITE,
		nullptr,
		&overlapped,
		nullptr
	);
	if (!success)
	{
		//printf("call to ReadDirectoryChangesW failed\n");
		return;
	}
}