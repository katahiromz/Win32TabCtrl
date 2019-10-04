#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <cassert>
#include <string>
#include "resource.h"

HWND g_hPages[2];

LPTSTR LoadStringDx(INT nID)
{
    static UINT s_index = 0;
    const UINT cchBuffMax = 1024;
    static TCHAR s_sz[2][cchBuffMax];

    TCHAR *pszBuff = s_sz[s_index];
    s_index = (s_index + 1) % ARRAYSIZE(s_sz);
    pszBuff[0] = 0;
    if (!::LoadString(NULL, nID, pszBuff, cchBuffMax))
        assert(0);
    return pszBuff;
}

void Page1_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    PostMessage(GetParent(GetParent(hwnd)), WM_COMMAND, id, 0);
}

void Page2_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    PostMessage(GetParent(GetParent(hwnd)), WM_COMMAND, id, 0);
}

INT_PTR CALLBACK
Page1DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_COMMAND, Page1_OnCommand);
    }
    return 0;
}

INT_PTR CALLBACK
Page2DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_COMMAND, Page2_OnCommand);
    }
    return 0;
}

void DoChoosePage(HWND hwnd, INT iPage)
{
    for (INT i = 0; i < ARRAYSIZE(g_hPages); ++i)
    {
        if (i == iPage)
            ShowWindow(g_hPages[i], SW_SHOW);
        else
            ShowWindow(g_hPages[i], SW_HIDE);
    }
}

void DoAdjustPages(HWND hwnd)
{
    HWND hCtl1 = GetDlgItem(hwnd, ctl1);

    RECT rc;
    GetClientRect(hCtl1, &rc);
    TabCtrl_AdjustRect(hCtl1, FALSE, &rc);

    for (INT i = 0; i < ARRAYSIZE(g_hPages); ++i)
    {
        MoveWindow(g_hPages[i], rc.left, rc.top,
                   rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }
}

void DoAddTabs(HWND hwnd)
{
    TC_ITEM item;
    INT i;
    HWND hCtl1 = GetDlgItem(hwnd, ctl1);

    ZeroMemory(&item, sizeof(item));
    item.mask = TCIF_TEXT;
    item.pszText = LoadStringDx(101);
    i = TabCtrl_InsertItem(hCtl1, 0, &item);
    assert(i != -1);

    ZeroMemory(&item, sizeof(item));
    item.mask = TCIF_TEXT;
    item.pszText = LoadStringDx(102);
    i = TabCtrl_InsertItem(hCtl1, 1, &item);
    assert(i != -1);
}

void DoCreatePages(HWND hwnd)
{
    HWND hCtl1 = GetDlgItem(hwnd, ctl1);
    g_hPages[0] = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TAB1), hCtl1, Page1DialogProc);
    g_hPages[1] = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TAB2), hCtl1, Page2DialogProc);
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    DoAddTabs(hwnd);
    DoCreatePages(hwnd);
    DoAdjustPages(hwnd);
    DoChoosePage(hwnd, 0);
    return TRUE;
}

void DoDestroyPages(HWND hwnd)
{
    for (INT i = 0; i < ARRAYSIZE(g_hPages); ++i)
    {
        DestroyWindow(g_hPages[i]);
        g_hPages[i]  = NULL;
    }
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        DoDestroyPages(hwnd);
        EndDialog(hwnd, id);
        break;
    case rad1:
        SetDlgItemTextW(hwnd, stc1, L"Clicked rad1");
        break;
    case rad2:
        SetDlgItemTextW(hwnd, stc1, L"Clicked rad2");
        break;
    case chx1:
        SetDlgItemTextW(hwnd, stc1, L"Clicked chx1");
        break;
    }
}

LRESULT OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
{
    switch (pnmhdr->code)
    {
    case TCN_SELCHANGE:
        DoChoosePage(hwnd, TabCtrl_GetCurSel(GetDlgItem(hwnd, ctl1)));
        break;
    }
    return 0;
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    InitCommonControls();
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, DialogProc);
    return 0;
}
