#include "ImageUtils.h"

hstring ImageUtils::SaveCover(IRandomAccessStreamReference image)
{
	auto cover_stream = image.OpenReadAsync().get();
	auto cover_buffer = Buffer(5000000);
	auto temp_folder = StorageFolder::GetFolderFromPathAsync(filesystem::temp_directory_path().wstring()).get();
	auto cover_file = temp_folder.CreateFileAsync(L"rainmeter_media-player-cover.png", CreationCollisionOption::ReplaceExisting).get();

	cover_stream.ReadAsync(cover_buffer, cover_buffer.Capacity(), InputStreamOptions::ReadAhead).get();
	FileIO::WriteBufferAsync(cover_file, cover_buffer).get();

	return cover_file.Path();
}

hstring ImageUtils::CropCover(hstring original)
{
    // folder
    auto temp_folder = StorageFolder::GetFolderFromPathAsync(filesystem::temp_directory_path().wstring()).get();
    auto cropped_file = temp_folder.CreateFileAsync(L"rainmeter_media-player-cover_cropped.png", CreationCollisionOption::ReplaceExisting).get();
    
    // initialize gdi+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    {
        // crop image
        Bitmap b(300, 300);
        Graphics g(&b);
        Bitmap cover(original.c_str());
        Gdiplus::Rect r(0, 0, 300, 300);
        g.DrawImage(&cover, r, 33, 0, 233, 233, UnitPixel);

        // save image
        const CLSID pngEncoderClsId = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } }; // image/png  : {557cf406-1a04-11d3-9a73-0000f81ef32e}
        b.Save(cropped_file.Path().c_str(), &pngEncoderClsId, NULL);
    }
    
    // gdi+ shutdown
    GdiplusShutdown(gdiplusToken);

    return cropped_file.Path();
}