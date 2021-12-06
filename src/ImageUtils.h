/* Copyright (C) 2021 Tudor Butufei
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>. */

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
