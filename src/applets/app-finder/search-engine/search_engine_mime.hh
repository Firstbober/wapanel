#pragma once
#include <array>
#include <string>
#include <tuple>
#include <vector>

namespace se {

enum class FileType { Other, Directory, Video, Audio, Image, Document, DesktopEntry };

extern std::array<std::tuple<FileType, std::vector<const char *>>, 5> mime_ext;

}