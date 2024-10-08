// bdlsb_fixedmeminput.h                                              -*-C++-*-
#ifndef INCLUDED_BDLSB_FIXEDMEMINPUT
#define INCLUDED_BDLSB_FIXEDMEMINPUT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a basic input stream buffer using a client buffer.
//
//@CLASSES:
//   bdlsb::FixedMemInput: basic input stream buffer using client memory
//
//@SEE_ALSO: bdlsb_fixedmeminstreambuf
//
//@DESCRIPTION: This component provides a mechanism, `bdlsb::FixedMemInput`,
// that implements the input portion of the `bsl::basic_streambuf` protocol
// using a client-supplied memory buffer.  Method names necessarily correspond
// to the protocol-specified method names.  Clients supply the character buffer
// at stream buffer construction, and can later reinitialize the stream buffer
// with a different character buffer by calling the `pubsetbuf` method.  This
// component provides none of the output-related functionality of
// `bsl::basic_streambuf` nor does it use locales in any way.  The only
// difference between this component and `bdlsb::FixedMemInStreamBuf` is that
// the class `bdlsb::FixedMemInput` does *not* derive from a `bsl::streambuf`,
// and is generally more efficient (at initialization and due to the lack of
// virtual functions).  It is especially designed for streaming a very small
// amount of information from a fixed-length buffer using a
// `bslx_genericinstream`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `bdlsb::FixedMemInput`
/// - - - - - - - - - - - - - - - - - - - - - - -
// The `bdlsb::FixedMemInput` class is intended to be used as a template
// parameter to the `bslx::GenericInStream` class.  Such specialization
// provides user with performance efficient way to unexternalize BDEX encoded
// data from an existing character buffer.
//
// See the `bslx_genericinstream` component usage example for a more practical
// example of using `bslx` streams.
//
// This example demonstrates instantiating a template, `bslx::GenericInStream`,
// on a `bdlsb::FixedMemInput` object and using the `bslx::GenericInStream`
// object to stream in some data.
//
// First, create `bslx::ByteOutStream` `outStream` and externalize some user
// data to it.  Note that this code only prepares the character buffer that is
// used to illustrate the purpose of the `bdlsb::FixedMemInput` class.
// ```
// bslx::ByteOutStream outStream(20131127);
//
// unsigned int MAGIC = 0x1812;
//
// outStream.putUint32(MAGIC);
// outStream.putInt32(83);
// outStream.putString(bsl::string("test"));
// assert(outStream.isValid());
// ```
// Next, create a `bdlsb::FixedMemInput` stream buffer initialized with the
// buffer from the `bslx::ByteOutStream` object `outStream`:
// ```
// bdlsb::FixedMemInput streamBuffer(outStream.data(), outStream.length());
// ```
// Then, create the `bslx::GenericInStream` stream parameterized with
// `bdlsb::FixedMemInput`:
// ```
// bslx::GenericInStream<bdlsb::FixedMemInput>  inStream(&streamBuffer);
// ```
// Now, use resulting `inStream` to unexternalize user data:
// ```
// unsigned int  magic = 0;
// int           key;
// bsl::string   value;
//
// inStream.getUint32(magic);
// inStream.getInt32(key);
// inStream.getString(value);
// assert(inStream.isValid());
// ```
// Finally, verify that the data from the supplied buffer was unexternalized
// correctly:
// ```
// assert(MAGIC  == magic);
// assert(83     == key);
// assert("test" == value);
// ```

#include <bdlscm_version.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ios.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlsb {

                        // ===================
                        // class FixedMemInput
                        // ===================

/// This class, like `bdlsb::FixedMemInStreamBuf`, implements the input
/// functionality of the `basic_streambuf` interface, using client-supplied
/// `char *` memory.  It has an interface identical to
/// `bdlsb::FixedMemInStreamBuf` but does *not* inherit from
/// `bsl::streambuf`.  This implementation is advantageous for performance
/// reasons, as the overhead of the initialization and virtual function
/// calls of a `bsl::streambuf` can be undesirable.  It is especially
/// designed for streaming a very small amount of information from a
/// fixed-length buffer using a `bslx_genericinstream` when the number of
/// characters read from the input is guaranteed not to exceed the length of
/// the buffer.  Note that this class is not designed to be derived from.
class FixedMemInput {

