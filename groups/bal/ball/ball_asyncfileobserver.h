// ball_asyncfileobserver.h                                           -*-C++-*-
#ifndef INCLUDED_BALL_ASYNCFILEOBSERVER
#define INCLUDED_BALL_ASYNCFILEOBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an asynchronous observer that logs to a file and `stdout`.
//
//@CLASSES:
//  ball::AsyncFileObserver: observer that outputs logs to a file and `stdout`
//
//@SEE_ALSO: ball_record, ball_context, ball_observer, ball_fileobserver
//
//@DESCRIPTION: This component provides a concrete implementation of the
// `ball::Observer` protocol, `ball::AsyncFileObserver`, for *asynchronously*
// publishing log records to `stdout` and, optionally, to a user-specified
// file.  A `ball::AsyncFileObserver` (informally, "async file observer")
// object processes the log records received through its `publish` method by
// pushing the supplied `ball::Record` object onto a queue and (typically)
// returning immediately (see {Log Record Queue}).  Each enqueued record is
// ultimately published when an independent publication thread removes the log
// record from the queue and writes it to the configured log file and `stdout`.
// The following inheritance hierarchy diagram shows the classes involved and
// their methods:
// ```
//            ,-----------------------.
//           ( ball::AsyncFileObserver )
//            `-----------------------'
//                        |              ctor
//                        |              disableFileLogging
//                        |              disablePublishInLocalTime
//                        |              disableSizeRotation
//                        |              disableStdoutLoggingPrefix
//                        |              disableTimeIntervalRotation
//                        |              enableFileLogging
//                        |              enableStdoutLoggingPrefix
//                        |              enablePublishInLocalTime
//                        |              forceRotation
//                        |              rotateOnSize
//                        |              rotateOnTimeInterval
//                        |              setLogFormat
//                        |              setOnFileRotationCallback
//                        |              setStdoutThreshold
//                        |              shutdownPublicationThread
//                        |              startPublicationThread
//                        |              stopPublicationThread
//                        |              suppressUniqueFileNameOnRotation
//                        |              getLogFormat
//                        |              isFileLoggingEnabled
//                        |              isPublicationThreadRunning
//                        |              isPublishInLocalTimeEnabled
//                        |              isStdoutLoggingPrefixEnabled
//                        |              isSuppressUniqueFileNameOnRotation
//                        |              recordQueueLength
//                        |              rotationLifetime
//                        |              rotationSize
//                        |              stdoutThreshold
//                        V
//                 ,--------------.
//                ( ball::Observer )
//                 `--------------'
//                                       dtor
//                                       publish
//                                       releaseRecords
// ```
// The format of published log records is user-configurable for both logging to
// `stdout` and logging to a file (see {Log Record Formatting} below).
// Although logging to a file is initially disabled following construction, the
// most common use-case for `ball::AsyncFileObserver` is to also log to a file,
// enabled by calling the `enableFileLogging` method.  In addition, an async
// file observer may be configured to perform automatic log file rotation (see
// {Log File Rotation} below).
//
///Async File Observer Configuration Synopsis
///------------------------------------------
// `ball::AsyncFileObserver` offers several constructor arguments and
// manipulators that may be used to configure various aspects of an async file
// observer object.  These are summarized in the following tables along with
// the accessors that can be used to query the current state of the
// configuration.  Further details are provided in the following sections and
// the function-level documentation.
// ```
// +-----------------------+---------------------------------+
// | Aspect                | Constructor Arguments           |
// +=======================+=================================+
// | Log Record Timestamps | publishInLocalTime              |
// +-----------------------+---------------------------------+
// | 'stdout' Logging      | stdoutThreshold                 |
// +-----------------------+---------------------------------+
// | Log Record Queue      | maxRecordQueueSize              |
// |                       | dropRecordsOnFullQueueThreshold |
// +-----------------------+---------------------------------+
//
// +-------------+------------------------------------+
// | Aspect      | Related Methods                    |
// +=============+====================================+
// | Log Record  | setLogFormat                       |
// | Formatting  | enableStdoutLoggingPrefix          |
// |             | disableStdoutLoggingPrefix         |
// |             | getLogFormat                       |
// |             | isStdoutLoggingPrefixEnabled       |
// +-------------+------------------------------------+
// | Log Record  | enablePublishInLocalTime           |
// | Timestamps  | disablePublishInLocalTime          |
// |             | isPublishInLocalTimeEnabled        |
// +-------------+----------------------------------- +
// | File        | enableFileLogging                  |
// | Logging     | disableFileLogging                 |
// |             | isFileLoggingEnabled               |
// +-------------+----------------------------------- +
// | 'stdout'    | setStdoutThreshold                 |
// | Logging     | enableStdoutLoggingPrefix          |
// |             | disableStdoutLoggingPrefix         |
// |             | stdoutThreshold                    |
// |             | isStdoutLoggingPrefixEnabled       |
// +-------------+------------------------------------+
// | Log File    | rotateOnSize                       |
// | Rotation    | rotateOnTimeInterval               |
// |             | disableSizeRotation                |
// |             | disableTimeIntervalRotation        |
// |             | setOnFileRotationCallback          |
// |             | suppressUniqueFileNameOnRotation   |
// |             | rotationSize                       |
// |             | rotationLifetime                   |
// |             | isSuppressUniqueFileNameOnRotation |
// +-------------+------------------------------------+
// | Publication | startPublicationThread             |
// | Thread      | stopPublicationThread              |
// | Management  | shutdownPublicationThread          |
// |             | isPublicationThreadRunning         |
// +-------------+------------------------------------+
// ```
// In general, a `ball::AsyncFileObserver` object can be dynamically configured
// throughout its lifetime (in particular, before or after being registered
// with a logger manager).  Note that for `ball::AsyncFileObserver`,
// configuration changes that affect how records are logged (e.g.,
// `enablePublishInLocalTime` and `disablePublishInLocalTime`) impact both
// those records received by the `publish` method subsequent to making the
// configuration change as well as those records that are already on the queue.
//
///Log Record Queue
///----------------
// The log record queue of an async file observer has a configurable, but
// fixed, maximum size, where the default is 8192.  The `maxRecordQueueSize`
// constructor argument may be used to specify an alternative maximum size for
// the record queue that can be either larger or smaller than the default.
//
// By default, an async file observer is configured for the `publish` method to
// drop *all* records that it receives while the queue is full.  This behavior
// can be modified by supplying a `Severity::Level` for the constructor
// argument `dropRecordsOnFullQueueThreshold` whereby only those records whose
// severity is less severe than that threshold are dropped.  Each record whose
// severity is at least as severe as `dropRecordsOnFullQueueThreshold` will
// cause the `publish` method to *block* until the queue has spare capacity to
// accommodate the record (presumably freed up by the publication thread having
// consumed a record from the queue).  For example, if this threshold is set to
// `e_WARN` on construction, `e_INFO`, `e_DEBUG`, and `e_TRACE` records are
// dropped by `publish` when the queue is full whereas `e_WARN`, `e_ERROR`, and
// `e_FATAL` records will cause `publish` to block while waiting for room in
// the queue.  Note that while `publish` will never block with the default
// threshold setting, clients should consider the trade-offs between dropping
// potentially critical log messages versus possibly blocking in `publish` on
// such messages.
//
// An async file observer maintains a count of the records that are dropped,
// periodically publishing a warning (i.e., an internally generated log record
// with severity `e_WARN`) that reports the number of dropped records.  The
// record count is reset to 0 after each such warning is published, so each
// dropped record is counted only once.
//
///Log Record Formatting
///---------------------
// By default, the output format of published log records (whether to `stdout`
// or a user-specified file) is:
// ```
// DATE_TIME PID:THREAD-ID SEVERITY FILE:LINE CATEGORY MESSAGE USER-FIELDS
// ```
// where `DATE` and `TIME` are of the form `DDMonYYYY` and `HH:MM:SS.mmm`,
// respectively (`Mon` being the 3-letter abbreviation for the month).  For
// example, assuming that no user-defined fields are present, a log record
// will have the following appearance when the default format is in effect:
// ```
// 18MAY2005_18:58:12.076 7959:1 WARN ball_asyncfileobserver.t.cpp:404 TEST hi
// ```
// The default format for records published to `stdout` (only) can be shortened
// by calling `disableStdoutLoggingPrefix`.  This method has the effect of
// reducing the above example message to the following when output to `stdout`:
// ```
// WARN ball_asyncfileobserver.t.cpp:404 TEST hi
// ```
// For additional flexibility, the `setLogFormat` method can be called to
// configure the format of published records.  This method takes two arguments:
// one specifies the format of records logged to `stdout` and the other applies
// to records that are logged to a file.  The respective formats are specified
// using `printf`-style (`%`-prefixed) conversion specifications.  (See
// {`ball_recordstringformatter`} for information on how format specifications
// are defined and interpreted.)  For example, the following statement will
// force subsequent records to be logged in a format that is almost identical
// to the default long format except that the timestamp attribute will be
// written in ISO 8601 format:
// ```
// asyncFileObserver.setLogFormat("\n%I %p:%t %s %f:%l %c %m %u\n",
//                                "\n%I %p:%t %s %f:%l %c %m %u\n");
// ```
// Once a customized format is specified for `stdout`, calling
// `disableStdoutLoggingPrefix` will switch to the default short format, i.e.,
// "\n%s %f:%l %c %m %u\n".  If `enableStdoutLoggingPrefix` is subsequently
// called, the customized format specified in the most recent call to
// `setLogFormat` will be reinstated.
//
// Note that the observer emits newline characters at the beginning and at the
// end of a log record by default, so the user needs to add them explicitly to
// the format string to preserve this behavior.
//
// Also note that in the sample long-form message above the timestamp has
// millisecond precision (`18MAY2005_18:58:12.076`).  If microsecond precision
// is desired instead, consider using either the `%D` or `%O` format
// specification supported by `ball_recordstringformatter`.
//
///Log Record Timestamps
///---------------------
// By default, the timestamp attributes of published records are written in UTC
// time (unless `true` is supplied for the optional `publishInLocalTime`
// constructor flag).  To write timestamps in local time instead, call the
// `enablePublishInLocalTime` method.  Note that the local time offset is
// calculated using the UTC timestamp of each record.  To revert to UTC time,
// call the `disablePublishInLocalTime` method.  Whether UTC time or local time
// is in effect can be queried via `isPublishInLocalTimeEnabled`.
//
// Note that log record timestamps that are output (to either `stdout` or to a
// file) reflect the times at which the records were received by the `publish`
// method of the async file observer (and pushed onto the queue).  That is, the
// length of time that a record resides on the queue does not impact the
// record's timestamp when it is eventually output by the publication thread.
//
///Local Time Offset Calculations
/// - - - - - - - - - - - - - - -
// The calculation of the local time offset adds some overhead to the
// publication of each log record.  If this overhead is an issue, it can be
// mitigated by installing a high-performance local-time offset callback for
// `bdlt::CurrentTime` in `main`.  See {`bsls_systemtime`} for the details of
// installing such a callback and see {`baltzo_localtimeoffsetutil`} for an
// example facility.  Note that such callbacks can improve performance for all
// users of `bdlt::CurrentTime`, not just the `ball` logger.
//
///Log Filename Patterns
///---------------------
// The `enableFileLogging` method supports the use of `%`-escape sequences to
// specify log filenames.  The recognized sequences are as follows:
// ```
// %Y - current year   (4 digits with leading zeros)
// %M - current month  (2 digits with leading zeros)
// %D - current day    (2 digits with leading zeros)
// %h - current hour   (2 digits with leading zeros)
// %m - current minute (2 digits with leading zeros)
// %s - current second (2 digits with leading zeros)
// %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
// %p - process ID
// ```
// The date and time elements of the derived filename are based on the time
// when the log file is created.  Furthermore, these elements are based on
// either UTC time or local time depending on the value returned by
// `isPublishInLocalTimeEnabled`.  (See {Log Record Timestamps} for the
// similarity with the treatment of record timestamps.)
//
// For example, a log filename pattern of "task.log.%Y%M%D_%h%m%s" will yield
// the filename `task.log.20110501_123000` if the file is created on
// 01-May-2011 at 12:30:00 local time (assuming `enablePublishInLocalTime` was
// called).
//
///Log File Rotation
///-----------------
// A `ball::AsyncFileObserver` may be configured to perform automatic rotation
// of log files based on simple file rotation conditions (or rules).
//
///File Rotation Conditions
/// - - - - - - - - - - - -
// Rotation rules may be established based on the size of the log file (i.e., a
// "rotation-on-size" rule), and a periodic time interval (i.e., a
// "rotation-on-time-interval" rule).  These rules are independently enabled by
// the `rotateOnSize` and `rotateOnTimeInterval` methods, respectively.  If
// both rules are in effect, log file rotation is performed whenever either
// rule applies.
//
///Rotated File Naming
///- - - - - - - - - -
// When a log file is rotated, a new filename is generated using the pattern
// supplied to `enableFileLogging`.  If the file having the new name does not
// exist, the current log file is closed, and the logging continues to the new
// file.
//
// If the file having the new name already exits, then the behavior of the file
// rotation is further controlled by the flag set with
// `suppressUniqueFileNameOnRotation`:
//
// * `suppressUniqueFileNameOnRotation(false)` (*default* behavior)
//   The current log filename is renamed by appending a timestamp in the form
//   ".%Y%M%D_%h%m%s" where the timestamp indicates when the file being
//   rotated was last opened (the time of either the last file rotation or the
//   last call to `enableFileLogging`, whichever is most recent).  As with the
//   timestamps of logged records, the timestamps appended to log filenames
//   upon rotation will be in UTC time or local time depending on the value
//   returned by `isPublishInLocalTimeEnabled`.
// * `suppressUniqueFileNameOnRotation(true)`
//   The logging continues to the *current* log file, effectively suppressing
//   log filename rotation.  This may happen when the log file pattern does
//   not contain %-escape sequences indicating a time period, or the rotation
//   interval is less than the time period encoded by %-escape sequences.  In
//   order to rotate log files in this mode, the log file pattern MUST contain
//   %-escape sequences that specify date and (optionally) time.  For example,
//   the log filename pattern "app_%Y%M%D.log" will produce a single log file
//   per calendar day (assuming, the rotation on time is enabled and the
//   rotation happens at least once a day).
//
// The two tables below illustrate the names of old and new log files when a
// file rotation occurs.  We assume that the log file is rotated on 2011-May-21
// at 12:29:59 local time and that the last rotation occurred at 12:30:00 on
// the previous day.  We further assume that `enablePublishInLocalTime` was
// called, so that all date and time elements are rendered in local time.
//
// The first table shows the name change (if any) of the (old) log file being
// rotated when the flag controlled by `suppressUniqueFileNameOnRotation`
// is set to `false`:
// ```
// Disabled: 'suppressUniqueFileNameOnRotation'
//
// For brevity:
//     <TS1> = 20210520_123000
//     <TS2> = 20210521_122959 (aka next day, about the same time)
//
// +----------------+-----------------+----------------+----------------------
// | Pattern        | Filename Before | Filename After | Rotated Filename
// |                | Rotation        | Rotation       |
// +----------------+-----------------+----------------+----------------------
// | "a.log"        | a.log           | a.log          | a.log.<TS1>
// | "a.log.%T"     | a.log.<TS1>     | a.log.<TS2>    | a.log.<TS1>
// | "a.log.%Y%M"   | a.log.202105    | a.log.202105   | a.log.202105.<TS1>
// | "a.log.%Y%M%D" | a.log.20210520  | a.log.20110521 | a.log.20210520
// +----------------+-----------------+----------------+----------------------
// ```
// Note that upon rotation a timestamp was appended to the name of the rotated
// file when the log pattern does not contain %-escape sequences indicating a
// time period ("a.log"), or the rotation period (in our case, one day) is less
// than the time period encoded in the pattern (in case of "a.log.%Y%M" the
// period is one month).
//
// The next table shows the rotated name when the flag controlled by
// `suppressUniqueFileNameOnRotation` is set to `true`, and (possibly new) name
// of the (new) log file following rotation:
// ```
// Enabled: 'suppressUniqueFileNameOnRotation'
//
// +----------------+-----------------+----------------+----------------------
// | Pattern        | Filename Before | Filename After | Rotated Filename
// |                | Rotation        | Rotation       |
// +----------------+-----------------+----------------+----------------------
// | "a.log"        | a.log           | a.log          | none
// | "a.log.%T"     | a.log.<TS1>     | a.log.<TS2>    | a.log.<TS1>
// | "a.log.%Y%M"   | a.log.202105    | a.log.202105   | none
// | "a.log.%Y%M%D" | a.log.20210520  | a.log.20110521 | a.log.20210520
// +----------------+-----------------+----------------+----------------------
// ```
// Note that the original filename is reused when the log pattern does not
// contain %-escape sequences indicating a time period ("a.log"), or the
// rotation period (in our case, one day) is less than the time period encoded
// in the pattern (in case of "a.log.%Y%M" the period is one month).
//
// Also note, that in any cases, when the log pattern includes "%T", or encodes
// a time period that coincides the rotation period (in case of "a.log.%Y%M%D"
// the period is one day), then a unique name on each rotation is produced with
// the (local) time at which file rotation occurred embedded in the filename.
//
///Thread Safety
///-------------
// All public methods of `ball::AsyncFileObserver` are thread-safe, and can be
// called concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through the Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a `ball::AsyncFileObserver` within the
// `ball` logging system.
//
// First, we initialize the `ball` logging subsystem with the default
// configuration:
// ```
// ball::LoggerManagerConfiguration configuration;
// ball::LoggerManagerScopedGuard   guard(configuration);
//
// ball::LoggerManager& manager = ball::LoggerManager::singleton();
// ```
// Note that the application is now prepared to log messages using the `ball`
// logging subsystem, but until the application registers an observer, all log
// records will be discarded.
//
// Then, we create a shared pointer to a `ball::AsyncFileObserver` object,
// `observerPtr`, having default attributes.  Note that a default-constructed
// async file observer has a maximum (fixed) size of 8192 for its log record
// queue and will drop incoming log records when that queue is full.  (See
// {Log Record Queue} for further information.)
// ```
// bsl::shared_ptr<ball::AsyncFileObserver> observer =
//                                bsl::make_shared<ball::AsyncFileObserver>();
// ```
// Next, we set the required logging format by calling the `setLogFormat`
// method.  The statement below outputs timestamps in ISO 8601 format to a log
// file and in `bdlt`-style (default) format to `stdout`, where timestamps are
// output with millisecond precision in both cases:
// ```
// observer->setLogFormat("%I %p:%t %s %f:%l %c %m\n",
//                        "%d %p:%t %s %f:%l %c %m\n");
// ```
// Note that both of the above format specifications omit user fields (`%u`) in
// the output.  Also note that, unlike the default, this format does not emit a
// blank line between consecutive log messages.
//
// Next, we start the publication thread by invoking `startPublicationThread`:
// ```
// observer->startPublicationThread();
// ```
// Then, we register the async file observer with the logger manager.  Upon
// successful registration, the observer will start to receive log records via
// the `publish` method:
// ```
// int rc = manager.registerObserver(observer, "asyncObserver");
// assert(0 == rc);
// ```
// Next, we set the log category and log a few records with different logging
// severity.  By default, only the records with `e_WARN`, `e_ERROR`, or
// `e_FATAL` severity will be logged to `stdout`.  Note that logging to a file
// is not enabled by default:
// ```
// BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");
//
// BALL_LOG_INFO << "Will not be published on 'stdout'.";
// BALL_LOG_WARN << "This warning *will* be published on 'stdout'.";
// ```
// Then, we change the default severity for logging to `stdout` by calling the
// `setStdoutThreshold` method:
// ```
// observerPtr->setStdoutThreshold(ball::Severity::e_INFO);
//
// BALL_LOG_DEBUG << "This debug message is not published on 'stdout'.";
// BALL_LOG_INFO  << "This info will be published on 'stdout'.";
// BALL_LOG_WARN  << "This warning will be published on 'stdout'.";
// ```
// Next, we disable logging to `stdout` and enable logging to a file:
// ```
// observer->setStdoutThreshold(ball::Severity::e_OFF);
//
// // Create and log records to a file named "/var/log/task/task.log".
// observer->enableFileLogging("/var/log/task/task.log");
// ```
// Note that logs are now asynchronously written to the file.
//
// Then, we specify rules for log file rotation based on the size and time
// interval:
// ```
// // Rotate the file when its size becomes greater than or equal to 32
// // megabytes.
// observer->rotateOnSize(1024 * 32);
//
// // Rotate the file every 24 hours.
// observer->rotateOnTimeInterval(bdlt::DatetimeInterval(1));
// ```
// Note that in this configuration the user may end up with multiple log files
// for a specific day (because of the rotation-on-size rule).
//
// Next, we demonstrate how to correctly shut down the async file observer.  We
// first stop the publication thread by explicitly calling the
// `stopPublicationThread` method.  This method blocks until all the log
// records that were on the record queue on entry to `stopPublicationThread`
// have been published:
// ```
// observer->stopPublicationThread();
// ```
// Then, we disable the log rotation rules established earlier and also
// completely disable logging to a file:
// ```
// observer->disableSizeRotation();
//
// observer->disableTimeIntervalRotation();
//
// observer->disableFileLogging();
// ```
// Note that stopping the publication thread and disabling various features of
// the async file observer is not strictly necessary before object destruction.
// In particular, if a publication thread is still running when the destructor
// is invoked, all records on the record queue upon entry are published and
// then the publication thread is automatically stopped before destroying the
// async file observer.  In any case, all resources managed by the async file
// observer will be released when the object is destroyed.
//
// Finally, we can deregister our async file observer from the `ball` logging
// subsystem entirely (and destroy the observer later):
// ```
// rc = manager.deregisterObserver("asyncObserver");
// assert(0 == rc);
// ```

