#pragma once

#include <CamLib/TrayIcon.h>
#include <iostream>
#include <sstream>
#include <string>

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

    // Attributes
public:
    CBitmap &Logo()
    {
        return m_bmLogo;
    }

    BOOL PreCreateWindow(CREATESTRUCT &cs) override;


public:
    ~CMainFrame() override;
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext &dc) const override;
#endif

protected: // control bar embedded members
    CStatusBar m_wndStatusBar;
    CToolBar m_wndToolBar;
    CBitmap m_ToolbarBitmap256;
    CBitmap m_ToolbarBitmapMask;
    CImageList m_ilToolBar;
    CBitmap m_bmLogo;

    // Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    //}}AFX_MSG
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
private:
    CTrayIcon m_TrayIcon;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

extern int g_maxx_screen;
extern int g_maxy_screen;
extern int g_minx_screen;
extern int g_miny_screen;
