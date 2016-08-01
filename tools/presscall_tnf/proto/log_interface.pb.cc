// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "log_interface.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace log_server {

namespace {

const ::google::protobuf::Descriptor* LOGRequest_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  LOGRequest_reflection_ = NULL;
const ::google::protobuf::Descriptor* LOGResponse_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  LOGResponse_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_log_5finterface_2eproto() {
  protobuf_AddDesc_log_5finterface_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "log_interface.proto");
  GOOGLE_CHECK(file != NULL);
  LOGRequest_descriptor_ = file->message_type(0);
  static const int LOGRequest_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGRequest, log_content_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGRequest, echo_),
  };
  LOGRequest_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      LOGRequest_descriptor_,
      LOGRequest::default_instance_,
      LOGRequest_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGRequest, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGRequest, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(LOGRequest));
  LOGResponse_descriptor_ = file->message_type(1);
  static const int LOGResponse_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGResponse, retcode_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGResponse, retmsg_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGResponse, echo_),
  };
  LOGResponse_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      LOGResponse_descriptor_,
      LOGResponse::default_instance_,
      LOGResponse_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGResponse, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LOGResponse, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(LOGResponse));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_log_5finterface_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    LOGRequest_descriptor_, &LOGRequest::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    LOGResponse_descriptor_, &LOGResponse::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_log_5finterface_2eproto() {
  delete LOGRequest::default_instance_;
  delete LOGRequest_reflection_;
  delete LOGResponse::default_instance_;
  delete LOGResponse_reflection_;
}

void protobuf_AddDesc_log_5finterface_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\023log_interface.proto\022\nlog_server\"/\n\nLOG"
    "Request\022\023\n\013log_content\030\001 \001(\014\022\014\n\004echo\030\002 \001"
    "(\014\"<\n\013LOGResponse\022\017\n\007retcode\030\001 \002(\005\022\016\n\006re"
    "tmsg\030\002 \001(\014\022\014\n\004echo\030\003 \001(\014", 144);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "log_interface.proto", &protobuf_RegisterTypes);
  LOGRequest::default_instance_ = new LOGRequest();
  LOGResponse::default_instance_ = new LOGResponse();
  LOGRequest::default_instance_->InitAsDefaultInstance();
  LOGResponse::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_log_5finterface_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_log_5finterface_2eproto {
  StaticDescriptorInitializer_log_5finterface_2eproto() {
    protobuf_AddDesc_log_5finterface_2eproto();
  }
} static_descriptor_initializer_log_5finterface_2eproto_;


// ===================================================================

const ::std::string LOGRequest::_default_log_content_;
const ::std::string LOGRequest::_default_echo_;
#ifndef _MSC_VER
const int LOGRequest::kLogContentFieldNumber;
const int LOGRequest::kEchoFieldNumber;
#endif  // !_MSC_VER

LOGRequest::LOGRequest()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void LOGRequest::InitAsDefaultInstance() {
}