  public:
    // TYPES
    typedef char                    char_type;
    typedef bsl::char_traits<char>  traits_type;
    typedef traits_type::int_type   int_type;
    typedef traits_type::pos_type   pos_type;
    typedef traits_type::off_type   off_type;

  private:
    // PRIVATE DATA MEMBERS
    const char      *d_buffer_p;      // buffer (held, not owned)
    bsl::size_t      d_bufferSize;    // buffer size (not length of stream)
    bsl::size_t      d_pos;           // current read position

  private:
    // NOT IMPLEMENTED
    FixedMemInput(const FixedMemInput&);
    FixedMemInput& operator=(const FixedMemInput&);

  public:
    // CREATORS

    /// Create a `FixedMemInput` using the specified `buffer` of the
    /// specified `length`.  The position indicator is set to the beginning
    /// of the `buffer`.  The behavior is undefined unless
    /// `buffer != 0 && length > 0` or `length == 0`.  Note that `buffer` is
    /// held but not owned.
    FixedMemInput(const char *buffer, bsl::size_t length);

    /// Destroy this stream buffer.
    //! ~FixedMemInput() = default;

    // MANIPULATORS

    /// Return the number of characters available from the current read
    /// position in this stream buffer.
    bsl::streamsize in_avail();

                             // *** 27.5.2.2.2 buffer and positioning: ***

    /// Reinitialize this stream buffer to use the specified character `buffer`
    /// having the specified `length`.  Return a pointer providing modifiable
    /// access to this stream buffer.  The behaviour is undefined unless
    /// `buffer != 0 && length > 0` or `length == 0`.  Upon reinitialization
    /// for use of the new  buffer, the position indicator is set to the
    /// beginning of the `buffer`.  Note that `buffer` is held but not owned.
    FixedMemInput *pubsetbuf(const char *buffer, bsl::streamsize  length);

    /// Set the position indicator to the relative specified `offset` from the
    /// base position indicated by the specified `way` and return the resulting
    /// absolute position on success or pos_type(-1) on failure.  Optionally
    /// specify `which` area of the stream buffer.  The seek operation will
    /// fail if `which` does not include the flag `bsl::ios_base::in` or if the
    /// resulting absolute position is less than zero or greater than the value
    /// returned by `length`.
    pos_type pubseekoff(off_type                offset,
                        bsl::ios_base::seekdir  way,
                        bsl::ios_base::openmode which = bsl::ios_base::in);

    /// Set the position indicator to the specified `position` and return the
    /// resulting absolute position on success or pos_type(-1) on failure.
    /// Optionally specify `which` area of the stream buffer.  The `seekpos`
    /// operation will fail if `which` does not include the flag
    /// `bsl::ios_base::in` or if position is less then zero or greater than
    /// the value returned by `length`.
    pos_type pubseekpos(pos_type                position,
                        bsl::ios_base::openmode which = bsl::ios_base::in);

                             // *** 27.5.2.2.3 Get area: ***

    /// Return the character at the current read position from this buffer, or
    /// `traits_type::eof()` if the end of the buffer is reached and advance
    /// read position indicator.
    int_type sbumpc();

    /// Return the character at the current read position from this buffer, or
    /// `traits_type::eof()` if the end of the buffer is reached.
    int_type sgetc();

    /// Read the specified `length` characters to the specified `destination`.
    /// Return the number of characters successfully read from this buffer,
    /// which is either equal to the `length` parameter or equal to the
    /// distance from the current read position to the end of the input buffer,
    /// whichever is smaller, and move the read cursor position by this amount.
    /// The behavior is undefined unless '0 <= length'.
    bsl::streamsize sgetn(char_type *destination, bsl::streamsize length);

    /// Advance the current read position and return the character at the
    /// resulting position from this buffer, or `traits_type::eof()` if the end
    /// of the buffer is reached.
    int_type snextc();

                             // *** 27.5.2.2.4 Putback: ***

