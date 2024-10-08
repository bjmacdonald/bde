// balst_resolver_dwarfreader.h                                       -*-C++-*-

#ifndef INCLUDED_BALST_RESOLVER_DWARFREADER
#define INCLUDED_BALST_RESOLVER_DWARFREADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mechanism for reading DWARF information from object files.
//
//@CLASSES:
//   balst::Resolver_DwarfReader: reading mechanism
//
//@SEE_ALSO: balst_resolverimpl_elf
//           balst_resolver_filehelper
//
//@DESCRIPTION: This component provides a class,
// `balst::Resolver_DwarfReader`, that is optimized for reading
// information from object files that are in the DWARF format.  The Elf object
// file format is used on Linux and Solaris platforms, and the DWARF file
// format is used within ELF files to encode source file name and line number
// information.  The Elf format is described by documents at:
// * `http://en.wikipedia.org/wiki/Executable_and_Linkable_Format`
// * `http://downloads.openwatcom.org/ftp/devel/docs/elf-64-gen.pdf`
// * `http://www.sco.com/developers/gabi/latest/contents.html`
// The DWARF format is described by documents at:
// * `http://dwarfstd.org`
//
// Note that this file does not include everything necessary to resolve DWARF
// information.  Most of that functionality is in
// `balst_resolverimpl_elf`, and this component only describe a tool
// used within that effort.
//
///Usage
///-----
// This component is an implementation detail of `balst` and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <balscm_version.h>

#include <balst_objectfileformat.h>
#include <balst_resolver_filehelper.h>

#include <bdlb_bitutil.h>

#include <bdls_filesystemutil.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace balst {

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_DWARF)

                            // ==========================
                            // class Resolver_DwarfReader
                            // ==========================

class Resolver_DwarfReader {
  public:
    // PUBLIC TYPES
    typedef bdls::FilesystemUtil::Offset Offset;
    typedef bsls::Types::UintPtr         UintPtr;
    typedef bsls::Types::IntPtr          IntPtr;
    typedef bsls::Types::Uint64          Uint64;

    struct Section {
        // Refers to one section of a segment.

        // DATA
        Offset d_offset;    // offset of the section in the file
        Offset d_size;      // size of that section in bytes

        // CREATOR
        Section();
            // Create a zero-value 'Section' object.

        // MANIPULATOR
        void reset(Offset offset = 0, Offset size = 0);
            // Reset this 'Section' object to have the specified 'offset' and
            // the specified 'size'.
    };

    // PUBLIC CONSTANTS
    enum { k_SCRATCH_BUF_LEN = 32 * 1024 - 64 };
        // length in bytes of d_buffer_p; 32K minus a little so we don't
        // waste a page

                               // DWARF enums

    // The DWARF documents are at: http://www.dwarfstd.org/Download.php.
    //
    // The DWARF specs specify no 'struct's, only several hundred identifiers
    // of the form 'DW_*' defining integer values.  Some sources provide
    // include files 'dwarf.h' specifying 'enum's for all these values, but
    // accessing these include files from DPKG proved problematic, and the
    // versions available only provided DWARF 3 identifiers and we needed some
    // DWARF 4 id's too.  The 'dwarf.h' available from Red Hat was LGPL'ed,
    // posing potential licensing problems with copying or cut/pasting it into
    // the BDE code base, which is licensed more permissively than LGPL.
    //
    // The simplest approach was to implement these enums directly copying them
    // from the spec.  The following are a subset of all the id's in the spec,
    // limited to the ones we use in this package.
    //
    // Rather than name the enum's 'DW_*' as they appear in the spec, we name
    // them 'e_DW_*' according to BDE convention.

    enum Dwarf3Enums {
        // These values were obtained from the DWARF 3 Spec.

