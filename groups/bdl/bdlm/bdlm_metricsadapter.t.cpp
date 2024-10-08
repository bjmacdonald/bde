// bdlm_metricsadapter.t.cpp                                          -*-C++-*-
#include <bdlm_metricsadapter.h>

#include <bdlm_metric.h>            // for testing only
#include <bdlm_metricdescriptor.h>  // for testing only

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_keyword.h>
#include <bsls_protocoltest.h>
#include <bsls_review.h>
#include <bsls_timeinterval.h>

#include <bsl_cstddef.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test defines a protocol class the purpose of which is to
// provide an interface for registering collection objects with a monitoring
// system.
//
// Global Concerns:
//  - The test driver is robust w.r.t. reuse in other, similar components.
//  - It is possible to create a concrete implementation of the protocol.
//  - At no time is memory allocated from the global allocator except examples.
//  - At no time is memory allocated from the default alloc. except examples.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~MetricsAdapter();
//
// MANIPULATORS
// [ 1] virtual CH registerCollectionCallback(metricDescriptor, cb) = 0;
// [ 1] virtual int removeCollectionCallback(const CH& handle) = 0;
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                   GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void testMetric(BloombergLP::bdlm::Metric *value)
{
    (void)value;
}

// ============================================================================
//                   GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef bdlm::MetricsAdapter ProtocolClass;

static bsl::string s_returnString;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int incrementInstanceCount(const bdlm::MetricDescriptor& metricDescriptor)
    {
        (void)metricDescriptor;
        return markDone();
    }

    CallbackHandle registerCollectionCallback(
                  const bdlm::MetricDescriptor& metricDescriptor,
                  const Callback&               callback) BSLS_KEYWORD_OVERRIDE
    {
        (void)metricDescriptor;
        (void)callback;
        return markDone();
    }

    int removeCollectionCallback(const CallbackHandle& handle)
                                                          BSLS_KEYWORD_OVERRIDE
    {
        (void)handle;
        return markDone();
    }
};

}  // close unnamed namespace

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing the `bdlm::MetricsAdapter` Protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates an elided concrete implementation of the
// `bdlm::MetricsAdapter` protocol that allows for registering metric callback
// functions with a monitoring system.
//
// First, we define the interface of a limited `my_MetricsMonitor` class that
// allows only one metric collection function to be registered:
// ```

    /// This class implements a metric monitoring system.
    class my_MetricsMonitor {

        // ...

        // DATA
        bdlm::Metric                   d_value;     // metric supplied to
                                                    // `d_callback`

        bsl::string                    d_name;      // register metric name

        bdlm::MetricsAdapter::Callback d_callback;  // registered callback

      public:
        // ...

        // MANIPULATORS

        /// Register the specified `callback` with this monitoring system,
        /// using the specified `name` for display purposes.  Return a
        /// callback handle to be used with `removeCallback`.
        bdlm::MetricsAdapter::CallbackHandle registerCallback(
                                const bsl::string&                   name,
                                const bdlm::MetricsAdapter::Callback callback);

        /// Remove the callback associated with the specified `handle`.
        /// Return 0 on success, or a non-zero value if `handle` cannot be
        /// found.
        int removeCallback(const bdlm::MetricsAdapter::CallbackHandle& handle);

        /// Invoke the registered callback.
        void update();

        // ACCESSORS

        /// Return the name of the registered metric.
        const bsl::string& name() const;

        /// Return the value computed by the invocations of the registered
        /// callback.
        double value() const;
    };
// ```
// Then, we implement the functions:
// ```
    // MANIPULATORS
    bdlm::MetricsAdapter::CallbackHandle my_MetricsMonitor::registerCallback(
                                 const bsl::string&                   name,
                                 const bdlm::MetricsAdapter::Callback callback)
    {
        d_value    = bdlm::Metric::Gauge(0.0);
        d_name     = name;
        d_callback = callback;

        return 1;
    }

    int my_MetricsMonitor::removeCallback(
                            const bdlm::MetricsAdapter::CallbackHandle& handle)
    {
        (void)handle;
        return 0;
    }

    void my_MetricsMonitor::update()
    {
        d_callback(&d_value);
    }

    // ACCESSORS
    const bsl::string& my_MetricsMonitor::name() const
    {
        return d_name;
    }

    double my_MetricsMonitor::value() const
    {
        return d_value.theGauge();
    }
// ```
// Next, we define the implementation class of the `bdlm::MetricsAdapter`
// protocol:
// ```

    /// This class implements an interface for clients and suppliers of
    /// metrics adapters.
    class my_MetricsAdapter : public bdlm::MetricsAdapter {

        // DATA
        my_MetricsMonitor *d_monitor_p;  // pointer to monitor to use for
                                         // metrics (held not owned)

      public:
        // CREATORS

        /// Create a `my_MetricsAdapter` using the specified `monitor` for
        /// registered callbacks.
        my_MetricsAdapter(my_MetricsMonitor *monitor);

        /// Destroy this object.
        ~my_MetricsAdapter() BSLS_KEYWORD_OVERRIDE;

        // MANIPULATORS

        /// Register the specified `callback` with a monitoring system,
        /// using the specified `metricDescriptor` for the registration.
        /// Return the callback handle to be used with
        /// `removeCollectionCallback`.  Note the information used for
        /// registration is implementation dependant, and may involve values
        /// computed from the supplied arguments.
        CallbackHandle registerCollectionCallback(
                 const bdlm::MetricDescriptor& metricDescriptor,
                 const Callback&               callback) BSLS_KEYWORD_OVERRIDE;

        int removeCollectionCallback(const CallbackHandle& handle)
                                                         BSLS_KEYWORD_OVERRIDE;
            // Remove the callback associated with the specified `handle`.
            // Return 0 on success, or a non-zero value if `handle` cannot be
            // found.
    };
