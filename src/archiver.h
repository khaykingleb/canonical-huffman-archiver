#pragma once

#include <string>
#include <vector>

/*
 * A class for archiving and unarchiving files.
 */
class Archiver
{
public:
    /*
     * Constructor.
     * @param archive_path The path to the archive file.
     * @param file_paths The paths to the files to archive.
     */
    Archiver(const std::string& archive_path, const std::vector<std::string>& file_paths);

    /*
     * Constructor.
     * @param archive_path The path to the archive file.
     */
    Archiver(const std::string& archive_path);

    /*
     * Compress the files to the archive.
     */
    void Compress() const;

    /*
     * Decompress the archive to get the files.
     */
    void Decompress() const;

private:
    std::string archive_path_;
    std::vector<std::string> file_paths_;
};