        e_DW_AT_sibling = 0x01,
        e_DW_AT_location = 0x02,
        e_DW_AT_name = 0x03,
        e_DW_AT_ordering = 0x09,
        e_DW_AT_byte_size = 0x0b,
        e_DW_AT_bit_offset = 0x0c,
        e_DW_AT_bit_size = 0x0d,
        e_DW_AT_stmt_list = 0x10,
        e_DW_AT_low_pc = 0x11,
        e_DW_AT_high_pc = 0x12,
        e_DW_AT_language = 0x13,
        e_DW_AT_discr = 0x15,
        e_DW_AT_discr_value = 0x16,
        e_DW_AT_visibility = 0x17,
        e_DW_AT_import = 0x18,
        e_DW_AT_string_length = 0x19,
        e_DW_AT_common_reference = 0x1a,
        e_DW_AT_comp_dir = 0x1b,
        e_DW_AT_const_value = 0x1c,
        e_DW_AT_containing_type = 0x1d,
        e_DW_AT_default_value = 0x1e,
        e_DW_AT_inline = 0x20,
        e_DW_AT_is_optional = 0x21,
        e_DW_AT_lower_bound = 0x22,
        e_DW_AT_producer = 0x25,
        e_DW_AT_prototyped = 0x27,
        e_DW_AT_return_addr = 0x2a,
        e_DW_AT_start_scope = 0x2c,
        e_DW_AT_bit_stride = 0x2e,
        e_DW_AT_upper_bound = 0x2f,
        e_DW_AT_abstract_origin = 0x31,
        e_DW_AT_accessibility = 0x32,
        e_DW_AT_address_class = 0x33,
        e_DW_AT_artificial = 0x34,
        e_DW_AT_base_types = 0x35,
        e_DW_AT_calling_convention = 0x36,
        e_DW_AT_count = 0x37,
        e_DW_AT_data_member_location = 0x38,
        e_DW_AT_decl_column = 0x39,
        e_DW_AT_decl_file = 0x3a,
        e_DW_AT_decl_line = 0x3b,
        e_DW_AT_declaration = 0x3c,
        e_DW_AT_discr_list = 0x3d,
        e_DW_AT_encoding = 0x3e,
        e_DW_AT_external = 0x3f,
        e_DW_AT_frame_base = 0x40,
        e_DW_AT_friend = 0x41,
        e_DW_AT_identifier_case = 0x42,
        e_DW_AT_macro_info = 0x43,
        e_DW_AT_namelist_item = 0x44,
        e_DW_AT_priority = 0x45,
        e_DW_AT_segment = 0x46,
        e_DW_AT_specification = 0x47,
        e_DW_AT_static_link = 0x48,
        e_DW_AT_type = 0x49,
        e_DW_AT_use_location = 0x4a,
        e_DW_AT_variable_parameter = 0x4b,
        e_DW_AT_virtuality = 0x4c,
        e_DW_AT_vtable_elem_location = 0x4d,
        e_DW_AT_allocated = 0x4e,
        e_DW_AT_associated = 0x4f,
        e_DW_AT_data_location = 0x50,
        e_DW_AT_byte_stride = 0x51,
        e_DW_AT_entry_pc = 0x52,
        e_DW_AT_use_UTF8 = 0x53,
        e_DW_AT_extension = 0x54,
        e_DW_AT_ranges = 0x55,
        e_DW_AT_trampoline = 0x56,
        e_DW_AT_call_column = 0x57,
        e_DW_AT_call_file = 0x58,
        e_DW_AT_call_line = 0x59,
        e_DW_AT_description = 0x5a,
        e_DW_AT_binary_scale = 0x5b,
        e_DW_AT_decimal_scale = 0x5c,
        e_DW_AT_small = 0x5d,
        e_DW_AT_decimal_sign = 0x5e,
        e_DW_AT_digit_count = 0x5f,
        e_DW_AT_picture_string = 0x60,
        e_DW_AT_mutable = 0x61,
        e_DW_AT_threads_scaled = 0x62,
        e_DW_AT_explicit = 0x63,
        e_DW_AT_object_pointer = 0x64,
        e_DW_AT_endianity = 0x65,
        e_DW_AT_elemental = 0x66,
        e_DW_AT_pure = 0x67,
        e_DW_AT_recursive = 0x68,

        e_DW_CHILDREN_no = 0x00,
        e_DW_CHILDREN_yes = 0x01,

