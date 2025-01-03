#include "Tracker.h"

#include <fmt/format.h>

namespace Typhoon {
std::string Tracker::decode_hex(const std::string &encoded) {
    std::string decoded;
    decoded.reserve(encoded.length() / 2);
    for (size_t i = 0; i < encoded.size(); i += 2) {
        decoded.push_back(static_cast<char>(std::stoi(encoded.substr(i, 2), nullptr, 16)));
    }
    return decoded;
}

// TODO: make this more unique
auto Tracker::generate_peer_id() -> std::string { return "-AD0001-000000000000"; }

HttpTracker::HttpTracker(asio::io_context &io_context) : io_context_(io_context) {}

auto HttpTracker::announce(const Torrent &torrent, bool compact) -> asio::awaitable<Response> {
    auto announce_url = torrent.announce();

    auto params = ada::url_search_params{};
    params.set("info_hash", decode_hex(torrent.info_hash()));
    params.set("peer_id", generate_peer_id());
    // TODO: if port is taken, try another in range
    params.set("port", "6881");
    // TODO: timer for interval and keep these updated
    params.set("uploaded", "0");
    params.set("downloaded", "0");
    params.set("left", "0");
    params.set("compact", compact ? "1" : "0");
    announce_url.set_search(params.to_string());

    // Prepare HTTP request
    auto request = fmt::format(
        "GET {}{} HTTP/1.0\r\n"
        "Host: {}\r\n"
        "\r\n",
        announce_url.get_pathname(), announce_url.get_search(), announce_url.get_hostname());

    // Make HTTP request
    auto http_response =
        co_await make_http_request(announce_url.get_hostname(), announce_url.get_port(), request);

    if (http_response.status_code != 200)
        throw std::runtime_error(fmt::format("HTTP error: {}", http_response.status_code));

    auto decoded = Bencode::decode(http_response.body);
    auto decoded_dict = rva::get<Bencode::Dict>(decoded);

    if (decoded_dict.contains("failure reason"))
        throw std::runtime_error(fmt::format(
            "Tracker error: {}", rva::get<std::string>(decoded_dict.at("failure reason"))));

    if (decoded_dict.contains("warning message"))
        fmt::println(stderr, "Tracker warning: {}",
                     rva::get<std::string>(decoded_dict.at("warning message")));

    if (!decoded_dict.contains("peers"))
        throw std::runtime_error("Tracker response missing peers");

    auto response = Response{};
    response.interval = rva::get<i64>(decoded_dict.at("interval"));
    if (decoded_dict.contains("min interval"))
        response.min_interval = rva::get<i64>(decoded_dict.at("min interval"));
    if (decoded_dict.contains("tracker id"))
        response.tracker_id = rva::get<std::string>(decoded_dict.at("tracker id"));
    response.complete = rva::get<i64>(decoded_dict.at("complete"));
    response.incomplete = rva::get<i64>(decoded_dict.at("incomplete"));

    if (compact) {
        auto peers = rva::get<std::string>(decoded_dict.at("peers"));
        if (peers.size() % 6 != 0)
            throw std::runtime_error(fmt::format("Invalid compact peers length: {}", peers.size()));
        for (std::size_t i = 0; i < peers.size(); i += 6) {
            auto ip =
                fmt::format("{}.{}.{}.{}", static_cast<u8>(peers[i]), static_cast<u8>(peers[i + 1]),
                            static_cast<u8>(peers[i + 2]), static_cast<u8>(peers[i + 3]));
            auto port = static_cast<u16>((peers[i + 4] << 8) | peers[i + 5]);
            response.peers.push_back(
                {std::nullopt, asio::ip::tcp::endpoint(asio::ip::make_address_v4(ip), port)});
        }
    } else {
        auto peers = rva::get<Bencode::List>(decoded_dict.at("peers"));
        response.peers.reserve(peers.size());
        for (const auto &peer_value : peers) {
            auto peer_dict = rva::get<Bencode::Dict>(peer_value);
            auto [id, endpoint] = Peer{};
            id = rva::get<std::string>(peer_dict.at("peer id"));
            auto ip = rva::get<std::string>(peer_dict.at("ip"));
            auto port = rva::get<i64>(peer_dict.at("port"));
            auto ec = asio::error_code{};
            auto address = asio::ip::make_address(ip, ec);
            if (ec) {
                auto resolver = asio::ip::tcp::resolver(io_context_);
                auto results =
                    co_await resolver.async_resolve(ip, std::to_string(port), asio::use_awaitable);
                endpoint = *results.begin();
            } else {
                endpoint = asio::ip::tcp::endpoint(address, port);
            }
            response.peers.push_back({id, endpoint});
        }
    }

    co_return response;
}

asio::awaitable<HttpTracker::HttpResponse> HttpTracker::make_http_request(
    const std::string_view host, const std::string_view port, const std::string_view request) {
    auto executor = co_await asio::this_coro::executor;
    auto resolver = asio::ip::tcp::resolver(executor);
    auto results = co_await resolver.async_resolve(host, port, asio::use_awaitable);

    auto socket = asio::ip::tcp::socket(executor);
    co_await asio::async_connect(socket, results.begin(), results.end(), asio::use_awaitable);

    co_await async_write(socket, asio::buffer(request), asio::use_awaitable);

    asio::streambuf response;
    co_await async_read_until(socket, response, "\r\n\r\n", asio::use_awaitable);

    HttpResponse http_response;
    std::istream response_stream(&response);

    // Parse status line
    std::string http_version;
    response_stream >> http_version;
    response_stream >> http_response.status_code;
    std::string status_message;
    std::getline(response_stream, status_message);

    // Parse headers
    std::string header;
    while (std::getline(response_stream, header) && header != "\r") {
        auto delimiter = header.find(':');
        auto key = header.substr(0, delimiter);
        auto value = header.substr(delimiter + 2, header.size() - delimiter - 3);
        http_response.headers[key] = value;
    }

    // Read body
    auto content_length = std::stoi(http_response.headers["Content-Length"]);
    http_response.body.reserve(content_length);

    if (response.size() > 0) {
        std::stringstream ss;
        ss << &response;
        http_response.body = ss.str();
    }

    asio::error_code ec;
    while (co_await async_read(socket, response, asio::transfer_at_least(1),
                               asio::redirect_error(asio::use_awaitable, ec))) {
        std::stringstream ss;
        ss << &response;
        http_response.body += ss.str();
    }

    if (ec && ec != asio::error::eof)
        throw std::system_error(ec);

    co_return http_response;
}
}
