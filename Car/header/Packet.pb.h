// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Packet.proto

#ifndef PROTOBUF_Packet_2eproto__INCLUDED
#define PROTOBUF_Packet_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace tutorial {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_Packet_2eproto();
void protobuf_AssignDesc_Packet_2eproto();
void protobuf_ShutdownFile_Packet_2eproto();

class Packet;

// ===================================================================

class Packet : public ::google::protobuf::Message {
 public:
  Packet();
  virtual ~Packet();

  Packet(const Packet& from);

  inline Packet& operator=(const Packet& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Packet& default_instance();

  void Swap(Packet* other);

  // implements Message ----------------------------------------------

  Packet* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet& from);
  void MergeFrom(const Packet& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 headerType = 1;
  inline bool has_headertype() const;
  inline void clear_headertype();
  static const int kHeaderTypeFieldNumber = 1;
  inline ::google::protobuf::int32 headertype() const;
  inline void set_headertype(::google::protobuf::int32 value);

  // optional int32 channel = 2;
  inline bool has_channel() const;
  inline void clear_channel();
  static const int kChannelFieldNumber = 2;
  inline ::google::protobuf::int32 channel() const;
  inline void set_channel(::google::protobuf::int32 value);

  // optional int32 warnMsg = 3;
  inline bool has_warnmsg() const;
  inline void clear_warnmsg();
  static const int kWarnMsgFieldNumber = 3;
  inline ::google::protobuf::int32 warnmsg() const;
  inline void set_warnmsg(::google::protobuf::int32 value);

  // optional int32 speed = 4;
  inline bool has_speed() const;
  inline void clear_speed();
  static const int kSpeedFieldNumber = 4;
  inline ::google::protobuf::int32 speed() const;
  inline void set_speed(::google::protobuf::int32 value);

  // optional int32 directionx = 5;
  inline bool has_directionx() const;
  inline void clear_directionx();
  static const int kDirectionxFieldNumber = 5;
  inline ::google::protobuf::int32 directionx() const;
  inline void set_directionx(::google::protobuf::int32 value);

  // optional int32 directiony = 6;
  inline bool has_directiony() const;
  inline void clear_directiony();
  static const int kDirectionyFieldNumber = 6;
  inline ::google::protobuf::int32 directiony() const;
  inline void set_directiony(::google::protobuf::int32 value);

  // repeated int32 data = 8 [packed = true];
  inline int data_size() const;
  inline void clear_data();
  static const int kDataFieldNumber = 8;
  inline ::google::protobuf::int32 data(int index) const;
  inline void set_data(int index, ::google::protobuf::int32 value);
  inline void add_data(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      data() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_data();

  // @@protoc_insertion_point(class_scope:tutorial.Packet)
 private:
  inline void set_has_headertype();
  inline void clear_has_headertype();
  inline void set_has_channel();
  inline void clear_has_channel();
  inline void set_has_warnmsg();
  inline void clear_has_warnmsg();
  inline void set_has_speed();
  inline void clear_has_speed();
  inline void set_has_directionx();
  inline void clear_has_directionx();
  inline void set_has_directiony();
  inline void clear_has_directiony();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 headertype_;
  ::google::protobuf::int32 channel_;
  ::google::protobuf::int32 warnmsg_;
  ::google::protobuf::int32 speed_;
  ::google::protobuf::int32 directionx_;
  ::google::protobuf::int32 directiony_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > data_;
  mutable int _data_cached_byte_size_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(7 + 31) / 32];

  friend void  protobuf_AddDesc_Packet_2eproto();
  friend void protobuf_AssignDesc_Packet_2eproto();
  friend void protobuf_ShutdownFile_Packet_2eproto();

  void InitAsDefaultInstance();
  static Packet* default_instance_;
};
// ===================================================================


// ===================================================================

// Packet

// optional int32 headerType = 1;
inline bool Packet::has_headertype() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet::set_has_headertype() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet::clear_has_headertype() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet::clear_headertype() {
  headertype_ = 0;
  clear_has_headertype();
}
inline ::google::protobuf::int32 Packet::headertype() const {
  return headertype_;
}
inline void Packet::set_headertype(::google::protobuf::int32 value) {
  set_has_headertype();
  headertype_ = value;
}

// optional int32 channel = 2;
inline bool Packet::has_channel() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet::set_has_channel() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet::clear_has_channel() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet::clear_channel() {
  channel_ = 0;
  clear_has_channel();
}
inline ::google::protobuf::int32 Packet::channel() const {
  return channel_;
}
inline void Packet::set_channel(::google::protobuf::int32 value) {
  set_has_channel();
  channel_ = value;
}

// optional int32 warnMsg = 3;
inline bool Packet::has_warnmsg() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Packet::set_has_warnmsg() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Packet::clear_has_warnmsg() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Packet::clear_warnmsg() {
  warnmsg_ = 0;
  clear_has_warnmsg();
}
inline ::google::protobuf::int32 Packet::warnmsg() const {
  return warnmsg_;
}
inline void Packet::set_warnmsg(::google::protobuf::int32 value) {
  set_has_warnmsg();
  warnmsg_ = value;
}

// optional int32 speed = 4;
inline bool Packet::has_speed() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Packet::set_has_speed() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Packet::clear_has_speed() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Packet::clear_speed() {
  speed_ = 0;
  clear_has_speed();
}
inline ::google::protobuf::int32 Packet::speed() const {
  return speed_;
}
inline void Packet::set_speed(::google::protobuf::int32 value) {
  set_has_speed();
  speed_ = value;
}

// optional int32 directionx = 5;
inline bool Packet::has_directionx() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Packet::set_has_directionx() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Packet::clear_has_directionx() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Packet::clear_directionx() {
  directionx_ = 0;
  clear_has_directionx();
}
inline ::google::protobuf::int32 Packet::directionx() const {
  return directionx_;
}
inline void Packet::set_directionx(::google::protobuf::int32 value) {
  set_has_directionx();
  directionx_ = value;
}

// optional int32 directiony = 6;
inline bool Packet::has_directiony() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void Packet::set_has_directiony() {
  _has_bits_[0] |= 0x00000020u;
}
inline void Packet::clear_has_directiony() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void Packet::clear_directiony() {
  directiony_ = 0;
  clear_has_directiony();
}
inline ::google::protobuf::int32 Packet::directiony() const {
  return directiony_;
}
inline void Packet::set_directiony(::google::protobuf::int32 value) {
  set_has_directiony();
  directiony_ = value;
}

// repeated int32 data = 8 [packed = true];
inline int Packet::data_size() const {
  return data_.size();
}
inline void Packet::clear_data() {
  data_.Clear();
}
inline ::google::protobuf::int32 Packet::data(int index) const {
  return data_.Get(index);
}
inline void Packet::set_data(int index, ::google::protobuf::int32 value) {
  data_.Set(index, value);
}
inline void Packet::add_data(::google::protobuf::int32 value) {
  data_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
Packet::data() const {
  return data_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
Packet::mutable_data() {
  return &data_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace tutorial

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Packet_2eproto__INCLUDED