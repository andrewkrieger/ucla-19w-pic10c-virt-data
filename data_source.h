// vim: tw=60
#include <random>
#include <string>
#include <utility>
#include <vector>

class DataSource {
 public:
  virtual ~DataSource();
  std::vector<double> read();
  double read_time() const;

 protected:
  DataSource();

 private:
  double read_time_;
  virtual std::vector<double> do_read() = 0;
};

class ReadOneDataSource : public DataSource {
 private:
  std::vector<double> do_read() final;
  virtual std::pair<bool, double> do_read_one() = 0;
};

class ConsoleDataSource : public ReadOneDataSource {
 public:
  explicit ConsoleDataSource(
      const std::string& prompt = "");

 private:
  std::string prompt_;
  std::pair<bool, double> do_read_one() override;
};

class RandomNormalDataSource : public DataSource {
 public:
  RandomNormalDataSource(double mean = 0.0,
                         double stdev = 1.0,
                         size_t count = 0, size_t seed = 0);

 private:
  std::mt19937_64 rng_;
  std::normal_distribution<double> distr_;
  size_t count_;
  std::vector<double> do_read() override;
};
