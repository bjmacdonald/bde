// ball_fileobserver.h                                                -*-C++-*-
#ifndef INCLUDED_BALL_FILEOBSERVER
#define INCLUDED_BALL_FILEOBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe observer that logs to a file and to 'stdout'.
//
//@CLASSES:
//  ball::FileObserver: observer that writes log records to a file and 'stdout'
//
//@SEE_ALSO: ball_record, ball_context, ball_observer, ball_fileobserver2
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol, 'ball::FileObserver', for publishing log records
// to 'stdout' and, optionally, to a user-specified file.  The following
// inheritance hierarchy diagram shows the classes involved and their methods:
//..
//                ,------------------.
//               ( ball::FileObserver )
//                `------------------'
//                         |              ctor
//                         |              disableFileLogging
//                         |              disableTimeIntervalRotation
//                         |              disableSizeRotation
//                         |              disableStdoutLoggingPrefix
//                         |              disablePublishInLocalTime
//                         |              enableFileLogging
//                         |              enableStdoutLoggingPrefix
//                         |              enablePublishInLocalTime
//                         |              forceRotation
//                         |              rotateOnSize
//                         |              rotateOnTimeInterval
//                         |              setOnFileRotationCallback
//                         |              setStdoutThreshold
//                         |              setLogFormat
//                         |              suppressUniqueFileNameOnRotation
//                         |              getLogFormat
//                         |              isFileLoggingEnabled
//                         |              isStdoutLoggingPrefixEnabled
//                         |              isPublishInLocalTimeEnabled
//                         |              isSuppressUniqueFileNameOnRotation
//                         |              rotationLifetime
//                         |              rotationSize
//                         |              stdoutThreshold
//                         V
//                  ,--------------.
//                 ( ball::Observer )
//                  `--------------'
//                                        dtor
//                                        publish
//                                        releaseRecords
//..
// A 'ball::FileObserver' object processes the log records received through its
// 'publish' method by writing them to 'stdout' and, if so configured, to a
// user-specified file.  The format of published log records is
// user-configurable for both destinations, 'stdout' and file (see {Log Record
// Formatting} below).  Although logging to a file is initially disabled
// following construction, the most common use-case for 'ball::FileObserver' is
// to also log to a file, enabled by calling the 'enableFileLogging' method.
// In addition, a file observer may be configured to perform automatic log file
// rotation (see {Log File Rotation} below).
//
///File Observer Configuration Synopsis
///------------------------------------
// 'ball::FileObserver' offers several constructor arguments and manipulators
// that may be used to configure various aspects of a file observer object.
// These are summarized in the following tables along with the accessors that
// can be used to query the current state of the configuration.  Further
// details are provided in the following sections and the function-level
// documentation.
//..
// +-----------------------+-----------------------+
// | Aspect                | Constructor Arguments |
// +=======================+=======================+
// | Log Record Timestamps | publishInLocalTime    |
// +-----------------------+-----------------------+
// | 'stdout' Logging      | stdoutThreshold       |
// +-----------------------+-----------------------+
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
// |             |                                    |
// +-------------+------------------------------------+
// | File        | enableFileLogging                  |
// | Logging     | disableFileLogging                 |
// |             | isFileLoggingEnabled               |
// |             |                                    |
// +-------------+------------------------------------+
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
//..
// In general, a 'ball::FileObserver' object can be dynamically configured
// throughout its lifetime (in particular, before or after being registered
// with a logger manager).  However, note that for 'ball::FileObserver',
// configuration changes that affect how records are logged (e.g.,
// 'enablePublishInLocalTime' and 'disablePublishInLocalTime') impact only
// those records that are published subsequent to making the configuration
// change.
//
///Log Record Formatting
///---------------------
// By default, the output format of published log records, whether written to
// 'stdout' or to a user-specified file, is:
//..
//  DATE_TIME PID:THREAD-ID SEVERITY FILE:LINE CATEGORY MESSAGE USER-FIELDS
//..
// where 'DATE' and 'TIME' are of the form 'DDMonYYYY' and 'HH:MM:SS.mmm',
// respectively ('Mon' being the 3-letter abbreviation for the month).  For
// example, a log record will have the following appearance when the default
// format is in effect (assuming that no user-defined fields are present):
//..
//  18MAY2005_18:58:12.076 7959:1 WARN ball_fileobserver.t.cpp:404 TEST hello!
//..
// The default format for records published to 'stdout' (only) can be shortened
// by calling 'disableStdoutLoggingPrefix'.  This method has the effect of
// reducing the above example message to the following when output to 'stdout':
//..
//  WARN ball_fileobserver.t.cpp:404 TEST hello!
//..
// For additional flexibility, the 'setLogFormat' method can be called to
// configure the format of published records.  This method takes two arguments:
// the first argument specifies the format of records logged to a file and the
// second applies to records that are logged to 'stdout'.  The respective
// formats are specified using 'printf'-style ('%'-prefixed) conversion
// specifications.  (See {'ball_recordstringformatter'} for information on how
// format specifications are defined and interpreted.)  For example, the
// following statement will force subsequent records to be logged in a format
// that is almost identical to the default long format except that the
// timestamp attribute will be written in ISO 8601 format:
//..
//  fileObserver.setLogFormat("\n%I %p:%t %s %f:%l %c %m %u\n",
//                            "\n%I %p:%t %s %f:%l %c %m %u\n");
//..
// Once a customized format is specified for 'stdout', calling
// 'disableStdoutLoggingPrefix' will switch to the default short format, i.e.,
// "\n%s %f:%l %c %m %u\n".  If 'enableStdoutLoggingPrefix' is subsequently
// called, the customized format specified in the most recent call to
// 'setLogFormat' will be reinstated.  Note that the observer emits newline
// characters at the beginning and at the end of a log record by default, so
// the user needs to add them explicitly to the format string to preserve this
// behavior.
//
// Note that in the sample long-form message above the timestamp has
// millisecond precision ('18MAY2005_18:58:12.076').  If microsecond precision
// is desired instead, consider using either the '%D' or '%O' format
// specification supported by 'ball_recordstringformatter'.
//
///Log Record Timestamps
///---------------------
// By default, the timestamp attributes of published records are written in UTC
// time (unless 'true' is supplied for the optional 'publishInLocalTime'
// constructor flag).  To write timestamps in local time instead, call the
// 'enablePublishInLocalTime' method.  Note that the local time offset is
// calculated using the UTC timestamp of each record.  To revert to UTC time,
// call the 'disablePublishInLocalTime' method.  Whether UTC time or local time
// is in effect can be queried via 'isPublishInLocalTimeEnabled'.
//
///Local Time Offset Calculations
/// - - - - - - - - - - - - - - -
// The calculation of the local time offset adds some overhead to the
// publication of each log record.  If this overhead is an issue, it can be
// mitigated by installing a high-performance local-time offset callback for
// 'bdlt::CurrentTime' in 'main'.  See {'bsls_systemtime'} for the details of
// installing such a callback and see {'baltzo_localtimeoffsetutil'} for an
// example facility.  Note that such callbacks can improve performance for all
// users of 'bdlt::CurrentTime', not just the 'ball' logger.
//
///Log Filename Patterns
///---------------------
// The 'enableFileLogging' method supports the use of '%'-escape sequences to
// specify log filenames.  The recognized sequences are as follows:
//..
//  %Y - current year   (4 digits with leading zeros)
//  %M - current month  (2 digits with leading zeros)
//  %D - current day    (2 digits with leading zeros)
//  %h - current hour   (2 digits with leading zeros)
//  %m - current minute (2 digits with leading zeros)
//  %s - current second (2 digits with leading zeros)
//  %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
//  %p - process ID
//..
// The date and time elements of the derived filename are based on the time
// when the log file is created.  Furthermore, these elements are based on
// either UTC time or local time depending on the value returned by
// 'isPublishInLocalTimeEnabled'.  (See {Log Record Timestamps} for the
// similarity with the treatment of record timestamps.)
//
// For example, a log filename pattern of "task.log.%Y%M%D_%h%m%s" will yield
// the filename 'task.log.20110501_123000' if the file is created on
// 01-May-2011 at 12:30:00 local time (assuming 'enablePublishInLocalTime' was
// called).
//
///Log File Rotation
///-----------------
// A 'ball::FileObserver' may be configured to perform automatic rotation of
// log files based on simple file rotation conditions (or rules).
//
///File Rotation Conditions
/// - - - - - - - - - - - -
// Rotation rules may be established based on the size of the log file (i.e., a
// "rotation-on-size" rule), and a periodic time interval (i.e., a
// "rotation-on-time-interval" rule).  These rules are independently enabled by
// the 'rotateOnSize' and 'rotateOnTimeInterval' methods, respectively.  If
// both rules are in effect, log file rotation is performed whenever either
// rule applies.
//
///Rotated File Naming
///- - - - - - - - - -
// When a log file is rotated, a new filename is generated using the pattern
// supplied to 'enableFileLogging'.  If the file having the new name does not
// exist, the current log file is closed, and the logging continues to the new
// file.
//
// If the file having the new name already exits, then the behavior of the file
// rotation is further controlled by the flag set with
// 'suppressUniqueFileNameOnRotation':
//
//: o 'suppressUniqueFileNameOnRotation(false)' (*default* behavior)
//:   The current log filename is renamed by appending a timestamp in the form
//:   ".%Y%M%D_%h%m%s" where the timestamp indicates when the file being
//:   rotated was last opened (the time of either the last file rotation or the
//:   last call to 'enableFileLogging', whichever is most recent).  As with the
//:   timestamps of logged records, the timestamps appended to log filenames
//:   upon rotation will be in UTC time or local time depending on the value
//:   returned by 'isPublishInLocalTimeEnabled'.
//:
//: o 'suppressUniqueFileNameOnRotation(true)'
//:   The logging continues to the *current* log file, effectively suppressing
//:   log filename rotation.  This may happen when the log file pattern does
//:   not contain %-escape sequences indicating a time period, or the rotation
//:   interval is less than the time period encoded by %-escape sequences.  In
//:   order to rotate log files in this mode, the log file pattern MUST contain
//:   %-escape sequences that specify date and (optionally) time.  For example,
//:   the log filename pattern "app_%Y%M%D.log" will produce a single log file
//:   per calendar day (assuming, the rotation on time is enabled and the
//:   rotation happens at least once a day).
//
// The two tables below illustrate the names of old and new log files when a
// file rotation occurs.  We assume that the log file is rotated on 2011-May-21
// at 12:29:59 local time and that the last rotation occurred at 12:30:00 on
// the previous day.  We further assume that 'enablePublishInLocalTime' was
// called, so that all date and time elements are rendered in local time.
//
// The first table shows the name change (if any) of the (old) log file being
// rotated when the flag controlled by 'suppressUniqueFileNameOnRotation'
// is set to 'false':
//..
//  Disabled: 'suppressUniqueFileNameOnRotation'
//
//  For brevity:
//      <TS1> = 20210520_123000
//      <TS2> = 20210521_122959 (aka next day, about the same time)
//
//  +----------------+-----------------+----------------+----------------------
//  | Pattern        | Filename Before | Filename After | Rotated Filename
//  |                | Rotation        | Rotation       |
//  +----------------+-----------------+----------------+----------------------
//  | "a.log"        | a.log           | a.log          | a.log.<TS1>
//  | "a.log.%T"     | a.log.<TS1>     | a.log.<TS2>    | a.log.<TS1>
//  | "a.log.%Y%M"   | a.log.202105    | a.log.202105   | a.log.202105.<TS1>
//  | "a.log.%Y%M%D" | a.log.20210520  | a.log.20110521 | a.log.20210520
//  +----------------+-----------------+----------------+----------------------
//..
// Note that upon rotation a timestamp was appended to the name of the rotated
// file when the log pattern does not contain %-escape sequences indicating a
// time period ("a.log"), or the rotation period (in our case, one day) is less
// than the time period encoded in the pattern (in case of "a.log.%Y%M" the
// period is one month).
//
// The next table shows the rotated name when the flag controlled by
// 'suppressUniqueFileNameOnRotation' is set to 'true', and (possibly new) name
// of the (new) log file following rotation:
//..
//  Enabled: 'suppressUniqueFileNameOnRotation'
//
//  +----------------+-----------------+----------------+----------------------
//  | Pattern        | Filename Before | Filename After | Rotated Filename
//  |                | Rotation        | Rotation       |
//  +----------------+-----------------+----------------+----------------------
//  | "a.log"        | a.log           | a.log          | none
//  | "a.log.%T"     | a.log.<TS1>     | a.log.<TS2>    | a.log.<TS1>
//  | "a.log.%Y%M"   | a.log.202105    | a.log.202105   | none
//  | "a.log.%Y%M%D" | a.log.20210520  | a.log.20110521 | a.log.20210520
//  +----------------+-----------------+----------------+----------------------
//..
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
// All methods of 'ball::FileObserver' are thread-safe, and can be called
// concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Basic Usage
/// - - - - - - - - - -
// First, we create a 'ball::LoggerManagerConfiguration' object, 'lmConfig',
// and set the logging "pass-through" level -- the level at which log records
// are published to registered observers -- to 'DEBUG':
//..
//  int main()
//  {
//      ball::LoggerManagerConfiguration lmConfig;
//      lmConfig.setDefaultThresholdLevelsIfValid(ball::Severity::e_DEBUG);
//..
// Next, create a 'ball::LoggerManagerScopedGuard' object whose constructor
// takes the configuration object just created.  The guard will initialize the
// logger manager singleton on creation and destroy the singleton upon
// destruction.  This guarantees that any resources used by the logger manager
// will be properly released when they are not needed:
//..
//      ball::LoggerManagerScopedGuard guard(lmConfig);
//      ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Next, we create a 'ball::FileObserver' object and register it with the
// 'ball' logging system;
//..
//      bsl::shared_ptr<ball::FileObserver> observer =
//                                      bsl::make_shared<ball::FileObserver>();
//      int rc = manager.registerObserver(observer, "default");
//      ASSERT(0 == rc);
//..
// The default format for outputting log records can be changed by calling the
// 'setLogFormat' method.  The statement below outputs record timestamps in ISO
// 8601 format to the log file and in 'bdlt'-style (default) format to
// 'stdout', where timestamps are output with millisecond precision in both
// cases:
//..
//      observer->setLogFormat("%I %p:%t %s %f:%l %c %m\n",
//                             "%d %p:%t %s %f:%l %c %m\n");
//..
// Note that both of the above format specifications omit user fields ('%u') in
// the output.  Also note that, unlike the default, this format does not emit a
// blank line between consecutive log messages.
//
// Henceforth, all messages that are published by the logging system will be
// transmitted to the 'publish' method of 'observer'.  By default, only the
// messages with a 'e_WARN', 'e_ERROR', or 'e_FATAL' severity will be logged to
// 'stdout':
//..
//      BALL_LOG_SET_CATEGORY("main")
//      BALL_LOG_INFO << "Will not be published on 'stdout'.";
//      BALL_LOG_WARN << "This warning *will* be published on 'stdout'.";
//..
// This default can be changed by specifying an optional argument to the
// 'ball::FileObserver' constructor or by calling the 'setStdoutThreshold'
// method:
//..
//      observer->setStdoutThreshold(ball::Severity::e_INFO);
//
//      BALL_LOG_DEBUG << "This debug message is not published on 'stdout'.";
//      BALL_LOG_INFO  << "This info message *will* be published on 'stdout'.";
//      BALL_LOG_WARN  << "This warning will be published on 'stdout'.";
//..
// The user can log all messages to a specified file and specify rotation rules
// based on the size of the log file or its lifetime:
//..
//      // Create and log records to a file named "/var/log/task/task.log".
//      observer->enableFileLogging("/var/log/task/task.log");
//
//      // Disable 'stdout' logging.
//      observer->setStdoutThreshold(ball::Severity::e_OFF);
//
//      // Rotate the file when its size becomes greater than or equal to 256
//      // megabytes.
//      observer->rotateOnSize(1024 * 256);
//
//      // Rotate the file every 24 hours.
//      observer->rotateOnTimeInterval(bdlt::DatetimeInterval(1));
//..
// Note that in this configuration the user may end up with multiple log files
// for any given day (because of the rotation-on-size rule).  This feature can
// be disabled dynamically later:
//..
//      observer->disableSizeRotation();
//      return 0;
//  }
//..