#include <balscm_version.h>

#include <ball_context.h>
#include <ball_fileobserver.h>
#include <ball_fileobserver2.h>
#include <ball_observer.h>
#include <ball_record.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>

#include <bdlcc_boundedqueue.h>

#include <bdlt_datetimeinterval.h>

#include <bslma_allocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_threadutil.h>

#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_string.h>

#include <string>                  // 'std::string', 'std::pmr::string'

namespace BloombergLP {
namespace ball {

                          // ===============================
                          // struct AsyncFileObserver_Record
                          // ===============================

/// PRIVATE STRUCT.  For use by the `ball::AsyncFileObserver` implementation
/// only.  This `struct` holds a log record and its associated context.
struct AsyncFileObserver_Record {

    // PUBLIC DATA
    bsl::shared_ptr<const Record> d_record;   // log record
    Context                       d_context;  // context of log record
};

                          // =======================
                          // class AsyncFileObserver
                          // =======================

/// This class implements the `Observer` protocol.  The `publish` method of
/// this class outputs log records asynchronously to `stdout` and optionally
/// to a user-specified file.  This class is thread-safe; different threads
/// can operate on an object concurrently.  This class is exception-neutral
/// with no guarantee of rollback.  In no event is memory leaked.
class AsyncFileObserver : public Observer {

