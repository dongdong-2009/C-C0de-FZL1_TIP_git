/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/gwx32.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IGwx32 wrapper class

class IGwx32 : public COleDispatchDriver
{
public:
	IGwx32() {}		// Calls COleDispatchDriver default constructor
	IGwx32(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IGwx32(const IGwx32& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	BOOL GetRedraw();
	void SetRedraw(BOOL);
	long GetScaleMode();
	void SetScaleMode(long);
	unsigned long GetBackgroundColor();
	void SetBackgroundColor(unsigned long);

// Operations
public:
	void ShowWindow();
	void RefreshWindow();
	void BringWindowToTop();
	void HideWindow();
	void StartRuntime();
	void StopRuntime();
	BOOL ToggleRuntime();
	void ExitApplication();
	void MinimizeWindow();
	void MaximizeWindow();
	void RestoreWindow();
	LPDISPATCH GetVisibleObjectFromName(LPCTSTR objectName);
	LPDISPATCH CreateEllipse(float left, float top, float width, float height, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, long edgeStyle, 
		BOOL isHidden, LPCTSTR objectName);
	LPDISPATCH CreateRectangle(float left, float top, float width, float height, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, long edgeStyle, 
		BOOL isHidden, LPCTSTR objectName, BOOL rounded);
	LPDISPATCH CreateText(float x, float y, LPCTSTR text, long alignment, BOOL stretchText, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, 
		long edgeStyle, BOOL isHidden, LPCTSTR objectName);
	LPDISPATCH CreateSymbol(LPCTSTR objectName);
	BOOL UngroupSymbol(LPCTSTR objectName);
	BOOL DeleteObject(LPCTSTR objectName);
	void DeselectAllObjects();
	LPDISPATCH CreatePolyline(const VARIANT& vertices, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, unsigned long shadowColor, long edgeStyle, BOOL isHidden, LPCTSTR objectName);
	LPDISPATCH CreateArc(long arcType, float centerX, float centerY, float radiusX, float radiusY, float startAngle, float endAngle, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, 
		unsigned long shadowColor, long edgeStyle, BOOL isHidden, LPCTSTR objectName);
	void SetDisplayDimensions(long width, long height);
	void GetDisplayDimensions(long* width, long* height);
	void SetViewDimensions(long left, long top, long width, long height);
	void GetViewDimensions(long* left, long* top, long* width, long* height);
	LPDISPATCH GetDynamicObjectFromName(LPCTSTR objectName);
	BOOL SetAliasDefinition(LPCTSTR aliasName, LPCTSTR newDefinition);
	LPDISPATCH GetPointObjectFromName(LPCTSTR pointName);
	void SetWindowDimensionsPixels(long left, long top, long width, long height);
	void SetWindowDimensionsPercent(float left, float top, float width, float height);
	void GetWindowDimensionsPercent(float* left, float* top, float* width, float* height);
	void GetWindowDimensionsPixels(long* left, long* top, long* width, long* height);
	BOOL DeleteDynamic(LPCTSTR objectName);
	BOOL FileOpen(LPCTSTR filename);
	BOOL FileSave();
	BOOL FileSaveAs(LPCTSTR filename);
	BOOL FileNew();
	BOOL FilePrint();
	LPDISPATCH GetHeadObject();
	BOOL SetHeadObject(LPCTSTR objectName);
	void CloseAllPopupWindows();
	LPDISPATCH OpenPopupWindow(LPCTSTR filename, BOOL modal, BOOL center, BOOL hidden);
	BOOL ClosePopupWithTitle(LPCTSTR titleSubstring);
	LPDISPATCH GetOpenPopupWithTitle(LPCTSTR titleSubstring);
	LPDISPATCH CreateSizeDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, long sizeType, BOOL clip, float startSize, float endSize);
	LPDISPATCH CreateLocationDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, float offsetX, float offsetY, BOOL slider, BOOL tracking, short numberOfDetents, BOOL continuousUpdate);
	LPDISPATCH CreateRotationDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, float startAngle, float endAngle, float pivotX, float pivotY, BOOL clockwise, BOOL dial, BOOL tracking, short numberOfDetents, BOOL continuousUpdate);
	LPDISPATCH CreateHideDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL hideWhenTrue, BOOL disableObject);
	LPDISPATCH CreateFlashDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL hideObject, BOOL flashWhenTrue, BOOL altStateWhenOff, BOOL changeFill, BOOL changeLine, BOOL changeShadow, unsigned long altFillColor, 
		unsigned long altLineColor, unsigned long altShadowColor);
	LPDISPATCH CreateAnalogColorDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL changeFill, BOOL changeLine, BOOL changeShadow, unsigned long startFillColor, unsigned long endFillColor, unsigned long startLineColor, 
		unsigned long endLineColor, unsigned long startShadowColor, unsigned long endShadowColor, BOOL defaultColorAbove, BOOL defaultColorBelow);
	LPDISPATCH CreatePickDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, long pickAction, long pickType, long executionTrigger, long mouseButton, BOOL initiallySelected, LPCTSTR groupName, LPCTSTR filename, BOOL modal, BOOL center, 
		LPCTSTR value1, LPCTSTR value2, LPCTSTR value3);
	LPDISPATCH CreateButton(long buttonType, float x, float y, LPCTSTR label, long alignment, BOOL stretchText, BOOL isFilled, unsigned long fillColor, unsigned long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, 
		unsigned long shadowColor, long edgeStyle, BOOL isHidden, LPCTSTR objectName);
	LPDISPATCH CreateTimedate(LPCTSTR textObjectName, LPCTSTR dynamicObjectName, long formatType, LPCTSTR timeFormat, LPCTSTR dateFormat);
	LPDISPATCH CreateProcessPoint(LPCTSTR textObjectName, LPCTSTR dynamicObjectName, long dataType, BOOL update, BOOL dataEntry, BOOL hasInitialValue, const VARIANT& initialValue, LPCTSTR format);
	LPDISPATCH CreateAnimator(LPCTSTR symbolName, LPCTSTR dynamicObjectName, BOOL animateWhenTrue, BOOL visibleWhenOff, BOOL currentFrameWhenOff);
	LPDISPATCH CreateAnalogSelector(LPCTSTR symbolName, LPCTSTR dynamicObjectName, BOOL hiddenWhenAbove, BOOL hiddenWhenBelow);
	LPDISPATCH CreateDigitalColorDynamic(LPCTSTR visibleObjectName, LPCTSTR dynamicObjectName, BOOL changeColorWhenTrue, BOOL changeFill, BOOL changeLine, BOOL changeShadow, unsigned long fillColor, unsigned long lineColor, 
		unsigned long shadowColor, LPCTSTR dataSource);
	LPDISPATCH CreateDigitalSelector(LPCTSTR symbolName, LPCTSTR dynamicObjectName);
	CString GetFileName();
	BOOL IsRuntimeMode();
	void GetClientDimensionsPixels(long* left, long* top, long* width, long* height);
	BOOL PopCurrentSymbol();
	BOOL PopAllCurrentSymbol();
	long GetNumberOfTopLevelVisibles();
	LPDISPATCH GetVisibleObjectFromIndex(long index);
	BOOL OpenTagBrowser(long hWndParent, BSTR* tagName);
	BOOL SelectPaletteColor(unsigned long* SelectedColor);
	void DuplicateSelection();
	BOOL QueryRanges(LPCTSTR dataSource, double* lowRange, double* highRange);
	BOOL QueryDataType(LPCTSTR dataSource, long* dataType);
	BOOL SetRuntimeUpdateRate(long updateRate);
	BOOL SetTooltipProperties(BOOL ShowDynamicTips, BOOL ShowDynamicObjectName, BOOL ShowDynamicObjectDescription, BOOL ShowDataSourceName, BOOL ShowDataSourceValue, BOOL ShowStaticTooltips, BOOL ShowStaticObjectName, 
		BOOL ShowStaticObjectDescription);
	BOOL SetRuntimeFocusProperties(BOOL ShowHandCursor, BOOL ShowFocusRectangle, unsigned long fillColor, unsigned long BorderColor);
	LPDISPATCH GwxGetVbaProject();
	long GetFrameWindowHandle();
	CString GetObjectTypeName();
	LPDISPATCH OpenEmbeddedWindow(LPCTSTR filename, BOOL center, BOOL hidden);
	BOOL DisplayBack();
	BOOL DisplayForward();
	void LogicalPointToScreen(long* x, long* y);
	void ViewHome();
	void ViewUnzoom();
	void ViewPan(long offsetX, long offsetY);
	void ViewZoomPercent(float newZoom);
	void ViewFitToWindow();
	void ViewShowWholeDisplay();
	void ViewBoxZoom();
	void ViewZoomCustomDialog();
	LPDISPATCH GetParentDisplay();
	BOOL SetAliases(LPCTSTR commandString);
	BOOL SetAliasesFromFile(LPCTSTR filename);
	long GetOpenPopupCount();
	LPDISPATCH GetOpenPopupByIndex(long popupIndex);
	BOOL IsPopupWindow();
	BOOL IsEmbeddedPopupWindow();
	void CloseWindow();
	BOOL FileOpenSetAliases(LPCTSTR filename, long aliasCommandType, LPCTSTR aliasCommandString);
	LPDISPATCH OpenPopupWinSetAliases(LPCTSTR filename, BOOL modal, BOOL center, BOOL hidden, long aliasCommandType, LPCTSTR aliasCommandString);
	LPDISPATCH OpenEmbeddedWinSetAliases(LPCTSTR filename, BOOL center, BOOL hidden, long aliasCommandType, LPCTSTR aliasCommandString);
	BOOL OpenSetAliasesDialog(long aliasCommandType, LPCTSTR aliasCommandString, LPCTSTR objectName);
	void LogicalPointToClient(long* x, long* y);
	LPDISPATCH CreateBitmap(LPCTSTR filename, float left, float top, float width, float height, BOOL isTransparent, long transparentColor, long lineColor, long lineWidth, long lineStyle, BOOL hasShadow, long shadowColor, long edgeStyle, 
		BOOL isHidden, LPCTSTR objectName);
	LPDISPATCH CreateMetafile(LPCTSTR filename, float left, float top, float width, float height, LPCTSTR objectName);
	BOOL FileSaveAsType(LPCTSTR filename, short displayType);
	BOOL TestCustomSecurityItem(LPCTSTR customString);
	CString GetFilePath();
	CString GetWorkingDirectory();
	BOOL ExportBitmapCaptureOfView(LPCTSTR filename);
	LPDISPATCH CreateStateField(LPCTSTR textObjectName, LPCTSTR dynamicObjectName, BOOL update, BOOL dataEntry, BOOL hasInitialValue, const VARIANT& initialValue, LPCTSTR stateConfiguration, LPCTSTR defaultState);
	BOOL SetGradientFill(long style, unsigned long color1, unsigned long color2, BOOL isTwoColor, float brightness, BOOL reverseColors, short steps, float horizontalOffset, float verticalOffset);
	void ClearGradientFill();
	BOOL SetLayerOverrideHide(LPCTSTR layerName, BOOL hidden);
	BOOL ToggleLayerOverrideHide(LPCTSTR layerName);
	void SetModifiedFlag(BOOL Modified);
	BOOL IsModified();
	BOOL MoveSelectionToFront();
	BOOL MoveSelectionToBack();
	BOOL MoveSelectionForward();
	BOOL MoveSelectionBackward();
	BOOL GetSystemWideLanguage(BSTR* Language, BSTR* LanguageSubset, long* LocaleID, BOOL* SynchronizeLCID);
	BOOL SetSystemWideLanguage(LPCTSTR Language, LPCTSTR LanguageSubset, long LocaleID, BOOL SynchronizeLCID);
	void CreateLayer(LPCTSTR layerName);
	void RemoveLayer(LPCTSTR layerName);
	long GetFileVersion();
	long SetFileVersion(long NewFileVersion);
	BOOL GetFileContainsVBA();
	void EnableRuntimeEvents(BOOL LeftButton, BOOL MiddleButton, BOOL RightButton, BOOL MouseMove, BOOL Keyboard);
	long ReplaceTag(LPCTSTR OldSubstring, LPCTSTR NewSubstring);
	long ReplaceHost(LPCTSTR OldHostName, LPCTSTR NewHostName);
	LPDISPATCH GetVisibleObjectFromPosition(long x, long y, BOOL GoIntoSymbol);
	long ReplaceFilePath(LPCTSTR OldSubstring, LPCTSTR NewSubsring);
	long ReplaceHostEx(LPCTSTR OldHostNameSubstring, LPCTSTR NewHostNameSubstring, BOOL MatchCase, BOOL MatchWholeWord);
	BOOL SetTransparency(BOOL EnableTransparency, unsigned long transparentColor, BOOL EnableAlpha, short AlphaBlending);
	BOOL GetTransparency(BOOL* TransparencyEnabled, unsigned long* transparentColor, BOOL* AlphaEnabled, short* AlphaBlending);
	BOOL SetGASThemes(LPCTSTR Themes);
	CString GetGASThemes();
	BOOL InsertLibraryObject(LPDISPATCH libraryObject);
	BOOL ConvertGwxSymbolToLibraryObject(LPDISPATCH GwxSymbol, LPDISPATCH* libraryObject);
	BOOL InsertLibraryObjectByName(LPCTSTR category, LPCTSTR symbol);
	BOOL InsertLibraryObjectByIndex(LPCTSTR category, long index);
	BOOL InsertOleObject(LPCTSTR progID, LPCTSTR objName);
	BOOL SelectPaletteColorRGB(VARIANT* red, VARIANT* green, VARIANT* blue);
	long GetLoadTabsCount();
	BOOL GetLoadTab(long index, BSTR* label, BSTR* File, long* AliasCmdType, BSTR* AliasCmdString, BSTR* GlobalAliasCmdString);
	CString GetEventScriptText(long Event);
	long GetEventScriptType(long Event);
	void SetEventScriptText(long Event, LPCTSTR text);
	void SetEventScriptType(long Event, long Type);
};
