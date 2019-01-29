// vim: tw=60
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "data_source.h"

// Sample command line arguments:
//   stats --stdin
//   stats --stdin --prompt="Enter datum"
//   stats --file=data.txt
//   stats --csv=data.csv --column=3
//   stats --random-normal --mean=4.0 --stdev=0.5 --count=10
std::unique_ptr<DataSource> get_data_source(
    std::vector<std::string> args) {
  if (args.empty() || args[0] == "--stdin") {
    std::string prompt;
    for (size_t i = 1; i < args.size(); i++) {
      if (args[i].substr(0, 9) == "--prompt=") {
        prompt = args[1].substr(9);
      } else {
        std::cerr << "Unrecognized option '" << args[i]
                  << "' for input --stdin\n";
        return nullptr;
      }
    }
    return std::make_unique<ConsoleDataSource>(prompt);
  } else if (args[0].substr(0, 7) == "--file=") {
    std::string filename = args[0].substr(7);
    if (args.size() > 1) {
      std::cerr << "Unrecognized option '" << args[1]
                << "' for input --file\n";
      return nullptr;
    }
    std::cerr << "Input option --file not implemented.\n";
    return nullptr;
  } else if (args[0].substr(0, 6) == "--csv=") {
    std::string filename = args[0].substr(6);
    size_t column = 0;
    for (size_t i = 1; i < args.size(); i++) {
      if (args[i].substr(0, 9) == "--column=") {
        column =
            std::strtoull(args[i].c_str() + 9, nullptr, 10);
      } else {
        std::cerr << "Unrecognized option '" << args[i]
                  << "' for input --csv\n";
        return nullptr;
      }
    }
    std::cerr << "Input option --csv not implemented\n";
    return nullptr;
  } else if (args[0].substr(0, 15) == "--random-normal") {
    double mean = 0.0;
    double stdev = 1.0;
    size_t count = 0;
    for (size_t i = 1; i < args.size(); i++) {
      if (args[i].substr(0, 7) == "--mean=") {
        mean = strtod(args[i].c_str() + 7, nullptr);
      } else if (args[i].substr(0, 8) == "--stdev=") {
        stdev = strtod(args[i].c_str() + 8, nullptr);
      } else if (args[i].substr(0, 8) == "--count=") {
        count =
            std::strtoull(args[i].c_str() + 8, nullptr, 10);
      } else {
        std::cerr << "Unrecognized option '" << args[i]
                  << "' for input --random-normal\n";
        return nullptr;
      }
    }
    return std::make_unique<RandomNormalDataSource>(
        mean, stdev, count);
  } else {
    std::cerr << "Unrecognized input option '" << args[0]
              << "'\n";
    return nullptr;
  }
}

int main(int argc, char** argv) {
  // Parse command line arguments.
  std::vector<std::string> args;
  for (int i = 1; i < argc; i++) {
    args.push_back(argv[i]);
  }
  std::unique_ptr<DataSource> data_source =
      get_data_source(args);
  if (!data_source) {
    std::cerr << "Bad arguments\n";
    return 1;
  }

  // Read data, using DataSource from command line args.
  std::vector<double> data = data_source->read();
  std::cout << "Read " << data.size() << " data in "
            << data_source->read_time() << " seconds.\n";

  // Process data.
  int N = data.size();
  std::cout << "N = " << N << '\n';
  if (N == 0) {
    return 0;
  }
  double sum = 0.0;
  for (double d : data) {
    sum += d;
  }
  double avg = sum / data.size();
  std::cout << "Avg = " << avg << '\n';

  double sum_diff_sq = 0.0;
  for (double d : data) {
    double diff = d - avg;
    sum_diff_sq += diff * diff;
  }
  double var = sum_diff_sq / N;
  std::cout << "Var = " << var << '\n';
  double stdev = std::sqrt(var);
  std::cout << "Stdev = " << stdev << '\n';
  return 0;
}
