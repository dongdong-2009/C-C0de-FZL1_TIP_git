#pragma once

template<class T>
class CProxy_IATSEventSubscriptionEvents :
	public IConnectionPointImpl<T, &__uuidof(_IATSEventSubscriptionEvents)>
{
public:
	HRESULT Fire_EventUpdate(SHORT error, IATSEventList * pATSEvents)
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
				CComVariant avarParams[2];
				avarParams[1] = error;
				avarParams[1].vt = VT_I2;
				avarParams[0] = pATSEvents;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 2, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};

