/**
 * Copyright(C) 2018  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "window_select_ui.h"
#include "window_utils.h"
#include "fmt_helper.h"

#include <afxdialogex.h>
#include <psapi.h>
#include <fmt/printf.h>
#include <vector>
#include <string>


IMPLEMENT_DYNAMIC(window_select_ui, CDialogEx)

/* \todo Create a list of processes that have actual visible windows for this user.
 - For each of these processes create a capture source.
 - for each of these processes create a button on the ui.
 - on press call supplied callback to start recording (covering record window use case).
*/

constexpr auto button_width = 250;
constexpr auto button_height = 250;



//bool is_main_window(HWND hwnd)
//{
    //return GetWindow(hwnd, GW_OWNER) == 0 && IsWindowVisible(hwnd);
    //bool result = GetWindow(hwnd, GW_OWNER) == 0;
    //result = result && GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
    //&& IsWindowVisible(hwnd);

        //WS_EX_ACCEPTFILES
    //GWL_EXSTYLE
//}

BOOL CALLBACK enum_windows_callback(HWND hwnd, LPARAM lparam)
{
    auto window_set = reinterpret_cast<get_windows_info*>(lparam);
    if ( hwnd != window_set->ignore_camstudio_wnd
      && hwnd != window_set->ignore_progman_wnd
      && hwnd != window_set->ignore_shell_tray_wnd
      && IsWindowVisible(hwnd)
      && !IsIconic(hwnd))
    {
        DWORD process_id = 0;
        GetWindowThreadProcessId(hwnd, &process_id);
        const auto process_name = get_process_name(process_id);

        // \todo don't use process names to exclude windows from the list
        //if (process_name.empty() || process_name == L"Explorer.EXE" || process_name == L"CamStudioRecorder.exe")
            //return TRUE;

        const auto window_title = get_window_title(hwnd);

        window_data data = {process_id, process_name, window_title, hwnd};
        window_set->windows.emplace_back(data);
    }

    return TRUE;
}

std::vector<window_data> window_select_ui::get_windows()
{
    get_windows_info result;

    result.ignore_camstudio_wnd = get_the_parent(GetSafeHwnd());
    result.ignore_shell_tray_wnd = ::FindWindow(L"Shell_TrayWnd", 0);
    result.ignore_progman_wnd = ::FindWindow(L"Progman", 0); // DWM Thumbnail desktop

    EnumWindows(enum_windows_callback, reinterpret_cast<LPARAM>(&result));
    return result.windows;
}

window_select_ui::window_select_ui(CWnd* pParent, const std::function<void(const HWND window_handle)> &completed)
    : CDialogEx(IDD_WINDOW_SELECT, pParent)
    , completed_(completed)
{
}

window_select_ui::~window_select_ui()
{
}

BOOL window_select_ui::OnInitDialog()
{
    const auto windows = get_windows();
    fmt::print("window count: {}\n", (int)windows.size());

    CRect button_rect(0, 0, button_width, button_height);

    constexpr auto button_horizontal_count = 6;
    const auto columns = windows.size() % button_horizontal_count;
    const auto rows = windows.size() / button_horizontal_count;

    // set size
    SetWindowPos(NULL, -1, -1,
        std::min<int>(windows.size(), button_horizontal_count) * button_width,
        rows * button_height,
        SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

    int i = 0;
    for (const auto &window : windows)
    {
        auto btn = std::make_unique<window_button>(window);

        const auto x = i % button_horizontal_count;
        const auto y = i / button_horizontal_count;
        auto rect = button_rect;
        rect.MoveToXY(x * button_width, y * button_height);

        // GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST
        // WS_EX_TOPMOST

        //auto style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW;
        //auto style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW;// | WS_EX_TOPMOST;// | BS_PUSHBUTTON;
        auto style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;


        //auto ex_style = WS_EX_TOPMOST | WS_OVERLAPPED;//WS_CHILD | WS_OVERLAPPED;
        //bool ret = btn->CreateEx(ex_style, _T("BUTTON"), window.process_name.c_str(), style, rect, this, IDC_WINDOW_SELECT_BUTTON + i);
        btn->Create(window.process_name.c_str(), style, rect, this, IDC_WINDOW_SELECT_BUTTON + i);
        //if (!ret)
            //fmt::print("button creation failed\n");
        //btn->Create(_T("STATIC"), window.process_name.c_str(), style, rect, this, IDC_WINDOW_SELECT_BUTTON + i);
        //btn->ShowWindow(SW_SHOW);

        //auto henk = btn->GetExStyle();
        //fmt::printf("0x%X\n", henk);

        //const auto btn_hwnd = btn->GetSafeHwnd();
        //if (!is_top_most(btn_hwnd))
        //    fmt::print("not top most\n");
        //SetWindowLong(btn_hwnd, GWL_EXSTYLE, GetWindowLong(btn_hwnd, GWL_EXSTYLE) | WS_EX_TOPMOST);

            //GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;

        capture_windows_.emplace_back(std::move(btn));
        ++i;
    }
    return TRUE;
}

BOOL window_select_ui::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
    const auto button_id = nID - IDC_WINDOW_SELECT_BUTTON;
    if (button_id < 0 || button_id > capture_windows_.size())
        return CDialogEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

    const auto &window = capture_windows_.at(button_id);
    const auto &window_data = window->get_data();
    fmt::print(L"pressed window select button: {} {}\n", button_id, window_data.process_name);
    return TRUE;
}

//void window_select_ui::OnMouseMove(UINT nFlags, CPoint point)
//{
//    // TODO: Add your message handler code here and/or call default
//    for (const auto &window : capture_windows_)
//    {
//        window->OnMouseMove(nFlags, point);
//    }
//
//    CDialogEx::OnMouseMove(nFlags, point);
//}



void window_select_ui::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(window_select_ui, CDialogEx)
    ON_BN_CLICKED(IDC_WINDOW_SELECT_BUTTON, &window_select_ui::on_select_window)
//    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void window_select_ui::on_select_window()
{
    fmt::print("on clicked\n");

    if (completed_)
        completed_(0);
}
