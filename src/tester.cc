#include <cstddef>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <new>

#include "base/timer.h"
#include "boost/program_options.hpp"
#include "boost/scoped_array.hpp"
#include "containers/testable.h"
#include "containers/testable_stl_set.h"

namespace po = boost::program_options;
using std::clog;
using std::endl;


const size_t kDefaultNumObjects = 1 << 20;
const size_t kDefaultNumQueries = 1 << 20;

const size_t kExitSuccess       = 0;
const size_t kExitHelp          = 1;
const size_t kExitMemoryFailure = 2;


size_t FLAGS_num_objects;
size_t FLAGS_num_queries;
int    FLAGS_seed;

namespace {

struct InfoEntry {
  size_t num_objects_;
  size_t num_queries_;
  double objects_generation_time_;
  double queries_generation_time_;
  double construction_time_;
  double total_queries_time_;
  double checking_time_;
}; // struct InfoEntry

std::ostream& operator << (std::ostream& os, const InfoEntry& entry) {
  os << "Num objects: " << entry.num_objects_ << endl;
  os << "Num queries: " << entry.num_queries_ << endl;
  os << "Construction time: " << entry.construction_time_ << endl;
  os << "Total queries time: " << entry.total_queries_time_ << endl;
  os << "Checking time: " << entry.checking_time_ << endl;

  return os;
}

template<typename T, typename Derived>
void TestQueries(containers::TestableInterface<T, Derived> *testable,
		 size_t num_objects, const T *objects,
		 size_t num_queries, const T *queries,
		 const bool *answers,
		 InfoEntry *entry) {
  boost::scoped_array<bool>
    current_answers(new (std::nothrow) bool [num_queries]);

  if (current_answers.get() == NULL) {
    clog << "TestQueries: can't allocate memory, terminating...\n" << endl;
    exit(kExitMemoryFailure);
  }

  entry->num_objects_ = num_objects;
  entry->num_queries_ = num_queries;

  base::Timer timer;
  testable->Assign(num_objects, objects);
  entry->construction_time_ = timer.Elapsed();

  timer.Restart();
  for (size_t query = 0; query < num_queries; ++query)
    current_answers[query] = testable->Contains(queries[query]);
  entry->total_queries_time_ = timer.Elapsed();

  timer.Restart();
  assert(std::equal(answers, answers + num_queries, current_answers));
  entry->checking_time_ = timer.Elapsed();
}

}  // namespace

int main(int argc, char **argv) {
  po::options_description description("Allowed options");
  description.add_options()
    ("help,h", "produce help message")
    ("num_objects,n",
     po::value<size_t>(&FLAGS_num_objects)->default_value(kDefaultNumObjects),
     "number of objects in tests")
    ("num_queries,m",
     po::value<size_t>(&FLAGS_num_queries)->default_value(kDefaultNumQueries),
     "number of queries in tests")
    ("seed,s",
     po::value<int>(&FLAGS_seed)->default_value(0),
     "seed for random generator, if zero, current time is used as seed")
    ;

  po::variables_map variables_map;
  po::store(po::parse_command_line(argc, argv, description), variables_map);
  po::notify(variables_map);

  if (variables_map.count("help")) {
    clog << description << endl;
    return kExitHelp;
  }

  if (FLAGS_seed == 0)
    FLAGS_seed = time(NULL);
  srand(FLAGS_seed);

  clog << "Num objects: " << FLAGS_num_objects << endl;
  clog << "Num queries: " << FLAGS_num_queries << endl;
  clog << "Seed: " << FLAGS_seed << endl;

  return kExitSuccess;
}