LOGRequest::LOGRequest(const LOGRequest& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void LOGRequest::SharedCtor() {
  _cached_size_ = 0;
  log_content_ = const_cast< ::std::string*>(&_default_log_content_);
  echo_ = const_cast< ::std::string*>(&_default_echo_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

LOGRequest::~LOGRequest() {
  SharedDtor();
}

void LOGRequest::SharedDtor() {
  if (log_content_ != &_default_log_content_) {
    delete log_content_;
  }
  if (echo_ != &_default_echo_) {
    delete echo_;
  }
  if (this != default_instance_) {
  }
}

void LOGRequest::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* LOGRequest::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return LOGRequest_descriptor_;
}

const LOGRequest& LOGRequest::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_log_5finterface_2eproto();  return *default_instance_;
}

LOGRequest* LOGRequest::default_instance_ = NULL;

LOGRequest* LOGRequest::New() const {
  return new LOGRequest;
}

void LOGRequest::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (log_content_ != &_default_log_content_) {
        log_content_->clear();
      }
    }
    if (_has_bit(1)) {
      if (echo_ != &_default_echo_) {
        echo_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool LOGRequest::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional bytes log_content = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_log_content()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_echo;
        break;
      }
      
      // optional bytes echo = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_echo:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_echo()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void LOGRequest::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional bytes log_content = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->log_content(), output);
  }
  
  // optional bytes echo = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      2, this->echo(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* LOGRequest::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional bytes log_content = 1;
  if (_has_bit(0)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->log_content(), target);
  }
  
  // optional bytes echo = 2;
  if (_has_bit(1)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->echo(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int LOGRequest::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional bytes log_content = 1;
    if (has_log_content()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->log_content());
    }
    
    // optional bytes echo = 2;
    if (has_echo()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->echo());
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

void LOGRequest::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const LOGRequest* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const LOGRequest*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void LOGRequest::MergeFrom(const LOGRequest& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_log_content(from.log_content());
    }
    if (from._has_bit(1)) {
      set_echo(from.echo());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void LOGRequest::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void LOGRequest::CopyFrom(const LOGRequest& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool LOGRequest::IsInitialized() const {
  
  return true;
}

void LOGRequest::Swap(LOGRequest* other) {
  if (other != this) {
    std::swap(log_content_, other->log_content_);
    std::swap(echo_, other->echo_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata LOGRequest::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = LOGRequest_descriptor_;
  metadata.reflection = LOGRequest_reflection_;
  return metadata;
}


// ===================================================================

const ::std::string LOGResponse::_default_retmsg_;
const ::std::string LOGResponse::_default_echo_;
#ifndef _MSC_VER
const int LOGResponse::kRetcodeFieldNumber;
const int LOGResponse::kRetmsgFieldNumber;
const int LOGResponse::kEchoFieldNumber;
#endif  // !_MSC_VER

LOGResponse::LOGResponse()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void LOGResponse::InitAsDefaultInstance() {
}

LOGResponse::LOGResponse(const LOGResponse& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void LOGResponse::SharedCtor() {
  _cached_size_ = 0;
  retcode_ = 0;
  retmsg_ = const_cast< ::std::string*>(&_default_retmsg_);
  echo_ = const_cast< ::std::string*>(&_default_echo_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

LOGResponse::~LOGResponse() {
  SharedDtor();
}

void LOGResponse::SharedDtor() {
  if (retmsg_ != &_default_retmsg_) {
    delete retmsg_;
  }
  if (echo_ != &_default_echo_) {
    delete echo_;
  }
  if (this != default_instance_) {
  }
}

void LOGResponse::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* LOGResponse::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return LOGResponse_descriptor_;
}

const LOGResponse& LOGResponse::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_log_5finterface_2eproto();  return *default_instance_;
}

LOGResponse* LOGResponse::default_instance_ = NULL;

LOGResponse* LOGResponse::New() const {
  return new LOGResponse;
}

void LOGResponse::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    retcode_ = 0;
    if (_has_bit(1)) {
      if (retmsg_ != &_default_retmsg_) {
        retmsg_->clear();
      }
    }
    if (_has_bit(2)) {
      if (echo_ != &_default_echo_) {
        echo_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool LOGResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 retcode = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &retcode_)));
          _set_bit(0);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_retmsg;
        break;
      }
      
      // optional bytes retmsg = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_retmsg:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_retmsg()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_echo;
        break;
      }
      
      // optional bytes echo = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_echo:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_echo()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void LOGResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 retcode = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->retcode(), output);
  }
  
  // optional bytes retmsg = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      2, this->retmsg(), output);
  }
  
  // optional bytes echo = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      3, this->echo(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* LOGResponse::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 retcode = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->retcode(), target);
  }
  
  // optional bytes retmsg = 2;
  if (_has_bit(1)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->retmsg(), target);
  }
  
  // optional bytes echo = 3;
  if (_has_bit(2)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        3, this->echo(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int LOGResponse::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 retcode = 1;
    if (has_retcode()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->retcode());
    }
    
    // optional bytes retmsg = 2;
    if (has_retmsg()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->retmsg());
    }
    
    // optional bytes echo = 3;
    if (has_echo()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->echo());
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

void LOGResponse::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const LOGResponse* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const LOGResponse*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void LOGResponse::MergeFrom(const LOGResponse& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_retcode(from.retcode());
    }
    if (from._has_bit(1)) {
      set_retmsg(from.retmsg());
    }
    if (from._has_bit(2)) {
      set_echo(from.echo());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void LOGResponse::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void LOGResponse::CopyFrom(const LOGResponse& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool LOGResponse::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void LOGResponse::Swap(LOGResponse* other) {
  if (other != this) {
    std::swap(retcode_, other->retcode_);
    std::swap(retmsg_, other->retmsg_);
    std::swap(echo_, other->echo_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata LOGResponse::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = LOGResponse_descriptor_;
  metadata.reflection = LOGResponse_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace log_server

// @@protoc_insertion_point(global_scope)