    /// Move the current read position back one character if the current read
    /// position is not at the beginning of the buffer and the previous
    /// position contains the specified character `c`, and return that
    /// character.  Otherwise, return `traits_type::eof()` and do not move the
    /// current read position.
    int_type sputbackc(char c);

    /// Move the current read position back one character if the current read
    /// position is not at the beginning of the buffer, and return the
    /// character at the resulting current read position from this buffer.
    /// Return `traits_type::eof()` otherwise.
    int_type sungetc();

    // ACCESSORS

    /// Return the size for the buffer held by this buffer, in bytes, supplied
    /// at construction.
    bsl::size_t capacity() const;

    /// Return the address of the non-modifiable character buffer held by this
    /// stream buffer.
    const char *data() const;

    /// Return the number of characters that can be successfully read from this
    /// stream buffer before reading `traits_type::eof()` -- i.e., the number
    /// of characters between the current read position and the end of this
    /// buffer.
    bsl::size_t length() const;
};

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                        // -------------------
                        // class FixedMemInput
                        // -------------------

// CREATORS
inline
FixedMemInput::FixedMemInput(const char *buffer, bsl::size_t length)
: d_buffer_p(const_cast<char *>(buffer))
, d_bufferSize(length)
, d_pos(0)
{
    BSLS_ASSERT(buffer || 0 == length);
}

// MANIPULATORS
inline
bsl::streamsize FixedMemInput::in_avail()
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_pos == d_bufferSize)) {
        return bsl::streamsize(-1);                                   // RETURN
    }
    return static_cast<bsl::streamsize>(d_bufferSize - d_pos);
}

inline
FixedMemInput *FixedMemInput::pubsetbuf(const char      *buffer,
                                        bsl::streamsize  length)

{
    BSLS_ASSERT(buffer || 0 == length);
    BSLS_ASSERT(0 <= length);

    d_buffer_p   = buffer;
    d_bufferSize = static_cast<bsl::size_t>(length);
    d_pos        = 0;
    return this;
}

inline
FixedMemInput::int_type FixedMemInput::sbumpc()
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_pos >= d_bufferSize)) {
        return traits_type::eof();                                    // RETURN
    }
    const int_type i = traits_type::to_int_type(d_buffer_p[d_pos]);
    d_pos += 1;
    return i;
}

inline
FixedMemInput::int_type FixedMemInput::sgetc()
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_pos >= d_bufferSize)) {
        return traits_type::eof();                                    // RETURN
    }
    //return traits_type::to_int_type(d_buffer_p[static_cast<IntPtr>(d_pos)]);
    return traits_type::to_int_type(d_buffer_p[d_pos]);
}

inline
bsl::streamsize
FixedMemInput::sgetn(char *destination, bsl::streamsize length)
{
    BSLS_ASSERT(0 <= length);

    const bsl::size_t current = d_pos;
    d_pos += static_cast<bsl::size_t>(length);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY((d_pos > d_bufferSize) ||
                                              (d_pos < current) )) {
        d_pos    = d_bufferSize;
        length = static_cast<bsl::streamsize>(d_bufferSize - current);
    }
    bsl::memcpy(destination, d_buffer_p+current, length);
    return length;
}

inline
FixedMemInput::int_type FixedMemInput::snextc()
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_pos >= d_bufferSize)) {
        return traits_type::eof();                                    // RETURN
    }
    d_pos += 1;
    return sgetc();
}

inline
FixedMemInput::int_type FixedMemInput::sputbackc(char c)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == d_pos)
     || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(c != d_buffer_p[d_pos-1])) {
        return traits_type::eof();                                    // RETURN
    }
    d_pos -= 1;
    return traits_type::to_int_type(c);
}

inline
FixedMemInput::int_type FixedMemInput::sungetc()
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == d_pos)) {
        return traits_type::eof();                                    // RETURN
    }
    d_pos -= 1;
    return traits_type::to_int_type(d_buffer_p[d_pos]);
}

// ACCESSORS
inline
bsl::size_t FixedMemInput::capacity() const
{
    return d_bufferSize;
}

inline
const char *FixedMemInput::data() const
{
    return d_buffer_p;
}

inline
bsl::size_t FixedMemInput::length() const
{
    return (d_bufferSize - d_pos);
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