#include <balscm_version.h>

#include <ball_fileobserver2.h>
#include <ball_observer.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>

#include <bdlt_datetimeinterval.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_mutex.h>

#include <bsls_libraryfeatures.h>

#include <bsl_memory.h>
#include <bsl_string.h>

#include <string>           // 'std::string', 'std::pmr::string'

namespace BloombergLP {
namespace ball {

class Context;
class Record;

                          // ==================
                          // class FileObserver
                          // ==================

class FileObserver : public Observer {
    // This class implements the 'Observer' protocol.  The 'publish' method of
    // this class outputs the log records that it receives to 'stdout' and
    // optionally to a user-specified file.  This class is thread-safe;
    // different threads can operate on an object concurrently.  This class is
    // exception-neutral with no guarantee of rollback.  In no event is memory
    // leaked.

    // DATA
    RecordStringFormatter d_logFileFormatter;   // record formatter used when
                                                // logging to a file

    RecordStringFormatter d_stdoutFormatter;    // record formatter used when
                                                // logging to 'stdout'

    Severity::Level       d_stdoutThreshold;    // minimum severity for records
                                                // logged to 'stdout'

    bool                  d_useRegularFormatOnStdoutFlag;
                                                // 'true' if records published
                                                // to 'stdout' in regular
                                                // (long) format, otherwise
                                                // short format is used