    // PRIVATE TYPES

    /// State of the publication thread, as captured by `d_threadState`.
    enum ThreadState {

        e_RUNNING,        // the publication thread is running

        e_NOT_RUNNING     // the publication thread is not running
    };

    // DATA
    FileObserver                   d_fileObserver;   // forward most public
                                                     // method calls to this
                                                     // (non-async) file
                                                     // observer member

    bslmt::ThreadUtil::Handle      d_threadHandle;   // handle of asynchronous
                                                     // publication thread

    bdlcc::BoundedQueue<AsyncFileObserver_Record>
                                   d_recordQueue;    // fixed-size queue of
                                                     // records processed by
                                                     // the publication thread

    bsls::AtomicInt                d_threadState;    // the publication thread
                                                     // state, one of the
                                                     // values of `ThreadState`

    Severity::Level                d_dropRecordsOnFullQueueThreshold;
                                                     // records with severity
                                                     // below this threshold
                                                     // are dropped when the
                                                     // queue is full; default
                                                     // is `Severity::e_OFF`

    bsls::AtomicInt                d_dropCount;      // number of dropped
                                                     // records; reset to 0
                                                     // each time drop count is
                                                     // published

    bsl::function<void()>          d_publishThreadEntryPoint;
                                                     // publication thread
                                                     // entry point functor

