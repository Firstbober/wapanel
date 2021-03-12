#include "search_engine_mime.hh"

namespace se {

std::array<std::tuple<FileType, std::vector<const char*>>, 5> mime_ext {
	std::make_tuple(FileType::Document, std::vector {".abw", ".arc", ".doc", ".docx", ".odp", ".ods", ".odt", ".ppt", ".pptx", ".rtf", ".xls", ".xlsx", ".txt"}),
	std::make_tuple(FileType::Video, std::vector {".mp4", ".mpeg", ".webm", ".avi", "3gp", ".mkv", ".ogv"}),
	std::make_tuple(FileType::Audio, std::vector {".mp3", ".aac", ".opus", ".ogx", ".oga", ".ogg", ".opus", ".wav", ".weba"}),
	std::make_tuple(FileType::Image, std::vector {".bmp", ".gif", ".ico", ".jpg", ".jpeg", ".png", ".tif", ".tiff", ".webp"}),
	std::make_tuple(FileType::DesktopEntry, std::vector {".desktop"})
};

}