    bool                  d_publishInLocalTime; // 'true' if timestamps of
                                                // records are output in local
                                                // time, otherwise UTC time

    bool                  d_userFieldsLoggingFlag;
                                                // 'true' if user-defined
                                                // fields published
                                                // (!DEPRECATED!)

    bsl::string           d_stdoutLongFormat;   // initially set to default
                                                // long format for records
                                                // printed to 'stdout'; updated
                                                // by 'setLogFormat'

    bsl::string           d_stdoutShortFormat;  // default short format for
                                                // records printed to 'stdout'

    mutable bslmt::Mutex  d_mutex;              // serialize operations

    FileObserver2         d_fileObserver2;      // forward most operations to
                                                // this object

  private:
    // NOT IMPLEMENTED
    FileObserver(const FileObserver&);
    FileObserver& operator=(const FileObserver&);

  public:
    // TYPES
    typedef FileObserver2::OnFileRotationCallback OnFileRotationCallback;
        // 'OnFileRotationCallback' is an alias for a user-supplied callback
        // function that is invoked after the file observer attempts to rotate
        // its log file.  The callback takes two arguments: (1) an integer
        // status value where 0 indicates a new log file was successfully
        // created and a non-zero value indicates an error occurred during
        // rotation, and (2) a string that provides the name of the rotated log
        // file if the rotation was successful.  E.g.:
        //..
        //  void onLogFileRotation(int                rotationStatus,
        //                         const bsl::string& rotatedLogFileName);
        //..

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FileObserver, bslma::UsesBslmaAllocator);