    mutable bslmt::Mutex           d_mutex;          // serialize operations

    bslma::Allocator              *d_allocator_p;    // memory allocator (held,
                                                     // not owned)

  private:
    // NOT IMPLEMENTED
    AsyncFileObserver(const AsyncFileObserver&);
    AsyncFileObserver& operator=(const AsyncFileObserver&);

    // PRIVATE MANIPULATORS

    /// Initialize members of this object that do not vary between
    /// constructor overloads.  Note that this method should be removed when
    /// C++11 constructor chaining is available on all supported platforms.
    void construct();

    /// Publish records from the record queue, to the log file and `stdout`,
    /// until signaled to stop.  The behavior is undefined if this method is
    /// invoked concurrently from multiple threads, i.e., it is *not*
    /// thread-safe.  Note that this function is the entry point for the
    /// publication thread.
    void publishThreadEntryPoint();

  public:
    // TYPES

    /// `OnFileRotationCallback` is an alias for a user-supplied callback
    /// function that is invoked after the file observer attempts to rotate
    /// its log file.  The callback takes two arguments: (1) an integer
    /// status value where 0 indicates a new log file was successfully
    /// created and a non-zero value indicates an error occurred during
    /// rotation, and (2) a string that provides the name of the rotated log
    /// file if the rotation was successful.  E.g.:
    /// ```
    /// void onLogFileRotation(int                rotationStatus,
    ///                        const bsl::string& rotatedLogFileName);
    /// ```
    typedef FileObserver::OnFileRotationCallback OnFileRotationCallback;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AsyncFileObserver,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create an async file observer that asynchronously publishes log
    /// records to `stdout` if their severity is at least as severe as the
    /// optionally specified `stdoutThreshold` level, and has file logging
    /// initially disabled.  If `stdoutThreshold` is not specified, log
    /// records are published to `stdout` if their severity is at least as
    /// severe as `Severity::e_WARN`.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.  Records received by the
    /// `publish` method are appended to a queue having a maximum (fixed)
    /// length of 8192, and published later by an independent publication
    /// thread.  All records received while the queue is full are discarded.
    /// (See {Log Record Queue} for further information.)  Note that
    /// `isPublishInLocalTimeEnabled` returns `false` following construction
    /// indicating that the timestamp attribute of published records will be
    /// written in UTC time (see `enablePublishInLocalTime`).  Also note
    /// that independent default record formats are in effect for `stdout`
    /// and file logging (see `setLogFormat`).
    explicit AsyncFileObserver(bslma::Allocator *basicAllocator  = 0);
    explicit AsyncFileObserver(Severity::Level   stdoutThreshold,
                               bslma::Allocator *basicAllocator  = 0);

