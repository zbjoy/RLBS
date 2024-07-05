// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rlbs.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_rlbs_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_rlbs_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3008000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3008000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_rlbs_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_rlbs_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[3]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_rlbs_2eproto;
namespace rlbs {
class GetRouteRequest;
class GetRouteRequestDefaultTypeInternal;
extern GetRouteRequestDefaultTypeInternal _GetRouteRequest_default_instance_;
class GetRouteResponse;
class GetRouteResponseDefaultTypeInternal;
extern GetRouteResponseDefaultTypeInternal _GetRouteResponse_default_instance_;
class HostInfo;
class HostInfoDefaultTypeInternal;
extern HostInfoDefaultTypeInternal _HostInfo_default_instance_;
}  // namespace rlbs
PROTOBUF_NAMESPACE_OPEN
template<> ::rlbs::GetRouteRequest* Arena::CreateMaybeMessage<::rlbs::GetRouteRequest>(Arena*);
template<> ::rlbs::GetRouteResponse* Arena::CreateMaybeMessage<::rlbs::GetRouteResponse>(Arena*);
template<> ::rlbs::HostInfo* Arena::CreateMaybeMessage<::rlbs::HostInfo>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace rlbs {

enum MessageId : int {
  ID_UNKNOW = 0,
  ID_GetRouteRequest = 1,
  ID_GetRouteResponse = 2,
  MessageId_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  MessageId_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool MessageId_IsValid(int value);
constexpr MessageId MessageId_MIN = ID_UNKNOW;
constexpr MessageId MessageId_MAX = ID_GetRouteResponse;
constexpr int MessageId_ARRAYSIZE = MessageId_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MessageId_descriptor();
template<typename T>
inline const std::string& MessageId_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MessageId>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MessageId_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MessageId_descriptor(), enum_t_value);
}
inline bool MessageId_Parse(
    const std::string& name, MessageId* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MessageId>(
    MessageId_descriptor(), name, value);
}
// ===================================================================

class HostInfo :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:rlbs.HostInfo) */ {
 public:
  HostInfo();
  virtual ~HostInfo();

  HostInfo(const HostInfo& from);
  HostInfo(HostInfo&& from) noexcept
    : HostInfo() {
    *this = ::std::move(from);
  }

