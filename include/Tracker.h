#pragma once

#include <fmt/base.h>

#include <asio.hpp>
#include <string>

#include "Bencode.h"
#include "Torrent.h"

namespace Typhoon {
class Tracker {
   public:
    struct Peer {
        /// Peer's self-selected ID
        std::optional<std::string> id;
        /// Peer's TCP Endpoint
        asio::ip::tcp::endpoint endpoint;
    };

    struct Response {
        /// Interval in seconds that the client should wait between sending regular
        /// requests to the tracker.
        i64 interval;
        /// Minimum announce interval. If present clients must not re-announce more
        /// frequently than this.
        std::optional<i64> min_interval;
        /// String that the client should send back on its next announcements.
        std::optional<std::string> tracker_id;
        /// Number of peers with the entire file, i.e. seeders.
        i64 complete;
        /// Number of non-seeder peers, aka "leechers".
        i64 incomplete;
        /// List of peers
        std::vector<Peer> peers;
    };

    virtual ~Tracker() = default;
    virtual asio::awaitable<Response> announce(const Torrent &torrent, bool compact) = 0;

   protected:
    static std::string decode_hex(const std::string &encoded);
    static std::string generate_peer_id();
};

class HttpTracker final : public Tracker {
   public:
    explicit HttpTracker(asio::io_context &io_context);
    asio::awaitable<Response> announce(const Torrent &torrent, bool compact) override;

   private:
    asio::io_context &io_context_;

    struct HttpResponse {
        unsigned int status_code;
        std::map<std::string, std::string> headers;
        std::string body;
    };

    static asio::awaitable<HttpResponse> make_http_request(std::string_view host,
                                                           std::string_view port,
                                                           std::string_view request);
};
}

template <>
struct fmt::formatter<Typhoon::Tracker::Peer> : formatter<std::string> {
    auto format(const Typhoon::Tracker::Peer &peer, format_context &ctx) const {
        auto out = ctx.out();
        return format_to(out,
                         "Peer{{\n"
                         "  ID: {}\n"
                         "  Endpoint: {}:{}\n"
                         "}}",
                         peer.id.value_or(""), peer.endpoint.address().to_string(),
                         peer.endpoint.port());
    }
};

template <>
struct fmt::formatter<Typhoon::Tracker::Response> {
    static constexpr auto parse(const format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Typhoon::Tracker::Response &response, format_context &ctx) const {
        auto out = ctx.out();
        return format_to(out,
                         "Tracker::Response{{\n"
                         "  Interval: {}\n"
                         "  Min Interval: {}\n"
                         "  Tracker ID: {}\n"
                         "  Complete: {}\n"
                         "  Incomplete: {}\n"
                         "  Peers ({}):\n"
                         // "    {}\n"
                         "}}",
                         response.interval, response.min_interval.value_or(0),
                         response.tracker_id.value_or(""), response.complete, response.incomplete,
                         response.peers.size());
    }
};
