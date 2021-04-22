#include "search_engine.hh"
#include "search_engine_blocklist.hh"
#include <filesystem>
#include <fnmatch.h>
#include <fstream>
#include <iostream>
#include <pwd.h>
#include <queue>
#include <unistd.h>
#include <xdg.hpp>

namespace se {

auto SearchEngine::analyze_results(std::vector<fs::path> found_files) -> void {
	if (found_files.empty()) return;

	std::vector<File> files;

	for (auto &&file : found_files) {
		File _file;
		bool found_ext = false;

		if (file.empty()) continue;

		_file.path = file;

		if (fs::is_directory(file)) {
			_file.type = FileType::Directory;
			goto push;
		}

		for (auto &[type, exts] : mime_ext) {
			for (auto &ext : exts) {
				if (file.extension().string() == ext) {
					_file.type = type;
					found_ext = true;
				}
			}
		}

		if (!found_ext) { _file.type = FileType::Other; }

	push:
		files.push_back(_file);
	}

	for (auto &&[id, handle] : this->m_handles) {
		handle(files);
	}
}

auto SearchEngine::search_directory(std::string text, fs::path directory, bool is_home) -> void {
	if (m_stopsearch) { return; }

	const char *to_find = text.c_str();

	std::queue<fs::path> sub_directories;
	std::vector<fs::path> found_files;

	for (auto &&entry : fs::directory_iterator(directory, fs::directory_options::skip_permission_denied)) {
		if (entry.is_symlink()) continue;

		if (entry.path().filename().string()[0] == '.') { continue; }

		bool blocked_file = false;
		bool blocked_dir = false;

		for (auto &&blocked_name : blocklist::files) {
			if (entry.path().string().ends_with(blocked_name)) blocked_file = true;
		}
		if (blocked_file) continue;

		if (entry.is_directory()) {
			for (auto &&blocked_name : blocklist::directories) {
				if (entry.path().string().ends_with(blocked_name)) blocked_dir = true;
			}

			if (blocked_dir) continue;
		}

		std::string filename = entry.path().filename().string();
		for (auto &&c : filename) {
			c = towlower(c);
		}

		if (fnmatch(to_find, filename.c_str(), 0) == 0) { found_files.push_back(entry); }

		if (entry.is_directory()) { sub_directories.push(entry); }
	}

	analyze_results(found_files);

	std::vector<std::thread> threads;

	while (!sub_directories.empty()) {
		if (is_home) {
			threads.push_back(std::thread(&SearchEngine::search_directory, this, text, sub_directories.front(), false));
		} else {
			search_directory(text, sub_directories.front(), false);
		}
		sub_directories.pop();
	}

	if (is_home) {
		for (auto &&th : threads) {
			if (th.joinable()) { th.join(); }
		}
	}
}

auto SearchEngine::search_applications(std::string text) -> void {}

SearchEngine::SearchEngine() {
	if ((m_homedir = getenv("HOME")) == NULL) { m_homedir = getpwuid(getuid())->pw_dir; }
	update_mountpoints();

	for (auto &&path : xdg::DataDirs()) {
		if (fs::exists(path.string() + "/applications")) { m_app_dirs.push_back(path.string() + "/applications"); }
	}

	if (fs::exists(xdg::DataHomeDir().string() + "/applications")) {
		m_app_dirs.push_back(xdg::DataHomeDir().string() + "/applications");
	}
}
SearchEngine::~SearchEngine() {
	for (auto &&thread : m_threads) {
		if (thread.joinable()) thread.join();
	}
}

auto SearchEngine::register_result_handle(std::function<void(std::vector<File>)> handle) -> int {
	this->m_handles[this->m_handles.size()] = handle;
	return this->m_handles.size() - 1;
}
auto SearchEngine::unregister_handle(int id) -> void { this->m_handles.erase(id); }

auto SearchEngine::search(std::string text) -> void {
	/*
	for (auto &&thread : m_threads) {
		if (thread.joinable()) thread.join();
	}
	*/
	m_threads.clear();

	this->m_stopsearch = false;

	for (auto &&c : text) {
		c = towlower(c);
	}
	std::string to_find = "*" + text + "*";

	m_threads.push_back(std::thread(&SearchEngine::search_directory, this, to_find, m_homedir, true));

	for (auto &&mount_point : m_mount_points) {
		m_threads.push_back(std::thread(&SearchEngine::search_directory, this, to_find, mount_point, true));
	}

	for (auto &&app_dir : m_app_dirs) {
		search_directory(to_find, app_dir, false);
	}

	std::cout << "searching " << text << std::endl;
}

auto SearchEngine::get_all_desktop_entries() -> void {
	for (auto &&app_dir : m_app_dirs) {
		search_directory("*", app_dir, false);
	}
}

auto SearchEngine::update_mountpoints() -> void {
	m_mount_points = {};

	if (fs::exists("/etc/mtab")) {
		std::ifstream file("/etc/mtab");

		std::string mount_drive;
		std::string mount_location;
		std::string file_system;
		std::string flags;

		int magic_1;
		int magic_2;

		while (file >> mount_drive >> mount_location >> file_system >> flags >> magic_1 >> magic_2) {
			if (mount_drive[0] != '/') continue;

			if (!mount_location.starts_with("/boot") && mount_location.length() > 1)
				m_mount_points.push_back(mount_location);
		}
	}
}

auto SearchEngine::stop_search() -> void {
	this->m_stopsearch = true;

	for (auto &&thread : m_threads) {
		if (thread.joinable()) { thread.join(); }
	}
}

}