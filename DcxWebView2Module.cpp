#include "defines.h"
#include "DcxWebView2Module.h"
#include "Dcx.h"

#pragma warning(push)
#pragma warning(disable: 26425)	//warning C26425 : Assigning 'nullptr' to a static variable.

DcxWebView2Module::~DcxWebView2Module() noexcept
{
	unload();
}

bool DcxWebView2Module::load()
{
	if (isUseable())
		return false;

	// WebView2Module Loading
	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Loading WebView2Loader.dll..."));
	m_hModule = LoadLibrary(TEXT("WebView2Loader.dll"));
	if (!m_hModule)
	{
		TString tsBuf(gsl::narrow_cast<TString::size_type>(MIRC_BUFFER_SIZE_CCH));
		GetModuleFileNameW(getDllModule(), tsBuf.to_wchr(), gsl::narrow_cast<DWORD>(tsBuf.capacity_cch()));
		tsBuf.deltok(gsl::narrow_cast<UINT>(tsBuf.numtok(L'\\')), L'\\');
		tsBuf.addtok(L"WebView2Loader.dll", L'\\');
		m_hModule = LoadLibrary(tsBuf.to_wchr());
	}
	if (m_hModule)
	{
		// Get function pointers.
#pragma warning(push)
#pragma warning(disable: 4191)
#pragma warning(disable: 26493)	//warning C26493 : Don't use C-style casts that would perform a static_cast downcast, const_cast, or reinterpret_cast. (type.4: http://go.microsoft.com/fwlink/p/?LinkID=620420)

		GetAvailableCoreWebView2BrowserVersionStringUx = reinterpret_cast<decltype(::GetAvailableCoreWebView2BrowserVersionString)*>(::GetProcAddress(m_hModule, "GetAvailableCoreWebView2BrowserVersionString"));
		CreateCoreWebView2EnvironmentWithOptionsUx = reinterpret_cast<decltype(::CreateCoreWebView2EnvironmentWithOptions)*>(::GetProcAddress(m_hModule, "CreateCoreWebView2EnvironmentWithOptions"));
		CreateCoreWebView2EnvironmentUx = reinterpret_cast<decltype(::CreateCoreWebView2Environment)*>(::GetProcAddress(m_hModule, "CreateCoreWebView2Environment"));
		CompareBrowserVersionsUx = reinterpret_cast<decltype(::CompareBrowserVersions)*>(::GetProcAddress(m_hModule, "CompareBrowserVersions"));

#pragma warning(pop)

		if (GetAvailableCoreWebView2BrowserVersionStringUx && CreateCoreWebView2EnvironmentWithOptionsUx && CreateCoreWebView2EnvironmentUx && CompareBrowserVersionsUx)
		{
			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Found WebView2 Loader Functions"));
		}
		else {
			unload();

			DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Unable to find WebView2Loader.dll functions."));

			//throw Dcx::dcxException("There was a problem loading WebView2Loader Library");
		}
	}
	else {
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Failed to load WebView2 dll"));
		Dcx::error(TEXT("LoadDLL"), TEXT("Warning Unable to Load WebView2Loader.dll, web2ctrl will not work."));
	}
	return isUseable();
}

bool DcxWebView2Module::unload() noexcept
{
	if (isUseable())
	{
		GSL_SUPPRESS(lifetime.1) FreeLibrary(m_hModule);
		m_hModule = nullptr;
	}
	// make sure all functions are nullptr
	GetAvailableCoreWebView2BrowserVersionStringUx = nullptr;
	CreateCoreWebView2EnvironmentWithOptionsUx = nullptr;
	CreateCoreWebView2EnvironmentUx = nullptr;
	CompareBrowserVersionsUx = nullptr;

	return isUseable();
}
#pragma warning(pop)


HRESULT DcxWebView2Module::dcxGetAvailableCoreWebView2BrowserVersionString(PCWSTR browserExecutableFolder, LPWSTR* versionInfo) noexcept
{
	if (GetAvailableCoreWebView2BrowserVersionStringUx)
		return GetAvailableCoreWebView2BrowserVersionStringUx(browserExecutableFolder, versionInfo);

	return E_NOTIMPL;
}

HRESULT DcxWebView2Module::dcxCreateCoreWebView2EnvironmentWithOptions(PCWSTR browserExecutableFolder, PCWSTR userDataFolder, ICoreWebView2EnvironmentOptions* environmentOptions, ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler) noexcept
{
	if (CreateCoreWebView2EnvironmentWithOptionsUx)
		return CreateCoreWebView2EnvironmentWithOptionsUx(browserExecutableFolder, userDataFolder, environmentOptions, environmentCreatedHandler);

	return E_NOTIMPL;
}

HRESULT DcxWebView2Module::dcxCreateCoreWebView2Environment(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler) noexcept
{
	if (CreateCoreWebView2EnvironmentUx)
		return CreateCoreWebView2EnvironmentUx(environmentCreatedHandler);

	return E_NOTIMPL;
}

HRESULT DcxWebView2Module::dcxCompareBrowserVersions(PCWSTR version1, PCWSTR version2, int* result) noexcept
{
	if (CompareBrowserVersionsUx)
		return CompareBrowserVersionsUx(version1, version2, result);

	return E_NOTIMPL;
}
