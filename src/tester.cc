#include <cstddef>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>
#include <vector>

#include "base/panic.h"
#include "base/timer.h"
#include "boost/program_options.hpp"
#include "boost/scoped_array.hpp"
#include "boost/scoped_ptr.hpp"
#include "containers/testable.h"
#include "containers/testable_binary_search.h"
#include "containers/testable_bucket_binary_search.h"
#include "containers/testable_stl_set.h"
#include "generators/data_generator_interface.h"
#include "generators/data_random_generator.h"
#include "generators/queries_generator_interface.h"
#include "generators/queries_random_generator.h"
#include "typelist/macros.h"
#include "typelist/typelist.h"

namespace po = boost::program_options;
using generators::DataGeneratorInterface;
using generators::DataRandomGenerator;
using generators::QueriesGeneratorInterface;
using generators::QueriesRandomGenerator;

using base::Vector;
using boost::scoped_array;
using boost::scoped_ptr;
using std::clog;
using std::endl;
using std::fixed;
using std::setprecision;
using std::string;
using std::vector;
using typelist::Length;
using typelist::NullType;
using typelist::TypeList;


const size_t kDefaultNumObjects = 1 << 20;
const size_t kDefaultNumQueries = 1 << 20;
const size_t kDefaultBucketSize = 500;
const size_t kNumDimensions     = 3;

const int kConstructionPrecision = 6;
const int kQueriesPrecision      = 6;
const int kCheckingPrecision     = 6;

const int kExitSuccess       = 0;
const int kExitHelp          = 1;
const int kExitMemoryFailure = 2;


size_t FLAGS_num_objects;
size_t FLAGS_num_queries;
size_t FLAGS_bucket_size;
int    FLAGS_seed;

namespace {

struct InfoEntry {
  string method_name_;
  size_t num_objects_;
  size_t num_queries_;
  double construction_time_;
  double total_queries_time_;
  double checking_time_;
}; // struct InfoEntry

std::ostream& operator << (std::ostream& os, const InfoEntry& entry) {
  os << "Method name: " << entry.method_name_ << endl;

  os << "Num objects: " << entry.num_objects_ << endl;
  os << "Num queries: " << entry.num_queries_ << endl;

  os << fixed;

  os << "Construction time: " << setprecision(kConstructionPrecision) <<
    entry.construction_time_ << endl;
  os << "Total queries time: " << setprecision(kQueriesPrecision) <<
    entry.total_queries_time_ << endl;
  os << "Checking time: " << setprecision(kCheckingPrecision) <<
    entry.checking_time_ << endl;

  return os;
}

template<typename T, typename Derived>
void TestQueries(containers::TestableInterface<T, Derived>* testable,
		 size_t num_objects, const T* objects,
		 size_t num_queries, const T* queries,
		 const bool* answers,
		 InfoEntry* entry) {
  boost::scoped_array<bool>
    current_answers(new (std::nothrow) bool [num_queries]);

  if (current_answers.get() == NULL)
    panic("TestQueries: can't allocate memory, terminating...",
	  kExitMemoryFailure);

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
  assert(std::equal(answers, answers + num_queries, current_answers.get()));
  entry->checking_time_ = timer.Elapsed();
}

template<size_t, typename, typename>
struct MetaTestContainer;

template<size_t I, typename D>
struct MetaTestContainer<I, D, NullType> {
  static void TestContainer(vector<InfoEntry> &,
			    size_t, const D*,
			    size_t, const D*,
			    const bool*) {
  }
}; // struct MetaTestContainer

template<size_t I, typename D, typename T, typename U>
struct MetaTestContainer<I, D, TypeList<T, U> > {
  static void TestContainer(vector<InfoEntry>& entries,
			    size_t num_objects, const D* objects,
			    size_t num_queries, const D* queries,
			    const bool* answers) {
    boost::scoped_ptr< containers::TestableInterface<D, T> > interface(new T());
    TestQueries(interface.get(),
		num_objects, objects,
		num_queries, queries, answers,
		&entries[I]);
    MetaTestContainer<I + 1, D, U>::TestContainer(entries,
						  num_objects, objects,
						  num_queries, queries,
						  answers);
  }
}; // struct MetaTestContainer

template<typename D, typename TList>
void TestContainers(vector<InfoEntry>& entries,
		    size_t num_objects, const D* objects,
		    size_t num_queries, const D* queries,
		    const bool* answers) {
  assert(Length<TList>::kLength == entries.size());
  MetaTestContainer<0, D, TList>::TestContainer(entries,
						num_objects, objects,
						num_queries, queries,
						answers);
}

template<typename T>
void GenerateData(DataGeneratorInterface<T>* generator,
		  size_t num_objects, T* objects) {
  for (size_t i = 0; i < num_objects; ++i)
    generator->Generate(&objects[i]);
}

template<typename T>
void GenerateQueries(QueriesGeneratorInterface<T>* generator,
		     size_t num_queries, T* queries) {
  for (size_t i = 0; i < num_queries; ++i)
    generator->Generate(&queries[i]);
}

template<typename T>
void GenerateAnswers(size_t num_objects, const T* objects,
		     size_t num_queries, const T* queries,
		     bool* answers) {
  boost::scoped_array<T> buffer(new (std::nothrow) T [num_objects]);
  if (buffer.get() == NULL)
    panic("GenerateAnswers: can't allocate memory, terminating...",
	  kExitMemoryFailure);
  std::copy(objects, objects + num_objects, buffer.get());
  std::sort(buffer.get(), buffer.get() + num_objects);

  for (size_t query = 0; query < num_queries; ++query)
    answers[query] = std::binary_search(buffer.get(),
					buffer.get() + num_objects,
					queries[query]);
}

}  // namespace

