
#include "filewriter.h"

FileWriter::FileWriter(const std::string& archive_path)
    : archive_path_(archive_path)
    , file_(archive_path_, std::ofstream::binary)

{
    if (!file_.is_open())
    {
        throw std::runtime_error("The file writer cannot open " + archive_path_);
    }
}

FileWriter::~FileWriter()
{
    if (file_.is_open())
    {
        file_.close();
    }
}

void FileWriter::Flush()
{
    file_.put(buffer_byte_); // возможно нужно использовать кастование к char
}

void FileWriter::WriteBits(unsigned char c, size_t bits_count)
{
    for (size_t i = 0; i < bits_count; ++i)
    {
        buffer_byte_ |= (c & 1) << bits_pos_;
        c >>= 1;
        ++bits_pos_;

        if (bits_pos_ == 8)
        {
            Flush();
            buffer_byte_ = 0;
            bits_pos_ = 0;
        }
    }
}
