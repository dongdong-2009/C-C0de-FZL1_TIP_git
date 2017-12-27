/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/gwx32.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "gwx32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IGwx32 properties

BOOL IGwx32::GetRedraw()
{
	BOOL result;
	GetProperty(0x1, VT_BOOL, (void*)&result);
	return result;
}

void IGwx32::SetRedraw(BOOL propVal)
{
	SetProperty(0x1, VT_BOOL, propVal);
}

long IGwx32::GetScaleMode()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void IGwx32::SetScaleMode(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

unsigned long IGwx32::GetBackgroundColor()
{
	unsigned long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void IGwx32::SetBackgroundColor(unsigned long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// IGwx32 operations

void IGwx32::ShowWindow()
{
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::RefreshWindow()
{
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::BringWindowToTop()
{
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::HideWindow()
{
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::StartRuntime()
{
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::StopRuntime()
{
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL IGwx32::ToggleRuntime()
{
	BOOL result;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IGwx32::ExitApplication()
{
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::MinimizeWindow()
{
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::MaximizeWindow()
{
	InvokeHelper(0xd, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::RestoreWindow()
{
	InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPDISPATCH IGwx32::GetVisibleObjectFromName(LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		objectName);
	return result;
}

LPDISPATCH IGwx32::CreateEllipse(float left, float top, float width, float height, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, long edgeStyle, 
		BOOL isHidden, LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		left, top, width, height, isFilled, fillColor, lineColor, lineWidth, lineStyle, hasShadow, shadowColor, edgeStyle, isHidden, objectName);
	return result;
}

LPDISPATCH IGwx32::CreateRectangle(float left, float top, float width, float height, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, long edgeStyle, 
		BOOL isHidden, LPCTSTR objectName, BOOL rounded)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR VTS_BOOL;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		left, top, width, height, isFilled, fillColor, lineColor, lineWidth, lineStyle, hasShadow, shadowColor, edgeStyle, isHidden, objectName, rounded);
	return result;
}

LPDISPATCH IGwx32::CreateText(float x, float y, LPCTSTR text, long alignment, BOOL stretchText, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, 
		long edgeStyle, BOOL isHidden, LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_R4 VTS_R4 VTS_BSTR VTS_I4 VTS_BOOL VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		x, y, text, alignment, stretchText, isFilled, fillColor, lineColor, lineWidth, lineStyle, hasShadow, shadowColor, edgeStyle, isHidden, objectName);
	return result;
}

LPDISPATCH IGwx32::CreateSymbol(LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		objectName);
	return result;
}

BOOL IGwx32::UngroupSymbol(LPCTSTR objectName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		objectName);
	return result;
}

BOOL IGwx32::DeleteObject(LPCTSTR objectName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		objectName);
	return result;
}

void IGwx32::DeselectAllObjects()
{
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPDISPATCH IGwx32::CreatePolyline(const VARIANT& vertices, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, long edgeStyle, BOOL isHidden, LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&vertices, isFilled, fillColor, lineColor, lineWidth, lineStyle, hasShadow, shadowColor, edgeStyle, isHidden, objectName);
	return result;
}

LPDISPATCH IGwx32::CreateArc(long arcType, float centerX, float centerY, float radiusX, float radiusY, float startAngle, float endAngle, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, 
		unsigned long shadowColor, long edgeStyle, BOOL isHidden, LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		arcType, centerX, centerY, radiusX, radiusY, startAngle, endAngle, isFilled, fillColor, lineColor, lineWidth, lineStyle, hasShadow, shadowColor, edgeStyle, isHidden, objectName);
	return result;
}

void IGwx32::SetDisplayDimensions(long width, long height)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 width, height);
}

void IGwx32::GetDisplayDimensions(long* width, long* height)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 width, height);
}

void IGwx32::SetViewDimensions(long left, long top, long width, long height)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 left, top, width, height);
}

void IGwx32::GetViewDimensions(long* left, long* top, long* width, long* height)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 left, top, width, height);
}

LPDISPATCH IGwx32::GetDynamicObjectFromName(LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		objectName);
	return result;
}