int main(int argc, char** argv) {
  po::options_description description("Allowed options");
  description.add_options()
    ("help,h", "produce help message")
    ("num_objects,n",
     po::value<size_t>(&FLAGS_num_objects)->default_value(kDefaultNumObjects),
     "number of objects in tests")
    ("num_queries,m",
     po::value<size_t>(&FLAGS_num_queries)->default_value(kDefaultNumQueries),
     "number of queries in tests")
    ("bucket_size,b",
     po::value<size_t>(&FLAGS_bucket_size)->default_value(kDefaultBucketSize),
     "bucket size in binary search")
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

  typedef Vector<kNumDimensions, double> DataType;
  typedef TYPELIST_3(containers::TestableStlSet<DataType>,
		     containers::TestableBinarySearch<DataType>,
		     containers::TestableBucketBinarySearch<DataType>) TList;
  vector<InfoEntry> entries(Length<TList>::kLength);
  entries[0].method_name_ = "Stl Set";
  entries[1].method_name_ = "Stl Binary Search";
  entries[2].method_name_ = "Bucket Binary Search";

  scoped_array<DataType> objects(new (std::nothrow)
				 DataType [FLAGS_num_objects]);
  if (objects.get() == NULL)
    panic("main: can't allocate memory for objects, terminating...",
	  kExitMemoryFailure);
  scoped_ptr<DataGeneratorInterface<DataType> > data_generator(
    new DataRandomGenerator<DataType>());
  GenerateData(data_generator.get(), FLAGS_num_objects, objects.get());

  scoped_array<DataType> queries(new (std::nothrow)
				 DataType [FLAGS_num_queries]);
  if (queries.get() == NULL)
    panic("main: can't allocate memory for queries, terminating...",
	  kExitMemoryFailure);
  scoped_ptr<QueriesGeneratorInterface<DataType> > queries_generator(
    new QueriesRandomGenerator<DataType>(FLAGS_num_objects, objects.get()));
  GenerateQueries(queries_generator.get(), FLAGS_num_queries, queries.get());

  scoped_array<bool> answers(new (std::nothrow)
			     bool [FLAGS_num_queries]);
  if (answers.get() == NULL)
    panic("main: can't allocate memory for answers, terminating...",
	  kExitMemoryFailure);
  GenerateAnswers(FLAGS_num_objects, objects.get(),
		  FLAGS_num_queries, queries.get(),
		  answers.get());

  TestContainers<DataType, TList>(entries,
  				  FLAGS_num_objects, objects.get(),
  				  FLAGS_num_queries, queries.get(),
  				  answers.get());

  std::cout << endl;
  std::copy(entries.begin(), entries.end(),
	    std::ostream_iterator<InfoEntry>(std::cout, "\n"));
  return kExitSuccess;
}
