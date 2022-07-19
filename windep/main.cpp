// copyright MIT License Copyright (c) 2021, Albert Farrakhov

#include <iostream>

#include "cxxopts/cxxopts.hpp"
#include "pe.h"
#include "traversing.h"
#include "version.h"
#include "view.h"
#include "writer.h"

int main(int argc, char **argv) {
  try {
    windep::exc::SeException::SetTranslator();
    cxxopts::Options options(
        "windep.exe",
        "Small utility to find all DLL dependencies of the PE binary");
    options.positional_help("<binary>");
    options.parse_positional({"image"});
    options.add_options()("i,image", "Binary image",
                          cxxopts::value<std::string>())(
        "c,functions", "Enable functions output",
        cxxopts::value<bool>()->default_value("false"))(
        "d,delayed", "Enable delayed imports",
        cxxopts::value<bool>()->default_value("false"))(
        "f,format", "Output format. Possible values: ascii, json",
        cxxopts::value<std::string>()->default_value("ascii"))(
        "t,indent", "Output rows indent",
        cxxopts::value<uint8_t>()->default_value("2"))(
        "h,help", "Print help", cxxopts::value<bool>()->default_value("false"))(
        "v,version", "Print version",
        cxxopts::value<bool>()->default_value("false"));
    auto args = options.parse(argc, argv);

    if (args.count("help")) {
      std::cout << options.help() << std::endl;
      return 0;
    } else if (args.count("version")) {
      std::cout << VERSION << std::endl;
      return 0;
    }

    const auto &image = args["image"].as<std::string>();
    const auto is_delayed = args["delayed"].as<bool>();
    const auto &format = args["format"].as<std::string>();
    const auto functions = args["functions"].as<bool>();
    const auto indent = args["indent"].as<uint8_t>();
    const auto pe_image_factory =
        std::make_shared<windep::image::pe::PeImageFactory>(is_delayed);
    windep::image::ImageDependencyFactory dep_factory{image, pe_image_factory};
    auto root = dep_factory.Create();
    auto stdout_writer = std::make_shared<windep::StdoutWriter>();
    auto view = windep::view::Factory{format}.Create(functions, indent);
    view->Show(root, stdout_writer);
  } catch (const std::exception &e) {
    std::cerr << "[-] " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
