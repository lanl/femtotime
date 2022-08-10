/**
 * @file msgpack.hpp
 * @author Patrick Norton <pnorton@lanl.gov>
 * @date 9 Aug 2022
 */
#pragma once

// [MessagePack headers]
#include <msgpack.hpp>

// [femtotime headers]
#include "femtotime/GPStime.hpp"

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(v2) {
namespace adaptor {

template<>
struct pack<femtotime::gps_time_t> {
  template<typename Stream>
  packer<Stream>& operator()(msgpack::packer<Stream>& o,
                             const femtotime::gps_time_t &t) const {
    // NOTE: Technically, this won't work properly on systems that use
    // ones-complement for their arithmetic (it's not UB, just
    // implementation-defined). However, there is no way that we are compiling
    // on one of those systems (and it will be illegal in C++20 anyways).
    // Thankfully, C++17 disallows non-binary representations (like decimal), so
    // *that* isn't an issue.
    auto femtos = static_cast<femtotime::uint128_t>(t.get_fs());
    auto high_bits = static_cast<uint64_t>(femtos >> 64);
    auto low_bits = static_cast<uint64_t>(femtos);
    o.pack_array(2);
    o.pack_uint64(high_bits);
    o.pack_uint64(low_bits);
    return o;
  }
};

template<>
struct convert<femtotime::gps_time_t> {
  msgpack::object const& operator()(msgpack::object const& o,
                                    femtotime::gps_time_t& t) const {
    if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) {
      throw msgpack::type_error();
    }
    auto high_bits = o.via.array.ptr[0].via.u64;
    auto low_bits = o.via.array.ptr[1].via.u64;
    auto femtos = (static_cast<femtotime::uint128_t>(high_bits) << 64)
      + static_cast<femtotime::uint128_t>(low_bits);
    t = femtotime::gps_time_t(static_cast<femtotime::int128_t>(femtos));
    return o;
  }
};

} // namespace adaptor
} // namespace MSGPACK_API_VERSION_NAMESPACE
} // namespace msgpack