  inline HostInfo& operator=(const HostInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline HostInfo& operator=(HostInfo&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const HostInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const HostInfo* internal_default_instance() {
    return reinterpret_cast<const HostInfo*>(
               &_HostInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(HostInfo* other);
  friend void swap(HostInfo& a, HostInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline HostInfo* New() const final {
    return CreateMaybeMessage<HostInfo>(nullptr);
  }

  HostInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<HostInfo>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const HostInfo& from);
  void MergeFrom(const HostInfo& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(HostInfo* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "rlbs.HostInfo";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_rlbs_2eproto);
    return ::descriptor_table_rlbs_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // int32 ip = 1;
  void clear_ip();
  static const int kIpFieldNumber = 1;
  ::PROTOBUF_NAMESPACE_ID::int32 ip() const;
  void set_ip(::PROTOBUF_NAMESPACE_ID::int32 value);

  // int32 port = 2;
  void clear_port();
  static const int kPortFieldNumber = 2;
  ::PROTOBUF_NAMESPACE_ID::int32 port() const;
  void set_port(::PROTOBUF_NAMESPACE_ID::int32 value);

  // @@protoc_insertion_point(class_scope:rlbs.HostInfo)
 private:
  class HasBitSetters;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::int32 ip_;
  ::PROTOBUF_NAMESPACE_ID::int32 port_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_rlbs_2eproto;
};
// -------------------------------------------------------------------

class GetRouteRequest :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:rlbs.GetRouteRequest) */ {
 public:
  GetRouteRequest();
  virtual ~GetRouteRequest();

  GetRouteRequest(const GetRouteRequest& from);
  GetRouteRequest(GetRouteRequest&& from) noexcept
    : GetRouteRequest() {
    *this = ::std::move(from);
  }

  inline GetRouteRequest& operator=(const GetRouteRequest& from) {
    CopyFrom(from);
    return *this;
  }
  inline GetRouteRequest& operator=(GetRouteRequest&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const GetRouteRequest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const GetRouteRequest* internal_default_instance() {
    return reinterpret_cast<const GetRouteRequest*>(
               &_GetRouteRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(GetRouteRequest* other);
  friend void swap(GetRouteRequest& a, GetRouteRequest& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline GetRouteRequest* New() const final {
    return CreateMaybeMessage<GetRouteRequest>(nullptr);
  }

  GetRouteRequest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<GetRouteRequest>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const GetRouteRequest& from);
  void MergeFrom(const GetRouteRequest& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(GetRouteRequest* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "rlbs.GetRouteRequest";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_rlbs_2eproto);
    return ::descriptor_table_rlbs_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // int32 modid = 1;
  void clear_modid();
  static const int kModidFieldNumber = 1;
  ::PROTOBUF_NAMESPACE_ID::int32 modid() const;
  void set_modid(::PROTOBUF_NAMESPACE_ID::int32 value);

  // int32 cmdid = 2;
  void clear_cmdid();
  static const int kCmdidFieldNumber = 2;
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid() const;
  void set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value);

  // @@protoc_insertion_point(class_scope:rlbs.GetRouteRequest)
 private:
  class HasBitSetters;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::int32 modid_;
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_rlbs_2eproto;
};
// -------------------------------------------------------------------

class GetRouteResponse :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:rlbs.GetRouteResponse) */ {
 public:
  GetRouteResponse();
  virtual ~GetRouteResponse();

  GetRouteResponse(const GetRouteResponse& from);
  GetRouteResponse(GetRouteResponse&& from) noexcept
    : GetRouteResponse() {
    *this = ::std::move(from);
  }

  inline GetRouteResponse& operator=(const GetRouteResponse& from) {
    CopyFrom(from);
    return *this;
  }
  inline GetRouteResponse& operator=(GetRouteResponse&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const GetRouteResponse& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const GetRouteResponse* internal_default_instance() {
    return reinterpret_cast<const GetRouteResponse*>(
               &_GetRouteResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  void Swap(GetRouteResponse* other);
  friend void swap(GetRouteResponse& a, GetRouteResponse& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline GetRouteResponse* New() const final {
    return CreateMaybeMessage<GetRouteResponse>(nullptr);
  }

  GetRouteResponse* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<GetRouteResponse>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const GetRouteResponse& from);
  void MergeFrom(const GetRouteResponse& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(GetRouteResponse* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "rlbs.GetRouteResponse";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_rlbs_2eproto);
    return ::descriptor_table_rlbs_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .rlbs.HostInfo host = 3;
  int host_size() const;
  void clear_host();
  static const int kHostFieldNumber = 3;
  ::rlbs::HostInfo* mutable_host(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::rlbs::HostInfo >*
      mutable_host();
  const ::rlbs::HostInfo& host(int index) const;
  ::rlbs::HostInfo* add_host();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::rlbs::HostInfo >&
      host() const;

  // int32 modid = 1;
  void clear_modid();
  static const int kModidFieldNumber = 1;
  ::PROTOBUF_NAMESPACE_ID::int32 modid() const;
  void set_modid(::PROTOBUF_NAMESPACE_ID::int32 value);

  // int32 cmdid = 2;
  void clear_cmdid();
  static const int kCmdidFieldNumber = 2;
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid() const;
  void set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value);

  // @@protoc_insertion_point(class_scope:rlbs.GetRouteResponse)
 private:
  class HasBitSetters;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::rlbs::HostInfo > host_;
  ::PROTOBUF_NAMESPACE_ID::int32 modid_;
  ::PROTOBUF_NAMESPACE_ID::int32 cmdid_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_rlbs_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// HostInfo

// int32 ip = 1;
inline void HostInfo::clear_ip() {
  ip_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 HostInfo::ip() const {
  // @@protoc_insertion_point(field_get:rlbs.HostInfo.ip)
  return ip_;
}
inline void HostInfo::set_ip(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  ip_ = value;
  // @@protoc_insertion_point(field_set:rlbs.HostInfo.ip)
}

// int32 port = 2;
inline void HostInfo::clear_port() {
  port_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 HostInfo::port() const {
  // @@protoc_insertion_point(field_get:rlbs.HostInfo.port)
  return port_;
}
inline void HostInfo::set_port(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  port_ = value;
  // @@protoc_insertion_point(field_set:rlbs.HostInfo.port)
}

// -------------------------------------------------------------------

// GetRouteRequest

// int32 modid = 1;
inline void GetRouteRequest::clear_modid() {
  modid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteRequest::modid() const {
  // @@protoc_insertion_point(field_get:rlbs.GetRouteRequest.modid)
  return modid_;
}
inline void GetRouteRequest::set_modid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  modid_ = value;
  // @@protoc_insertion_point(field_set:rlbs.GetRouteRequest.modid)
}

// int32 cmdid = 2;
inline void GetRouteRequest::clear_cmdid() {
  cmdid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteRequest::cmdid() const {
  // @@protoc_insertion_point(field_get:rlbs.GetRouteRequest.cmdid)
  return cmdid_;
}
inline void GetRouteRequest::set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  cmdid_ = value;
  // @@protoc_insertion_point(field_set:rlbs.GetRouteRequest.cmdid)
}

// -------------------------------------------------------------------

// GetRouteResponse

// int32 modid = 1;
inline void GetRouteResponse::clear_modid() {
  modid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResponse::modid() const {
  // @@protoc_insertion_point(field_get:rlbs.GetRouteResponse.modid)
  return modid_;
}
inline void GetRouteResponse::set_modid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  modid_ = value;
  // @@protoc_insertion_point(field_set:rlbs.GetRouteResponse.modid)
}

// int32 cmdid = 2;
inline void GetRouteResponse::clear_cmdid() {
  cmdid_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 GetRouteResponse::cmdid() const {
  // @@protoc_insertion_point(field_get:rlbs.GetRouteResponse.cmdid)
  return cmdid_;
}
inline void GetRouteResponse::set_cmdid(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  cmdid_ = value;
  // @@protoc_insertion_point(field_set:rlbs.GetRouteResponse.cmdid)
}

// repeated .rlbs.HostInfo host = 3;
inline int GetRouteResponse::host_size() const {
  return host_.size();
}
inline void GetRouteResponse::clear_host() {
  host_.Clear();
}
inline ::rlbs::HostInfo* GetRouteResponse::mutable_host(int index) {
  // @@protoc_insertion_point(field_mutable:rlbs.GetRouteResponse.host)
  return host_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::rlbs::HostInfo >*
GetRouteResponse::mutable_host() {
  // @@protoc_insertion_point(field_mutable_list:rlbs.GetRouteResponse.host)
  return &host_;
}
inline const ::rlbs::HostInfo& GetRouteResponse::host(int index) const {
  // @@protoc_insertion_point(field_get:rlbs.GetRouteResponse.host)
  return host_.Get(index);
}
inline ::rlbs::HostInfo* GetRouteResponse::add_host() {
  // @@protoc_insertion_point(field_add:rlbs.GetRouteResponse.host)
  return host_.Add();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::rlbs::HostInfo >&
GetRouteResponse::host() const {
  // @@protoc_insertion_point(field_list:rlbs.GetRouteResponse.host)
  return host_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace rlbs

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::rlbs::MessageId> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::rlbs::MessageId>() {
  return ::rlbs::MessageId_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_rlbs_2eproto
