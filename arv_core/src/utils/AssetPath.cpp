#include "utils/AssetPath.h"
#include <filesystem>

namespace arv {

    std::string AssetPath::s_AssetDirectory = "/Users/cmoeller/dev/projects/ARVision/arv-studio/assets";

    void AssetPath::SetAssetDirectory(const std::string& directory) {
        s_AssetDirectory = directory;
    }

    const std::string& AssetPath::GetAssetDirectory() {
        return s_AssetDirectory;
    }

    std::string AssetPath::Resolve(const std::string& relativePath) {
        std::filesystem::path basePath = std::filesystem::current_path();
        std::filesystem::path assetPath = basePath / s_AssetDirectory / relativePath;
        return assetPath.string();
    }

    bool AssetPath::Exists(const std::string& relativePath) {
        return std::filesystem::exists(Resolve(relativePath));
    }

}
