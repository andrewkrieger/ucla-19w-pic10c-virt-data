#include <random>
#include <string>
#include <utility>
#include <vector>

// A polymorphic interface for reading data, in the form of a list of double
// values.
//
// The core method in DataSource is read(), which reads data from some source.
// Subtypes override the do_read() method to implement read(). DataSource() adds
// a little extra code to read() to record how long the do_read() call takes (in
// seconds).
//
// It's a common idiom and good practice to separate polymorphic interfaces into
// public, non-virtual functions like read() and private (or occasionally
// protected) virtual functions like do_read. An explanation of why is here:
//
//     http://www.gotw.ca/publications/mill18.htm
//
// Here's a summary of the advantages:
//
// 1.  The public interface method read() can add extra code to check
//     preconditions (for debugging) or perform other actions (like timing the
//     do_read() call, or logging the call, or so on).
// 2.  The implementation doesn't have to look the same as the interface. In
//     this case, read() and do_read() are both functions that take no arguments
//     and return a vector. But if it made sense, we could have different
//     looking implantation functions. For example, maybe we want do_pre_read()
//     to initialize data structures, do_read() to do the actual read, and
//     do_post_read() to clean up.
// 3.  We can change the interface and implementations separately and
//     iteratively, rather than changing everything all at once. For complex
//     programs, and especially complex programs/types with lots of users and
//     developers, this is a major benefit.
//
//     For example, maybe we want to add a new argument `size_t limit` to
//     read(). In step 1, we add the argument, read() still calls do_read(),
//     then truncates the vector if needed. In step 2, we add a new
//     implementation function: `virtual std::vector<double> do_read2(size_t
//     limit)`.  We provide a default implementation, which calls do_read() and
//     truncates. Then we change read() to call do_read2(). Then in step 3, we
//     implement do_read2() for each subtype individually. Now we get the
//     benefit: no more vector truncation! In step 4, we can clean up by
//     deleting the unused do_read() function.
class DataSource {
 public:
  // If you have virtual functions, you should often have a public virtual
  // destructor. The actual rule is, if anyone will ever delete your class
  // through a base class pointer, e.g.
  //
  //     DataSource* data_source = /* ... */;
  //     delete data_source;
  //
  // or
  //
  //     int main() {
  //         std::unique_ptr<DataSource> data_source = /* ... */;
  //         // ...
  //         // end of scope - the unique_ptr is destroyed, and its destructor
  //         // calls delete on a private DataSource*.
  //     }
  virtual ~DataSource();

  // This is the first half of the public interface/private virtual
  // implementation pattern described above.
  std::vector<double> read();

  // This is a non-virtual, non-polymorphic function. It's just a regular
  // method.
  double read_time() const;

 protected:
  // Only subclasses can call this constructor. It's not really necessary to
  // make the ctor protected, since you couldn't make a DataSource object anyway
  // (it's an abstract class, because do_read() is pure virtual).
  //
  // You might want to declare a ctor if you need to pass in arguments (e.g. if
  // we took a `std::string label` or something). In that case, it ought to be
  // protected.
  DataSource();

 private:
  // Just an ordinary instance variable...
  double read_time_;

  // This is the second half of the public interface/private virtual
  // implementation pattern described above.
  virtual std::vector<double> do_read() = 0;
};

// This is a helper class, for the convenience of people implementing
// DataSource. Often, our data source will produce values one at a time, until
// it runs out. The do_read_one() method supports that pattern.
class ReadOneDataSource : public DataSource {
 private:
  // This is a `final` override. Like the `override` keyword, the compiler will
  // warn us if there wasn't a `virtual std::vector<double> do_read();` function
  // in the base class. Also, because we wrote `final` instead of `override`,
  // later subclasses aren't allowed to change `do_read()`. There's a decent
  // into to override and final here:
  //
  //     http://www.modernescpp.com/index.php/override-and-final
  std::vector<double> do_read() final;

  // Subclasses must either return a pair `(true, value)` where value is the
  // next double to add; or return `(false, <anything>)` to indicate end of
  // data.
  virtual std::pair<bool, double> do_read_one() = 0;
};

// The first actual implementation of DataSource. Reads numbers interactively
// from the command line.
//
// Internally, ConsoleDataSource uses the helper class ReadOneDataSource. So,
// our job here is to implement do_read_one(). The other data below, like the
// member variable and constructor, exist to help us write do_read_one().
class ConsoleDataSource : public ReadOneDataSource {
 public:
  // Allow callers to customize the prompt string.
  explicit ConsoleDataSource(const std::string& prompt = "");

 private:
  std::string prompt_;

  // Like final, override indicates that we intend to override a virtual method.
  // If you make a mistake (like spelling "do_read_one" wrong, or adding const,
  // or adding the wrong arguments) you'll get a compiler warning. Without
  // override, you could accidentally make an overload or an unrelated method,
  // and you'd be surprised to see that you didn't actually override
  // do_read_one() from the base class.
  std::pair<bool, double> do_read_one() override;
};

// A second implementation of DataSource that produces random numbers according
// to a normal distribution.
class RandomNormalDataSource : public DataSource {
 public:
  // Customizable parameters are passed via the ctor. Note that count is
  // necessary, so we know when to stop.
  //
  // If provided, the seed will be used to initialize the RNG. If you pass the
  // same seed value, you'll get the same sequence of random-looking numbers.
  // This can be useful for testing.
  RandomNormalDataSource(size_t count, double mean = 0.0, double stdev = 1.0,
                         size_t seed = 0);

 private:
  // The first two instance variables are part of the C++11 random number
  // generation library. You can ignore the details for now.
  std::mt19937_64 rng_;
  std::normal_distribution<double> distr_;
  // Number of numbers to produce.
  size_t count_;
  // As in ConsoleDataSource, we use override for safety. Always use override or
  // final when overriding virtual methods!
  std::vector<double> do_read() override;
};
