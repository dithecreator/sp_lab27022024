#include "ParentProcessDlg.h"

ParentProcessDlg* ParentProcessDlg::ptr = NULL;
HANDLE hMutex;
ParentProcessDlg::ParentProcessDlg(void)
{
	ptr = this;
	hMutex = CreateMutex(NULL, FALSE, TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}"));
}

ParentProcessDlg::~ParentProcessDlg(void)
{
	
}

void ParentProcessDlg::Cls_OnClose(HWND hwnd)
{
	ReleaseMutex(hMutex);
	EndDialog(hwnd, 0);
}

void MessageAboutError(DWORD dwError)
{
	LPVOID lpMsgBuf = NULL;
	TCHAR szBuf[300]; 

	BOOL fOK = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	if(lpMsgBuf != NULL)
	{
		wsprintf(szBuf, TEXT("Ошибка %d: %s"), dwError, lpMsgBuf); 
		MessageBox(0, szBuf, TEXT("Сообщение об ошибке"), MB_OK | MB_ICONSTOP); 
		LocalFree(lpMsgBuf);
	}
}

void ParentProcessDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    if (IDC_BUTTON1 == id)
    {
        // Захватываем мьютекс
        WaitForSingleObject(hMutex, INFINITE);

        // Запускаем первый процесс
        STARTUPINFO st = { sizeof(st) };
        PROCESS_INFORMATION pr;
        TCHAR filename[20];
        wsprintf(filename, TEXT("%s"), TEXT("CodingThread.exe"));
        if (!CreateProcess(NULL, filename, NULL, NULL, 0, 0, NULL, NULL, &st, &pr))
        {
            MessageAboutError(GetLastError());
            // Освобождаем мьютекс в случае ошибки
            ReleaseMutex(hMutex);
            return;
        }
        CloseHandle(pr.hThread);
        CloseHandle(pr.hProcess);

        // Освобождаем мьютекс после завершения первого процесса
        ReleaseMutex(hMutex);

        // Захватываем мьютекс снова
        WaitForSingleObject(hMutex, INFINITE);

        // Запускаем второй процесс
        STARTUPINFO st1 = { sizeof(st1) };
        PROCESS_INFORMATION pr1;
        TCHAR filename1[20];
        wsprintf(filename1, TEXT("%s"), TEXT("CodingThread1.exe"));
        if (!CreateProcess(NULL, filename1, NULL, NULL, 0, 0, NULL, NULL, &st1, &pr1))
        {
            MessageAboutError(GetLastError());
            // Освобождаем мьютекс в случае ошибки
            ReleaseMutex(hMutex);
            return;
        }
        CloseHandle(pr1.hThread);
        CloseHandle(pr1.hProcess);

        // Освобождаем мьютекс после завершения второго процесса
        ReleaseMutex(hMutex);
    }
}


BOOL CALLBACK ParentProcessDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}