        e_DW_FORM_addr = 0x01,
        e_DW_FORM_block2 = 0x03,
        e_DW_FORM_block4 = 0x04,
        e_DW_FORM_data2 = 0x05,
        e_DW_FORM_data4 = 0x06,
        e_DW_FORM_data8 = 0x07,
        e_DW_FORM_string = 0x08,
        e_DW_FORM_block = 0x09,
        e_DW_FORM_block1 = 0x0a,
        e_DW_FORM_data1 = 0x0b,
        e_DW_FORM_flag = 0x0c,
        e_DW_FORM_sdata = 0x0d,
        e_DW_FORM_strp = 0x0e,
        e_DW_FORM_udata = 0x0f,
        e_DW_FORM_ref_addr = 0x10,
        e_DW_FORM_ref1 = 0x11,
        e_DW_FORM_ref2 = 0x12,
        e_DW_FORM_ref4 = 0x13,
        e_DW_FORM_ref8 = 0x14,
        e_DW_FORM_ref_udata = 0x15,
        e_DW_FORM_indirect = 0x16,
        e_DW_FORM_line_strp = 0x1f,

        e_DW_INL_declared_inlined = 0x03,

        e_DW_LNCT_path = 0x01,
        e_DW_LNCT_directory_index = 0x02,
        e_DW_LNCT_timestamp = 0x03,
        e_DW_LNCT_size = 0x04,
        e_DW_LNCT_MD5 = 0x05,
        e_DW_LNCT_lo_user = 0x2000,
        e_DW_LNCT_hi_user = 0x3fff,

        e_DW_LNE_end_sequence = 0x01,
        e_DW_LNE_set_address = 0x02,
        e_DW_LNE_define_file = 0x03,

        e_DW_LNS_copy = 0x01,
        e_DW_LNS_advance_pc = 0x02,
        e_DW_LNS_advance_line = 0x03,
        e_DW_LNS_set_file = 0x04,
        e_DW_LNS_set_column = 0x05,
        e_DW_LNS_negate_stmt = 0x06,
        e_DW_LNS_set_basic_block = 0x07,
        e_DW_LNS_const_add_pc = 0x08,
        e_DW_LNS_fixed_advance_pc = 0x09,
        e_DW_LNS_set_prologue_end = 0x0a,
        e_DW_LNS_set_epilogue_begin = 0x0b,
        e_DW_LNS_set_isa = 0x0c,

        e_DW_TAG_array_type = 0x01,
        e_DW_TAG_class_type = 0x02,
        e_DW_TAG_entry_point = 0x03,
        e_DW_TAG_enumeration_type = 0x04,
        e_DW_TAG_formal_parameter = 0x05,
        e_DW_TAG_imported_declaration = 0x08,
        e_DW_TAG_label = 0x0a,
        e_DW_TAG_lexical_block = 0x0b,
        e_DW_TAG_member = 0x0d,
        e_DW_TAG_pointer_type = 0x0f,
        e_DW_TAG_reference_type = 0x10,
        e_DW_TAG_compile_unit = 0x11,
        e_DW_TAG_string_type = 0x12,
        e_DW_TAG_structure_type = 0x13,
        e_DW_TAG_subroutine_type = 0x15,
        e_DW_TAG_typedef = 0x16,
        e_DW_TAG_union_type = 0x17,
        e_DW_TAG_unspecified_parameters = 0x18,
        e_DW_TAG_variant = 0x19,
        e_DW_TAG_common_block = 0x1a,
        e_DW_TAG_common_inclusion = 0x1b,
        e_DW_TAG_inheritance = 0x1c,
        e_DW_TAG_inlined_subroutine = 0x1d,
        e_DW_TAG_module = 0x1e,
        e_DW_TAG_ptr_to_member_type = 0x1f,
        e_DW_TAG_set_type = 0x20,
        e_DW_TAG_subrange_type = 0x21,
        e_DW_TAG_with_stmt = 0x22,
        e_DW_TAG_access_declaration = 0x23,
        e_DW_TAG_base_type = 0x24,
        e_DW_TAG_catch_block = 0x25,
        e_DW_TAG_const_type = 0x26,
        e_DW_TAG_constant = 0x27,
        e_DW_TAG_enumerator = 0x28,
        e_DW_TAG_file_type = 0x29,
        e_DW_TAG_friend = 0x2a,
        e_DW_TAG_namelist = 0x2b,
        e_DW_TAG_namelist_item = 0x2c,
        e_DW_TAG_packed_type = 0x2d,
        e_DW_TAG_subprogram = 0x2e,
        e_DW_TAG_template_type_parameter = 0x2f,
        e_DW_TAG_template_value_parameter = 0x30,
        e_DW_TAG_thrown_type = 0x31,
        e_DW_TAG_try_block = 0x32,
        e_DW_TAG_variant_part = 0x33,
        e_DW_TAG_variable = 0x34,
        e_DW_TAG_volatile_type = 0x35,
        e_DW_TAG_dwarf_procedure = 0x36,
        e_DW_TAG_restrict_type = 0x37,
        e_DW_TAG_interface_type = 0x38,
        e_DW_TAG_namespace = 0x39,
        e_DW_TAG_imported_module = 0x3a,
        e_DW_TAG_unspecified_type = 0x3b,
        e_DW_TAG_partial_unit = 0x3c,
        e_DW_TAG_imported_unit = 0x3d,
        e_DW_TAG_condition = 0x3f,
        e_DW_TAG_shared_type = 0x40,
        e_DW_TAG_lo_user = 0x4080,
        e_DW_TAG_hi_user = 0xffff
    };