    // CREATORS
    FileObserver();
    explicit FileObserver(bslma::Allocator *basicAllocator);
    explicit FileObserver(Severity::Level   stdoutThreshold,
                          bslma::Allocator *basicAllocator  = 0);
        // Create a file observer that publishes log records to 'stdout' if
        // their severity is at least as severe as the optionally specified
        // 'stdoutThreshold' level, and has file logging initially disabled.
        // If 'stdoutThreshold' is not specified, log records are published to
        // 'stdout' if their severity is at least as severe as
        // 'Severity::e_WARN'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that 'isPublishInLocalTimeEnabled'
        // returns 'false' following construction indicating that the timestamp
        // attribute of published records will be written in UTC time (see
        // 'enablePublishInLocalTime').  Also note that independent default
        // record formats are in effect for 'stdout' and file logging (see
        // 'setLogFormat').

    FileObserver(Severity::Level   stdoutThreshold,
                 bool              publishInLocalTime,
                 bslma::Allocator *basicAllocator = 0);
        // Create a file observer that publishes log records to 'stdout' if
        // their severity is at least as severe as the specified
        // 'stdoutThreshold' level, and has file logging initially disabled.
        // The timestamp attribute of published records is written in local
        // time if the specified 'publishInLocalTime' flag is 'true', and in
        // UTC time otherwise.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that following construction
        // independent default record formats are in effect for 'stdout' and
        // file logging (see 'setLogFormat').

