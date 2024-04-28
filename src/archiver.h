#pragma once

#include <string>
#include <vector>

class Archiver
{
public:
    Archiver(const std::string& archive_path, const std::vector<std::string>& file_paths);
    Archiver(const std::string& archive_path);
    void Compress();
    void Decompress();

private:
    std::string archive_path_;
    std::vector<std::string> file_paths_;
};
