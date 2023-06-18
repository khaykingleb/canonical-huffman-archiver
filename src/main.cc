#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    using Arguments = std::vector<std::string>;

    po::options_description desc("Allowed options");
    desc.add_options()                                                                     //
        ("help,h", "Print usage message")                                                  //
        ("compress,e", po::value<Arguments>()->multitoken(), "Compress files to archive")  //
        ("decompress,d", po::value<std::string>(), "Decompress archive");

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
    } else if (vm.count("compress")) {
        Arguments input = vm["compress"].as<Arguments>();
        std::string archive_path = input.at(0);
        Arguments file_paths = Arguments(input.begin() + 1, input.end());
    } else if (vm.count("decompress")) {
        std::string archive_path = vm["decompress"].as<std::string>();
    } else {
        std::cout << "Please, specify valid argument. For more information, type `./archiver -h`."
                  << std::endl;
    }
    return 0;
}
