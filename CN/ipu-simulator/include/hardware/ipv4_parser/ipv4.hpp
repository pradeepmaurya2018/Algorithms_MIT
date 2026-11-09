#pragma once

#include <cstddef>
#include <cstdint>

struct Ipv4Metadata {
    std::uint8_t version{0};
    std::uint8_t ihl{0};

    std::uint8_t dscp{0};
    std::uint8_t ecn{0};

    std::uint16_t total_length{0};
    std::uint16_t identification{0};

    bool reserved_flag{false};
    bool dont_fragment{false};
    bool more_fragments{false};

    std::uint16_t fragment_offset{0};

    std::uint8_t ttl{0};
    std::uint8_t protocol{0};

    std::uint16_t header_checksum{0};
    bool checksum_valid{false};

    std::uint32_t src_ip{0};
    std::uint32_t dst_ip{0};

    std::size_t header_length{0};
    std::size_t payload_offset{0};
    std::size_t payload_length{0};
};
