// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: constraint_solver/search_limit.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "constraint_solver/search_limit.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace operations_research {

namespace {

const ::google::protobuf::Descriptor* SearchLimitProto_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  SearchLimitProto_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_constraint_5fsolver_2fsearch_5flimit_2eproto() {
  protobuf_AddDesc_constraint_5fsolver_2fsearch_5flimit_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "constraint_solver/search_limit.proto");
  GOOGLE_CHECK(file != NULL);
  SearchLimitProto_descriptor_ = file->message_type(0);
  static const int SearchLimitProto_offsets_[6] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, time_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, branches_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, failures_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, solutions_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, smart_time_check_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, cumulative_),
  };
  SearchLimitProto_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      SearchLimitProto_descriptor_,
      SearchLimitProto::default_instance_,
      SearchLimitProto_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SearchLimitProto, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(SearchLimitProto));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_constraint_5fsolver_2fsearch_5flimit_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    SearchLimitProto_descriptor_, &SearchLimitProto::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_constraint_5fsolver_2fsearch_5flimit_2eproto() {
  delete SearchLimitProto::default_instance_;
  delete SearchLimitProto_reflection_;
}

void protobuf_AddDesc_constraint_5fsolver_2fsearch_5flimit_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n$constraint_solver/search_limit.proto\022\023"
    "operations_research\"\347\001\n\020SearchLimitProto"
    "\022!\n\004time\030\001 \001(\003:\0239223372036854775807\022%\n\010b"
    "ranches\030\002 \001(\003:\0239223372036854775807\022%\n\010fa"
    "ilures\030\003 \001(\003:\0239223372036854775807\022&\n\tsol"
    "utions\030\004 \001(\003:\0239223372036854775807\022\037\n\020sma"
    "rt_time_check\030\005 \001(\010:\005false\022\031\n\ncumulative"
    "\030\006 \001(\010:\005false", 293);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "constraint_solver/search_limit.proto", &protobuf_RegisterTypes);
  SearchLimitProto::default_instance_ = new SearchLimitProto();
  SearchLimitProto::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_constraint_5fsolver_2fsearch_5flimit_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_constraint_5fsolver_2fsearch_5flimit_2eproto {
  StaticDescriptorInitializer_constraint_5fsolver_2fsearch_5flimit_2eproto() {
    protobuf_AddDesc_constraint_5fsolver_2fsearch_5flimit_2eproto();
  }
} static_descriptor_initializer_constraint_5fsolver_2fsearch_5flimit_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int SearchLimitProto::kTimeFieldNumber;
const int SearchLimitProto::kBranchesFieldNumber;
const int SearchLimitProto::kFailuresFieldNumber;
const int SearchLimitProto::kSolutionsFieldNumber;
const int SearchLimitProto::kSmartTimeCheckFieldNumber;
const int SearchLimitProto::kCumulativeFieldNumber;
#endif  // !_MSC_VER

SearchLimitProto::SearchLimitProto()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:operations_research.SearchLimitProto)
}

void SearchLimitProto::InitAsDefaultInstance() {
}

SearchLimitProto::SearchLimitProto(const SearchLimitProto& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:operations_research.SearchLimitProto)
}

void SearchLimitProto::SharedCtor() {
  _cached_size_ = 0;
  time_ = GOOGLE_LONGLONG(9223372036854775807);
  branches_ = GOOGLE_LONGLONG(9223372036854775807);
  failures_ = GOOGLE_LONGLONG(9223372036854775807);
  solutions_ = GOOGLE_LONGLONG(9223372036854775807);
  smart_time_check_ = false;
  cumulative_ = false;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

SearchLimitProto::~SearchLimitProto() {
  // @@protoc_insertion_point(destructor:operations_research.SearchLimitProto)
  SharedDtor();
}

void SearchLimitProto::SharedDtor() {
  if (this != default_instance_) {
  }
}

void SearchLimitProto::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SearchLimitProto::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SearchLimitProto_descriptor_;
}

const SearchLimitProto& SearchLimitProto::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_constraint_5fsolver_2fsearch_5flimit_2eproto();
  return *default_instance_;
}

SearchLimitProto* SearchLimitProto::default_instance_ = NULL;

SearchLimitProto* SearchLimitProto::New() const {
  return new SearchLimitProto;
}