    /// Create an async file observer that asynchronously publishes log
    /// records to `stdout` if their severity is at least as severe as the
    /// specified `stdoutThreshold` level, and has file logging initially
    /// disabled.  The timestamp attribute of published records is written
    /// in local time if the specified `publishInLocalTime` flag is `true`,
    /// and in UTC time otherwise.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.  Records received by the
    /// `publish` method are appended to a queue having a maximum (fixed)
    /// length of 8192, and published later by an independent publication
    /// thread.  All records received while the queue is full are discarded.
    /// (See {Log Record Queue} for further information.)  Note that
    /// independent default record formats are in effect for `stdout` and
    /// file logging (see `setLogFormat`).
    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      bslma::Allocator *basicAllocator = 0);

    /// Create an async file observer that asynchronously publishes log
    /// records to `stdout` if their severity is at least as severe as the
    /// specified `stdoutThreshold` level, and has file logging initially
    /// disabled.  The timestamp attribute of published records is written
    /// in local time if the specified `publishInLocalTime` flag is `true`,
    /// and in UTC time otherwise.  Records received by the `publish` method
    /// are appended to a queue having the specified (fixed)
    /// `maxRecordQueueSize`, and published later by an independent
    /// publication thread.  Optionally specify a
    /// `dropRecordsOnFullQueueThreshold` indicating the severity threshold
    /// below which records received when the queue is full will be
    /// discarded; records received whose severity is at least as severe as
    /// this threshold will block the calling thread if the queue is full,
    /// until space is available.  If `dropRecordsOnFullQueueThreshold` is
    /// not specified, all records received while the queue is full are
    /// discarded.  (See {Log Record Queue} for further information.)
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.  Note that independent default record formats are in effect
    /// for `stdout` and file logging (see `setLogFormat`).
    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      int               maxRecordQueueSize,
                      bslma::Allocator *basicAllocator = 0);
    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      int               maxRecordQueueSize,
                      Severity::Level   dropRecordsOnFullQueueThreshold,
                      bslma::Allocator *basicAllocator = 0);

    /// Publish all records that were on the record queue upon entry if a
    /// publication thread is running, stop the publication thread (if any),
    /// close the log file if file logging is enabled, and destroy this
    /// async file observer.
    ~AsyncFileObserver() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Disable file logging for this async file observer.  This method has
    /// no effect if file logging is not enabled.  Calling this method will
    /// prevent the logging to a file of any unpublished records held by
    /// this observer.  Note that records subsequently received through the
    /// `publish` method as well as those that are currently on the queue
    /// may still be logged to `stdout` after calling this method.
    void disableFileLogging();

    /// Disable publishing of the timestamp attribute of records in local
    /// time by this async file observer; henceforth, timestamps will be in
    /// UTC time.  This method has no effect if publishing in local time is
    /// not enabled.  Note that this method also affects log filenames (see
    /// {Log Filename Patterns}).  Also note that this method affects
    /// records subsequently received through the `publish` method as well
    /// as those that are currently on the queue.
    void disablePublishInLocalTime();

    /// Disable log file rotation based on log file size for this async file
    /// observer.  This method has no effect if rotation-on-size is not
    /// enabled.
    void disableSizeRotation();

    /// Disable this async file observer from using the long output format
    /// when logging to `stdout`.  Henceforth, this async file observer will
    /// use the default short output format ("\n%s %f:%l %c %m %u\n") when
    /// logging to `stdout`.  This method has no effect if the long output
    /// format for `stdout` logging is not enabled.  Note that this method
    /// omits the "%d %p:%t " prefix from the default long output format.
    /// Also note that this method affects records subsequently received
    /// through the `publish` method as well as those that are currently on
    /// the queue.
    void disableStdoutLoggingPrefix();

    /// Disable log file rotation based on a periodic time interval for this
    /// async file observer.  This method has no effect if
    /// rotation-on-time-interval is not enabled.
    void disableTimeIntervalRotation();

    /// Enable logging of all records published to this async file observer
    /// to a file whose name is derived from the specified
    /// `logFilenamePattern`.  Return 0 on success, a positive value if file
    /// logging is already enabled (with no effect), and a negative value
    /// otherwise.  The basename of `logFilenamePattern` may contain
    /// `%`-escape sequences that are interpreted as follows:
    /// ```
    ///  %Y - current year   (4 digits with leading zeros)
    ///  %M - current month  (2 digits with leading zeros)
    ///  %D - current day    (2 digits with leading zeros)
    ///  %h - current hour   (2 digits with leading zeros)
    ///  %m - current minute (2 digits with leading zeros)
    ///  %s - current second (2 digits with leading zeros)
    ///  %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
    ///  %p - process ID
    /// ```
    /// Each time a log file is opened by this async file observer (upon a
    /// successful call to this method and following each log file
    /// rotation), the name of the new log file is derived from
    /// `logFilenamePattern` by interpreting the above recognized `%`-escape
    /// sequences.  If `isPublishInLocalTimeEnabled` returns `true`, the
    /// `%`-escape sequences related to time will be substituted with local
    /// time values, and UTC time values otherwise.  Note that, if a
    /// publication thread is running, it will now publish queued records to
    /// the log file where it previously was only publishing queued records
    /// to `stdout`.
    int enableFileLogging(const char *logFilenamePattern);

    /// Enable this async file observer to use the long output format when
    /// logging to `stdout`.  Henceforth, this async file observer will use
    /// the output format for `stdout` logging that was set by the most
    /// recent call to `setLogFormat`, or the default long output format
    /// ("\n%d %p:%t %s %f:%l %c %m %u\n") if `setLogFormat` has not yet
    /// been called.  This method has no effect if the long output format
    /// for `stdout` logging is already enabled.  Note that this method
    /// affects records subsequently received through the `publish` method
    /// as well as those that are currently on the queue.
    void enableStdoutLoggingPrefix();

    /// Enable publishing of the timestamp attribute of records in local
    /// time by this async file observer.  This method has no effect if
    /// publishing in local time is already enabled.  Note that this method
    /// also affects log filenames (see {Log Filename Patterns}).  Also note
    /// that this method affects records subsequently received through the
    /// `publish` method as well as those that are currently on the queue.
    void enablePublishInLocalTime();

    /// Forcefully perform a log file rotation by this async file observer.
    /// Close the current log file, rename the log file if necessary, and
    /// open a new log file.  This method has no effect if file logging is
    /// not enabled.  See {Rotated File Naming} for details on filenames of
    /// rotated log files.
    void forceRotation();

    using Observer::publish;  // Avoid hiding base class.

    /// Process the record referenced by the specified `record` shared
    /// pointer having the specified publishing `context` by writing the
    /// record and `context` to the current log file if file logging is
    /// enabled for this async file observer, and to `stdout` if the
    /// severity of `record` is at least as severe as the value returned by
    /// `stdoutThreshold`.  `record` and `context` are appended to the
    /// record queue and published (asynchronously) later by the publication
    /// thread.  If the queue is full, `record` and `context` are discarded
    /// unless the constructor taking `dropRecordsOnFullQueueThreshold` was
    /// used; if `dropRecordsOnFullQueueThreshold` was supplied with a
    /// `Severity::Level` at construction, `record` and `context` are
    /// discarded only if the severity of `record` is below that threshold,
    /// otherwise, this method will block waiting until space is available
    /// on the queue.  See {Log Record Queue} for further information.
    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Discard any shared references to `Record` objects that were supplied
    /// to the `publish` method, and are held by this observer.  Note that
    /// this operation should be called if resources underlying the
    /// previously provided shared pointers must be released.  Also note
    /// that all currently queued records are discarded.
    void releaseRecords() BSLS_KEYWORD_OVERRIDE;

    /// Set this async file observer to perform log file rotation when the
    /// size of the file exceeds the specified `size` (in kilobytes).  This
    /// rule replaces any rotation-on-size rule currently in effect.  The
    /// behavior is undefined unless `size > 0`.
    void rotateOnSize(int size);

    /// Set this file observer to perform a periodic log file rotation at
    /// multiples of the specified `interval`.  Optionally specify a
    /// `startTime` indicating the datetime to use as the starting point for
    /// computing the periodic rotation schedule.  If
    /// `isPublishInLocalTimeEnabled` is `true`, the `startTime` is
    /// interpreted as local time, and as a UTC time otherwise.  If
    /// `startTime` is not specified, the current time is used.  This rule
    /// replaces any rotation-on-time-interval rule currently in effect.
    /// The behavior is undefined unless `0 < interval.totalMilliseconds()`.
    /// Note that `startTime` may be a fixed time in the past; e.g., a
    /// reference time of `bdlt::Datetime(1, 1, 1)` and an interval of 24
    /// hours would configure a periodic rotation at midnight each day.
    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval,
                              const bdlt::Datetime&         startTime);

    /// Set the format specifications for log records written to the log
    /// file and to `stdout` to the specified `logFileFormat` and
    /// `stdoutFormat`, respectively.  If the default short output format is
    /// currently in effect for logging to `stdout`, this method has the
    /// effect of calling `enableStdoutLoggingPrefix` (see that method and
    /// `disableStdoutLoggingPrefix`).  See {Log Record Formatting} for
    /// details on the syntax of format specifications.  Note that default
    /// formats are in effect following construction until this method is
    /// called ("\n%d %p:%t %s %f:%l %c %m %u\n" for both file and `stdout`
    /// logging).  Also note that the observer emits newline characters at
    /// the beginning and at the end of a log record by default, so the user
    /// needs to add them explicitly to the format string to preserve this
    /// behavior.  Also note that this method affects records subsequently
    /// received through the `publish` method as well as those that are
    /// currently on the queue.
    void setLogFormat(const char *logFileFormat, const char *stdoutFormat);

    /// Set the specified `onRotationCallback` to be invoked after each time
    /// this async file observer attempts to perform a log file rotation.
    /// The behavior is undefined if the supplied function calls either
    /// `setOnFileRotationCallback`, `forceRotation`, or `publish` on this
    /// async file observer (i.e., the supplied callback should *not*
    /// attempt to write to the `ball` log).
    void setOnFileRotationCallback(
                             const OnFileRotationCallback& onRotationCallback);

    /// Set the minimum severity of records logged to `stdout` by this async
    /// file observer to the specified `stdoutThreshold` level.  Note that
    /// if the value of `stdoutThreshold` is `Severity::e_OFF`, logging to
    /// `stdout` is disabled.  Also note that this method affects records
    /// subsequently received through the `publish` method as well as those
    /// that are currently on the queue.
    void setStdoutThreshold(Severity::Level stdoutThreshold);

    /// Stop the publication thread without waiting for log records
    /// currently on the record queue to be published.  Return 0 on success,
    /// and a non-zero value if there is an error joining the publication
    /// thread.  Note that log records received by the `publish` method will
    /// continue to be added to the queue after the publication thread is
    /// shut down.
    int shutdownPublicationThread();

    /// Start a publication thread to asynchronously publish log records
    /// from the record queue.  If a publication thread is already active,
    /// this operation has no effect.  Return 0 on success, and a non-zero
    /// value if there is an error creating the publication thread.  Note
    /// that log records received by the `publish` method may have
    /// accumulated on the queue before the publication thread is started.
    int startPublicationThread();

    /// Block until all records that were on the record queue upon entry
    /// have been published, then stop the publication thread.  If there is
    /// no publication thread this operation has no effect.  Return 0 on
    /// success, and a non-zero value if there is an error joining the
    /// publication thread.  Note that log records received by the `publish`
    /// method will continue to be added to the queue after the publication
    /// thread is stopped.
    int stopPublicationThread();

    /// Suppress generating a unique log file name upon rotation if the
    /// specified `suppress` is `true`, and generate a unique filename
    /// otherwise.  See {Rotated File Naming} for details.
    void suppressUniqueFileNameOnRotation(bool suppress);

    // ACCESSORS

    /// Load the format specification for log records written by this async
    /// file observer to the log file into the specified `*logFileFormat`
    /// address and the format specification for log records written to
    /// `stdout` into the specified `*stdoutFormat` address.  See {Log
    /// Record Formatting} for details on the syntax of format
    /// specifications.
    void getLogFormat(const char **logFileFormat,
                      const char **stdoutFormat) const;

    /// Return `true` if file logging is enabled for this async file
    /// observer, and `false` otherwise.  Load the optionally specified
    /// `result` with the name of the current log file if file logging is
    /// enabled, and leave `result` unmodified otherwise.  Note that records
    /// received through the `publish` method of this async file observer
    /// may still be logged to `stdout` when this method returns `false`.
    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
    bool isFileLoggingEnabled(std::string *result) const;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
    /// Return `true` if file logging is enabled for this async file
    /// observer, and `false` otherwise.  Load the specified `result` with
    /// the name of the current log file if file logging is enabled, and
    /// leave `result` unmodified otherwise.  Note that records received
    /// through the `publish` method of this async file observer may still
    /// be logged to `stdout` when this method returns `false`.
    bool isFileLoggingEnabled(std::pmr::string *result) const;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING

    /// Return `true` if a publication thread is running, and `false`
    /// otherwise.  Note that records received by the `publish` method will
    /// still be added to the record queue even if there is no publication
    /// thread running.
    bool isPublicationThreadRunning() const;

    /// Return `true` if this async file observer writes the timestamp
    /// attribute of records that it publishes in local time, and `false`
    /// otherwise (in which case timestamps are written in UTC time).  Note
    /// that the value returned by this method also affects log filenames
    /// (see {Log Filename Patterns}).
    bool isPublishInLocalTimeEnabled() const;

    /// Return `true` if this async file observer uses the long output
    /// format when writing to `stdout`, and `false` otherwise (in which
    /// case the default short output format is used).  See
    /// `enableStdoutLoggingPrefix` and `disableStdoutLoggingPrefix`.
    bool isStdoutLoggingPrefixEnabled() const;

    /// Return `true` if the log filename uniqueness check on rotation is
    /// suppressed, and false otherwise.
    bool isSuppressUniqueFileNameOnRotation() const;

    /// Return `true` if the logging of user-defined fields is enabled for
    /// this async file observer, and `false` otherwise.
    ///
    /// **DEPRECATED**: Do not use.
    bool isUserFieldsLoggingEnabled() const;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    /// Return the difference between the local time and UTC time in effect
    /// when this async file observer was constructed.  Note that this value
    /// remains unchanged during the lifetime of this object and therefore
    /// may become incorrect when the difference between the local time and
    /// UTC time changes (e.g., when transitioning into or out of daylight
    /// savings time).
    ///
    /// **DEPRECATED**: Use `bdlt::LocalTimeOffset` instead.
    bdlt::DatetimeInterval localTimeOffset() const;
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    /// Return the number of log records currently on the record queue of
    /// this async file observer.
    bsl::size_t recordQueueLength() const;

    /// Return the log file lifetime that will trigger a file rotation by
    /// this async file observer if rotation-on-lifetime is in effect, and a
    /// 0 time interval otherwise.
    bdlt::DatetimeInterval rotationLifetime() const;

    /// Return the size (in kilobytes) of the log file that will trigger a
    /// file rotation by this async file observer if rotation-on-size is in
    /// effect, and 0 otherwise.
    int rotationSize() const;

    /// Return the minimum severity of records that will be logged to
    /// `stdout` by this async file observer.  Note that records with a
    /// threshold less severe than `stdoutThreshold()` may still be output
    /// to the log file if file logging is enabled.
    Severity::Level stdoutThreshold() const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class AsyncFileObserver
                          // -----------------------

