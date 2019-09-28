//---------------------------------------------------------------------------
#include <vcl.h>

#include <mmdeviceapi.h>
#include <endpointvolume.h>

#pragma hdrstop

#include "volume.h"

/*---------------------------------------------------------------------------
	Volume range: 0.0f - 1.0f (scalar)
*/
double ChangeVolume(double nVolume, BOOL bRelative)
{
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
						  __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	if (hr != S_OK) throw Exception(L"ChangeVolume()::CoCreateInstance() failed");

	IMMDevice *defaultDevice = NULL;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	if (hr != S_OK) throw Exception(L"ChangeVolume()::GetDefaultAudioEndpoint() failed");
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume *endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
		 CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	if (hr != S_OK) throw Exception(L"ChangeVolume()::Activate() failed");
	defaultDevice->Release();
	defaultDevice = NULL;

	float currentVolume = 0;
	double newVolume;
	//hr = endpointVolume->GetMasterVolumeLevel(&currentVolume);
	//printf("Current volume in dB is: %f\n", currentVolume);
	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	if (hr != S_OK) throw Exception(L"ChangeVolume()::GetMasterVolumeLevelScalar() failed");
	//printf("Current volume as a scalar is: %f\n", currentVolume);

	if (bRelative) {
		newVolume = nVolume + (double)currentVolume;
		if (newVolume > 1) newVolume = 1;
		if (newVolume < 0) newVolume = 0;
	} else {
		newVolume = nVolume;
	}

	//hr = endpointVolume->SetMasterVolumeLevel(newVolume, NULL);
	hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
	if (hr != S_OK) throw Exception(L"ChangeVolume()::SetMasterVolumeLevelScalar() failed");

	endpointVolume->Release();
	CoUninitialize();

	return newVolume;
}

//---------------------------------------------------------------------------
BOOL ChangeMute(BOOL bMute, BOOL bToggle, BOOL bOnlyPeek)
{
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
						  __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	if (hr != S_OK) throw Exception(L"ChangeMute()::CoCreateInstance() failed");

	IMMDevice *defaultDevice = NULL;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	if (hr != S_OK) throw Exception(L"ChangeMute()::GetDefaultAudioEndpoint() failed");
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume *endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
		 CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	if (hr != S_OK) throw Exception(L"ChangeMute()::Activate() failed");
	defaultDevice->Release();
	defaultDevice = NULL;

	BOOL currentMute = FALSE;
	BOOL newMute;
	hr = endpointVolume->GetMute(&currentMute);
	if (hr != S_OK) throw Exception(L"ChangeMute()::GetMute() failed");

	if (bOnlyPeek) {
		newMute = currentMute;
	} else
	{
		if (bToggle) {
			newMute = !currentMute;
		} else {
			newMute = bMute;
		}

		hr = endpointVolume->SetMute(newMute, NULL);
		if (hr != S_OK && hr != S_FALSE) throw Exception(L"ChangeMute()::SetMute() failed");
	}

	endpointVolume->Release();
	CoUninitialize();

	return newMute;
}

//---------------------------------------------------------------------------

