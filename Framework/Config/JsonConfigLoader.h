#pragma once
#include "Config/ConfigLoader.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace GameServer::Config {

class JsonConfigLoader : public IConfigLoader {
public:
    ServerConfig Load(const std::string& path) override;
};

}