    ~FileObserver();
        // Close the log file of this file observer if file logging is enabled,
        // and destroy this file observer.

    // MANIPULATORS
    void disableFileLogging();
        // Disable file logging for this file observer.  This method has no
        // effect if file logging is not enabled.  Note that records
        // subsequently received through the 'publish' method of this file
        // observer may still be logged to 'stdout' after calling this method.

    void disableLifetimeRotation();
        // Disable log file rotation based on a periodic time interval for this
        // file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.
        //
        // !DEPRECATED!: Use 'disableTimeIntervalRotation' instead.

    void disableTimeIntervalRotation();
        // Disable log file rotation based on a periodic time interval for this
        // file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.

    void disableSizeRotation();
        // Disable log file rotation based on log file size for this file
        // observer.  This method has no effect if rotation-on-size is not
        // enabled.

    void disableStdoutLoggingPrefix();
        // Disable this file observer from using the long output format when
        // logging to 'stdout'.  Henceforth, this file observer will use the
        // default short output format ("\n%s %f:%l %c %m %u\n") when logging
        // to 'stdout'.  This method has no effect if the long output format
        // for 'stdout' logging is not enabled.  Note that this method omits
        // the "%d %p:%t " prefix from the default long output format.

    void disableUserFieldsLogging();
        // Disable the logging of user-defined fields by this file observer.
        // This method has no effect if logging of user-defined fields is not
        // enabled, or if a format string other than the default one is in
        // effect.
        //
        // !DEPRECATED!: Use 'setLogFormat' instead.

    void disablePublishInLocalTime();
        // Disable publishing of the timestamp attribute of records in local
        // time by this file observer; henceforth, timestamps will be in UTC
        // time.  This method has no effect if publishing in local time is not
        // enabled.  Note that this method also affects log filenames (see {Log
        // Filename Patterns}).

