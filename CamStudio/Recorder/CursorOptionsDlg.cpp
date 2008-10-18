// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
//
// CursorOptionsDlg.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "CursorOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Internal Vars..should put into class header
CFileDialog *iconFileDlg;
HCURSOR previewcursor = NULL;
HCURSOR loadcursor;
HCURSOR customcursor;
int customsel;
int recordcursor;
int cursortype;
int highlightcursor;
int highlightsize;
int highlightshape;
int highlightclick;
COLORREF highlightcolor;
COLORREF highlightclickcolorleft;
COLORREF highlightclickcolorright;
int initpaint=TRUE;
int sliding=FALSE;

//extern CFileDialog *iconFileDlg;
extern HCURSOR hLoadCursor;
extern HCURSOR hCustomCursor;
extern CString strCursorDir;
extern CString strCursorFilePath;

DWORD arrIconInfo[] = {
	IDI_CUSTOMICON_CONTEXTHELP,
	IDI_CUSTOMICON_MAGNIFY,
	IDI_CUSTOMICON_NODRAG,
	IDI_CUSTOMICON_SPLITBARH,
	IDI_CUSTOMICON_SPLITBARV,
	IDI_CUSTOMICON_TOPOFTABLE,
	IDI_CUSTOMICON_BOOK1,
	IDI_CUSTOMICON_BOOK2,
	IDI_CUSTOMICON_CLIP1,
	IDI_CUSTOMICON_CLIP2,
	IDI_CUSTOMICON_CLOCK1,
	IDI_CUSTOMICON_CLOCK2,
	IDI_CUSTOMICON_CARDFILE1,
	IDI_CUSTOMICON_CARDFILE2,
	IDI_CUSTOMICON_DISK1,
	IDI_CUSTOMICON_DISK2,
	IDI_CUSTOMICON_FILES1,
	IDI_CUSTOMICON_FILES2,
	IDI_CUSTOMICON_FOLDER2,
	IDI_CUSTOMICON_FOLDER1,
	IDI_CUSTOMICON_MAIL1,
	IDI_CUSTOMICON_MAIL2,
	IDI_CUSTOMICON_NOTE1,
	IDI_CUSTOMICON_NOTE2,
	IDI_CUSTOMICON_PEN1,
	IDI_CUSTOMICON_PEN2,
	IDI_CUSTOMICON_PENCIL1,
	IDI_CUSTOMICON_PENCIL2,
	IDI_CUSTOMICON_PHONE1,
	IDI_CUSTOMICON_PHONE2,
	IDI_CUSTOMICON_POINT1,
	IDI_CUSTOMICON_POINT2,
	IDI_CUSTOMICON_SERCURITY,
	IDI_CUSTOMICON_SECURITY2
};

/////////////////////////////////////////////////////////////////////////////
// CCursorOptionsDlg dialog

CCursorOptionsDlg::CCursorOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCursorOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCursorOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CCursorOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCursorOptionsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RADIO1, ctrlButtonHideCursor);
}

BEGIN_MESSAGE_MAP(CCursorOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CCursorOptionsDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_CURSOR1, OnCursor1)
	ON_BN_CLICKED(IDC_CURSOR2, OnCursor2)
	ON_BN_CLICKED(IDC_CURSOR3, OnCursor3)
	ON_BN_CLICKED(IDC_FILECURSOR, OnFilecursor)
	ON_CBN_SELCHANGE(IDC_CUSTOMCURSOR, OnSelchangeCustomcursor)
	ON_CBN_SELCHANGE(IDC_HIGHLIGHTSHAPE, OnSelchangeHighlightshape)
	ON_BN_CLICKED(IDC_HIGHLIGHTCOLOR, OnHighlightcolor)
	ON_BN_CLICKED(IDC_HIGHLIGHTCURSOR, OnHighlightcursor)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_HIGHLIGHTCOLORLB, OnHighlightColorLeft)
	ON_BN_CLICKED(IDC_HIGHLIGHTCOLORRB, OnHighlightColorRight)
	ON_BN_CLICKED(IDC_HIGHLIGHTCLICK, OnEnableVisualClickFeedback)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCursorOptionsDlg message handlers

