//-----------------------------------------------------------------------------
// File: GetDXVer.cpp
//
// Desc: Demonstrates how applications can detect what version of DirectX
//       is installed.
//
// (C) Copyright Microsoft Corp.  All rights reserved.
//-----------------------------------------------------------------------------

#include "defines.h"

#ifdef DCX_USE_DXSDK

#include <windows.h>
#include <stdio.h>
#pragma warning(push)
#pragma warning(disable: 4005 4995)
#include <tchar.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning(pop)
#include <dxdiag.h>

#ifdef __INTEL_COMPILER
#pragma warning( disable : 1419 )
#endif

HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter ) noexcept;
HRESULT GetDirectXVersionViaFileVersions( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
HRESULT GetFileVersion( const TCHAR *const szPath, ULARGE_INTEGER* pllFileVersion );
constexpr ULARGE_INTEGER MakeInt64( WORD a, WORD b, WORD c, WORD d ) noexcept;
constexpr int CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 ) noexcept;




//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns the DirectX version.
// Arguments:
//      pdwDirectXVersion - This can be nullptr.  If non-nullptr, the return value is:
//              0x00000000 = No DirectX installed
//              0x00010000 = DirectX 1.0 installed
//              0x00020000 = DirectX 2.0 installed
//              0x00030000 = DirectX 3.0 installed
//              0x00030001 = DirectX 3.0a installed
//              0x00050000 = DirectX 5.0 installed
//              0x00060000 = DirectX 6.0 installed
//              0x00060100 = DirectX 6.1 installed
//              0x00060101 = DirectX 6.1a installed
//              0x00070000 = DirectX 7.0 installed
//              0x00070001 = DirectX 7.0a installed
//              0x00080000 = DirectX 8.0 installed
//              0x00080100 = DirectX 8.1 installed
//              0x00080101 = DirectX 8.1a installed
//              0x00080102 = DirectX 8.1b installed
//              0x00080200 = DirectX 8.2 installed
//              0x00090000 = DirectX 9.0 installed
//      strDirectXVersion - Destination string to receive a string name of the DirectX Version.  Can be nullptr.
//      cchDirectXVersion - Size of destination buffer in characters.  Length should be at least 10 chars.
// Returns: S_OK if the function succeeds.
//          E_FAIL if the DirectX version info couldn't be determined.
//
// Please note that this code is intended as a general guideline. Your
// app will probably be able to simply query for functionality (via
// QueryInterface) for one or two components.
//
// Also please ensure your app will run on future releases of DirectX.
// For example:
//     "if( dwDirectXVersion != 0x00080100 ) return false;" is VERY BAD.
//     "if( dwDirectXVersion < 0x00080100 ) return false;" is MUCH BETTER.
//-----------------------------------------------------------------------------
HRESULT GetDXVersion(DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion)
{
	bool bGotDirectXVersion = false;

	// Init values to unknown
	if (pdwDirectXVersion)
		*pdwDirectXVersion = 0;
	if (strDirectXVersion && cchDirectXVersion > 0)
		strDirectXVersion[0] = 0;

	DWORD dwDirectXVersionMajor = 0;
	DWORD dwDirectXVersionMinor = 0;
	TCHAR cDirectXVersionLetter = TEXT(' ');

	// First, try to use dxdiag's COM interface to get the DirectX version.
	// The only downside is this will only work on DirectX9 or later.
	if (SUCCEEDED(GetDirectXVersionViaDxDiag(&dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter)))
		bGotDirectXVersion = true;

	if (!bGotDirectXVersion)
	{
		// Getting the DirectX version info from DxDiag failed,
		// so most likely we are on DirectX8.x or earlier
		if (SUCCEEDED(GetDirectXVersionViaFileVersions(&dwDirectXVersionMajor, &dwDirectXVersionMinor, &cDirectXVersionLetter)))
			bGotDirectXVersion = true;
	}

	// If both techniques failed, then return E_FAIL
	if (!bGotDirectXVersion)
		return E_FAIL;

	// Set the output values to what we got and return
	cDirectXVersionLetter = gsl::narrow_cast<TCHAR>(tolower(cDirectXVersionLetter));

	if (pdwDirectXVersion)
	{
		// If pdwDirectXVersion is non-nullptr, then set it to something
		// like 0x00080102 which would represent DirectX8.1b
		DWORD dwDirectXVersion = dwDirectXVersionMajor;
		dwDirectXVersion <<= 8;
		dwDirectXVersion += dwDirectXVersionMinor;
		dwDirectXVersion <<= 8;
		if (cDirectXVersionLetter >= TEXT('a') && cDirectXVersionLetter <= TEXT('z'))
			dwDirectXVersion += (cDirectXVersionLetter - TEXT('a')) + 1;

		*pdwDirectXVersion = dwDirectXVersion;
	}

	if (strDirectXVersion && cchDirectXVersion > 0)
	{
		// If strDirectXVersion is non-nullptr, then set it to something
		// like "8.1b" which would represent DirectX8.1b
		if (cDirectXVersionLetter == TEXT(' '))
			StringCchPrintf(strDirectXVersion, gsl::narrow_cast<size_t>(cchDirectXVersion), TEXT("%u.%u"), dwDirectXVersionMajor, dwDirectXVersionMinor);
		else
			StringCchPrintf(strDirectXVersion, gsl::narrow_cast<size_t>(cchDirectXVersion), TEXT("%u.%u%c"), dwDirectXVersionMajor, dwDirectXVersionMinor, cDirectXVersionLetter);
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetDirectXVersionViaDxDiag()
// Desc: Tries to get the DirectX version from DxDiag's COM interface
//-----------------------------------------------------------------------------
HRESULT GetDirectXVersionViaDxDiag(DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter) noexcept
{
	// Init COM.  COM may fail if its already been inited with a different
	// concurrency model.  And if it fails you shouldn't release it.
	HRESULT hr = CoInitialize(nullptr);
	const bool bCleanupCOM = SUCCEEDED(hr);

	// Get an IDxDiagProvider
	bool bGotDirectXVersion = false;
	IDxDiagProvider* pDxDiagProvider = nullptr;
	hr = CoCreateInstance(CLSID_DxDiagProvider,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IDxDiagProvider,
		(LPVOID*)&pDxDiagProvider);
	if (SUCCEEDED(hr))
	{
		// Fill out a DXDIAG_INIT_PARAMS struct
		DXDIAG_INIT_PARAMS dxDiagInitParam;
		ZeroMemory(&dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS));
		dxDiagInitParam.dwSize = sizeof(DXDIAG_INIT_PARAMS);
		dxDiagInitParam.dwDxDiagHeaderVersion = DXDIAG_DX9_SDK_VERSION;
		dxDiagInitParam.bAllowWHQLChecks = false;
		dxDiagInitParam.pReserved = nullptr;

		// Init the m_pDxDiagProvider
		hr = pDxDiagProvider->Initialize(&dxDiagInitParam);
		if (SUCCEEDED(hr))
		{
			IDxDiagContainer* pDxDiagRoot = nullptr;
			IDxDiagContainer* pDxDiagSystemInfo = nullptr;

			// Get the DxDiag root container
			hr = pDxDiagProvider->GetRootContainer(&pDxDiagRoot);
			if (SUCCEEDED(hr))
			{
				// Get the object called DxDiag_SystemInfo
				hr = pDxDiagRoot->GetChildContainer(L"DxDiag_SystemInfo", &pDxDiagSystemInfo);
				if (SUCCEEDED(hr))
				{
					VARIANT var;
					VariantInit(&var);

					bool bSuccessGettingMajor = false;
					bool bSuccessGettingMinor = false;
					bool bSuccessGettingLetter = false;

					// Get the "dwDirectXVersionMajor" property
					hr = pDxDiagSystemInfo->GetProp(L"dwDirectXVersionMajor", &var);
					if (SUCCEEDED(hr) && var.vt == VT_UI4)
					{
						if (pdwDirectXVersionMajor)
							*pdwDirectXVersionMajor = var.ulVal;
						bSuccessGettingMajor = true;
					}
					VariantClear(&var);

					// Get the "dwDirectXVersionMinor" property
					hr = pDxDiagSystemInfo->GetProp(L"dwDirectXVersionMinor", &var);
					if (SUCCEEDED(hr) && var.vt == VT_UI4)
					{
						if (pdwDirectXVersionMinor)
							*pdwDirectXVersionMinor = var.ulVal;
						bSuccessGettingMinor = true;
					}
					VariantClear(&var);

					// Get the "szDirectXVersionLetter" property
					hr = pDxDiagSystemInfo->GetProp(L"szDirectXVersionLetter", &var);
					if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
					{
						*pcDirectXVersionLetter = var.bstrVal[0];
						bSuccessGettingLetter = true;
					}
					VariantClear(&var);

					// If it all worked right, then mark it down
					if (bSuccessGettingMajor && bSuccessGettingMinor && bSuccessGettingLetter)
						bGotDirectXVersion = true;

					pDxDiagSystemInfo->Release();
				}

				pDxDiagRoot->Release();
			}
		}

		pDxDiagProvider->Release();
	}

	if (bCleanupCOM)
		CoUninitialize();

	if (bGotDirectXVersion)
		return S_OK;
	else
		return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: GetDirectXVersionViaFileVersions()
// Desc: Tries to get the DirectX version by looking at DirectX file versions
//-----------------------------------------------------------------------------
HRESULT GetDirectXVersionViaFileVersions(DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter)
{
	ULARGE_INTEGER llFileVersion = { 0 };
	TCHAR szPath[512] = { 0 };
	BOOL bFound = false;

	if (GetSystemDirectory(&szPath[0], MAX_PATH) != 0)
	{
		TCHAR szFile[512] = { 0 };
		szPath[MAX_PATH - 1] = 0;

		// Switch off the ddraw version
		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\ddraw.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if (CompareLargeInts(llFileVersion, MakeInt64(4, 2, 0, 95)) >= 0) // Win9x version
			{
				// flle is >= DirectX1.0 version, so we must be at least DirectX1.0
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 1;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}

			if (CompareLargeInts(llFileVersion, MakeInt64(4, 3, 0, 1096)) >= 0) // Win9x version
			{
				// flle is is >= DirectX2.0 version, so we must DirectX2.0 or DirectX2.0a (no redist change)
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 2;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}

			if (CompareLargeInts(llFileVersion, MakeInt64(4, 4, 0, 68)) >= 0) // Win9x version
			{
				// flle is is >= DirectX3.0 version, so we must be at least DirectX3.0
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 3;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}
		}

		// Switch off the d3drg8x.dll version
		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\d3drg8x.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if (CompareLargeInts(llFileVersion, MakeInt64(4, 4, 0, 70)) >= 0) // Win9x version
			{
				// d3drg8x.dll is the DirectX3.0a version, so we must be DirectX3.0a or DirectX3.0b  (no redist change)
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 3;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT('a');
				bFound = true;
			}
		}

		// Switch off the ddraw version
		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\ddraw.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if (CompareLargeInts(llFileVersion, MakeInt64(4, 5, 0, 155)) >= 0) // Win9x version
			{
				// ddraw.dll is the DirectX5.0 version, so we must be DirectX5.0 or DirectX5.2 (no redist change)
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 5;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}

			if (CompareLargeInts(llFileVersion, MakeInt64(4, 6, 0, 318)) >= 0) // Win9x version
			{
				// ddraw.dll is the DirectX6.0 version, so we must be at least DirectX6.0
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 6;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}

			if (CompareLargeInts(llFileVersion, MakeInt64(4, 6, 0, 436)) >= 0) // Win9x version
			{
				// ddraw.dll is the DirectX6.1 version, so we must be at least DirectX6.1
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 6;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 1;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}
		}

		// Switch off the dplayx.dll version
		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\dplayx.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if (CompareLargeInts(llFileVersion, MakeInt64(4, 6, 3, 518)) >= 0) // Win9x version
			{
				// ddraw.dll is the DirectX6.1 version, so we must be at least DirectX6.1a
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 6;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 1;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT('a');
				bFound = true;
			}
		}

		// Switch off the ddraw version
		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\ddraw.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if (CompareLargeInts(llFileVersion, MakeInt64(4, 7, 0, 700)) >= 0) // Win9x version
			{
				// TODO: find win2k version

				// ddraw.dll is the DirectX7.0 version, so we must be at least DirectX7.0
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 7;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}
		}

		// Switch off the dinput version
		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\dinput.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if (CompareLargeInts(llFileVersion, MakeInt64(4, 7, 0, 716)) >= 0) // Win9x version
			{
				// ddraw.dll is the DirectX7.0 version, so we must be at least DirectX7.0a
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 7;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT('a');
				bFound = true;
			}
		}

		// Switch off the ddraw version
		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\ddraw.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if ((HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts(llFileVersion, MakeInt64(4, 8, 0, 400)) >= 0) || // Win9x version
				(HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts(llFileVersion, MakeInt64(5, 1, 2258, 400)) >= 0)) // Win2k/WinXP version
			{
				// ddraw.dll is the DirectX8.0 version, so we must be at least DirectX8.0 or DirectX8.0a (no redist change)
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 8;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}
		}

		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\d3d8.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if ((HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts(llFileVersion, MakeInt64(4, 8, 1, 881)) >= 0) || // Win9x version
				(HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts(llFileVersion, MakeInt64(5, 1, 2600, 881)) >= 0)) // Win2k/WinXP version
			{
				// d3d8.dll is the DirectX8.1 version, so we must be at least DirectX8.1
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 8;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 1;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}

			if ((HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts(llFileVersion, MakeInt64(4, 8, 1, 901)) >= 0) || // Win9x version
				(HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts(llFileVersion, MakeInt64(5, 1, 2600, 901)) >= 0)) // Win2k/WinXP version
			{
				// d3d8.dll is the DirectX8.1a version, so we must be at least DirectX8.1a
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 8;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 1;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT('a');
				bFound = true;
			}
		}

		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\mpg2splt.ax"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if (CompareLargeInts(llFileVersion, MakeInt64(6, 3, 1, 885)) >= 0) // Win9x/Win2k/WinXP version
			{
				// quartz.dll is the DirectX8.1b version, so we must be at least DirectX8.1b
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 8;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 1;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT('b');
				bFound = true;
			}
		}

		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\dpnet.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			if ((HIWORD(llFileVersion.HighPart) == 4 && CompareLargeInts(llFileVersion, MakeInt64(4, 9, 0, 134)) >= 0) || // Win9x version
				(HIWORD(llFileVersion.HighPart) == 5 && CompareLargeInts(llFileVersion, MakeInt64(5, 2, 3677, 134)) >= 0)) // Win2k/WinXP version
			{
				// dpnet.dll is the DirectX8.2 version, so we must be at least DirectX8.2
				if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 8;
				if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 2;
				if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
				bFound = true;
			}
		}

		StringCchCopy(&szFile[0], Dcx::countof(szFile), &szPath[0]);
		StringCchCat(&szFile[0], Dcx::countof(szFile), TEXT("\\d3d9.dll"));
		if (SUCCEEDED(GetFileVersion(&szFile[0], &llFileVersion)))
		{
			// File exists, but be at least DirectX9
			if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 9;
			if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
			if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
			bFound = true;
		}
	}

	if (!bFound)
	{
		// No DirectX installed
		if (pdwDirectXVersionMajor) *pdwDirectXVersionMajor = 0;
		if (pdwDirectXVersionMinor) *pdwDirectXVersionMinor = 0;
		if (pcDirectXVersionLetter) *pcDirectXVersionLetter = TEXT(' ');
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetFileVersion()
// Desc: Returns ULARGE_INTEGER with a file version of a file, or a failure code.
//-----------------------------------------------------------------------------
HRESULT GetFileVersion(const TCHAR *const szPath, ULARGE_INTEGER* pllFileVersion)
{
	//if (szPath == nullptr || pllFileVersion == nullptr)
	//	return E_INVALIDARG;
	//
	//DWORD dwHandle;
	//UINT  cb;
	//cb = GetFileVersionInfoSize(szPath, &dwHandle);
	//if (cb > 0)
	//{
	//	BYTE* pFileVersionBuffer;
	//
	//	try {
	//		pFileVersionBuffer = new BYTE[cb];
	//	}
	//	catch (const std::bad_alloc)
	//	{
	//		return E_OUTOFMEMORY;
	//	}
	//
	//	if (GetFileVersionInfo(szPath, 0, cb, pFileVersionBuffer))
	//	{
	//		VS_FIXEDFILEINFO* pVersion = nullptr;
	//		if (VerQueryValue(pFileVersionBuffer, TEXT("\\"), (VOID**)&pVersion, &cb) &&
	//			pVersion != nullptr)
	//		{
	//			pllFileVersion->HighPart = pVersion->dwFileVersionMS;
	//			pllFileVersion->LowPart = pVersion->dwFileVersionLS;
	//			delete[] pFileVersionBuffer;
	//			return S_OK;
	//		}
	//	}
	//
	//	delete[] pFileVersionBuffer;
	//}
	//
	//return E_FAIL;

	try {
		if (szPath == nullptr || pllFileVersion == nullptr)
		return E_INVALIDARG;

		DWORD dwHandle = 0;

		if (UINT  cb = GetFileVersionInfoSize(szPath, &dwHandle); cb > 0)
		{
			if (auto pFileVersionBuffer = std::make_unique<BYTE[]>(cb); GetFileVersionInfo(szPath, 0, cb, pFileVersionBuffer.get()))
			{
				if (VS_FIXEDFILEINFO* pVersion = nullptr; VerQueryValue(pFileVersionBuffer.get(), TEXT("\\"), (VOID**)&pVersion, &cb) && pVersion)
				{
					pllFileVersion->HighPart = pVersion->dwFileVersionMS;
					pllFileVersion->LowPart = pVersion->dwFileVersionLS;
					return S_OK;
				}
			}
		}

	}
	catch (const std::bad_alloc)
	{
		return E_OUTOFMEMORY;
	}
	return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: MakeInt64()
// Desc: Returns a ULARGE_INTEGER where a<<48|b<<32|c<<16|d<<0
//-----------------------------------------------------------------------------
constexpr ULARGE_INTEGER MakeInt64(WORD a, WORD b, WORD c, WORD d) noexcept
{
	//ULARGE_INTEGER ull;
	//ull.HighPart = (DWORD)MAKELONG(b, a);
	//ull.LowPart = (DWORD)MAKELONG(d, c);
	//return ull;

	return{ static_cast<DWORD>(MAKELONG(d, c)), static_cast<DWORD>(MAKELONG(b, a)) };
}




//-----------------------------------------------------------------------------
// Name: CompareLargeInts()
// Desc: Returns 1 if ullParam1 > ullParam2
//       Returns 0 if ullParam1 = ullParam2
//       Returns -1 if ullParam1 < ullParam2
//-----------------------------------------------------------------------------
constexpr int CompareLargeInts(ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2) noexcept
{
	if (ullParam1.HighPart > ullParam2.HighPart)
		return 1;
	if (ullParam1.HighPart < ullParam2.HighPart)
		return -1;

	if (ullParam1.LowPart > ullParam2.LowPart)
		return 1;
	if (ullParam1.LowPart < ullParam2.LowPart)
		return -1;

	return 0;
}
#endif // ifdef DCX_USE_DXSDK