    int enableFileLogging(const char *logFilenamePattern);
        // Enable logging of all records published to this file observer to a
        // file whose name is derived from the specified 'logFilenamePattern'.
        // Return 0 on success, a positive value if file logging is already
        // enabled (with no effect), and a negative value otherwise.  The
        // basename of 'logFilenamePattern' may contain '%'-escape sequences
        // that are interpreted as follows:
        //..
        //   %Y - current year   (4 digits with leading zeros)
        //   %M - current month  (2 digits with leading zeros)
        //   %D - current day    (2 digits with leading zeros)
        //   %h - current hour   (2 digits with leading zeros)
        //   %m - current minute (2 digits with leading zeros)
        //   %s - current second (2 digits with leading zeros)
        //   %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
        //   %p - process ID
        //..
        // Each time a log file is opened by this file observer (upon a
        // successful call to this method and following each log file
        // rotation), the name of the new log file is derived from
        // 'logFilenamePattern' by interpreting the above recognized '%'-escape
        // sequences.  If 'isPublishInLocalTimeEnabled' returns 'true', the
        // '%'-escape sequences related to time will be substituted with local
        // time values, and UTC time values otherwise.  See {Log Filename
        // Patterns}.

    int enableFileLogging(const char *logFilenamePattern,
                          bool        appendTimestampFlag);
        // Enable logging of all records published to this file observer to a
        // file whose name is derived from the specified 'logFilenamePattern'
        // and append a timestamp to the log filename if the specified
        // 'appendTimestampFlag' is 'true'.  Return 0 on success, a positive
        // value if file logging is already enabled (with no effect), and a
        // negative value otherwise.  If the 'appendTimestampFlag' is 'true'
        // and 'logFilenamePattern' does not contain any '%'-escape sequences,
        // this method behaves as if ".%T" is appended to 'logFilenamePattern'.
        //
        // !DEPRECATED!: Use 'enableFileLogging(logFilenamePattern)' instead
        // (use the ".%T" pattern to replicate 'true == appendTimestampFlag'
        // behavior).

    void enableStdoutLoggingPrefix();
        // Enable this file observer to use the long output format when logging
        // to 'stdout'.  Henceforth, this file observer will use the output
        // format for 'stdout' logging that was set by the most recent call to
        // 'setLogFormat', or the default long output format
        // ("\n%d %p:%t %s %f:%l %c %m %u\n") if 'setLogFormat' has not yet
        // been called.  This method has no effect if the long output format
        // for 'stdout' logging is already enabled.

    void enableUserFieldsLogging();
        // Enable the logging of user-defined fields by this file observer.
        // This method has no effect if logging of user-defined fields is
        // already enabled, or if a format string other than the default one is
        // in effect.
        //
        // !DEPRECATED!: Use 'setLogFormat' instead.

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this file observer.  This method has no effect if publishing
        // in local time is already enabled.  Note that this method also
        // affects log filenames (see {Log Filename Patterns}).

    void publish(const Record& record, const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by writing 'record' and 'context' to the current log file
        // if file logging is enabled for this file observer, and to 'stdout'
        // if the severity of 'record' is at least as severe as the value
        // returned by 'stdoutThreshold'.

    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context);
        // Process the record referenced by the specified 'record' shared
        // pointer having the specified publishing 'context' by writing the
        // record and 'context' to the current log file if file logging is
        // enabled for this file observer, and to 'stdout' if the severity of
        // 'record' is at least as severe as the value returned by
        // 'stdoutThreshold'.

    void releaseRecords();
        // Discard any shared references to 'Record' objects that were supplied
        // to the 'publish' method, and are held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared pointers must be released.

    void forceRotation();
        // Forcefully perform a log file rotation by this file observer.  Close
        // the current log file, rename the log file if necessary, and open a
        // new log file.  This method has no effect if file logging is not
        // enabled.  See {Rotated File Naming} for details on filenames of
        // rotated log files.

    void rotateOnLifetime(const bdlt::DatetimeInterval& timeInterval);
        // Set this file observer to perform a periodic log file rotation at
        // multiples of the specified 'timeInterval'.  The behavior is
        // undefined unless '0 < timeInterval.totalMilliseconds()'.  This rule
        // replaces any rotation-on-time-interval rule currently in effect.
        //
        // !DEPRECATED!: Use 'rotateOnTimeInterval' instead.