void CCursorOptionsDlg::OnRadio1()
{
	// TODO: Add your control notification handler code here

	recordcursor = 0;
	ctrlButtonHideCursor.SetCheck(TRUE);
	((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);

	((CButton *) GetDlgItem(IDC_CURSOR1))->EnableWindow(FALSE);
	((CButton *) GetDlgItem(IDC_CURSOR2))->EnableWindow(FALSE);
	((CButton *) GetDlgItem(IDC_CURSOR3))->EnableWindow(FALSE);
	((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
	((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	RefreshPreviewCursor();

}

void CCursorOptionsDlg::OnRadio2()
{
	// TODO: Add your control notification handler code here

	recordcursor = 1;
	ctrlButtonHideCursor.SetCheck(FALSE);
	((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);

	((CButton *) GetDlgItem(IDC_CURSOR1))->EnableWindow(TRUE);
	((CButton *) GetDlgItem(IDC_CURSOR2))->EnableWindow(TRUE);
	((CButton *) GetDlgItem(IDC_CURSOR3))->EnableWindow(TRUE);

	if (cursortype == 0) {

		((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	}
	else if (cursortype == 1) {

		((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	}
	else {

		((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(TRUE);

	}

	RefreshPreviewCursor();

}

void CCursorOptionsDlg::OnCursor1()
{
	// TODO: Add your control notification handler code here
	cursortype = 0;

	((CButton *) GetDlgItem(IDC_CURSOR1))->SetCheck(TRUE);
	((CButton *) GetDlgItem(IDC_CURSOR2))->SetCheck(FALSE);
	((CButton *) GetDlgItem(IDC_CURSOR3))->SetCheck(FALSE);

	((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
	((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	RefreshPreviewCursor();

}

void CCursorOptionsDlg::OnCursor2()
{
	// TODO: Add your control notification handler code here
	cursortype = 1;

	((CButton *) GetDlgItem(IDC_CURSOR1))->SetCheck(FALSE);
	((CButton *) GetDlgItem(IDC_CURSOR2))->SetCheck(TRUE);
	((CButton *) GetDlgItem(IDC_CURSOR3))->SetCheck(FALSE);

	((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(TRUE);
	((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	RefreshPreviewCursor();

}

void CCursorOptionsDlg::OnCursor3()
{
	// TODO: Add your control notification handler code here
	cursortype = 2;

	((CButton *) GetDlgItem(IDC_CURSOR1))->SetCheck(FALSE);
	((CButton *) GetDlgItem(IDC_CURSOR2))->SetCheck(FALSE);
	((CButton *) GetDlgItem(IDC_CURSOR3))->SetCheck(TRUE);

	((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
	((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(TRUE);

	RefreshPreviewCursor();
}

BOOL CCursorOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	customsel = iCustomSel;
	recordcursor = bRecordCursor;
	cursortype = iCursorType;
	highlightcursor = bHighlightCursor;
	highlightsize = iHighlightSize;
	highlightshape = iHighlightShape;
	highlightcolor = clrHighlightColor;
	loadcursor = hLoadCursor;
	customcursor = hCustomCursor;
	highlightclick = bHighlightClick;
	highlightclickcolorleft = clrHighlightClickColorLeft;
	highlightclickcolorright = clrHighlightClickColorRight;

	HICON loadFileIcon= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	((CButton *) GetDlgItem(IDC_FILECURSOR))->SetIcon(loadFileIcon);

	//Highlight UI
	((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->EnableWindow(TRUE);
	((CComboBox *) GetDlgItem(IDC_HIGHLIGHTSHAPE))->EnableWindow(TRUE);

	((CStatic *) GetDlgItem(IDC_STATIC_SIZE))->EnableWindow(TRUE);
	((CStatic *) GetDlgItem(IDC_STATIC_SHAPE))->EnableWindow(TRUE);
	((CStatic *) GetDlgItem(IDC_STATIC_HALFSIZE))->EnableWindow(TRUE);

	((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->SetRange(1,128,TRUE);
	((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->SetPos(highlightsize);
	((CComboBox *) GetDlgItem(IDC_HIGHLIGHTSHAPE))->SetCurSel(highlightshape);
	((CButton *) GetDlgItem(IDC_HIGHLIGHTCURSOR))->SetCheck(highlightcursor);
	((CButton *) GetDlgItem(IDC_HIGHLIGHTCLICK))->SetCheck(highlightclick);

	//RefreshHighlight doesn't work in init dialog....so defer this to OnPaint
	initpaint=TRUE;
	//RefreshHighlight();

	if (highlightcursor) {

		((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->EnableWindow(TRUE);
		((CComboBox *) GetDlgItem(IDC_HIGHLIGHTSHAPE))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLOR))->EnableWindow(TRUE);

		((CStatic *) GetDlgItem(IDC_STATIC_SIZE))->EnableWindow(TRUE);
		((CStatic *) GetDlgItem(IDC_STATIC_SHAPE))->EnableWindow(TRUE);
		((CStatic *) GetDlgItem(IDC_STATIC_HALFSIZE))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCLICK))->EnableWindow(TRUE);

		if (highlightclick)
		{
			((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORLB))->EnableWindow(TRUE);
			((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORRB))->EnableWindow(TRUE);
		}
		else
		{
			((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORLB))->EnableWindow(FALSE);
			((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORRB))->EnableWindow(FALSE);
		}

	}
	else {

		((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->EnableWindow(FALSE);
		((CComboBox *) GetDlgItem(IDC_HIGHLIGHTSHAPE))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLOR))->EnableWindow(FALSE);

		((CStatic *) GetDlgItem(IDC_STATIC_SIZE))->EnableWindow(FALSE);
		((CStatic *) GetDlgItem(IDC_STATIC_SHAPE))->EnableWindow(FALSE);
		((CStatic *) GetDlgItem(IDC_STATIC_HALFSIZE))->EnableWindow(FALSE);

		((CButton *) GetDlgItem(IDC_HIGHLIGHTCLICK))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORLB))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORRB))->EnableWindow(FALSE);

	}

	//Highlight UI

	((CButton *) GetDlgItem(IDC_CURSOR1))->EnableWindow(TRUE);
	((CButton *) GetDlgItem(IDC_CURSOR2))->EnableWindow(TRUE);
	((CButton *) GetDlgItem(IDC_CURSOR3))->EnableWindow(TRUE);
	((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(TRUE);
	((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(TRUE);

	((CComboBox *) GetDlgItem(IDC_CUSTOMCURSOR))->SetCurSel(customsel);

	RefreshPreviewCursor();

	if (cursortype==0) {

		((CButton *) GetDlgItem(IDC_CURSOR1))->SetCheck(TRUE);
		((CButton *) GetDlgItem(IDC_CURSOR2))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_CURSOR3))->SetCheck(FALSE);

		((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	}
	else if (cursortype==1) {

		((CButton *) GetDlgItem(IDC_CURSOR1))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_CURSOR2))->SetCheck(TRUE);
		((CButton *) GetDlgItem(IDC_CURSOR3))->SetCheck(FALSE);

		((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	}
	else {

		((CButton *) GetDlgItem(IDC_CURSOR1))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_CURSOR2))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_CURSOR3))->SetCheck(TRUE);

		((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(TRUE);

	}

	if (recordcursor==0) {

		ctrlButtonHideCursor.SetCheck(TRUE);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);

		((CButton *) GetDlgItem(IDC_CURSOR1))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_CURSOR2))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_CURSOR3))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_CUSTOMCURSOR))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_FILECURSOR))->EnableWindow(FALSE);

	}
	else {

		ctrlButtonHideCursor.SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);

		((CButton *) GetDlgItem(IDC_CURSOR1))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_CURSOR2))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_CURSOR3))->EnableWindow(TRUE);

	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCursorOptionsDlg::OnSelchangeCustomcursor()
{
	// TODO: Add your control notification handler code here
	RefreshPreviewCursor();
}

void CCursorOptionsDlg::RefreshPreviewCursor()
{

	if (recordcursor==0) {

		previewcursor = NULL;

	}
	else {

		if (cursortype==0) {

			previewcursor = GetCursor();

		}
		else if (cursortype==1) {

			customsel = ((CComboBox *) GetDlgItem(IDC_CUSTOMCURSOR))->GetCurSel();

			DWORD customicon;
			if (customsel<0) customsel = 0;
			customicon = arrIconInfo[customsel];

			previewcursor = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(customicon));
			customcursor = previewcursor;

		}
		else {

			previewcursor = loadcursor;

		} //cursortype

	} //recordcursor

	((CStatic *) GetDlgItem(IDC_ICONCURSOR))->SetIcon(previewcursor);

}

void CCursorOptionsDlg::OnFilecursor()
{
	// TODO: Add your command handler code here
	CString fileName;
	CString filt="Icon and Cursor Files (*.ico; *.cur)|*.ico;*.cur||";

	if (iconFileDlg == NULL)
		iconFileDlg = new CFileDialog(TRUE,"*.ico;*.cur","*.ico;*.cur",NULL,filt,this);

	if (iconFileDlg!=NULL) {

		char dirx[200];
		GetWindowsDirectory(dirx,200);
		CString initdir(dirx);
		initdir = initdir + "\\cursors";

		iconFileDlg->m_ofn.Flags|=OFN_FILEMUSTEXIST;
		iconFileDlg->m_ofn.lpstrTitle="File to load";

		if (strCursorDir=="")
			strCursorDir=initdir;
		iconFileDlg->m_ofn.lpstrInitialDir = strCursorDir;

		if (iconFileDlg->DoModal()==IDOK) {

			fileName=iconFileDlg->GetPathName();
			CString extension =fileName.Right(4);
			extension.MakeUpper();
			if ((extension==".ICO") || (extension==".CUR")) {

				loadcursor = LoadCursorFromFile(fileName);
				previewcursor = loadcursor;
				((CStatic *) GetDlgItem(IDC_ICONCURSOR))->SetIcon(previewcursor);

				//LoadBMP(fileName);
			}

			strCursorFilePath = fileName;

			fileName=fileName.Left(fileName.ReverseFind('\\'));
			strCursorDir = fileName;

		}
		delete iconFileDlg;
		iconFileDlg = NULL;
	}

}

void CCursorOptionsDlg::OnOK()
{
	// TODO: Add extra validation here
	if (cursortype==0) {

	}
	else if (cursortype==1) {

		customcursor = previewcursor;

	}
	else {

		loadcursor = previewcursor;

	} //cursortype

	iCustomSel = customsel;
	bRecordCursor = recordcursor;
	iCursorType = cursortype;
	bHighlightCursor = highlightcursor;
	iHighlightSize = highlightsize;
	iHighlightShape = highlightshape;
	clrHighlightColor = highlightcolor;
	hLoadCursor = loadcursor;
	hCustomCursor = customcursor;
	bHighlightClick=highlightclick;
	clrHighlightClickColorLeft=highlightclickcolorleft;
	clrHighlightClickColorRight=highlightclickcolorright;

	CDialog::OnOK();
}

void CCursorOptionsDlg::OnSelchangeHighlightshape()
{
	// TODO: Add your control notification handler code here
	highlightshape = ((CComboBox *) GetDlgItem(IDC_HIGHLIGHTSHAPE))->GetCurSel();

	RefreshHighlight();

}

void CCursorOptionsDlg::OnHighlightcolor()
{

	CColorDialog colerdlog(highlightcolor,CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT);
	if (colerdlog.DoModal() ==IDOK) {

		highlightcolor=colerdlog.GetColor();

	}

	RefreshHighlight();

}

void CCursorOptionsDlg::OnHighlightcursor()
{
	// TODO: Add your control notification handler code here
	highlightcursor = ((CButton *) GetDlgItem(IDC_HIGHLIGHTCURSOR))->GetCheck();
	if (highlightcursor) {

		((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->EnableWindow(TRUE);
		((CComboBox *) GetDlgItem(IDC_HIGHLIGHTSHAPE))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLOR))->EnableWindow(TRUE);

		((CStatic *) GetDlgItem(IDC_STATIC_SIZE))->EnableWindow(TRUE);
		((CStatic *) GetDlgItem(IDC_STATIC_SHAPE))->EnableWindow(TRUE);
		((CStatic *) GetDlgItem(IDC_STATIC_HALFSIZE))->EnableWindow(TRUE);

		((CButton *) GetDlgItem(IDC_HIGHLIGHTCLICK))->EnableWindow(TRUE);

	}
	else {

		((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->EnableWindow(FALSE);
		((CComboBox *) GetDlgItem(IDC_HIGHLIGHTSHAPE))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLOR))->EnableWindow(FALSE);

		((CStatic *) GetDlgItem(IDC_STATIC_SIZE))->EnableWindow(FALSE);
		((CStatic *) GetDlgItem(IDC_STATIC_SHAPE))->EnableWindow(FALSE);
		((CStatic *) GetDlgItem(IDC_STATIC_HALFSIZE))->EnableWindow(FALSE);

		((CButton *) GetDlgItem(IDC_HIGHLIGHTCLICK))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORLB))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORRB))->EnableWindow(FALSE);

	}

	RefreshHighlight();

}

void CCursorOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	highlightsize = ((CSliderCtrl *) GetDlgItem(IDC_HIGHLIGHTSIZE))->GetPos();
	sliding=TRUE;
	RefreshHighlight();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	sliding=FALSE;
}

void CCursorOptionsDlg::RefreshHighlight()
{

	CSize previewsize;
	previewsize.cx=64;
	previewsize.cy=64;
	int xoffset = 300;
	int yoffset = 230;

	int highlightsizehalf = highlightsize/2;

	double x1,x2,y1,y2;

	COLORREF usecolor = highlightcolor;

	if (highlightclick==1 && sliding==FALSE)
	{
		SHORT tmpShort = -999;
		tmpShort=GetKeyState(VK_LBUTTON);
		if (tmpShort!=0 && tmpShort!=1)
		{
			usecolor = highlightclickcolorleft;
			highlightsizehalf *= 1.5;
		}
		tmpShort=GetKeyState(VK_RBUTTON);
		if (tmpShort!=0 && tmpShort!=1)
		{
			usecolor = highlightclickcolorright;
			highlightsizehalf *= 1.5;
		}
	}

	//OffScreen Buffer
	CDC* cdc= GetWindowDC();
	HDC hdc= cdc->m_hDC;

	HBITMAP hbm = NULL;
	HBITMAP old_bitmap;
	HDC hdcBits = ::CreateCompatibleDC(hdc);
	hbm = (HBITMAP) ::CreateCompatibleBitmap(hdc,previewsize.cx,previewsize.cy);
	old_bitmap = (HBITMAP) ::SelectObject(hdcBits,hbm);

	if ((highlightshape == 0) || (highlightshape == 2)) { //circle and square

		x1 = (previewsize.cx - highlightsizehalf)/2.0;
		x2 = (previewsize.cx + highlightsizehalf)/2.0;
		y1 = (previewsize.cy - highlightsizehalf)/2.0;
		y2 = (previewsize.cy + highlightsizehalf)/2.0;
	}
	else if ((highlightshape == 1) || (highlightshape == 3)) { //ellipse and rectangle

		x1 = (previewsize.cx - highlightsizehalf)/2.0;
		x2 = (previewsize.cx + highlightsizehalf)/2.0;
		y1 = (previewsize.cy - highlightsizehalf/2.0)/2.0;
		y2 = (previewsize.cy + highlightsizehalf/2.0)/2.0;

	}

	HBRUSH ptbrush = (HBRUSH) ::GetStockObject(LTGRAY_BRUSH);
	HPEN nullpen = CreatePen( PS_NULL,0,0);
	HBRUSH hlbrush = CreateSolidBrush( usecolor );

	HBRUSH oldbrush = (HBRUSH) ::SelectObject(hdcBits,ptbrush);
	HPEN oldpen = (HPEN) ::SelectObject(hdcBits,nullpen);
	::Rectangle(hdcBits, 0,0,previewsize.cx+1,previewsize.cy+1);

	if (highlightcursor) { //draw the shape only if highlight cursor is selected

		::SelectObject(hdcBits,hlbrush);

		if ((highlightshape == 0) || (highlightshape == 1)) { //circle and ellipse
			::Ellipse(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
		}
		else if ((highlightshape == 2) || (highlightshape == 3)) { //square and rectangle
			::Rectangle(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
		}

		::SelectObject(hdcBits,oldbrush);

	}

	::SelectObject(hdcBits,oldpen);
	DeleteObject(hlbrush);
	DeleteObject(nullpen);

	//OffScreen Buffer
	BitBlt(hdc, xoffset, yoffset, previewsize.cx, previewsize.cy, hdcBits, 0, 0, SRCCOPY);
	SelectObject(hdcBits, old_bitmap);
	DeleteObject(hbm);
	DeleteDC(hdcBits);

	ReleaseDC(cdc);

}

void CCursorOptionsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	RefreshHighlight();
}

void CCursorOptionsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CCursorOptionsDlg::OnHighlightColorLeft()
{
	CColorDialog colerdlog(highlightclickcolorleft,CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT);
	if (colerdlog.DoModal() ==IDOK) {

		highlightclickcolorleft=colerdlog.GetColor();

	}

	RefreshHighlight();

}

void CCursorOptionsDlg::OnHighlightColorRight()
{
	CColorDialog colerdlog(highlightclickcolorright,CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT);
	if (colerdlog.DoModal() ==IDOK) {

		highlightclickcolorright=colerdlog.GetColor();

	}

	RefreshHighlight();

}

void CCursorOptionsDlg::OnEnableVisualClickFeedback()
{
	highlightclick = ((CButton *) GetDlgItem(IDC_HIGHLIGHTCLICK))->GetCheck();
	if (highlightclick) {

		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORLB))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORRB))->EnableWindow(TRUE);

	}
	else {

		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORLB))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_HIGHLIGHTCOLORRB))->EnableWindow(FALSE);

	}

	RefreshHighlight();
}

void CCursorOptionsDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnLButtonDown(nFlags, point);
}

void CCursorOptionsDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnLButtonUp(nFlags, point);
}

void CCursorOptionsDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnRButtonDown(nFlags, point);
}

void CCursorOptionsDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnRButtonUp(nFlags, point);
}

void CCursorOptionsDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CCursorOptionsDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnLButtonDblClk(nFlags, point);
}
