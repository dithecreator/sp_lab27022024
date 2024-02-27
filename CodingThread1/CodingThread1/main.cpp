#include "Windows.h"
#include "tchar.h"
#include "resource.h"
#include <fstream>
using namespace std;

HANDLE hMutex;

DWORD WINAPI Thread1(LPVOID lp)
{
    HWND hEdit = static_cast<HWND>(lp);
    //HWND hEdit = GetDlgItem(hDialog, IDC_EDIT1);
    //HWND hButton = GetDlgItem(hDialog, IDC_BUTTON1);
    char buf[100];
    ifstream in("music.txt", ios::in);

    if (!in)
    {
        MessageBoxA(0, "Ошибка открытия файла!", "Мьютекс", MB_OK | MB_ICONINFORMATION);
        return 1;
    }

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, L"{D93A10BF-B125-492E-9D9A-A3321C2D0EDB}");

    DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

    ofstream out("coding.txt", ios::out | ios::trunc);
    const int KEY = 100;
    while (!in.eof())
    {
        in.getline(buf, 100);
        out << buf << endl;

        // Добавляем новую строку к содержимому hEdit
        SendMessageA(hEdit, EM_REPLACESEL, 0, (LPARAM)buf);
        SendMessageA(hEdit, EM_REPLACESEL, 0, (LPARAM)"\r\n"); // Добавляем новую строку
    }

    out.close();
    in.close();
    ReleaseMutex(hMutex);
    return 0;
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{


	switch (uMsg)
	{
    case WM_INITDIALOG: {
        HWND hEdit = GetDlgItem(hwnd, IDC_EDIT1);

        HANDLE hThread = CreateThread(nullptr, 0, Thread1, hEdit, 0, nullptr);
        if (hThread != nullptr) {
            //WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }
    }
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON1){
			EndDialog(hwnd, 0);
	}

		break;

	case WM_TIMER:


		break;





	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;

	}
	return FALSE;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DLGPROC(DlgProc));
}