// MANIPULATORS
inline
void AsyncFileObserver::disableFileLogging()
{
    d_fileObserver.disableFileLogging();
}

inline
void AsyncFileObserver::disablePublishInLocalTime()
{
    d_fileObserver.disablePublishInLocalTime();
}

inline
void AsyncFileObserver::disableSizeRotation()
{
    d_fileObserver.disableSizeRotation();
}

inline
void AsyncFileObserver::disableStdoutLoggingPrefix()
{
    d_fileObserver.disableStdoutLoggingPrefix();
}

inline
void AsyncFileObserver::disableTimeIntervalRotation()
{
    d_fileObserver.disableTimeIntervalRotation();
}

inline
int AsyncFileObserver::enableFileLogging(const char *logFilenamePattern)
{
    return d_fileObserver.enableFileLogging(logFilenamePattern);
}

inline
void AsyncFileObserver::enablePublishInLocalTime()
{
    d_fileObserver.enablePublishInLocalTime();
}

inline
void AsyncFileObserver::enableStdoutLoggingPrefix()
{
    d_fileObserver.enableStdoutLoggingPrefix();
}

inline
void AsyncFileObserver::forceRotation()
{
    d_fileObserver.forceRotation();
}

inline
void AsyncFileObserver::rotateOnSize(int size)
{
    d_fileObserver.rotateOnSize(size);
}

