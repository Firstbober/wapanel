#pragma once
#include <string>
#include <vector>

enum class Category {
	Multimedia, // AudioVideo
	Development,
	Education,
	Game,
	Graphics,
	Network,
	Office,
	Science,
	Settings,
	System,
	Accessories, // Utility,
	Other // Uncategorized
};

struct AppEntry {
	std::string name;
	std::string description;
	std::string iconstring;
	std::string exec;
	std::string working_path;
};