    void rotateOnSize(int size);
        // Set this file observer to perform log file rotation when the size of
        // the file exceeds the specified 'size' (in kilobytes).  This rule
        // replaces any rotation-on-size rule currently in effect.  The
        // behavior is undefined unless 'size > 0'.

    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval,
                              const bdlt::Datetime&         startTime);
        // Set this file observer to perform a periodic log file rotation at
        // multiples of the specified 'interval'.  Optionally specify a
        // 'startTime' indicating the datetime to use as the starting point for
        // computing the periodic rotation schedule.  If
        // 'isPublishInLocalTimeEnabled' is 'true', the 'startTime' is
        // interpreted as local time, and as a UTC time otherwise.  If
        // 'startTime' is not specified, the current time is used.  This rule
        // replaces any rotation-on-time-interval rule currently in effect.
        // The behavior is undefined unless '0 < interval.totalMilliseconds()'.
        // Note that 'startTime' may be a fixed time in the past; e.g., a
        // reference time of 'bdlt::Datetime(1, 1, 1)' and an interval of 24
        // hours would configure a periodic rotation at midnight each day.

    void setOnFileRotationCallback(
                             const OnFileRotationCallback& onRotationCallback);
        // Set the specified 'onRotationCallback' to be invoked after each time
        // this file observer attempts to perform a log file rotation.  The
        // behavior is undefined if the supplied function calls either
        // 'setOnFileRotationCallback', 'forceRotation', or 'publish' on this
        // file observer (i.e., the supplied callback should *not* attempt to
        // write to the 'ball' log).

    void setStdoutThreshold(Severity::Level stdoutThreshold);
        // Set the minimum severity of records logged to 'stdout' by this file
        // observer to the specified 'stdoutThreshold' level.  Note that if the
        // value of 'stdoutThreshold' is 'Severity::e_OFF', logging to 'stdout'
        // is disabled.

    void setLogFormat(const char *logFileFormat, const char *stdoutFormat);
        // Set the format specifications for log records written to the log
        // file and to 'stdout' to the specified 'logFileFormat' and
        // 'stdoutFormat', respectively.  If the default short output format is
        // currently in effect for logging to 'stdout', this method has the
        // effect of calling 'enableStdoutLoggingPrefix' (see that method and
        // 'disableStdoutLoggingPrefix').  See {Log Record Formatting} for
        // details on the syntax of format specifications.  Note that default
        // formats are in effect following construction until this method is
        // called ("\n%d %p:%t %s %f:%l %c %m %u\n" for both file and 'stdout'
        // logging).  Also note that the observer emits newline characters at
        // the beginning and at the end of a log record by default, so the user
        // needs to add them explicitly to the format string to preserve this
        // behavior.

    void suppressUniqueFileNameOnRotation(bool suppress);
        // Suppress generating a unique log file name upon rotation if the
        // specified 'suppress' is 'true', and generate a unique filename
        // otherwise.  See {Rotated File Naming} for details.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the memory allocator used by this object.

    void getLogFormat(const char **logFileFormat,
                      const char **stdoutFormat) const;
        // Load the format specification for log records written by this file
        // observer to the log file into the specified '*logFileFormat' address
        // and the format specification for log records written to 'stdout'
        // into the specified '*stdoutFormat' address.  See {Log Record
        // Formatting} for details on the syntax of format specifications.

    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
    bool isFileLoggingEnabled(std::string *result) const;
        // Return 'true' if file logging is enabled for this file observer, and
        // 'false' otherwise.  Load the optionally specified 'result' with the
        // name of the current log file if file logging is enabled, and leave
        // 'result' unmodified otherwise.  Note that records received through
        // the 'publish' method of this file observer may still be logged to
        // 'stdout' when this method returns 'false'.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
    bool isFileLoggingEnabled(std::pmr::string *result) const;
        // Return 'true' if file logging is enabled for this file observer, and
        // 'false' otherwise.  Load the specified 'result' with the name of the
        // current log file if file logging is enabled, and leave 'result'
        // unmodified otherwise.  Note that records received through the
        // 'publish' method of this file observer may still be logged to
        // 'stdout' when this method returns 'false'.
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING

    bool isStdoutLoggingPrefixEnabled() const;
        // Return 'true' if this file observer uses the long output format when
        // writing to 'stdout', and 'false' otherwise (in which case the
        // default short output format is used).  See
        // 'enableStdoutLoggingPrefix' and 'disableStdoutLoggingPrefix'.

    bool isUserFieldsLoggingEnabled() const;
        // Return 'true' if the logging of user-defined fields is enabled for
        // this file observer, and 'false' otherwise.
        //
        // !DEPRECATED!: Do not use.

    bool isPublishInLocalTimeEnabled() const;
        // Return 'true' if this file observer writes the timestamp attribute
        // of records that it publishes in local time, and 'false' otherwise
        // (in which case timestamps are written in UTC time).  Note that the
        // value returned by this method also affects log filenames (see {Log
        // Filename Patterns}).

    bool isSuppressUniqueFileNameOnRotation() const;
        // Return 'true' if the log filename uniqueness check on rotation is
        // suppressed, and false otherwise.