inline
void AsyncFileObserver::rotateOnTimeInterval(
                                        const bdlt::DatetimeInterval& interval)
{
    d_fileObserver.rotateOnTimeInterval(interval);
}

inline
void AsyncFileObserver::rotateOnTimeInterval(
                                       const bdlt::DatetimeInterval& interval,
                                       const bdlt::Datetime&         startTime)
{
    d_fileObserver.rotateOnTimeInterval(interval, startTime);
}

inline
void AsyncFileObserver::setLogFormat(const char *logFileFormat,
                                     const char *stdoutFormat)
{
    d_fileObserver.setLogFormat(logFileFormat, stdoutFormat);
}

inline
void AsyncFileObserver::setOnFileRotationCallback(
                              const OnFileRotationCallback& onRotationCallback)
{
    d_fileObserver.setOnFileRotationCallback(onRotationCallback);
}

inline
void AsyncFileObserver::setStdoutThreshold(Severity::Level stdoutThreshold)
{
    d_fileObserver.FileObserver::setStdoutThreshold(stdoutThreshold);
}

inline
void AsyncFileObserver::suppressUniqueFileNameOnRotation(bool suppress)
{
    d_fileObserver.suppressUniqueFileNameOnRotation(suppress);
}

// ACCESSORS
inline
void AsyncFileObserver::getLogFormat(const char **logFileFormat,
                                     const char **stdoutFormat) const
{
    d_fileObserver.getLogFormat(logFileFormat, stdoutFormat);
}