BOOL IGwx32::SetAliasDefinition(LPCTSTR aliasName, LPCTSTR newDefinition)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		aliasName, newDefinition);
	return result;
}

LPDISPATCH IGwx32::GetPointObjectFromName(LPCTSTR pointName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		pointName);
	return result;
}

void IGwx32::SetWindowDimensionsPixels(long left, long top, long width, long height)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 left, top, width, height);
}

void IGwx32::SetWindowDimensionsPercent(float left, float top, float width, float height)
{
	static BYTE parms[] =
		VTS_R4 VTS_R4 VTS_R4 VTS_R4;
	InvokeHelper(0x21, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 left, top, width, height);
}

void IGwx32::GetWindowDimensionsPercent(float* left, float* top, float* width, float* height)
{
	static BYTE parms[] =
		VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4;
	InvokeHelper(0x22, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 left, top, width, height);
}

void IGwx32::GetWindowDimensionsPixels(long* left, long* top, long* width, long* height)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4;
	InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 left, top, width, height);
}

BOOL IGwx32::DeleteDynamic(LPCTSTR objectName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x24, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		objectName);
	return result;
}

BOOL IGwx32::FileOpen(LPCTSTR filename)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x25, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		filename);
	return result;
}

BOOL IGwx32::FileSave()
{
	BOOL result;
	InvokeHelper(0x26, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::FileSaveAs(LPCTSTR filename)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x27, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		filename);
	return result;
}

BOOL IGwx32::FileNew()
{
	BOOL result;
	InvokeHelper(0x28, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::FilePrint()
{
	BOOL result;
	InvokeHelper(0x29, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGwx32::GetHeadObject()
{
	LPDISPATCH result;
	InvokeHelper(0x2a, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::SetHeadObject(LPCTSTR objectName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2b, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		objectName);
	return result;
}

void IGwx32::CloseAllPopupWindows()
{
	InvokeHelper(0x2c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPDISPATCH IGwx32::OpenPopupWindow(LPCTSTR filename, BOOL modal, BOOL center, BOOL hidden)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL;
	InvokeHelper(0x2d, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, modal, center, hidden);
	return result;
}

BOOL IGwx32::ClosePopupWithTitle(LPCTSTR titleSubstring)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		titleSubstring);
	return result;
}

LPDISPATCH IGwx32::GetOpenPopupWithTitle(LPCTSTR titleSubstring)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2f, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		titleSubstring);
	return result;
}

LPDISPATCH IGwx32::CreateSizeDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, long sizeType, BOOL clip, float startSize, float endSize)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4 VTS_BOOL VTS_R4 VTS_R4;
	InvokeHelper(0x30, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, sizeType, clip, startSize, endSize);
	return result;
}

LPDISPATCH IGwx32::CreateLocationDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, float offsetX, float offsetY, BOOL slider, BOOL tracking, short numberOfDetents, BOOL continuousUpdate)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_R4 VTS_R4 VTS_BOOL VTS_BOOL VTS_I2 VTS_BOOL;
	InvokeHelper(0x31, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, offsetX, offsetY, slider, tracking, numberOfDetents, continuousUpdate);
	return result;
}

LPDISPATCH IGwx32::CreateRotationDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, float startAngle, float endAngle, float pivotX, float pivotY, BOOL clockwise, BOOL dial, BOOL tracking, short numberOfDetents, BOOL continuousUpdate)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_BOOL VTS_BOOL VTS_BOOL VTS_I2 VTS_BOOL;
	InvokeHelper(0x32, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, startAngle, endAngle, pivotX, pivotY, clockwise, dial, tracking, numberOfDetents, continuousUpdate);
	return result;
}

LPDISPATCH IGwx32::CreateHideDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL hideWhenTrue, BOOL disableObject)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL;
	InvokeHelper(0x33, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, hideWhenTrue, disableObject);
	return result;
}

LPDISPATCH IGwx32::CreateFlashDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL hideObject, BOOL flashWhenTrue, BOOL altStateWhenOff, BOOL changeFill, BOOL changeLine, BOOL changeShadow, unsigned long altFillColor, 
		unsigned long altLineColor, unsigned long altShadowColor)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x34, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, hideObject, flashWhenTrue, altStateWhenOff, changeFill, changeLine, changeShadow, altFillColor, altLineColor, altShadowColor);
	return result;
}

