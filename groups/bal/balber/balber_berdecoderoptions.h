// balber_berdecoderoptions.h                                         -*-C++-*-
#ifndef INCLUDED_BALBER_BERDECODEROPTIONS
#define INCLUDED_BALBER_BERDECODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for specifying BER decoding options.
//
//@CLASSES:
//  balber::BerDecoderOptions: options for decoding objects in BDE format
//
//@DESCRIPTION: Schema of options records for `balber` codecs

#include <balscm_version.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_printmethods.h>

#include <bsls_assert.h>

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace balber {

                          // =======================
                          // class BerDecoderOptions
                          // =======================

/// BER decoding options
class BerDecoderOptions {

  private:
    int   d_maxDepth;            // maximum recursion depth
    int   d_traceLevel;          // trace (verbosity) level
    int   d_maxSequenceSize;     // maximum sequence size
    bool  d_skipUnknownElements; // if 'true', skip unknown elements
    bool  d_defaultEmptyStrings; // if 'true', decode empty strings as their
                                 // default values, if any

  public:
    // TYPES
    enum {
        k_NUM_ATTRIBUTES = 5  // the number of attributes in this class

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , NUM_ATTRIBUTES = k_NUM_ATTRIBUTES
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum {
        /// index for "MaxDepth" attribute
        e_ATTRIBUTE_INDEX_MAX_DEPTH             = 0
      , e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS = 1
            // index for "SkipUnknownElements" attribute
      , e_ATTRIBUTE_INDEX_TRACE_LEVEL           = 2
            // index for "TraceLevel" attribute
      , e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE     = 3
            // index for "MaxSequenceSize" attribute
      , e_ATTRIBUTE_INDEX_DEFAULT_EMPTY_STRINGS = 4
            // index for "DefaultEmptyStrings" attribute

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ATTRIBUTE_INDEX_MAX_DEPTH             =
                                        e_ATTRIBUTE_INDEX_MAX_DEPTH
      , ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS =
                                        e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS
      , ATTRIBUTE_INDEX_TRACE_LEVEL           =
                                        e_ATTRIBUTE_INDEX_TRACE_LEVEL
      , ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE     =
                                        e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE
      , ATTRIBUTE_INDEX_DEFAULT_EMPTY_STRINGS =
                                        e_ATTRIBUTE_INDEX_DEFAULT_EMPTY_STRINGS
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum {
        /// id for `MaxDepth` attribute
        e_ATTRIBUTE_ID_MAX_DEPTH             = 0
      , e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS = 1
            // id for 'SkipUnknownElements' attribute
      , e_ATTRIBUTE_ID_TRACE_LEVEL           = 2
            // id for 'TraceLevel' attribute
      , e_ATTRIBUTE_ID_MAX_SEQUENCE_SIZE     = 3
            // id for 'MaxSequenceSize' attribute
      , e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS = 4
            // id for 'DefaultEmptyStrnigs" attribute

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ATTRIBUTE_ID_MAX_DEPTH             = e_ATTRIBUTE_ID_MAX_DEPTH
      , ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS =
                                           e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS
      , ATTRIBUTE_ID_TRACE_LEVEL           = e_ATTRIBUTE_ID_TRACE_LEVEL
      , ATTRIBUTE_ID_MAX_SEQUENCE_SIZE     = e_ATTRIBUTE_ID_MAX_SEQUENCE_SIZE
      , ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS =
                                           e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

  public:
    // CONSTANTS

    /// the name of this class (i.e., "BerDecoderOptions")
    static const char CLASS_NAME[];

    /// default value of `MaxDepth` attribute
    static const int DEFAULT_MAX_DEPTH;

    /// default value of `SkipUnknownElements` attribute
    static const bool DEFAULT_SKIP_UNKNOWN_ELEMENTS;

    /// default value of `TraceLevel` attribute
    static const int DEFAULT_TRACE_LEVEL;

    /// default value of `MaxSequenceSize` attribute
    static const int DEFAULT_MAX_SEQUENCE_SIZE;

    /// default value of `DefaultEmptyStrings` attribute
    static const bool DEFAULT_DEFAULT_EMPTY_STRINGS;

    /// attribute information for each attribute
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return the maximum valid BDEX format version, as indicated by the
    /// specified `versionSelector`, to be passed to the `bdexStreamOut`
    /// method.  Note that it is highly recommended that `versionSelector`
    /// be formatted as "YYYYMMDD", a date representation.  Also note that
    /// `versionSelector` should be a *compile*-time-chosen value that
    /// selects a format version supported by both externalizer and
    /// unexternalizer.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    static int maxSupportedBdexVersion(int versionSelector);

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    /// Return the most current BDEX streaming version number supported by
    /// this class.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    static int maxSupportedBdexVersion();

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);

    // CREATORS

    /// Create an object of type `BerDecoderOptions` having the default
    /// value.
    BerDecoderOptions();

    /// Create a 'BderDecoderOptons' object having the same value as the
    /// specified 'original'.
    //! BerDecoderOptions(const BerDecoderOptions& original) = default;

    /// Destroy this object.
    //! ~BerDecoderOptions() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified 'rhs' object.
    //! BerDecoderOptions& operator=(const BerDecoderOptions& rhs) = default;

    /// Assign to this object the value read from the specified input
    /// `stream` using the specified `version` format, and return a
    /// reference to `stream`.  If `stream` is initially invalid, this
    /// operation has no effect.  If `version` is not supported, this object
    /// is unaltered and `stream` is invalidated, but otherwise unmodified.
    /// If `version` is supported but `stream` becomes invalid during this
    /// operation, this object has an undefined, but valid, state.  Note
    /// that no version is read from `stream`.  See the `bslx` package-level
    /// documentation for more information on BDEX streaming of
    /// value-semantic types and containers.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `manipulator` (i.e., the invocation that terminated
    /// the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `id`, supplying
    /// `manipulator` with the corresponding attribute information
    /// structure.  Return the value returned from the invocation of
    /// `manipulator` if `id` identifies an attribute of this class, and -1
    /// otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Set the `MaxDepth` attribute of this object to the specified
    /// `value`.
    void setMaxDepth(int value);

    /// Set the `SkipUnknownElements` attribute of this object to the
    /// specified `value`.
    void setSkipUnknownElements(bool value);

    /// Set the `TraceLevel` attribute of this object to the specified
    /// `value`.
    void setTraceLevel(int value);

    /// Set the `MaxSequenceSize` attribute of this object to the specified
    /// `value`.
    void setMaxSequenceSize(int value);

    /// Set the `DefaultEmptyStrings` attribute of this object to the
    /// specified `value`.
    void setDefaultEmptyStrings(bool value);

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Write the value of this object, using the specified `version`
    /// format, to the specified output `stream`, and return a reference to
    /// `stream`.  If `stream` is initially invalid, this operation has no
    /// effect.  If `version` is not supported, `stream` is invalidated, but
    /// otherwise unmodified.  Note that `version` is not written to
    /// `stream`.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor` with
    /// the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the last
    /// invocation of `accessor` (i.e., the invocation that terminated the
    /// sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference to the non-modifiable "MaxDepth" attribute of
    /// this object.
    const int& maxDepth() const;

    /// Return a reference to the non-modifiable "SkipUnknownElements"
    /// attribute of this object.
    const bool& skipUnknownElements() const;

    /// Return a reference to the non-modifiable "TraceLevel" attribute of
    /// this object.
    const int& traceLevel() const;

    /// Return a reference to the non-modifiable "MaxSequenceSize" attribute
    /// of this object.
    const int& maxSequenceSize() const;

    /// Return a reference to the non-modifiable "DefaultEmptyStrings"
    /// attribute of this object.
    const bool& defaultEmptyStrings() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const BerDecoderOptions& lhs,
                const BerDecoderOptions& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const BerDecoderOptions& lhs,
                const BerDecoderOptions& rhs);

/// Format the specified `rhs` to the specified output `stream` and return a
/// reference providing modifiable access to `stream`.
inline
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const BerDecoderOptions& rhs);

}  // close package namespace


// TRAITS
BDLAT_DECL_SEQUENCE_TRAITS(balber::BerDecoderOptions)

namespace balber {

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class BerDecoderOptions
                          // -----------------------

// CLASS METHODS
inline
int BerDecoderOptions::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;  // versions start at 1
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int BerDecoderOptions::maxSupportedBdexVersion()
{
    return maxSupportedBdexVersion(0);
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

// CREATORS
inline
BerDecoderOptions::BerDecoderOptions()
: d_maxDepth(DEFAULT_MAX_DEPTH)
, d_traceLevel(DEFAULT_TRACE_LEVEL)
, d_maxSequenceSize(DEFAULT_MAX_SEQUENCE_SIZE)
, d_skipUnknownElements(DEFAULT_SKIP_UNKNOWN_ELEMENTS)
, d_defaultEmptyStrings(DEFAULT_DEFAULT_EMPTY_STRINGS)
{
}

// MANIPULATORS
template <class STREAM>
inline
STREAM& BerDecoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bslx::InStreamFunctions::bdexStreamIn(stream, d_maxDepth, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream,
                                                  d_skipUnknownElements, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_traceLevel, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream,
                                                  d_maxSequenceSize, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream,
                                                  d_defaultEmptyStrings, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void BerDecoderOptions::reset()
{
    d_maxDepth            = DEFAULT_MAX_DEPTH;
    d_traceLevel          = DEFAULT_TRACE_LEVEL;
    d_maxSequenceSize     = DEFAULT_MAX_SEQUENCE_SIZE;
    d_skipUnknownElements = DEFAULT_SKIP_UNKNOWN_ELEMENTS;
    d_defaultEmptyStrings = DEFAULT_DEFAULT_EMPTY_STRINGS;
}

template <class MANIPULATOR>
inline
int BerDecoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxDepth,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                &d_skipUnknownElements,
                ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_traceLevel,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                    &d_maxSequenceSize,
                    ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                &d_defaultEmptyStrings,
                ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DEFAULT_EMPTY_STRINGS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int BerDecoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth,
                           ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return manipulator(
               &d_skipUnknownElements,
               ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TRACE_LEVEL: {
        return manipulator(
                          &d_traceLevel,
                          ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_SEQUENCE_SIZE: {
        return manipulator(
                    &d_maxSequenceSize,
                    ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS: {
        return manipulator(
               &d_defaultEmptyStrings,
               ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DEFAULT_EMPTY_STRINGS]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int BerDecoderOptions::manipulateAttribute(MANIPULATOR&  manipulator,
                                           const char   *name,
                                           int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name,
                                                                   nameLength);
    if (!attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void BerDecoderOptions::setMaxDepth(int value)
{
    d_maxDepth = value;
}

inline
void BerDecoderOptions::setSkipUnknownElements(bool value)
{
    d_skipUnknownElements = value;
}

inline
void BerDecoderOptions::setTraceLevel(int value)
{
    d_traceLevel = value;
}

inline
void BerDecoderOptions::setMaxSequenceSize(int value)
{
    d_maxSequenceSize = value;
}

inline
void BerDecoderOptions::setDefaultEmptyStrings(bool value)
{
    d_defaultEmptyStrings = value;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& BerDecoderOptions::bdexStreamOut(STREAM& stream,
                                         int     version) const
{
    switch (version) {
      case 1: {
        bslx::OutStreamFunctions::bdexStreamOut(stream, d_maxDepth, 1);
        bslx::OutStreamFunctions::bdexStreamOut(
            stream, d_skipUnknownElements, 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, d_traceLevel, 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, d_maxSequenceSize, 1);
        bslx::OutStreamFunctions::bdexStreamOut(
            stream, d_defaultEmptyStrings, 1);
      } break;
      default: {
        stream.invalidate();
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int BerDecoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxDepth,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                d_skipUnknownElements,
                ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_traceLevel,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_maxSequenceSize,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                d_defaultEmptyStrings,
                ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DEFAULT_EMPTY_STRINGS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int BerDecoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth,
                        ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return accessor(
                d_skipUnknownElements,
                ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TRACE_LEVEL: {
        return accessor(d_traceLevel,
                        ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_SEQUENCE_SIZE: {
        return accessor(
                    d_maxSequenceSize,
                    ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS: {
        return accessor(
                d_defaultEmptyStrings,
                ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DEFAULT_EMPTY_STRINGS]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int BerDecoderOptions::accessAttribute(ACCESSOR&   accessor,
                                       const char *name,
                                       int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name,
                                                                   nameLength);
    if (!attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& BerDecoderOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
const bool& BerDecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}

inline
const int& BerDecoderOptions::traceLevel() const
{
    return d_traceLevel;
}

inline
const int& BerDecoderOptions::maxSequenceSize() const
{
    return d_maxSequenceSize;
}

inline
const bool& BerDecoderOptions::defaultEmptyStrings() const
{
    return d_defaultEmptyStrings;
}

}  // close package namespace

// FREE FUNCTIONS
inline
bool balber::operator==(const BerDecoderOptions& lhs,
                        const BerDecoderOptions& rhs)
{
    return  lhs.maxDepth()            == rhs.maxDepth()
         && lhs.skipUnknownElements() == rhs.skipUnknownElements()
         && lhs.traceLevel()          == rhs.traceLevel()
         && lhs.maxSequenceSize()     == rhs.maxSequenceSize()
         && lhs.defaultEmptyStrings() == rhs.defaultEmptyStrings();
}

inline
bool balber::operator!=(const BerDecoderOptions& lhs,
                        const BerDecoderOptions& rhs)
{
    return  lhs.maxDepth()            != rhs.maxDepth()
         || lhs.skipUnknownElements() != rhs.skipUnknownElements()
         || lhs.traceLevel()          != rhs.traceLevel()
         || lhs.maxSequenceSize()     != rhs.maxSequenceSize()
         || lhs.defaultEmptyStrings() != rhs.defaultEmptyStrings();
}

inline
bsl::ostream& balber::operator<<(bsl::ostream&            stream,
                                 const BerDecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

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
