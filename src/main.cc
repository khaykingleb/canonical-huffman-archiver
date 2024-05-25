#include "archiver.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    using Arguments = std::vector<std::string>;

    po::options_description desc("Allowed options");
    desc.add_options() //
        ("help,h", "Print usage message") //
        ("compress,c", po::value<Arguments>()->multitoken(), "Compress files to archive") //
        ("decompress,d", po::value<std::string>(), "Decompress archive");

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
    }
    else if (vm.count("compress"))
    {
        Arguments input = vm["compress"].as<Arguments>();

        std::string archive_path = input.at(0);
        if (!archive_path.ends_with(".huff"))
        {
            std::cout << "Archive file should have .huff extension." << std::endl;
            return 1;
        }

        Arguments file_paths = Arguments(input.begin() + 1, input.end());
        for (const auto& file_path : file_paths)
        {
            if (!file_path.ends_with(".txt"))
            {
                std::cout << "Only .txt files are supported." << std::endl;
                return 1;
            }
        }

        Archiver archiver(archive_path, file_paths);
        archiver.Compress();
    }
    else if (vm.count("decompress"))
    {
        std::string archive_path = vm["decompress"].as<std::string>();
        if (!archive_path.ends_with(".huff"))
        {
            std::cout << "Archive file should have .huff extension." << std::endl;
            return 1;
        }

        Archiver archiver(archive_path);
        archiver.Decompress();
    }
    else
    {
        std::cout << "Please, specify valid argument. For more information, type `./archiver -h`."
                  << std::endl;
    }
    return 0;
}