    enum Dwarf4Enums {
        // These values were obtained from the DWARF 4 Spec.

        e_DW_AT_signature = 0x69,
        e_DW_AT_main_subprogram = 0x6a,
        e_DW_AT_data_bit_offset = 0x6b,
        e_DW_AT_const_expr = 0x6c,
        e_DW_AT_enum_class = 0x6d,
        e_DW_AT_linkage_name = 0x6e,
        e_DW_AT_lo_user = 0x2000,
        e_DW_AT_hi_user = 0x3fff,

        e_DW_FORM_sec_offset = 0x17,
        e_DW_FORM_exprloc = 0x18,
        e_DW_FORM_flag_present = 0x19,
        e_DW_FORM_ref_sig8 = 0x20,

        e_DW_LNE_set_discriminator = 0x04,

        e_DW_TAG_mutable_type = 0x3e,
        e_DW_TAG_type_unit = 0x41,
        e_DW_TAG_rvalue_reference_type = 0x42,
        e_DW_TAG_template_alias = 0x43
    };

    enum Dwarf5Enums {
        e_DW_UT_compile = 0x1,
        e_DW_UT_type = 0x2,
        e_DW_UT_partial = 0x3,
        e_DW_UT_skeleton = 0x4,
        e_DW_UT_split_compile = 0x5,
        e_DW_UT_split_type = 0x6,
        e_DW_UT_lo_user = 0x80,
        e_DW_UT_hi_user = 0xff
    };

  private:
    // DATA
    balst::Resolver_FileHelper     *d_helper_p;      // filehelper for current
                                                     // segment

    char                           *d_buffer_p;      // buffer.
                                                     // k_SCRATCH_BUF_LEN long

    Offset                          d_offset;        // offset last read from

    Offset                          d_beginOffset;   // beg of current section

    Offset                          d_endOffset;     // end of current section

    const char                     *d_readPtr;       // current place to read
                                                     // from (in the buffer)

    const char                     *d_endPtr;        // end of what's in buffer

    int                             d_offsetSize;    // offset size determined
                                                     // by 'readInitalLength'

    int                             d_addressSize;   // address read by
                                                     // 'getAddress' or set by
                                                     // 'setAddressSize'.

  private:
    // NOT IMPLEMENTED
    Resolver_DwarfReader(const Resolver_DwarfReader&);
    Resolver_DwarfReader& operator=(const Resolver_DwarfReader&);

  private:
    // PRIVATE MANIPULATORS

    /// Determine if we are able to read the specified `numBytes` from
    /// `d_buffer`.  If not, `reload` is used to attempt to accomodate this
    /// `needBytes` request.  Return 0 if we are able to read `numBytes`
    /// from `d_buffer` after this invocation of `needBytes`, and a non-zero
    /// value otherwise.
    int needBytes(bsl::size_t numBytes);

