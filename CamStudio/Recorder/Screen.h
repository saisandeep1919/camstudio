#pragma once
#include "camcursor.h"
#include "profile.h"
#include "ximage.h"

class CCamera
{
public:
	CCamera();
	CCamera(const CCamCursor& rCursor, const sCaptionOpts& rCaption, const sTimestampOpts& rTimestamp, const sWatermarkOpts& rWatermark)
		: m_cCursor(rCursor)
		, m_sCaption(rCaption)
		, m_sTimestamp(rTimestamp)
		, m_sWatermark(rWatermark)
		, m_uFrameCount(0)
	{
	}
	virtual ~CCamera();

	size_t FrameCount() const					{return m_uFrameCount;}
	HCURSOR Save(HCURSOR hCursor)				{return m_cCursor.Save(hCursor);}
	void Set(const CCamCursor& rCursor)			{m_cCursor = rCursor;}
	void Set(const sCaptionOpts& rCaption)		{m_sCaption = rCaption;}
	void Set(const sTimestampOpts& rTimestamp)	{m_sTimestamp = rTimestamp;}
	void Set(const sWatermarkOpts& rWatermark)	
	{
		// if name changed, or brightness or contrast
		// must reload image
		bool bReload = (m_strWatermarkName != rWatermark.m_iaWatermark.text)
			|| (m_sWatermark.m_iaWatermark.m_lBrightness != rWatermark.m_iaWatermark.m_lBrightness)
			|| (m_sWatermark.m_iaWatermark.m_lContrast != rWatermark.m_iaWatermark.m_lContrast);
		m_sWatermark = rWatermark;
		if (bReload) {
			LoadWatermark();
		}
	}
	void SetView(const CRect& rView)
	{
		m_rectView = rView;
		m_rectFrame = CRect(0, 0, m_rectView.Width(), m_rectView.Height());
	}

	bool CaptureFrame(const CRect& rectView);
	LPBITMAPINFOHEADER Image() const
	{
		return static_cast<LPBITMAPINFOHEADER>(m_cImage.GetDIB());
	}

protected:
	void InsertCaption(CDC *pDC)				{VERIFY(AddCaption(pDC));}
	void InsertTimeStamp(CDC *pDC)				{VERIFY(AddTimestamp(pDC));}
	void InsertWatermark(CDC *pDC)				{VERIFY(AddWatermark(pDC));}
	void InsertCursor(CDC *pDC)					{VERIFY(AddCursor(pDC));}
	void InsertText(CDC* pDC, const CRect& rRect, TextAttributes& rTextAttrs);
	void InsertImage(CDC *pDC, CRect& rectFrame, const ImageAttributes& rImgAttr);
private:
	size_t m_uFrameCount;			// count of images captured
	CxImage m_cImage;				// result of CaptureFrame
	CxImage m_imageWatermark;		// Watermark Image
	CString m_strWatermarkName;		// Watermark filename
	CRect m_rectView;				// screen view to capture
	CRect m_rectFrame;				// logical frame to capture
	// annotation objects
	CCamCursor m_cCursor;
	sCaptionOpts m_sCaption;
	sTimestampOpts m_sTimestamp;
	sWatermarkOpts m_sWatermark;

	void InsertHighLight(CDC *pDC, CPoint pt);
	bool LoadWatermark();
	bool AddTimestamp(CDC* pDC);
	bool AddCaption(CDC* pDC);
	bool AddWatermark(CDC* pDC);
	bool AddCursor(CDC* pDC);
	bool Annotate(CDC* pDC);
};