LPDISPATCH IGwx32::CreateAnalogColorDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL changeFill, BOOL changeLine, BOOL changeShadow, unsigned long startFillColor, unsigned long endFillColor, unsigned long startLineColor, 
		unsigned long endLineColor, unsigned long startShadowColor, unsigned long endShadowColor, BOOL defaultColorAbove, BOOL defaultColorBelow)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_BOOL;
	InvokeHelper(0x35, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, changeFill, changeLine, changeShadow, startFillColor, endFillColor, startLineColor, endLineColor, startShadowColor, endShadowColor, defaultColorAbove, defaultColorBelow);
	return result;
}

LPDISPATCH IGwx32::CreatePickDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, long pickAction, long pickType, long executionTrigger, long mouseButton, BOOL initiallySelected, LPCTSTR groupName, LPCTSTR filename, BOOL modal, BOOL center, 
		LPCTSTR value1, LPCTSTR value2, LPCTSTR value3)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL VTS_BSTR VTS_BSTR VTS_BSTR;
	InvokeHelper(0x36, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, pickAction, pickType, executionTrigger, mouseButton, initiallySelected, groupName, filename, modal, center, value1, value2, value3);
	return result;
}

LPDISPATCH IGwx32::CreateButton(long buttonType, float x, float y, LPCTSTR label, long alignment, BOOL stretchText, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, 
		unsigned long shadowColor, long edgeStyle, BOOL isHidden, LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4 VTS_R4 VTS_R4 VTS_BSTR VTS_I4 VTS_BOOL VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR;
	InvokeHelper(0x37, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		buttonType, x, y, label, alignment, stretchText, isFilled, fillColor, lineColor, lineWidth, lineStyle, hasShadow, shadowColor, edgeStyle, isHidden, objectName);
	return result;
}

LPDISPATCH IGwx32::CreateTimedate(LPCTSTR textObjectName, LPCTSTR dynamicObjectName, long formatType, LPCTSTR timeFormat, LPCTSTR dateFormat)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR;
	InvokeHelper(0x38, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		textObjectName, dynamicObjectName, formatType, timeFormat, dateFormat);
	return result;
}

LPDISPATCH IGwx32::CreateProcessPoint(LPCTSTR textObjectName, LPCTSTR dynamicObjectName, long dataType, BOOL update, BOOL dataEntry, BOOL hasInitialValue, const VARIANT& initialValue, LPCTSTR format)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4 VTS_BOOL VTS_BOOL VTS_BOOL VTS_VARIANT VTS_BSTR;
	InvokeHelper(0x39, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		textObjectName, dynamicObjectName, dataType, update, dataEntry, hasInitialValue, &initialValue, format);
	return result;
}

LPDISPATCH IGwx32::CreateAnimator(LPCTSTR symbolName, LPCTSTR dynamicObjectName, BOOL animateWhenTrue, BOOL visibleWhenOff, BOOL currentFrameWhenOff)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL;
	InvokeHelper(0x3a, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		symbolName, dynamicObjectName, animateWhenTrue, visibleWhenOff, currentFrameWhenOff);
	return result;
}

LPDISPATCH IGwx32::CreateAnalogSelector(LPCTSTR symbolName, LPCTSTR dynamicObjectName, BOOL hiddenWhenAbove, BOOL hiddenWhenBelow)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL;
	InvokeHelper(0x3b, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		symbolName, dynamicObjectName, hiddenWhenAbove, hiddenWhenBelow);
	return result;
}

LPDISPATCH IGwx32::CreateDigitalColorDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL changeColorWhenTrue, BOOL changeFill, BOOL changeLine, BOOL changeShadow, unsigned long fillColor, unsigned long lineColor, 
		unsigned long shadowColor, LPCTSTR dataSource)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_BSTR;
	InvokeHelper(0x3c, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		visibleObjectName, dynamicObjectName, changeColorWhenTrue, changeFill, changeLine, changeShadow, fillColor, lineColor, shadowColor, dataSource);
	return result;
}