    /// Reload the buffer to accomodate a read of at least the specified
    /// `numBytes`.  If possible, read up to the end of the section or the
    /// size of the buffer, whichever is shorter.  Return 0 on success, and
    /// a non-zero value otherwise.
    int reload(bsl::size_t numBytes);

  public:
    // CLASS METHODS
    static
    const char *stringForAt(unsigned id);
        // Return the string equivalent of the specified 'e_DW_AT_*' 'id'.

    static
    const char *stringForForm(unsigned id);
        // Return the string equivalent of the specified 'e_DW_FORM_*' 'id'.

    static
    const char *stringForInlineState(unsigned inlineState);
        // Return the string equivalent of the specified 'e_DW_INL_*'
        // 'inlineState'.

    static
    const char *stringForLNCT(unsigned id);
        // Return the string equivalent of the specified 'e_DW_LNCT_*' 'id'.

    static
    const char *stringForLNE(unsigned id);
        // Return the string equivalent of the specified 'e_DW_LNE_*' 'id'.

    static
    const char *stringForLNS(unsigned id);
        // Return the string equivalent of the specified 'e_DW_LNS_*' 'id'.

    static
    const char *stringForTag(unsigned tag);
        // Return the string equivalent of the specified 'e_DW_TAG_*' 'tag'.

    // CREATORS

    /// Create a `Reader` object in a null state.
    Resolver_DwarfReader();

    /// Destroy this object.
    //! ~Resolver_DwarfReader() = default;

    // MANIPULATORS

    /// Disable this object for further use.
    void disable();

    /// Initialize this `Reader` object using the specified `fileHelper` and
    /// the specified `buffer`, to operate on the specified `section`, where
    /// the specified `libraryFileSize` is the size of the library or
    /// executable file.  `buffer` is assumed to be at least
    /// `k_SCRATCH_BUF_LEN` long.
    int init(balst::Resolver_FileHelper *fileHelper,
             char                       *buffer,
             const Section&              section,
             Offset                      libraryFileSize);

    /// Read to the specified `dst`.  This function will fail if
    /// `d_addressSize` has not been initialized by `readAddressSize` or
    /// `setAddressSize`.  Return 0 on success and a non-zero value
    /// otherwise.
    int readAddress(UintPtr *dst);

    /// Read to the specified `dst` according to the specified `form`.
    /// Return 0 on success and a non-zero value otherwise.
    int readAddress(UintPtr *dst, unsigned form);

    /// Read the address size from a single unsigned byte, check it, and
    /// assign `d_addressSize` to it.  Return 0 on success and a non-zero
    /// value otherwise.  It is an error if address size is not equal to the
    /// size of an `unsigned int` or of a `void *`.
    int readAddressSize();

    /// Read the initial length of the object according to the DWARF
    /// specification to the specified `*dst`, which is an 8-byte value.
    /// Read `*dst` first as a 4 byte value, setting the high-order 4 bytes
    /// to 0, and if the value is below 0xfffffff0, then that indicates that
    /// section offsets are to be read as 4 byte values within the object
    /// whose length is specified.  If the value is 0xffffffff, read the
    /// next 8 bytes into `*dst` and that indicates that section offsets are
    /// to be 8 bytes within the object whose length is specified.
    /// Initialize `d_offsetSize` accordingly.  Values in the range
    /// `[0xfffffff0, 0xffffffff)` are illegal for that first 4 bytes.
    /// Return 0 on success and a non-zero value otherwise.  Note that when
    /// we read a 4-byte value, we do not extend sign to the high order 4
    /// bytes of `*dst`.
    int readInitialLength(Offset *dst);

    /// Read a signed, variable-length number into the specified `*dst`.
    /// Return 0 on success and a non-zero value otherwise.
    template <class TYPE>
    int readLEB128(TYPE *dst);

    /// Read an unsigned, variable-length number into the specified `*dst`.
    /// Return 0 on success and a non-zero value otherwise.
    template <class TYPE>
    int readULEB128(TYPE *dst);