inline
bool AsyncFileObserver::isFileLoggingEnabled() const
{
    return d_fileObserver.isFileLoggingEnabled();
}

inline
bool AsyncFileObserver::isFileLoggingEnabled(bsl::string *result) const
{
    return d_fileObserver.isFileLoggingEnabled(result);
}

inline
bool AsyncFileObserver::isFileLoggingEnabled(std::string *result) const
{
    return d_fileObserver.isFileLoggingEnabled(result);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
inline
bool AsyncFileObserver::isFileLoggingEnabled(std::pmr::string *result) const
{
    return d_fileObserver.isFileLoggingEnabled(result);
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING

inline
bool AsyncFileObserver::isPublicationThreadRunning() const
{
    return bslmt::ThreadUtil::invalidHandle() != d_threadHandle;
}

inline
bool AsyncFileObserver::isPublishInLocalTimeEnabled() const
{
    return d_fileObserver.isPublishInLocalTimeEnabled();
}

inline
bool AsyncFileObserver::isStdoutLoggingPrefixEnabled() const
{
    return d_fileObserver.isStdoutLoggingPrefixEnabled();
}

inline
bool AsyncFileObserver::isSuppressUniqueFileNameOnRotation() const
{
    return d_fileObserver.isSuppressUniqueFileNameOnRotation();
}

inline
bool AsyncFileObserver::isUserFieldsLoggingEnabled() const
{
    return d_fileObserver.isUserFieldsLoggingEnabled();
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
bdlt::DatetimeInterval AsyncFileObserver::localTimeOffset() const
{
    return d_fileObserver.localTimeOffset();
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

inline
bsl::size_t AsyncFileObserver::recordQueueLength() const
{
    return d_recordQueue.numElements();
}

inline
bdlt::DatetimeInterval AsyncFileObserver::rotationLifetime() const
{
    return d_fileObserver.rotationLifetime();
}

inline
int AsyncFileObserver::rotationSize() const
{
    return d_fileObserver.rotationSize();
}

inline
Severity::Level AsyncFileObserver::stdoutThreshold() const
{
    return d_fileObserver.stdoutThreshold();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
