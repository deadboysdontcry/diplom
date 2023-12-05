#include <climits>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <filesystem>
#include <random>

int columnsCount = 3;
int rowsCount = 10;
int maxValue = 10;
std::string outputPath = std::filesystem::current_path() / "output.db.txt";

using randomIntGenerator = std::uniform_int_distribution<std::mt19937::result_type>; 

void printHelp() {
    std::cout << "cli for generating \"db\" files\n";
    std::cout << "Commands:\n";
    std::cout << "  --help                   : show list of all commands\n";
    std::cout << "  --columns_count={value}  : set columns count in db equal to {value}\n";
    std::cout << "  --rows_count={value}     : set rows count in db equal to {value}\n";
    std::cout << "  --max_value={value}      : set generated keys and value columns max value to {value}\n";
    std::cout << "  --output_path={path}     : set output path to {path}\n";
}

std::map<std::string, std::string> parseArguments(int argc, char *argv[]) {
    std::map<std::string, std::string> arguments;
    for (int i = 1; i < argc; ++i) {
        std::string argument = argv[i];
        size_t pos = argument.find('=');
        if (pos != std::string::npos) {
            std::string key = argument.substr(0, pos);
            std::string value = argument.substr(pos + 1);
            arguments[key] = value;
        } else {
            arguments[argument] = "";
        }
    }
    return arguments;
}

std::vector<int> GenerateRow(randomIntGenerator& generator, std::mt19937& rng) {
  std::vector<int> ret;
  for(int i = 0; i < columnsCount; ++i) {
    ret.emplace_back(generator(rng));
  }
  ret.emplace_back(generator(rng));
  return ret;
}

int main(int argc, char *argv[]) {
    std::map<std::string, std::string> arguments = parseArguments(argc, argv);

    if (auto it = arguments.find("--help"); it != arguments.end()) {
        printHelp();
        return 0;
    }

    if (auto it = arguments.find("--columns_count"); it != arguments.end()) {
        columnsCount = std::stoi(it->second);
    }
    if (auto it = arguments.find("--rows_count"); it != arguments.end()) {
        rowsCount = std::stoi(it->second);
    }
    if (auto it = arguments.find("--max_value"); it != arguments.end()) {
        maxValue = std::stoi(it->second);
    }
    if (auto it = arguments.find("--output_path"); it != arguments.end()) {
        outputPath =it->second;
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    randomIntGenerator generator(0, maxValue); 
    

    std::vector<std::vector<int>> rows;
    for(int i = 0; i < rowsCount; ++i) {
      rows.emplace_back(GenerateRow(generator, rng));
    }
    std::ranges::sort(rows);

    std::ofstream out(outputPath);
    bool firstRow{true};
    for (const auto& row : rows) {
        if (!firstRow) {
          out << "\n";
        }
        bool firstEl{true};
        for (auto el: row) {
          if (!firstEl) {
            out << "|";
          }
          out << el;
          firstEl = false;
        }
        firstRow = false;
    }
    out.close();
  
    return 0;
}
