#include <chrono>
#include <iostream>
#include <limits>

#include "data_source.h"

// A C++11 feature. If you don't need any custom behavior in your constructor or
// destructor, you can use `= default`.  You'll get the same behavior as if you
// hadn't written a custom ctor/dtor (mostly).
//
// We have to have some definition since we declared a dtor.  But this makes it
// clear that we don't do anything special.
DataSource::~DataSource() = default;

// Implementation of the public API method read().
//
// This calls do_read(), wrapping it in some C++11 code to measure start and end
// times.
std::vector<double> DataSource::read() {
  auto start = std::chrono::system_clock::now();
  std::vector<double> data = do_read();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> dur = end - start;
  read_time_ = dur.count();
  return data;
}

// A simple getter.
double DataSource::read_time() const { return read_time_; }

// A simple ctor. Initializes read_time_ to NaN ("Not a Number"). See:
// https://en.wikipedia.org/wiki/NaN
DataSource::DataSource()
    : read_time_(
          std::numeric_limits<double>::signaling_NaN()) {}

// The implementation of do_read for our ReadOneDataSource helper class. It just
// calls do_read_one() in a loop.
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

// Read one random number from the console.
std::pair<bool, double> ConsoleDataSource::do_read_one() {
  // Might take multiple tries in case of invalid numbers.
  for (;;) {
    if (prompt_.empty()) {
      // Default prompt.
      std::cout << "Enter a number";
    } else {
      // Custom prompt.
      std::cout << prompt_;
    }
    std::cout << " [type 'end' without quotes to end]: ";

    // Try to read a double...
    std::string word;
    std::cin >> word;
    if (word == "end") {
      // Stop if instructed to end.
      return std::make_pair(false, 0.0);
    } else {
      // Parse word into a double...
      char* endptr;
      double d = std::strtod(word.c_str(), &endptr);
      // ... on success, endptr points at the first character after the digits
      // from stdin. That should be a null character if all went well. If the
      // user typed "123abc", endptr would point at "abc".
      if (*endptr != '\0') {
        std::cout << "Format error; last input ignored"
                  << std::endl;
        // Ignore bad input, try again.
        continue;
      } else {
        // Return the user-provided value.
        return std::make_pair(true, d);
      }
    }
  }
}

// Again, you might want to skip over rng_ and distr_ on your first time through
// the code. They're from the C++11 random library, which is kind of complex.
RandomNormalDataSource::RandomNormalDataSource(size_t count,
                                               double mean,
                                               double stdev,
                                               size_t seed)
    : rng_(seed ? seed : std::time(nullptr)),
      distr_(mean, stdev),
      count_(count) {}

// Generate random numbers. Again, feel free to ignore the details of how distr_
// and rng_ work for now.
std::vector<double> RandomNormalDataSource::do_read() {
  std::vector<double> data;
  data.resize(count_);
  for (size_t i = 0; i < count_; i++) {
    data[i] = distr_(rng_);
  }
  return data;
}
