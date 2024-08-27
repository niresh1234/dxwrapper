/**
* Copyright (C) 2023 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include "d3d9.h"
#include "Utils\Utils.h"

// WndProc hook
bool EnableWndProcHook = false;

AddressLookupTableD3d9 ProxyAddressLookupTable9;		// Just used for m_IDirect3D9Ex interfaces only

void AdjustWindow(HWND MainhWnd, LONG displayWidth, LONG displayHeight, bool isWindowed);

void m_IDirect3D9Ex::InitInterface()
{
	EnableWndProcHook = (EnableWndProcHook || Config.EnableImgui);

	ProxyAddressLookupTable9.SaveAddress(this, ProxyInterface);
}
void m_IDirect3D9Ex::ReleaseInterface()
{
	ProxyAddressLookupTable9.DeleteAddress(this);
}

HRESULT m_IDirect3D9Ex::QueryInterface(REFIID riid, void** ppvObj)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ") " << riid;

	if (riid == IID_IUnknown || riid == WrapperID)
	{
		HRESULT hr = ProxyInterface->QueryInterface(WrapperID, ppvObj);

		if (SUCCEEDED(hr))
		{
			*ppvObj = this;
		}

		return hr;
	}

	HRESULT hr = ProxyInterface->QueryInterface(riid, ppvObj);

	if (SUCCEEDED(hr))
	{
		if (riid == IID_IDirect3D9 || riid == IID_IDirect3D9Ex)
		{
			*ppvObj = ProxyAddressLookupTable9.FindAddress<m_IDirect3D9Ex, void, LPVOID>(*ppvObj, nullptr, riid, nullptr);
		}
		else
		{
			LOG_LIMIT(100, __FUNCTION__ << " Warning: not wrapping interface: " << riid);
		}
	}

	return hr;
}

ULONG m_IDirect3D9Ex::AddRef()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->AddRef();
}

ULONG m_IDirect3D9Ex::Release()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	ULONG ref = ProxyInterface->Release();

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

void m_IDirect3D9Ex::LogAdapterNames()
{
#ifndef DEBUG
	static bool RunOnce = true;
	if (!RunOnce)
	{
		return;
	}
	RunOnce = false;
#endif // DEBUG
	UINT Adapter = ProxyInterface->GetAdapterCount();
	for (UINT x = 0; x < Adapter; x++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier = {};
		if (SUCCEEDED(ProxyInterface->GetAdapterIdentifier(x, NULL, &Identifier)))
		{
			Logging::Log() << __FUNCTION__ << " Adapter: " << x << " " << Identifier.DeviceName << " " << Identifier.Description;
		}
	}
}

HRESULT m_IDirect3D9Ex::EnumAdapterModes(THIS_ UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

UINT m_IDirect3D9Ex::GetAdapterCount()
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterCount();
}

HRESULT m_IDirect3D9Ex::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT m_IDirect3D9Ex::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT m_IDirect3D9Ex::GetAdapterModeCount(THIS_ UINT Adapter, D3DFORMAT Format)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterModeCount(Adapter, Format);
}

HMONITOR m_IDirect3D9Ex::GetAdapterMonitor(UINT Adapter)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetAdapterMonitor(Adapter);
}

HRESULT m_IDirect3D9Ex::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HRESULT m_IDirect3D9Ex::RegisterSoftwareDevice(void *pInitializeFunction)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->RegisterSoftwareDevice(pInitializeFunction);
}

HRESULT m_IDirect3D9Ex::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT m_IDirect3D9Ex::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT m_IDirect3D9Ex::CheckDeviceMultiSampleType(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (Config.EnableWindowMode)
	{
		Windowed = true;
	}

	return ProxyInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT m_IDirect3D9Ex::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (Config.EnableWindowMode)
	{
		Windowed = true;
	}

	return ProxyInterface->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);
}

HRESULT m_IDirect3D9Ex::CheckDeviceFormatConversion(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	return ProxyInterface->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

template <typename T>
HRESULT m_IDirect3D9Ex::CreateDeviceT(D3DPRESENT_PARAMETERS& d3dpp, bool& MultiSampleFlag, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, DEVICEDETAILS& DeviceDetails, D3DDISPLAYMODEEX* pFullscreenDisplayMode, T ppReturnedDeviceInterface)
{
	if (!pPresentationParameters || !ppReturnedDeviceInterface)
	{
		return D3DERR_INVALIDCALL;
	}

	BehaviorFlags = UpdateBehaviorFlags(BehaviorFlags);

	// Create new d3d9 device
	HRESULT hr = D3DERR_INVALIDCALL;

	// Check fullscreen
	bool ForceFullscreen = TestResolution(Adapter, pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

	// Setup presentation parameters
	CopyMemory(&d3dpp, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	UpdatePresentParameter(&d3dpp, hFocusWindow, DeviceDetails, ForceFullscreen, true);

	// Add WndProc before creating d3d9 device
	if (EnableWndProcHook)
	{
		WndProc::AddWndProc(DeviceDetails.DeviceWindow, false);
	}

	// Check for AntiAliasing
	if (Config.AntiAliasing != 0)
	{
		DWORD QualityLevels = 0;

		// Check AntiAliasing quality
		for (int x = min(D3DMULTISAMPLE_16_SAMPLES, Config.AntiAliasing); x > 0; x--)
		{
			D3DMULTISAMPLE_TYPE Samples = (D3DMULTISAMPLE_TYPE)x;
			D3DFORMAT BufferFormat = (d3dpp.BackBufferFormat) ? d3dpp.BackBufferFormat : D3DFMT_X8R8G8B8;
			D3DFORMAT StencilFormat = (d3dpp.AutoDepthStencilFormat) ? d3dpp.AutoDepthStencilFormat : D3DFMT_X8R8G8B8;

			if (SUCCEEDED(ProxyInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, BufferFormat, d3dpp.Windowed, Samples, &QualityLevels)) &&
				SUCCEEDED(ProxyInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, StencilFormat, d3dpp.Windowed, Samples, &QualityLevels)))
			{
				// Update Present Parameter for Multisample
				UpdatePresentParameterForMultisample(&d3dpp, Samples, (QualityLevels > 0) ? QualityLevels - 1 : 0);

				// Create Device
				hr = CreateDeviceT(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &d3dpp, (d3dpp.Windowed) ? nullptr : pFullscreenDisplayMode, ppReturnedDeviceInterface);

				// Check if device was created successfully
				if (SUCCEEDED(hr))
				{
					MultiSampleFlag = true;
					(*ppReturnedDeviceInterface)->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
					LOG_LIMIT(3, "Setting MultiSample " << d3dpp.MultiSampleType << " Quality " << d3dpp.MultiSampleQuality);
					break;
				}
			}
		}
		if (FAILED(hr))
		{
			LOG_LIMIT(100, __FUNCTION__ << " Failed to enable AntiAliasing!");
		}
	}

	// Create Device
	if (FAILED(hr))
	{
		// Update presentation parameters
		CopyMemory(&d3dpp, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
		UpdatePresentParameter(&d3dpp, hFocusWindow, DeviceDetails, ForceFullscreen, false);

		// Create Device
		hr = CreateDeviceT(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &d3dpp, (d3dpp.Windowed) ? nullptr : pFullscreenDisplayMode, ppReturnedDeviceInterface);
	}

	return hr;
}

HRESULT m_IDirect3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!pPresentationParameters || !ppReturnedDeviceInterface)
	{
		return D3DERR_INVALIDCALL;
	}

	DEVICEDETAILS DeviceDetails;

	bool MultiSampleFlag = false;
	D3DPRESENT_PARAMETERS d3dpp;

	HRESULT hr = CreateDeviceT(d3dpp, MultiSampleFlag, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, DeviceDetails, nullptr, ppReturnedDeviceInterface);

	if (SUCCEEDED(hr))
	{
		CopyMemory(pPresentationParameters, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));

		DeviceDetails.DeviceMultiSampleFlag = MultiSampleFlag;
		DeviceDetails.DeviceMultiSampleType = pPresentationParameters->MultiSampleType;
		DeviceDetails.DeviceMultiSampleQuality = pPresentationParameters->MultiSampleQuality;

		UINT DDKey = (UINT)ppReturnedDeviceInterface + (UINT)*ppReturnedDeviceInterface + (UINT)DeviceDetails.DeviceWindow;
		DeviceDetailsMap[DDKey] = DeviceDetails;

		*ppReturnedDeviceInterface = new m_IDirect3DDevice9Ex((LPDIRECT3DDEVICE9EX)*ppReturnedDeviceInterface, this, IID_IDirect3DDevice9, DDKey);

		return D3D_OK;
	}

	Logging::LogDebug() << __FUNCTION__ << " FAILED! " << (D3DERR)hr << " " << Adapter << " " << DeviceType << " " << hFocusWindow << " " << BehaviorFlags << " " << pPresentationParameters;
	return hr;
}

UINT m_IDirect3D9Ex::GetAdapterModeCountEx(THIS_ UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return NULL;
	}

	return ProxyInterfaceEx->GetAdapterModeCountEx(Adapter, pFilter);
}

HRESULT m_IDirect3D9Ex::EnumAdapterModesEx(THIS_ UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return D3DERR_INVALIDCALL;
	}

	return ProxyInterfaceEx->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
}

HRESULT m_IDirect3D9Ex::GetAdapterDisplayModeEx(THIS_ UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return D3DERR_INVALIDCALL;
	}

	return ProxyInterfaceEx->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
}

HRESULT m_IDirect3D9Ex::CreateDeviceEx(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!pPresentationParameters || !ppReturnedDeviceInterface)
	{
		return D3DERR_INVALIDCALL;
	}

	DEVICEDETAILS DeviceDetails;

	bool MultiSampleFlag = false;
	D3DPRESENT_PARAMETERS d3dpp;

	HRESULT hr = CreateDeviceT(d3dpp, MultiSampleFlag, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, DeviceDetails, pFullscreenDisplayMode, ppReturnedDeviceInterface);

	if (SUCCEEDED(hr))
	{
		CopyMemory(pPresentationParameters, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));

		DeviceDetails.DeviceMultiSampleFlag = MultiSampleFlag;
		DeviceDetails.DeviceMultiSampleType = pPresentationParameters->MultiSampleType;
		DeviceDetails.DeviceMultiSampleQuality = pPresentationParameters->MultiSampleQuality;

		UINT DDKey = (UINT)ppReturnedDeviceInterface + (UINT)*ppReturnedDeviceInterface + (UINT)DeviceDetails.DeviceWindow;
		DeviceDetailsMap[DDKey] = DeviceDetails;

		*ppReturnedDeviceInterface = new m_IDirect3DDevice9Ex(*ppReturnedDeviceInterface, this, IID_IDirect3DDevice9Ex, DDKey);

		return D3D_OK;
	}

	Logging::LogDebug() << __FUNCTION__ << " FAILED! " << (D3DERR)hr << " " << Adapter << " " << DeviceType << " " << hFocusWindow << " " << BehaviorFlags << " " << pPresentationParameters << " " << pFullscreenDisplayMode;
	return hr;
}

HRESULT m_IDirect3D9Ex::GetAdapterLUID(THIS_ UINT Adapter, LUID * pLUID)
{
	Logging::LogDebug() << __FUNCTION__ << " (" << this << ")";

	if (!ProxyInterfaceEx)
	{
		Logging::Log() << __FUNCTION__ << " Error: Calling extension function from a non-extension device!";
		return D3DERR_INVALIDCALL;
	}

	return ProxyInterfaceEx->GetAdapterLUID(Adapter, pLUID);
}

bool m_IDirect3D9Ex::TestResolution(UINT Adapter, DWORD BackBufferWidth, DWORD BackBufferHeight)
{
	if (Config.ForceExclusiveFullscreen && BackBufferWidth && BackBufferHeight)
	{
		// Enumerate modes for format XRGB
		UINT modeCount = ProxyInterface->GetAdapterModeCount(Adapter, D3DFMT_X8R8G8B8);

		D3DDISPLAYMODE d3ddispmode;

		// Loop through all modes looking for our requested resolution
		for (UINT i = 0; i < modeCount; i++)
		{
			// Get display modes here
			ZeroMemory(&d3ddispmode, sizeof(D3DDISPLAYMODE));
			if (FAILED(ProxyInterface->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &d3ddispmode)))
			{
				LOG_LIMIT(100, __FUNCTION__ << " Error: EnumAdapterModes failed");
				break;
			}
			// Check exact height and width match
			if (d3ddispmode.Width == BackBufferWidth && d3ddispmode.Height == BackBufferHeight)
			{
				return true;
			}
		}
	}
	return false;
}

DWORD UpdateBehaviorFlags(DWORD BehaviorFlags)
{
	if (Config.ForceMixedVertexProcessing)
	{
		BehaviorFlags &= ~(D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
	}
	else if (BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
	{
		BehaviorFlags &= ~(D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	else if (BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
	{
		BehaviorFlags &= ~(D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
	}
	else if (BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		BehaviorFlags &= ~(D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
		BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	return BehaviorFlags;
}

// Update Presentation Parameters
void UpdatePresentParameter(D3DPRESENT_PARAMETERS* pPresentationParameters, HWND hFocusWindow, DEVICEDETAILS& DeviceDetails, bool ForceExclusiveFullscreen, bool SetWindow)
{
	if (!pPresentationParameters)
	{
		return;
	}

	// Set vsync
	if (Config.EnableVSync && (Config.ForceVsyncMode || pPresentationParameters->PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE))
	{
		pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else if (Config.ForceVsyncMode)
	{
		pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	// Set windowed mode if enabled
	if (ForceExclusiveFullscreen)
	{
		pPresentationParameters->Windowed = false;
		if (!pPresentationParameters->FullScreen_RefreshRateInHz)
		{
			pPresentationParameters->FullScreen_RefreshRateInHz = Utils::GetRefreshRate(DeviceDetails.DeviceWindow);
		}
		if (pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
		}
	}
	else if (Config.EnableWindowMode)
	{
		pPresentationParameters->Windowed = true;
		pPresentationParameters->FullScreen_RefreshRateInHz = 0;
	}

	// Set refresh rate if using exclusive fullscreen mode
	if (Config.OverrideRefreshRate && !pPresentationParameters->Windowed)
	{
		pPresentationParameters->FullScreen_RefreshRateInHz = Config.OverrideRefreshRate;
	}

	// Store last window data
	LONG LastBufferWidth = DeviceDetails.BufferWidth;
	LONG LastBufferHeight = DeviceDetails.BufferHeight;
	HWND LastDeviceWindow = DeviceDetails.DeviceWindow;

	// Get current window data
	if (IsWindow(pPresentationParameters->hDeviceWindow) || IsWindow(DeviceDetails.DeviceWindow) || IsWindow(hFocusWindow))
	{
		DeviceDetails.BufferWidth = (pPresentationParameters->BackBufferWidth) ? pPresentationParameters->BackBufferWidth : DeviceDetails.BufferWidth;
		DeviceDetails.BufferHeight = (pPresentationParameters->BackBufferHeight) ? pPresentationParameters->BackBufferHeight : DeviceDetails.BufferHeight;
		DeviceDetails.DeviceWindow = (IsWindow(hFocusWindow)) ? hFocusWindow :
			(IsWindow(pPresentationParameters->hDeviceWindow)) ? pPresentationParameters->hDeviceWindow :
			DeviceDetails.DeviceWindow;

		// Check if window is minimized
		if (IsIconic(DeviceDetails.DeviceWindow))
		{
			ShowWindow(DeviceDetails.DeviceWindow, SW_RESTORE);
		}

		// Remove tool and topmost window
		if (DeviceDetails.DeviceWindow != LastDeviceWindow)
		{
			LONG lStyle = GetWindowLong(DeviceDetails.DeviceWindow, GWL_STYLE);
			LONG lExStyle = GetWindowLong(DeviceDetails.DeviceWindow, GWL_EXSTYLE);

			if (!(lStyle & WS_VISIBLE))
			{
				LOG_LIMIT(3, __FUNCTION__ << " Adding window WS_VISIBLE");

				SetWindowLong(DeviceDetails.DeviceWindow, GWL_STYLE, lStyle | WS_VISIBLE);
				SetWindowPos(DeviceDetails.DeviceWindow, ((lExStyle & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_TOP),
					0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			}

			if (lExStyle & (WS_EX_TOOLWINDOW | WS_EX_TOPMOST))
			{
				LOG_LIMIT(3, __FUNCTION__ << " Removing window" << ((lExStyle & WS_EX_TOOLWINDOW) ? " WS_EX_TOOLWINDOW" : "") << ((lExStyle & WS_EX_TOPMOST) ? " WS_EX_TOPMOST" : ""));

				SetWindowLong(DeviceDetails.DeviceWindow, GWL_EXSTYLE, lExStyle & ~(WS_EX_TOOLWINDOW | WS_EX_TOPMOST));
				SetWindowPos(DeviceDetails.DeviceWindow, ((lExStyle & WS_EX_TOPMOST) ? HWND_NOTOPMOST : HWND_TOP),
					0, 0, 0, 0, ((lExStyle & WS_EX_TOPMOST) ? NULL : SWP_NOZORDER) | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			}
		}

		// Get window width and height
		if (!DeviceDetails.BufferWidth || !DeviceDetails.BufferHeight)
		{
			RECT tempRect;
			GetClientRect(DeviceDetails.DeviceWindow, &tempRect);
			DeviceDetails.BufferWidth = tempRect.right;
			DeviceDetails.BufferHeight = tempRect.bottom;
		}
	}

	// Set window size
	if (SetWindow && pPresentationParameters->Windowed && IsWindow(DeviceDetails.DeviceWindow))
	{
		bool AnyChange = (LastBufferWidth != DeviceDetails.BufferWidth || LastBufferHeight != DeviceDetails.BufferHeight || LastDeviceWindow != DeviceDetails.DeviceWindow);

		// Adjust window
		RECT Rect;
		GetClientRect(DeviceDetails.DeviceWindow, &Rect);
		if (AnyChange || Rect.right - Rect.left != DeviceDetails.BufferWidth || Rect.bottom - Rect.top != DeviceDetails.BufferHeight)
		{
			AdjustWindow(DeviceDetails.DeviceWindow, DeviceDetails.BufferWidth, DeviceDetails.BufferHeight, pPresentationParameters->Windowed);
		}

		// Set fullscreen resolution
		if (AnyChange && Config.FullscreenWindowMode)
		{
			// Get monitor info
			MONITORINFOEX infoex = {};
			infoex.cbSize = sizeof(MONITORINFOEX);
			BOOL bRet = GetMonitorInfo(Utils::GetMonitorHandle(DeviceDetails.DeviceWindow), &infoex);

			// Get resolution list for specified monitor
			DEVMODE newSettings = {};
			newSettings.dmSize = sizeof(newSettings);
			if (EnumDisplaySettings(bRet ? infoex.szDevice : nullptr, ENUM_CURRENT_SETTINGS, &newSettings) != 0)
			{
				newSettings.dmPelsWidth = DeviceDetails.BufferWidth;
				newSettings.dmPelsHeight = DeviceDetails.BufferHeight;
				newSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
				ChangeDisplaySettingsEx(bRet ? infoex.szDevice : nullptr, &newSettings, nullptr, CDS_FULLSCREEN, nullptr);
			}
		}
	}
}

// Set Presentation Parameters for Multisample
void UpdatePresentParameterForMultisample(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD MultiSampleQuality)
{
	if (!pPresentationParameters)
	{
		return;
	}

	pPresentationParameters->MultiSampleType = MultiSampleType;
	pPresentationParameters->MultiSampleQuality = MultiSampleQuality;

	pPresentationParameters->Flags &= ~D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	pPresentationParameters->SwapEffect = D3DSWAPEFFECT_DISCARD;

	if (!pPresentationParameters->EnableAutoDepthStencil)
	{
		pPresentationParameters->EnableAutoDepthStencil = true;
		pPresentationParameters->AutoDepthStencilFormat = D3DFMT_D24S8;
	}

	pPresentationParameters->BackBufferCount = (pPresentationParameters->BackBufferCount) ? pPresentationParameters->BackBufferCount : 1;
}

// Adjusting the window position for WindowMode
void AdjustWindow(HWND MainhWnd, LONG displayWidth, LONG displayHeight, bool isWindowed)
{
	if (!IsWindow(MainhWnd) || !displayWidth || !displayHeight)
	{
		LOG_LIMIT(100, __FUNCTION__ << " Error: could not set window size, nullptr.");
		return;
	}

	// Set window active and focus
	if (Config.EnableWindowMode || isWindowed)
	{
		DWORD currentThreadId = GetCurrentThreadId();
		DWORD foregroundThreadId = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

		// Attach the input of the foreground window and current window
		AttachThreadInput(currentThreadId, foregroundThreadId, TRUE);

		// Set the window as the foreground window and active
		SetForegroundWindow(MainhWnd);
		SetFocus(MainhWnd);
		SetActiveWindow(MainhWnd);
		BringWindowToTop(MainhWnd);

		// Detach the input from the foreground window
		AttachThreadInput(currentThreadId, foregroundThreadId, FALSE);

		// Move window to top if not already topmost
		LONG lExStyle = GetWindowLong(MainhWnd, GWL_EXSTYLE);
		if (!(lExStyle & WS_EX_TOPMOST))
		{
			SetWindowLong(MainhWnd, GWL_EXSTYLE, lExStyle | WS_EX_TOPMOST);
			SetWindowPos(MainhWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			SetWindowLong(MainhWnd, GWL_EXSTYLE, lExStyle & ~WS_EX_TOPMOST);
			SetWindowPos(MainhWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		}
	}

	// Get screen width and height
	LONG screenWidth = 0, screenHeight = 0;
	Utils::GetScreenSize(MainhWnd, screenWidth, screenHeight);
	RECT screenRect = {};
	Utils::GetDesktopRect(MainhWnd, screenRect);

	// Get window style
	LONG lOriginalStyle = GetWindowLong(MainhWnd, GWL_STYLE);
	LONG lStyle = lOriginalStyle | WS_VISIBLE;
	LONG lExStyle = GetWindowLong(MainhWnd, GWL_EXSTYLE);

	// Get new style
	RECT Rect = { 0, 0, displayWidth, displayHeight };
	AdjustWindowRectEx(&Rect, lStyle | WS_OVERLAPPEDWINDOW, GetMenu(MainhWnd) != NULL, lExStyle);
	if (Config.WindowModeBorder && !Config.FullscreenWindowMode && screenWidth > Rect.right - Rect.left && screenHeight > Rect.bottom - Rect.top)
	{
		lStyle |= WS_OVERLAPPEDWINDOW;
	}
	else if (Config.FullscreenWindowMode)
	{
		lStyle &= ~WS_OVERLAPPEDWINDOW;
	}

	// Set window style
	if (Config.EnableWindowMode || (lOriginalStyle & WS_VISIBLE) == NULL)
	{
		lStyle = (Config.EnableWindowMode) ? lStyle : (lOriginalStyle | WS_VISIBLE);
		SetWindowLong(MainhWnd, GWL_STYLE, lStyle);
		SetWindowPos(MainhWnd, ((lExStyle & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_TOP), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	// Get new window rect
	Rect = { 0, 0, displayWidth, displayHeight };
	//AdjustWindowRectEx(&Rect, GetWindowLong(MainhWnd, GWL_STYLE), GetMenu(MainhWnd) != NULL, lExStyle);
	AdjustWindowRectEx(&Rect, GetMenu(MainhWnd) != NULL, lExStyle, GetWindowLong(MainhWnd, GWL_EXSTYLE));
	Rect = { 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top };

	// Get upper left window position
	bool SetWindowPositionFlag = Config.FullscreenWindowMode;
	LONG xLoc = 0, yLoc = 0;
	if (Config.SetInitialWindowPosition && !Config.FullscreenWindowMode &&
		(Config.InitialWindowPositionLeft == 0 || Rect.right + (LONG)Config.InitialWindowPositionLeft <= screenWidth) &&
		(Config.InitialWindowPositionTop == 0 || Rect.bottom + (LONG)Config.InitialWindowPositionTop <= screenHeight))
	{
		SetWindowPositionFlag = true;
		xLoc = Config.InitialWindowPositionLeft;
		yLoc = Config.InitialWindowPositionTop;
	}
	else if (Config.EnableWindowMode && !Config.FullscreenWindowMode && screenWidth >= Rect.right && screenHeight >= Rect.bottom)
	{
		SetWindowPositionFlag = true;
		xLoc = (screenWidth - Rect.right) / 2;
		yLoc = (screenHeight - Rect.bottom) / 2;
	}

	// Center and adjust size of window
	if (SetWindowPositionFlag)
	{
		// Use SetWindowPlacement to center and adjust size
		WINDOWPLACEMENT wndpl = {};
		wndpl.length = sizeof(WINDOWPLACEMENT);
		if (GetWindowPlacement(MainhWnd, &wndpl))
		{
			wndpl.rcNormalPosition = { xLoc, yLoc, Rect.right + xLoc, Rect.bottom + yLoc };
			SetWindowPlacement(MainhWnd, &wndpl);
		}
		// Use SetWindowPos to center and adjust size
		else
		{
			SetWindowPos(MainhWnd, ((lExStyle & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_TOP), xLoc, yLoc, Rect.right, Rect.bottom, SWP_SHOWWINDOW | SWP_NOZORDER);
		}
	}
}