    /// Read to the specified `*dst`, where the specified `offsetSize` is the
    /// number of low-order bytes to be read into the offset, where `*dst` is 8
    /// bytes, and extra high-order bytes are to be set to 0.  Do not extend
    /// sign.  Return 0 on success and a non-zero value otherwise.
    int readOffset(Offset      *dst,
                   bsl::size_t  offsetSize);

    /// Read to the specified `*dst` according to the specified `form`, where
    /// `form` is a DWARF enum of the `e_DW_FORM_*` category.  Return 0 on
    /// success and a non-zero value otherwise.
    int readOffsetFromForm(Offset   *dst,
                           unsigned  form);

    /// Read to the specified offset `*dst` according to `d_offsetSize`.
    /// Return 0 on success and a non-zero value otherwise.  Note that when
    /// the offset read is only 4 bytes, no sign extension takes place as
    /// we always expect a positive result.
    int readSectionOffset(Offset *dst);

    /// Read a null-terminated string to the specified `*dst`.  If no `dst`
    /// is specified, skip over the string without copying it.  This
    /// function will fail if the string length is greater than
    /// `k_SCRATCH_BUFFER_LEN - 1`.
    int readString(bsl::string *dst = 0);

    /// Read a null terminated string to the specified `*dst` from the
    /// specified `offset` plus `d_beginOffset`.  Note that, unlike most of
    /// the other `read` functions, this one is intended for random access
    /// so does not read a full buffer ahead, instead reading a fairly
    /// minimal amount of data near the specified location.
    int readStringAt(bsl::string *dst, Offset offset);

    /// Read to the specified string either from the current reader (if the
    /// specified `form` is `e_DW_FORM_string`) or read an offset from the
    /// current reader, then use that to read the string either from the
    /// specified `*strReader` (if `form` is `e_DW_FORM_strp`) or from the
    /// specified `*lineStrReader` (if `form` is `e_DW_FORM_line_strp`).
    /// Return 0 on success and a non-zero value otherwise.
    int readStringFromForm(bsl::string          *dst,
                           Resolver_DwarfReader *strReader,
                           Resolver_DwarfReader *lineStrReader,
                           unsigned              form);

    /// Read a value into the specified `*dst`, assuming that it is represented
    /// by `sizeof(*dst)` bytes.
    template <class TYPE>
    int readValue(TYPE *dst);

    /// Explicitly set the `d_addressSize` of this reader to the specified
    /// `size`.  This function will fail unless the size is the
    /// `sizeof(unsigned) == size` or `sizeof(UintPtr) == size`.
    int setAddressSize(unsigned size);

    /// Set the end offset to the specified `newOffset`.
    int setEndOffset(Offset newOffset);

    /// Skip forward over the specified `bytes` without reading them.
    int skipBytes(Offset bytes);

    /// Skip over a null terminated string without copying it.
    int skipString();

    /// Skip over data according to the specified `form`, which is an enum
    /// of type `e_DW_FORM_*`.
    int skipForm(unsigned form);

    /// Skip to the specified `offset`, which must be in the section associated
    /// with this reader.  Return 0 on success and a non-zero value otherwise.
    int skipTo(Offset offset);

    /// Skip a variable-length integer.  Note this will work for both
    /// LEB128's and ULEB128's.
    int skipULEB128();

    // ACCESSORS

    /// Return the address size field.
    int addressSize() const;

    /// Return `true` if the reader has reached the end of the section and
    /// `false` otherwise.
    bool atEndOfSection() const;

    /// Return the current offset taking the `d_reader` position into account.
    Offset offset() const;

    /// Return the offset length that was set by the `readInitialLength`
    /// function.
    Offset offsetSize() const;

    /// Return `false` if this reader is disabled and `true` otherwise.
    bool isEnabled() const;
};

// PRIVATE MANIPULATORS
inline
int Resolver_DwarfReader::needBytes(bsl::size_t numBytes)
{
    IntPtr diff = d_endPtr - d_readPtr;

    if (diff < static_cast<IntPtr>(numBytes)) {
        BSLS_ASSERT(0 <= diff);

        return reload(numBytes);                                      // RETURN
    }

    return 0;
}

