#include "context.h"
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

struct AllocatorSetup {
    AllocatorSetup()   {
        SLAT::Context::Initialise();
        logging::add_file_log("unit_tests.log");
        logging::add_common_attributes();
    }
    ~AllocatorSetup()  { /* shutdown your allocator/check memory leaks here */ }
};

BOOST_GLOBAL_FIXTURE( AllocatorSetup );

