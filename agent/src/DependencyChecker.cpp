#include "DependencyChecker.hpp"

#include <array>
#include <cstdlib>
#include <filesystem>
#include <sstream>

namespace
{
    std::string trim(std::string value)
    {
        while (!value.empty() && (value.back() == '\n' || value.back() == '\r' || value.back() == ' '))
            value.pop_back();
        return value;
    }

    std::string run(const std::string& command)
    {
        std::array<char, 256> buffer{};
        std::string output;
        const auto pipe = popen(command.c_str(), "r");
        if (pipe == nullptr) return {};
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
            output += buffer.data();
        pclose(pipe);
        return trim(output);
    }
}

bool DependencyChecker::commandAvailable(const std::string& command) const
{
    return std::system(("command -v " + command + " >/dev/null 2>&1").c_str()) == 0;
}

std::string DependencyChecker::commandVersion(const std::string& command) const
{
    return run(command + " --version 2>/dev/null | head -n 1");
}

std::string DependencyChecker::detectPlatform() const
{
#if defined(__ANDROID__)
    return "Android/Termux";
#elif defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

std::string DependencyChecker::detectPackageManager() const
{
    if (commandAvailable("pkg")) return "pkg";
    if (commandAvailable("apt-get")) return "apt";
    if (commandAvailable("dnf")) return "dnf";
    if (commandAvailable("pacman")) return "pacman";
    if (commandAvailable("brew")) return "brew";
    if (commandAvailable("winget")) return "winget";
    return {};
}

std::string DependencyChecker::installSuggestion(const std::string& packageManager) const
{
    if (packageManager == "pkg") return "pkg install clang cmake git make sqlite";
    if (packageManager == "apt") return "sudo apt-get update && sudo apt-get install -y build-essential cmake git libsqlite3-dev";
    if (packageManager == "dnf") return "sudo dnf install gcc-c++ cmake git sqlite-devel";
    if (packageManager == "pacman") return "sudo pacman -S --needed base-devel cmake git sqlite";
    if (packageManager == "brew") return "brew install cmake sqlite git";
    if (packageManager == "winget") return "winget install Kitware.CMake Git.Git LLVM.LLVM";
    return "Install a C++20 compiler, CMake, Git and SQLite development headers using your system package manager.";
}

DependencyReport DependencyChecker::check() const
{
    DependencyReport report;
    report.platform = detectPlatform();
    report.packageManager = detectPackageManager();

    const std::vector<std::string> commands{"cmake", "git", "sqlite3"};
    bool compilerFound = false;
    for (const auto& compiler : {std::string("c++"), std::string("g++"), std::string("clang++")})
    {
        if (commandAvailable(compiler))
        {
            compilerFound = true;
            report.dependencies.push_back({"C++20 compiler (" + compiler + ")", true, commandVersion(compiler), {}});
            break;
        }
    }
    if (!compilerFound)
        report.dependencies.push_back({"C++20 compiler", false, {}, {}});

    for (const auto& command : commands)
    {
        const bool available = commandAvailable(command);
        report.dependencies.push_back({command, available, available ? commandVersion(command) : std::string{}, {}});
    }

    report.dependencies.push_back({"Eigen / nlohmann-json / tiny-dnn", true,
        "Fetched by CMake when not installed locally", {}});
    report.ready = true;
    for (const auto& dependency : report.dependencies)
        report.ready = report.ready && dependency.available;
    return report;
}

std::string DependencyChecker::format(const DependencyReport& report) const
{
    std::ostringstream output;
    output << "Dependency check\n"
           << "Platform: " << report.platform << "\n"
           << "Package manager: " << (report.packageManager.empty() ? "not detected" : report.packageManager) << "\n\n";
    for (const auto& dependency : report.dependencies)
    {
        output << (dependency.available ? "[OK]   " : "[MISS] ") << dependency.name;
        if (!dependency.version.empty()) output << " - " << dependency.version;
        output << '\n';
    }
    output << "\nResult: " << (report.ready ? "environment ready" : "missing dependencies") << '\n';
    if (!report.ready)
        output << "Suggested install command:\n  " << installSuggestion(report.packageManager) << "\n"
               << "CMake will fetch Eigen, nlohmann/json and tiny-dnn during configuration.\n";
    return output.str();
}
