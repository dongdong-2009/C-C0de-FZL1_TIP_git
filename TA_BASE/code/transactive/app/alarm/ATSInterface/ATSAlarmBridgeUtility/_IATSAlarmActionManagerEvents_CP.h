#pragma once
//F5534AA7-F739-4aca-872E-BFE95315C94A
// const IID SchematicDisplayInfo_IID = { 0xF5534AA7, 0xF739, 0x4aca, { 0x87,0x2E,0xBF,0xE9,0x53,0x15,0xC9,0x4A}};

template<class T>
class CProxy_IATSAlarmActionManagerEvents :
	public IConnectionPointImpl<T, &__uuidof(_IATSAlarmActionManagerEvents)>
{
public:
	HRESULT Fire_DisplaySchematic(CComBSTR bstrAlarmID, CComBSTR bstrSchematic,  SHORT leftPosition)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				BSTR bstrData1 = SysAllocString(bstrAlarmID);
				BSTR bstrData2 = SysAllocString(bstrSchematic);
				CComVariant avarParams[3];
				avarParams[2] = bstrData1;
				avarParams[2].vt = VT_BSTR;
				avarParams[1] = bstrData2;
				avarParams[1].vt = VT_BSTR;
				avarParams[0] = leftPosition;
				avarParams[0].vt = VT_I2;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 3, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};

