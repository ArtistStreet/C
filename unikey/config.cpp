#include "config.h"
#include "lib.h"
#include <fstream>
#include <unordered_map>

static std::unordered_map<std::string, std::string> config;

void loadConfig(const std::string &path) {
    std::ifstream file(path);
    std::string line;
    while (getline(file, line)) {
        size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            config[key] = value;
        }
    }
}

bool getConfigBool(const std::string& key) {
    return config[key] == "true" || config[key] == "1";
}
