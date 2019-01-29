// vim: tw=60
#include <chrono>
#include <iostream>
#include <limits>

#include "data_source.h"

DataSource::~DataSource() = default;

std::vector<double> DataSource::read() {
  auto start = std::chrono::system_clock::now();
  std::vector<double> data = do_read();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> dur = end - start;
  read_time_ = dur.count();
  return data;
}

double DataSource::read_time() const { return read_time_; }

DataSource::DataSource()
    : read_time_(
          std::numeric_limits<double>::signaling_NaN()) {}

std::vector<double> ReadOneDataSource::do_read() {
  std::vector<double> data;
  for (;;) {
    std::pair<bool, double> one = do_read_one();
    if (one.first) {
      data.push_back(one.second);
    } else {
      break;
    }
  }
  return data;
}

ConsoleDataSource::ConsoleDataSource(
    const std::string& prompt)
    : prompt_(prompt) {}

std::pair<bool, double> ConsoleDataSource::do_read_one() {
  for (;;) {
    if (prompt_.empty()) {
      std::cout << "Enter a number";
    } else {
      std::cout << prompt_;
    }
    std::cout << " [type 'end' without quotes to end]: ";

    std::string word;
    std::cin >> word;
    if (word == "end") {
      return std::make_pair(false, 0.0);
    } else {
      char* endptr;
      double d = std::strtod(word.c_str(), &endptr);
      if (*endptr != '\0') {
        std::cout << "Format error; last input ignored"
                  << std::endl;
        continue;
      } else {
        return std::make_pair(true, d);
      }
    }
  }
}

RandomNormalDataSource::RandomNormalDataSource(double mean,
                                               double stdev,
                                               size_t count,
                                               size_t seed)
    : rng_(seed ? seed : std::time(nullptr)),
      distr_(mean, stdev),
      count_(count) {}

std::vector<double> RandomNormalDataSource::do_read() {
  std::vector<double> data;
  data.resize(count_);
  for (size_t i = 0; i < count_; i++) {
    data[i] = distr_(rng_);
  }
  return data;
}
