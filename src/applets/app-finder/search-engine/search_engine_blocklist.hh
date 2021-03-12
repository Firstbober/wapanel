#pragma once
#include <string>
#include <array>

namespace se::blocklist {

std::array files = { "desktop.ini", ".pid", ".ctl", ".lock", ".socket", ".sock", ".tdb" };
std::array directories = { "Windows"
						   "$Recycle.Bin",
						   "$RECYCLE.BIN",
						   "WindowsApps",
						   "PerfLogs/",
						   "ProgramData",
						   "PenTabletDriver",
						   "AppData",
						   "$WINDOWS.~BT",
						   "$SysReset",
						   "System Volume Information",
						   "Program Files/Windows",
						   "Program Files (x86)/Windows",
						   "NVPACK",
						   "NVIDIA/",
						   "vcpkg",

						   "node_modules",
						   "site-packages",
						   "android-ndk",
						   "android-sdk",
						   "npm-cache",
						   "go/src",
						   "org.eclipse",
						   "CodeWorksforAndroid",
						   "gradle",
						   "SDL2",
						   "__pycache__",
						   "python-core",

						   "wr-capture-sequence",

						   "/sys",
						   "/run",
						   "/var",
						   "/boot",
						   "/dev",
						   "/proc" };

}