LPDISPATCH IGwx32::CreateDigitalSelector(LPCTSTR symbolName, LPCTSTR dynamicObjectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x3d, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		symbolName, dynamicObjectName);
	return result;
}

CString IGwx32::GetFileName()
{
	CString result;
	InvokeHelper(0x3e, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::IsRuntimeMode()
{
	BOOL result;
	InvokeHelper(0x3f, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IGwx32::GetClientDimensionsPixels(long* left, long* top, long* width, long* height)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4;
	InvokeHelper(0x40, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 left, top, width, height);
}

BOOL IGwx32::PopCurrentSymbol()
{
	BOOL result;
	InvokeHelper(0x41, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::PopAllCurrentSymbol()
{
	BOOL result;
	InvokeHelper(0x42, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

long IGwx32::GetNumberOfTopLevelVisibles()
{
	long result;
	InvokeHelper(0x43, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGwx32::GetVisibleObjectFromIndex(long index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x44, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		index);
	return result;
}

BOOL IGwx32::OpenTagBrowser(long hWndParent, BSTR* tagName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR;
	InvokeHelper(0x45, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		hWndParent, tagName);
	return result;
}

BOOL IGwx32::SelectPaletteColor(unsigned long* SelectedColor)
{
	BOOL result;
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x46, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		SelectedColor);
	return result;
}

void IGwx32::DuplicateSelection()
{
	InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL IGwx32::QueryRanges(LPCTSTR dataSource, double* lowRange, double* highRange)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_PR8 VTS_PR8;
	InvokeHelper(0x48, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		dataSource, lowRange, highRange);
	return result;
}

BOOL IGwx32::QueryDataType(LPCTSTR dataSource, long* dataType)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_PI4;
	InvokeHelper(0x49, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		dataSource, dataType);
	return result;
}

BOOL IGwx32::SetRuntimeUpdateRate(long updateRate)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4a, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		updateRate);
	return result;
}

BOOL IGwx32::SetTooltipProperties(BOOL ShowDynamicTips, BOOL ShowDynamicObjectName, BOOL ShowDynamicObjectDescription, BOOL ShowDataSourceName, BOOL ShowDataSourceValue, BOOL ShowStaticTooltips, BOOL ShowStaticObjectName, 
		BOOL ShowStaticObjectDescription)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL;
	InvokeHelper(0x4b, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		ShowDynamicTips, ShowDynamicObjectName, ShowDynamicObjectDescription, ShowDataSourceName, ShowDataSourceValue, ShowStaticTooltips, ShowStaticObjectName, ShowStaticObjectDescription);
	return result;
}

BOOL IGwx32::SetRuntimeFocusProperties(BOOL ShowHandCursor, BOOL ShowFocusRectangle, unsigned long fillColor, unsigned long BorderColor)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BOOL VTS_BOOL VTS_I4 VTS_I4;
	InvokeHelper(0x4c, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		ShowHandCursor, ShowFocusRectangle, fillColor, BorderColor);
	return result;
}

LPDISPATCH IGwx32::GwxGetVbaProject()
{
	LPDISPATCH result;
	InvokeHelper(0x4d, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IGwx32::GetFrameWindowHandle()
{
	long result;
	InvokeHelper(0x4e, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

CString IGwx32::GetObjectTypeName()
{
	CString result;
	InvokeHelper(0x4f, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGwx32::OpenEmbeddedWindow(LPCTSTR filename, BOOL center, BOOL hidden)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL VTS_BOOL;
	InvokeHelper(0x50, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, center, hidden);
	return result;
}

BOOL IGwx32::DisplayBack()
{
	BOOL result;
	InvokeHelper(0x51, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::DisplayForward()
{
	BOOL result;
	InvokeHelper(0x52, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IGwx32::LogicalPointToScreen(long* x, long* y)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4;
	InvokeHelper(0x53, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 x, y);
}

void IGwx32::ViewHome()
{
	InvokeHelper(0x54, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::ViewUnzoom()
{
	InvokeHelper(0x55, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::ViewPan(long offsetX, long offsetY)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x56, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 offsetX, offsetY);
}

void IGwx32::ViewZoomPercent(float newZoom)
{
	static BYTE parms[] =
		VTS_R4;
	InvokeHelper(0x57, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 newZoom);
}

void IGwx32::ViewFitToWindow()
{
	InvokeHelper(0x58, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::ViewShowWholeDisplay()
{
	InvokeHelper(0x59, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::ViewBoxZoom()
{
	InvokeHelper(0x5a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IGwx32::ViewZoomCustomDialog()
{
	InvokeHelper(0x5b, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPDISPATCH IGwx32::GetParentDisplay()
{
	LPDISPATCH result;
	InvokeHelper(0x5c, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::SetAliases(LPCTSTR commandString)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5d, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		commandString);
	return result;
}

BOOL IGwx32::SetAliasesFromFile(LPCTSTR filename)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		filename);
	return result;
}

long IGwx32::GetOpenPopupCount()
{
	long result;
	InvokeHelper(0x5f, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IGwx32::GetOpenPopupByIndex(long popupIndex)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x60, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		popupIndex);
	return result;
}

BOOL IGwx32::IsPopupWindow()
{
	BOOL result;
	InvokeHelper(0x61, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::IsEmbeddedPopupWindow()
{
	BOOL result;
	InvokeHelper(0x62, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IGwx32::CloseWindow()
{
	InvokeHelper(0x63, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL IGwx32::FileOpenSetAliases(LPCTSTR filename, long aliasCommandType, LPCTSTR aliasCommandString)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_I4 VTS_BSTR;
	InvokeHelper(0x64, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		filename, aliasCommandType, aliasCommandString);
	return result;
}

LPDISPATCH IGwx32::OpenPopupWinSetAliases(LPCTSTR filename, BOOL modal, BOOL center, BOOL hidden, long aliasCommandType, LPCTSTR aliasCommandString)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL VTS_I4 VTS_BSTR;
	InvokeHelper(0x65, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, modal, center, hidden, aliasCommandType, aliasCommandString);
	return result;
}

LPDISPATCH IGwx32::OpenEmbeddedWinSetAliases(LPCTSTR filename, BOOL center, BOOL hidden, long aliasCommandType, LPCTSTR aliasCommandString)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL VTS_BOOL VTS_I4 VTS_BSTR;
	InvokeHelper(0x66, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, center, hidden, aliasCommandType, aliasCommandString);
	return result;
}

BOOL IGwx32::OpenSetAliasesDialog(long aliasCommandType, LPCTSTR aliasCommandString, LPCTSTR objectName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_BSTR VTS_BSTR;
	InvokeHelper(0x67, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		aliasCommandType, aliasCommandString, objectName);
	return result;
}

void IGwx32::LogicalPointToClient(long* x, long* y)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4;
	InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 x, y);
}

LPDISPATCH IGwx32::CreateBitmap(LPCTSTR filename, float left, float top, float width, float height, BOOL isTransparent, long transparentColor, long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, long shadowColor, long edgeStyle, 
		BOOL isHidden, LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_BOOL VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_I4 VTS_I4 VTS_BOOL VTS_BSTR;
	InvokeHelper(0x69, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, left, top, width, height, isTransparent, transparentColor, lineColor, lineWidth, lineStyle, hasShadow, shadowColor, edgeStyle, isHidden, objectName);
	return result;
}

LPDISPATCH IGwx32::CreateMetafile(LPCTSTR filename, float left, float top, float width, float height, LPCTSTR objectName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_BSTR;
	InvokeHelper(0x6a, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		filename, left, top, width, height, objectName);
	return result;
}

BOOL IGwx32::FileSaveAsType(LPCTSTR filename, short displayType)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_I2;
	InvokeHelper(0x6b, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		filename, displayType);
	return result;
}

BOOL IGwx32::TestCustomSecurityItem(LPCTSTR customString)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6c, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		customString);
	return result;
}

CString IGwx32::GetFilePath()
{
	CString result;
	InvokeHelper(0x6d, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IGwx32::GetWorkingDirectory()
{
	CString result;
	InvokeHelper(0x6e, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::ExportBitmapCaptureOfView(LPCTSTR filename)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6f, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		filename);
	return result;
}

LPDISPATCH IGwx32::CreateStateField(LPCTSTR textObjectName, LPCTSTR dynamicObjectName, BOOL update, BOOL dataEntry, BOOL hasInitialValue, const VARIANT& initialValue, LPCTSTR stateConfiguration, LPCTSTR defaultState)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL VTS_BOOL VTS_VARIANT VTS_BSTR VTS_BSTR;
	InvokeHelper(0x70, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		textObjectName, dynamicObjectName, update, dataEntry, hasInitialValue, &initialValue, stateConfiguration, defaultState);
	return result;
}

BOOL IGwx32::SetGradientFill(long style, unsigned long color1, unsigned long color2, BOOL isTwoColor, float brightness, BOOL reverseColors, short steps, float horizontalOffset, float verticalOffset)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_I4 VTS_BOOL VTS_R4 VTS_BOOL VTS_I2 VTS_R4 VTS_R4;
	InvokeHelper(0x71, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		style, color1, color2, isTwoColor, brightness, reverseColors, steps, horizontalOffset, verticalOffset);
	return result;
}

void IGwx32::ClearGradientFill()
{
	InvokeHelper(0x72, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL IGwx32::SetLayerOverrideHide(LPCTSTR layerName, BOOL hidden)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL;
	InvokeHelper(0x73, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		layerName, hidden);
	return result;
}

BOOL IGwx32::ToggleLayerOverrideHide(LPCTSTR layerName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x74, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		layerName);
	return result;
}

void IGwx32::SetModifiedFlag(BOOL Modified)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x75, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Modified);
}

BOOL IGwx32::IsModified()
{
	BOOL result;
	InvokeHelper(0x76, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::MoveSelectionToFront()
{
	BOOL result;
	InvokeHelper(0x77, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::MoveSelectionToBack()
{
	BOOL result;
	InvokeHelper(0x78, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::MoveSelectionForward()
{
	BOOL result;
	InvokeHelper(0x79, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::MoveSelectionBackward()
{
	BOOL result;
	InvokeHelper(0x7a, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::GetSystemWideLanguage(BSTR* Language, BSTR* LanguageSubset, long* LocaleID, BOOL* SynchronizeLCID)
{
	BOOL result;
	static BYTE parms[] =
		VTS_PBSTR VTS_PBSTR VTS_PI4 VTS_PBOOL;
	InvokeHelper(0x7b, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Language, LanguageSubset, LocaleID, SynchronizeLCID);
	return result;
}

BOOL IGwx32::SetSystemWideLanguage(LPCTSTR Language, LPCTSTR LanguageSubset, long LocaleID, BOOL SynchronizeLCID)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4 VTS_BOOL;
	InvokeHelper(0x7c, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Language, LanguageSubset, LocaleID, SynchronizeLCID);
	return result;
}

void IGwx32::CreateLayer(LPCTSTR layerName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x7d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 layerName);
}

void IGwx32::RemoveLayer(LPCTSTR layerName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x7e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 layerName);
}

long IGwx32::GetFileVersion()
{
	long result;
	InvokeHelper(0x7f, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IGwx32::SetFileVersion(long NewFileVersion)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x80, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		NewFileVersion);
	return result;
}

BOOL IGwx32::GetFileContainsVBA()
{
	BOOL result;
	InvokeHelper(0x81, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IGwx32::EnableRuntimeEvents(BOOL LeftButton, BOOL MiddleButton, BOOL RightButton, BOOL MouseMove, BOOL Keyboard)
{
	static BYTE parms[] =
		VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL;
	InvokeHelper(0x82, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 LeftButton, MiddleButton, RightButton, MouseMove, Keyboard);
}

long IGwx32::ReplaceTag(LPCTSTR OldSubstring, LPCTSTR NewSubstring)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x83, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		OldSubstring, NewSubstring);
	return result;
}

long IGwx32::ReplaceHost(LPCTSTR OldHostName, LPCTSTR NewHostName)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x84, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		OldHostName, NewHostName);
	return result;
}

LPDISPATCH IGwx32::GetVisibleObjectFromPosition(long x, long y, BOOL GoIntoSymbol)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_BOOL;
	InvokeHelper(0x85, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		x, y, GoIntoSymbol);
	return result;
}

long IGwx32::ReplaceFilePath(LPCTSTR OldSubstring, LPCTSTR NewSubsring)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x86, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		OldSubstring, NewSubsring);
	return result;
}

long IGwx32::ReplaceHostEx(LPCTSTR OldHostNameSubstring, LPCTSTR NewHostNameSubstring, BOOL MatchCase, BOOL MatchWholeWord)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL VTS_BOOL;
	InvokeHelper(0x87, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		OldHostNameSubstring, NewHostNameSubstring, MatchCase, MatchWholeWord);
	return result;
}

BOOL IGwx32::SetTransparency(BOOL EnableTransparency, unsigned long transparentColor, BOOL EnableAlpha, short AlphaBlending)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BOOL VTS_I4 VTS_BOOL VTS_I2;
	InvokeHelper(0x88, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		EnableTransparency, transparentColor, EnableAlpha, AlphaBlending);
	return result;
}

BOOL IGwx32::GetTransparency(BOOL* TransparencyEnabled, unsigned long* transparentColor, BOOL* AlphaEnabled, short* AlphaBlending)
{
	BOOL result;
	static BYTE parms[] =
		VTS_PBOOL VTS_PI4 VTS_PBOOL VTS_PI2;
	InvokeHelper(0x89, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		TransparencyEnabled, transparentColor, AlphaEnabled, AlphaBlending);
	return result;
}

BOOL IGwx32::SetGASThemes(LPCTSTR Themes)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x8a, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		Themes);
	return result;
}

CString IGwx32::GetGASThemes()
{
	CString result;
	InvokeHelper(0x8b, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::InsertLibraryObject(LPDISPATCH libraryObject)
{
	BOOL result;
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x8c, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		libraryObject);
	return result;
}

BOOL IGwx32::ConvertGwxSymbolToLibraryObject(LPDISPATCH GwxSymbol, LPDISPATCH* libraryObject)
{
	BOOL result;
	static BYTE parms[] =
		VTS_DISPATCH VTS_PDISPATCH;
	InvokeHelper(0x8d, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		GwxSymbol, libraryObject);
	return result;
}

BOOL IGwx32::InsertLibraryObjectByName(LPCTSTR category, LPCTSTR symbol)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x8e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		category, symbol);
	return result;
}

BOOL IGwx32::InsertLibraryObjectByIndex(LPCTSTR category, long index)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x8f, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		category, index);
	return result;
}

BOOL IGwx32::InsertOleObject(LPCTSTR progID, LPCTSTR objName)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x90, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		progID, objName);
	return result;
}

BOOL IGwx32::SelectPaletteColorRGB(VARIANT* red, VARIANT* green, VARIANT* blue)
{
	BOOL result;
	static BYTE parms[] =
		VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
	InvokeHelper(0x91, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		red, green, blue);
	return result;
}

long IGwx32::GetLoadTabsCount()
{
	long result;
	InvokeHelper(0x92, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

BOOL IGwx32::GetLoadTab(long index, BSTR* label, BSTR* File, long* AliasCmdType, BSTR* AliasCmdString, BSTR* GlobalAliasCmdString)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR VTS_PBSTR VTS_PI4 VTS_PBSTR VTS_PBSTR;
	InvokeHelper(0x93, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		index, label, File, AliasCmdType, AliasCmdString, GlobalAliasCmdString);
	return result;
}

CString IGwx32::GetEventScriptText(long Event)
{
	CString result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x94, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		Event);
	return result;
}

long IGwx32::GetEventScriptType(long Event)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x95, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Event);
	return result;
}

void IGwx32::SetEventScriptText(long Event, LPCTSTR text)
{
	static BYTE parms[] =
		VTS_I4 VTS_BSTR;
	InvokeHelper(0x96, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Event, text);
}

void IGwx32::SetEventScriptType(long Event, long Type)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x97, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Event, Type);
}
