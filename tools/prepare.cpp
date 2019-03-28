#include <iomanip>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include "../ext/validate20.cpp"


std::vector<uint8_t> read_rad_file(char const* filename) {
  std::ifstream input(filename, std::ios::binary);
  if(!input) {
    std::cerr << "Could not open input file " << filename << std::endl;
    exit(1);
  }

  input.seekg(0, std::ios::end);
  auto const size = input.tellg();
  input.seekg(0, std::ios::beg);

  std::vector<uint8_t> content(size);

  input.read(reinterpret_cast<char*>(content.data()), size);
  if(!input || input.gcount() != size) {
    std::cerr << "Failed to read input file" << std::endl;
    exit(1);
  }

  return content;
}


void validate_rad_file(std::vector<uint8_t> const& data) {
  auto const result = RADValidate(reinterpret_cast<void const*>(data.data()), data.size());
  if(result != nullptr) {
    std::cerr << result << std::endl;
    exit(1);
  }
}


void generate_data_file(char const* filename, std::vector<uint8_t> const& data) {
  std::ofstream output(filename, std::ios::binary);
  if(!output) {
    std::cerr << "Could not open output file " << filename << std::endl;
    exit(1);
  }

  output << "static uint8_t const music_data[] = {";
  for(int i = 0; i < data.size(); i++) {
    if(i % 16 == 0) {
      output << "\n  ";
    }

    output << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned>(data[i]);
    if(i != data.size() - 1) {
      output << ", ";
    }
  }
  output << "\n};\n";
}


int main(int argc, char** argv) {
  if(argc < 3) {
    std::cerr << "usage: " << argv[0] << "<rad-file> <out-file>" << std::endl;
    return 1;
  }

  auto data = read_rad_file(argv[1]);
  validate_rad_file(data);
  generate_data_file(argv[2], data);
}
