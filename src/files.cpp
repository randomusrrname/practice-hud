// Carpeta de imagenes: <configDir>/images/*.png

#include "common.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>

namespace {
    // Convierte el tipo de ruta de Geode (asp::fs::Path) a std::string,
    // probando los metodos habituales.
    template <class P>
    std::string pathStr(P const& p) {
        if constexpr (requires { std::string(p.string()); }) return std::string(p.string());
        else if constexpr (requires { std::string(p.c_str()); }) return std::string(p.c_str());
        else if constexpr (requires { std::string(p.toString()); }) return std::string(p.toString());
        else if constexpr (requires { std::string(p.str()); }) return std::string(p.str());
        else return fmt::format("{}", p);
    }
}

namespace phud {
    std::string imagesDir() {
        static std::string const dir = [] {
            auto base = pathStr(geode::Mod::get()->getConfigDir());
            auto d = std::filesystem::path(base) / "images";
            std::error_code ec;
            std::filesystem::create_directories(d, ec);
            return d.string();
        }();
        return dir;
    }

    std::vector<std::string> listImages() {
        std::vector<std::string> out;
        std::error_code ec;
        for (auto it = std::filesystem::directory_iterator(imagesDir(), ec);
             !ec && it != std::filesystem::directory_iterator(); it.increment(ec)) {
            std::error_code ec2;
            if (!it->is_regular_file(ec2)) continue;
            auto ext = it->path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (ext == ".png") out.push_back(it->path().filename().string());
            if (out.size() >= 100) break;
        }
        std::sort(out.begin(), out.end());
        return out;
    }
}
