#pragma once

#include <filesystem>

#include <Windows.h>
#include <gdiplus.h>

#include <mutex>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace std;
using namespace Gdiplus;

using namespace winrt;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;

namespace ImageUtils
{
	hstring SaveCover(IRandomAccessStreamReference);
	hstring CropCover(hstring);
}
