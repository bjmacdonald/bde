// bcemt_muteximpl_win32.t.cpp                                        -*-C++-*-
#include <bcemt_muteximpl_win32.h>

#include <bces_atomictypes.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

#ifdef BCES_PLATFORM__WIN32_THREADS

#include <windows.h>

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";   \
                aSsErT(1, #X, __LINE__); } }
#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J <<         \
                        "\t" << #K << ": " << K << "\n";                      \
                aSsErT(1, #X, __LINE__); } }
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o line feed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bcemt_MutexImpl<bces_Platform::Win32Threads> Obj;

void My_DoLock(Obj::NativeType* lock) {
    EnterCriticalSection(lock);
}

void My_DoUnlock(Obj::NativeType* lock) {
    LeaveCriticalSection(lock);
}

extern "C" {
   typedef void *(*ThreadFunction)(void *);
}

void My_Sleep(int ms)
{
    Sleep(ms);
}

#include <process.h>

struct StartArgs {
    ThreadFunction d_function;
    void* d_arg;
};

static unsigned _stdcall ThreadEntry (void* arg)
{
    StartArgs* args = (StartArgs*)arg;

    Sleep(100); // 100 ms - for our testing, let's just not deal with the
                // "invalid handle from too-quickly-executing thread" issue

    args->d_function(args->d_arg);
    free(arg);

    return 1;
}

void My_CreateDetachedThread(ThreadFunction function,
                             void* userData) {
    StartArgs *args = (StartArgs*)malloc(sizeof(StartArgs));
    args->d_function = function;
    args->d_arg = userData;

    unsigned dummy;
    HANDLE h = (HANDLE)_beginthreadex(0, 1000000, ThreadEntry,
                                      args, 0, &dummy);
    CloseHandle(h);
}

struct ThreadInfo {
    Obj *d_lock;
    bces_AtomicInt d_retval;
    bces_AtomicInt d_retvalSet;
};

extern "C" void* MyThread(void* arg_p) {
    ThreadInfo* arg = (ThreadInfo*)arg_p;

    arg->d_retval = arg->d_lock->tryLock();
    if (0 == arg->d_retval) {
        arg->d_lock->unlock();
    }
    arg->d_retvalSet = 1;
    return arg_p;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
    case 2: {
        // ------------------------------------------------------------------
        // Native mutex test
        //
        // Test that the native mutex is exposed properly by performing native
        // operations on it and checking the state of the Mutex object.
        // ------------------------------------------------------------------
        enum {
            MAX_SLEEP_CYCLES = 1000,
            SLEEP_MS = 100
        };

        if (verbose) {
            cout << "Native mutex test" << endl;
            cout << "=================" << endl;
        }
        Obj lock;

        Obj::NativeType* native = &lock.nativeMutex();

        My_DoLock(native);

        ThreadInfo args;
        args.d_lock = &lock;

        args.d_retval = 0;
        args.d_retvalSet = 0;
        My_CreateDetachedThread(MyThread, &args);
        for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
             ++i) {
            My_Sleep(SLEEP_MS);
        }
        ASSERT(args.d_retvalSet);
        ASSERT(0 != args.d_retval); // should fail
        if (verbose) {
            P(args.d_retval);
        }

        My_DoUnlock(native);

    } break;

    case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy a mutex.  Lock and verify that tryLock fails;
        // unlock and verify that tryLock succeeds.
        // ------------------------------------------------------------------
        if (verbose) {
            cout << "BREATHING TEST" << endl;
            cout << "==============" << endl;
        }
        enum {
            MAX_SLEEP_CYCLES = 1000,
            SLEEP_MS = 100
        };

        {
            Obj lock;
            lock.lock();

            ThreadInfo args;
            args.d_lock = &lock;

            args.d_retval = 0;
            args.d_retvalSet = 0;
            My_CreateDetachedThread(MyThread, &args);
            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail
            if (verbose) {
                P(args.d_retval);
            }

            lock.unlock();

            args.d_retval = 0;
            args.d_retvalSet = 0;
            My_CreateDetachedThread(MyThread, &args);
            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 == args.d_retval); // should succeed
            if (verbose) {
                P(args.d_retval);
            }

        }
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

#else

int main()
{
    return -1;
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
