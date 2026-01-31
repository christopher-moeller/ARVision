#pragma once

#include <string>
#include <filesystem>

namespace arv {

    /**
     * Simple asset path resolver.
     * Resolves relative asset paths to absolute paths based on the working directory.
     */
    class AssetPath {
    public:
        /**
         * Set the base directory for assets (relative to working directory).
         * Default: "arv-studio/assets"
         */
        static void SetAssetDirectory(const std::string& directory);

        /**
         * Get the current asset directory.
         */
        static const std::string& GetAssetDirectory();

        /**
         * Resolve a relative asset path to an absolute path.
         * Example: "textures/logo.png" -> "/full/path/to/arv-studio/assets/textures/logo.png"
         */
        static std::string Resolve(const std::string& relativePath);

        /**
         * Check if an asset exists at the given relative path.
         */
        static bool Exists(const std::string& relativePath);

    private:
        static std::string s_AssetDirectory;
    };

}