    bdlt::DatetimeInterval localTimeOffset() const;
        // Return the difference between the local time and UTC time in effect
        // when this file observer was constructed.  Note that this value
        // remains unchanged during the lifetime of this object and therefore
        // may become incorrect when the difference between the local time and
        // UTC time changes (e.g., when transitioning into or out of daylight
        // savings time).
        //
        // !DEPRECATED!: Use 'bdlt::LocalTimeOffset' instead.

    bdlt::DatetimeInterval rotationLifetime() const;
        // Return the lifetime of the log file that will trigger a file
        // rotation by this file observer if rotation-on-lifetime is in effect,
        // and a 0 time interval otherwise.

    int rotationSize() const;
        // Return the size (in kilobytes) of the log file that will trigger a
        // file rotation by this file observer if rotation-on-size is in
        // effect, and 0 otherwise.

    Severity::Level stdoutThreshold() const;
        // Return the minimum severity of records that will be logged to
        // 'stdout' by this file observer.  Note that records with a threshold
        // less severe than 'stdoutThreshold()' may still be output to the log
        // file if file logging is enabled.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // ------------------
                          // class FileObserver
                          // ------------------

// MANIPULATORS
inline
void FileObserver::disableFileLogging()
{
    d_fileObserver2.disableFileLogging();
}

inline
void FileObserver::disableLifetimeRotation()
{
    d_fileObserver2.disableTimeIntervalRotation();
}

inline
void FileObserver::disableSizeRotation()
{
    d_fileObserver2.disableSizeRotation();
}

inline
void FileObserver::disableTimeIntervalRotation()
{
    d_fileObserver2.disableTimeIntervalRotation();
}

inline
int FileObserver::enableFileLogging(const char *logFilenamePattern)
{
    return d_fileObserver2.enableFileLogging(logFilenamePattern);
}

inline
int FileObserver::enableFileLogging(const char *logFilenamePattern,
                                    bool        appendTimestampFlag)
{
    return d_fileObserver2.enableFileLogging(logFilenamePattern,
                                             appendTimestampFlag);
}

inline
void FileObserver::forceRotation()
{
    d_fileObserver2.forceRotation();
}

inline
void FileObserver::publish(const bsl::shared_ptr<const Record>& record,
                           const Context&                       context)
{
    publish(*record, context);
}

inline
void FileObserver::releaseRecords()
{
}

inline
void FileObserver::rotateOnLifetime(const bdlt::DatetimeInterval& timeInterval)
{
    d_fileObserver2.rotateOnTimeInterval(timeInterval);
}

inline
void FileObserver::rotateOnSize(int size)
{
    d_fileObserver2.rotateOnSize(size);
}

inline
void FileObserver::rotateOnTimeInterval(const bdlt::DatetimeInterval& interval)
{
    d_fileObserver2.rotateOnTimeInterval(interval);
}

inline
void FileObserver::rotateOnTimeInterval(
                                       const bdlt::DatetimeInterval& interval,
                                       const bdlt::Datetime&         startTime)
{
    d_fileObserver2.rotateOnTimeInterval(interval, startTime);
}

inline
void FileObserver::setOnFileRotationCallback(
                              const OnFileRotationCallback& onRotationCallback)
{
    d_fileObserver2.setOnFileRotationCallback(onRotationCallback);
}

inline
void FileObserver::suppressUniqueFileNameOnRotation(bool suppress)
{
    d_fileObserver2.suppressUniqueFileNameOnRotation(suppress);
}

// ACCESSORS
inline
bool FileObserver::isFileLoggingEnabled() const
{
    return d_fileObserver2.isFileLoggingEnabled();
}

inline
bool FileObserver::isFileLoggingEnabled(bsl::string *result) const
{
    return d_fileObserver2.isFileLoggingEnabled(result);
}

inline
bool FileObserver::isFileLoggingEnabled(std::string *result) const
{
    return d_fileObserver2.isFileLoggingEnabled(result);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
inline
bool FileObserver::isFileLoggingEnabled(std::pmr::string *result) const
{
    return d_fileObserver2.isFileLoggingEnabled(result);
}
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING

inline
bool FileObserver::isSuppressUniqueFileNameOnRotation() const
{
    return d_fileObserver2.isSuppressUniqueFileNameOnRotation();
}

inline
bdlt::DatetimeInterval FileObserver::localTimeOffset() const
{
    return d_fileObserver2.localTimeOffset();
}

inline
bdlt::DatetimeInterval FileObserver::rotationLifetime() const
{
    return d_fileObserver2.rotationLifetime();
}

inline
int FileObserver::rotationSize() const
{
    return d_fileObserver2.rotationSize();
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
