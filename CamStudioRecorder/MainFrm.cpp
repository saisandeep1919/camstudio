#include "stdafx.h"
#include "Recorder.h"
#include "MainFrm.h"
#include <CamLib/console.h>
#include <afxdatarecovery.h>

int g_maxx_screen;
int g_maxy_screen;
int g_minx_screen;
int g_miny_screen;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_MESSAGE(CTrayIcon::m_WM_TRAY_ICON_NOTIFY_MESSAGE, &CMainFrame::OnTrayNotify)
    ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
//#ifdef _DEBUG
    console::create();
//#endif
    // TODO: add member initialization code here
    m_bmLogo.LoadBitmap(IDB_BITMAP3);
}

CMainFrame::~CMainFrame()
{
    // ver 1.2
    m_ToolbarBitmap256.DeleteObject();
    m_ToolbarBitmapMask.DeleteObject();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE("Failed to create toolbar\n");
        return -1; // fail to create
    }

    if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
    {
        TRACE("Failed to create status bar\n");
        return -1; // fail to create
    }

    // ver 1.2
    m_ToolbarBitmap256.LoadBitmap(IDB_TOOLBAR256);
    m_ToolbarBitmapMask.LoadBitmap(IDB_TOOLBARMASK);

    HDC hScreenDC = ::GetDC(nullptr);
    int numbits = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
    // g_maxx_screen = ::GetDeviceCaps(hScreenDC, HORZRES);
    g_maxx_screen = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    // g_maxy_screen = ::GetDeviceCaps(hScreenDC, VERTRES);
    g_maxy_screen = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    g_minx_screen = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_miny_screen = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    ::ReleaseDC(nullptr, hScreenDC);

    if (numbits > 8)
    {
        // m_ilToolBar.Create(21,21, ILC_COLOR8 | ILC_MASK, 4, 4);
        m_ilToolBar.Create(21, 21, ILC_COLOR16 | ILC_MASK, 4, 4);
        // m_ilToolBar.Create(32,32, ILC_COLOR16 | ILC_MASK, 4, 4);
        m_ilToolBar.Add(&m_ToolbarBitmap256, &m_ToolbarBitmapMask);

        // VC6
        // m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilToolBar);

        // VC5
        ::SendMessage(m_wndToolBar.GetToolBarCtrl().m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)m_ilToolBar.GetSafeHandle());
    }

    m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

    // TODO: Remove this if you don't want tool tips or a resizeable toolbar
    m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    // m_wndToolBar.SetButtonText(1,"Record");
    // TODO: Delete these three lines if you don't want the toolbar to
    // be dockable
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    // ver 1.2
    m_TrayIcon.SetNotifyWnd(*this);
    m_TrayIcon.TraySetIcon(IDR_MAINFRAME);
    m_TrayIcon.TraySetToolTip(_T("CamStudio"));
    m_TrayIcon.TraySetMenu(IDR_TRAYMENU);
    m_TrayIcon.TrayShow();

    SetWindowText(_T("CamStudio"));
    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    // TODO: Modify the Window class or styles here by modifying

    // the CREATESTRUCT cs
    cs.x = 200;
    cs.y = 200;

    // use logo to set width and heights
    BITMAP bitmap;
    m_bmLogo.GetBitmap(&bitmap);

    cs.cx = bitmap.bmWidth - 26;  // Otherwise size won't be correct
    cs.cy = bitmap.bmHeight + 10; // Otherwise size won't be correct
    // add width of borders
    cs.cx += (::GetSystemMetrics(SM_CXFRAME) * 2) + ::GetSystemMetrics(SM_CXMENUSIZE);
    // add height of Caption + menu + toolbar + status + borders
    cs.cy += ::GetSystemMetrics(SM_CYCAPTION) + (::GetSystemMetrics(SM_CYMENU) * 2) // assume statusbar same height
             + ::GetSystemMetrics(SM_CXMENUSIZE) + (::GetSystemMetrics(SM_CYFRAME) * 2);

    cs.style &= ~FWS_ADDTOTITLE;
    cs.style &= ~WS_THICKFRAME;
    cs.style &= ~WS_MAXIMIZEBOX;
    cs.lpszClass = _T("CamStudio");

    return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnClose()
{
    // if want to close the application while a recording is going on...
    //if (g_bRecordState)
    //{
    //    MessageOut(this->m_hWnd, IDS_STRING_STOPBEFOREEXIT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
    //    return;
    //}

    // Workarround for CFrameWnd::OnClose();
    if (m_lpfnCloseProc != nullptr)
    {
        // if there is a close proc, then defer to it, and return
        // after calling it so the frame itself does not close.
        (*m_lpfnCloseProc)(this);
        return;
    }

    // Note: only queries the active document
    CDocument* pDocument = GetActiveDocument();
    if (pDocument != nullptr && !pDocument->CanCloseFrame(this))
    {
        // document can't close right now -- don't close it
        return;
    }
    CWinApp* pApp = AfxGetApp();
    if (pApp != nullptr && pApp->m_pMainWnd == this)
    {
        CDataRecoveryHandler *pHandler = pApp->GetDataRecoveryHandler();
        if ((pHandler != NULL) && (pHandler->GetShutdownByRestartManager()))
        {
            // If the application is being shut down by the Restart Manager, do
            // a final autosave.  This will mark all the documents as not dirty,
            // so the SaveAllModified call below won't prompt for save.
            pHandler->AutosaveAllDocumentInfo();
            pHandler->SaveOpenDocumentList();
        }

        // attempt to save all documents
        if (pDocument == nullptr && !pApp->SaveAllModified())
            return;     // don't close it

        if ((pHandler != nullptr) && (!pHandler->GetShutdownByRestartManager()))
        {
            // If the application is not being shut down by the Restart Manager,
            // delete any autosaved documents since everything is now fully saved.
            pHandler->DeleteAllAutosavedFiles();
        }

        // hide the application's windows before closing all the documents
        pApp->HideApplication();

        // close all documents first
        pApp->CloseAllDocuments(FALSE);

        // don't exit if there are outstanding component objects
        if (!AfxOleCanExitApp())
        {
            // take user out of control of the app
            AfxOleSetUserCtrl(FALSE);

            // don't destroy the main window and close down just yet
            //  (there are outstanding component (OLE) objects)
            return;
        }

        // there are cases where destroying the documents may destroy the
        //  main window of the application.
        if (!afxContextIsDLL && pApp->m_pMainWnd == NULL)
        {
            AfxPostQuitMessage(0);
            return;
        }
    }

    // detect the case that this is the last frame on the document and
    // shut down with OnCloseDocument instead.
    if (pDocument != nullptr && pDocument->m_bAutoDelete)
    {
        BOOL bOtherFrame = FALSE;
        POSITION pos = pDocument->GetFirstViewPosition();
        while (pos != nullptr)
        {
            CView* pView = pDocument->GetNextView(pos);
            ENSURE_VALID(pView);
            if (pView->GetParentFrame() != this)
            {
                bOtherFrame = TRUE;
                break;
            }
        }
        if (!bOtherFrame)
        {
            pDocument->OnCloseDocument();
            return;
        }

        // allow the document to cleanup before the window is destroyed
        pDocument->PreCloseFrame(this);
    }

    // then destroy the window
    DestroyWindow();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (m_TrayIcon.MinimizeToTray())
    {
        if ((nID & 0xFFF0) == SC_MINIMIZE)
        {
            ShowWindow(SW_HIDE);
            return;
        }
    }
    CFrameWnd::OnSysCommand(nID, lParam);
}
LRESULT CMainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
    return m_TrayIcon.OnTrayNotify(wParam, lParam);
}
