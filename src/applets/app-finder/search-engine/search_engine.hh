#pragma once
#include "search_engine_mime.hh"
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <vector>

namespace se {

namespace fs = std::filesystem;

struct File {
	FileType type;
	fs::path path;
};

class SearchEngine {
private:
	auto analyze_results(std::vector<fs::path> found_files) -> void;
	auto search_directory(std::string text, fs::path directory, bool is_home) -> void;
	auto search_applications(std::string text) -> void;

	const char *m_homedir;
	std::vector<std::string> m_mount_points;
	std::vector<std::thread> m_threads;

	bool m_stopsearch = false;
	std::vector<fs::path> m_app_dirs;

	std::map<int, std::function<void(std::vector<File>)>> m_handles;

public:
	SearchEngine();
	~SearchEngine();

	auto register_result_handle(std::function<void(std::vector<File>)> handle) -> int;
	auto unregister_handle(int id) -> void;

	auto search(std::string text) -> void;
	auto get_all_desktop_entries() -> void;
	auto stop_search() -> void;

	auto update_mountpoints() -> void;

	bool __x = false;
};

}