// ```
// Then, we implement the methods of `myMetricsAdapter`:
// ```
    // CREATORS
    my_MetricsAdapter::my_MetricsAdapter(my_MetricsMonitor *monitor)
    : d_monitor_p(monitor)
    {
    }

    my_MetricsAdapter::~my_MetricsAdapter()
    {
    }

    // MANIPULATORS
    bdlm::MetricsAdapter::CallbackHandle
                                 my_MetricsAdapter::registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               callback)
    {
        bsl::string name = metricDescriptor.metricNamespace() + '.'
                         + metricDescriptor.metricName() + '.'
                         + metricDescriptor.objectTypeName() + '.'
                         + metricDescriptor.objectTypeAbbreviation() + '.'
                         + metricDescriptor.objectIdentifier();

        return d_monitor_p->registerCallback(name, callback);
    }

    int my_MetricsAdapter::removeCollectionCallback(
                            const bdlm::MetricsAdapter::CallbackHandle& handle)
    {
        return d_monitor_p->removeCallback(handle);
    }
// ```
// Next, we provide the metric method, `my_metric`, which will compute its
// invocation count:
// ```
    void my_metric(BloombergLP::bdlm::Metric *value)
    {
        *value = value->theGauge() + 1.0;
    }
// ```

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bool expectGlobalAllocation = false;

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
    bool expectDefaultAllocation = false;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, replace `assert` with
        //    `ASSERT`, and insert `if (veryVerbose)` before all output
        //    operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Then, we instantiate a `my_MetricsMonitor` and a `myMetricsAdapter`:
// ```
        my_MetricsMonitor monitor;
        my_MetricsAdapter adapter(&monitor);
// ```
// Next, we construct a `bdlm::MetricDescriptor`, register the `my_metric`
// method with the `monitor`, and verify the `monitor` has the expected name
// for the metric:
// ```
        bdlm::MetricDescriptor descriptor("a", "b", 1, "c", "d", "e");

        adapter.registerCollectionCallback(descriptor, my_metric);

        ASSERT(monitor.name() == "a.b.c.d.e");
// ```
// Now, we invoke the `update` method a few times:
// ```
        monitor.update();
        monitor.update();
        monitor.update();
// ```
// Finally, we verify the metric has the expected value:
// ```
        ASSERT(monitor.value() == 3.0);
// ```
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        // 1. The protocol is abstract: no objects of it can be created.
        //
        // 2. The protocol has no data members.
        //
        // 3. The protocol has a virtual destructor.
        //
        // 4. All methods of the protocol are pure virtual.
        //
        // 5. All methods of the protocol are publicly accessible.
        //
        // Plan:
        // 1. Define a concrete derived implementation, `ProtocolClassTestImp`,
        //    of the protocol.
        //
        // 2. Create an object of the `bsls::ProtocolTest` class template
        //    parameterized by `ProtocolClassTestImp`, and use it to verify
        //    that:
        //
        //   1. The protocol is abstract. (C-1)
        //
        //   2. The protocol has no data members. (C-2)
        //
        //   3. The protocol has a virtual destructor. (C-3)
        //
        // 3. Use the `BSLS_PROTOCOLTEST_ASSERT` macro to verify that
        //    non-creator methods of the protocol are:
        //
        //   1. virtual, (C-4)
        //
        //   2. publicly accessible. (C-5)
        //
        // Testing:
        //   virtual ~MetricsAdapter();
        //   virtual CH registerCollectionCallback(metricDescriptor, cb) = 0;
        //   virtual int removeCollectionCallback(const CH& handle) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nCreate a test object.\n";

        bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);

        if (verbose) cout << "\nVerify that the protocol is abstract.\n";

        ASSERT(testObj.testAbstract());

        if (verbose) cout << "\nVerify that there are no data members.\n";

        ASSERT(testObj.testNoDataMembers());

        if (verbose) cout << "\nVerify that the destructor is virtual.\n";

        ASSERT(testObj.testVirtualDestructor());

        if (verbose) cout << "\nVerify that methods are public and virtual.\n";

        {
            bdlm::MetricDescriptor descriptor;

            BSLS_PROTOCOLTEST_ASSERT(testObj,
                                     registerCollectionCallback(descriptor,
                                                                &testMetric));
        }

        BSLS_PROTOCOLTEST_ASSERT(testObj, removeCollectionCallback(0));
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    if (!expectGlobalAllocation) {
        ASSERTV(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());
    }

    // CONCERN: In no case does memory come from the default allocator.

    if (!expectDefaultAllocation) {
        ASSERTV(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