template <class TYPE>
int Resolver_DwarfReader::readLEB128(TYPE *dst)      // DWARF doc 7.6
{
    BSLMF_ASSERT(static_cast<TYPE>(-1) < 0);    // 'TYPE' must be signed

    int rc;

    Uint64 tmpDst = 0;

    unsigned char u = 0x80;

    enum { k_MAX_SHIFT = sizeof(*dst) * 8 };

    unsigned shift = -7;
    do {
        rc = readValue(&u);
        if (rc) {
            // Assign to '*dst' to silence the confused optimizer complaining
            // about 'maybe used before set' in caller.

            *dst = 0;
            return -1;                                                // RETURN
        }

        const Uint64 masked = 0x7f & u;
        shift += 7;
        tmpDst |= masked << shift;
    } while (0x80 & u);

    if (static_cast<TYPE>(-1) < 0) {
        // signed type, extend sign

        const Uint64 negFlag = static_cast<Uint64>(0x40) << shift;
        if (negFlag & tmpDst) {
            tmpDst |= ~(negFlag - 1);
        }
    }

    *dst = static_cast<TYPE>(tmpDst);

    return 0;
}

template <class TYPE>
int Resolver_DwarfReader::readULEB128(TYPE *dst)     // DWARF doc 7.6
{
    Uint64 tmpDst = 0;

    unsigned char u = 0x80;

    unsigned shift = 0;
    for (; (0x80 & u); shift += 7) {
        int rc = readValue(&u);
        if (0 != rc) {
            // Assign to '*dst' to silence the confused optimizer complaining
            // about 'maybe used before set' in caller.

            *dst = 0;
            return -1;                                                // RETURN
        }

        const Uint64 masked = 0x7f & u;
        tmpDst |= masked << shift;
    }
#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)
    if (shift >= sizeof(*dst) * 8 + 7) {
        // Assign to '*dst' to silence the confused optimizer complaining about
        // 'maybe used before set' in caller.

        *dst = 0;
        return -1;                                                    // RETURN
    }
#endif

    *dst = static_cast<TYPE>(tmpDst);

    return 0;
}

template <class TYPE>
inline
int Resolver_DwarfReader::readValue(TYPE *dst)
{
    int rc = needBytes(sizeof(*dst));
    if (rc) {
        // Assign to '*dst' to silence the confused optimizer complaining about
        // 'maybe used before set' in caller.

        *dst = 0;
        return -1;                                                    // RETURN
    }

    bsl::memcpy(dst, d_readPtr, sizeof(*dst));
    d_readPtr += sizeof(*dst);

    return 0;
}

inline
int Resolver_DwarfReader::skipBytes(Offset bytes)
{
    BSLS_ASSERT(bytes >= 0);

    if (bytes > d_endPtr - d_readPtr) {
        Offset off = offset();

        if (off         < d_beginOffset) {
            return -1;                                                // RETURN
        }
        if (off + bytes > d_endOffset) {
            return -1;                                                // RETURN
        }

        // By setting 'd_readPtr == d_endPtr' we guarantee that the next read
        // will trigger a reload.

        d_offset  += bytes + (d_readPtr - d_buffer_p);
        d_readPtr =  d_buffer_p;
        d_endPtr  =  d_readPtr;
    }
    else {
        d_readPtr += bytes;
    }

    return 0;
}

inline
int Resolver_DwarfReader::skipString()
{
    do {
        int rc = needBytes(1);
        if (rc) {
            return -1;                                                // RETURN
        }
    } while (*d_readPtr++);

    return 0;
}

// ACCESSORS
inline
int Resolver_DwarfReader::addressSize() const
{
    return d_addressSize;
}

inline
bool Resolver_DwarfReader::atEndOfSection() const
{
    return d_readPtr == d_endPtr &&
                              d_endOffset - d_offset == d_readPtr - d_buffer_p;
}

inline
Resolver_DwarfReader::Offset
Resolver_DwarfReader::offset() const
{
    return d_offset + (d_readPtr - d_buffer_p);
}

inline
Resolver_DwarfReader::Offset
Resolver_DwarfReader::offsetSize() const
{
    return d_offsetSize;
}

inline
bool Resolver_DwarfReader::isEnabled() const
{
    return 0 != d_buffer_p;
}

#endif

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