void SearchLimitProto::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<SearchLimitProto*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  if (_has_bits_[0 / 32] & 63) {
    ZR_(smart_time_check_, cumulative_);
    time_ = GOOGLE_LONGLONG(9223372036854775807);
    branches_ = GOOGLE_LONGLONG(9223372036854775807);
    failures_ = GOOGLE_LONGLONG(9223372036854775807);
    solutions_ = GOOGLE_LONGLONG(9223372036854775807);
  }

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool SearchLimitProto::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:operations_research.SearchLimitProto)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int64 time = 1 [default = 9223372036854775807];
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &time_)));
          set_has_time();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_branches;
        break;
      }

      // optional int64 branches = 2 [default = 9223372036854775807];
      case 2: {
        if (tag == 16) {
         parse_branches:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &branches_)));
          set_has_branches();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_failures;
        break;
      }

      // optional int64 failures = 3 [default = 9223372036854775807];
      case 3: {
        if (tag == 24) {
         parse_failures:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &failures_)));
          set_has_failures();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_solutions;
        break;
      }

      // optional int64 solutions = 4 [default = 9223372036854775807];
      case 4: {
        if (tag == 32) {
         parse_solutions:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &solutions_)));
          set_has_solutions();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(40)) goto parse_smart_time_check;
        break;
      }

      // optional bool smart_time_check = 5 [default = false];
      case 5: {
        if (tag == 40) {
         parse_smart_time_check:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &smart_time_check_)));
          set_has_smart_time_check();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(48)) goto parse_cumulative;
        break;
      }

      // optional bool cumulative = 6 [default = false];
      case 6: {
        if (tag == 48) {
         parse_cumulative:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &cumulative_)));
          set_has_cumulative();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:operations_research.SearchLimitProto)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:operations_research.SearchLimitProto)
  return false;
#undef DO_
}

void SearchLimitProto::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:operations_research.SearchLimitProto)
  // optional int64 time = 1 [default = 9223372036854775807];
  if (has_time()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(1, this->time(), output);
  }

  // optional int64 branches = 2 [default = 9223372036854775807];
  if (has_branches()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(2, this->branches(), output);
  }

  // optional int64 failures = 3 [default = 9223372036854775807];
  if (has_failures()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(3, this->failures(), output);
  }

  // optional int64 solutions = 4 [default = 9223372036854775807];
  if (has_solutions()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(4, this->solutions(), output);
  }

  // optional bool smart_time_check = 5 [default = false];
  if (has_smart_time_check()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(5, this->smart_time_check(), output);
  }

  // optional bool cumulative = 6 [default = false];
  if (has_cumulative()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(6, this->cumulative(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:operations_research.SearchLimitProto)
}

::google::protobuf::uint8* SearchLimitProto::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:operations_research.SearchLimitProto)
  // optional int64 time = 1 [default = 9223372036854775807];
  if (has_time()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(1, this->time(), target);
  }

  // optional int64 branches = 2 [default = 9223372036854775807];
  if (has_branches()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(2, this->branches(), target);
  }

  // optional int64 failures = 3 [default = 9223372036854775807];
  if (has_failures()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(3, this->failures(), target);
  }

  // optional int64 solutions = 4 [default = 9223372036854775807];
  if (has_solutions()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(4, this->solutions(), target);
  }

  // optional bool smart_time_check = 5 [default = false];
  if (has_smart_time_check()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(5, this->smart_time_check(), target);
  }

  // optional bool cumulative = 6 [default = false];
  if (has_cumulative()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(6, this->cumulative(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:operations_research.SearchLimitProto)
  return target;
}

int SearchLimitProto::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int64 time = 1 [default = 9223372036854775807];
    if (has_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->time());
    }

    // optional int64 branches = 2 [default = 9223372036854775807];
    if (has_branches()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->branches());
    }

    // optional int64 failures = 3 [default = 9223372036854775807];
    if (has_failures()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->failures());
    }

    // optional int64 solutions = 4 [default = 9223372036854775807];
    if (has_solutions()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->solutions());
    }

    // optional bool smart_time_check = 5 [default = false];
    if (has_smart_time_check()) {
      total_size += 1 + 1;
    }

    // optional bool cumulative = 6 [default = false];
    if (has_cumulative()) {
      total_size += 1 + 1;
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SearchLimitProto::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const SearchLimitProto* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const SearchLimitProto*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void SearchLimitProto::MergeFrom(const SearchLimitProto& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_time()) {
      set_time(from.time());
    }
    if (from.has_branches()) {
      set_branches(from.branches());
    }
    if (from.has_failures()) {
      set_failures(from.failures());
    }
    if (from.has_solutions()) {
      set_solutions(from.solutions());
    }
    if (from.has_smart_time_check()) {
      set_smart_time_check(from.smart_time_check());
    }
    if (from.has_cumulative()) {
      set_cumulative(from.cumulative());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void SearchLimitProto::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SearchLimitProto::CopyFrom(const SearchLimitProto& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SearchLimitProto::IsInitialized() const {

  return true;
}

void SearchLimitProto::Swap(SearchLimitProto* other) {
  if (other != this) {
    std::swap(time_, other->time_);
    std::swap(branches_, other->branches_);
    std::swap(failures_, other->failures_);
    std::swap(solutions_, other->solutions_);
    std::swap(smart_time_check_, other->smart_time_check_);
    std::swap(cumulative_, other->cumulative_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata SearchLimitProto::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = SearchLimitProto_descriptor_;
  metadata.reflection = SearchLimitProto_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace operations_research

// @@protoc_insertion_point(global_scope)
