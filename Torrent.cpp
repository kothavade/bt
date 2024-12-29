#include <fmt/format.h>

#include "Torrent.h"
#include "Bencode.h"

Torrent::Torrent(const std::string_view metainfo)
{
    const auto bencode = Bencode{metainfo};
    const auto dict = std::get<Bencode::Dictionary>(bencode.value);
    announce_ = std::get<std::string_view>(dict.at("announce").value);
    if (dict.contains("announce-list"))
    {
        const auto& announce_list = std::get<Bencode::List>(dict.at("announce-list").value);
        std::vector<std::vector<std::string_view>> list;
        list.reserve(announce_list.size());
        for (const auto& tier_bc : announce_list)
        {
            const auto& tier = std::get<Bencode::List>(tier_bc.value);
            std::vector<std::string_view> tier_list;
            tier_list.reserve(tier.size());
            for (const auto& url : tier)
                tier_list.push_back(std::get<std::string_view>(url.value));
            list.push_back(tier_list);
        }
        announce_list_ = list;
    }
    if (dict.contains("creation date"))
    {
        const auto date = std::get<i64>(dict.at("creation date").value);
        creation_date_ = std::chrono::system_clock::time_point{std::chrono::seconds{date}};
    }
    if (dict.contains("comment"))
        comment_ = std::get<std::string_view>(dict.at("comment").value);
    if (dict.contains("created by"))
        created_by_ = std::get<std::string_view>(dict.at("created by").value);
    if (dict.contains("encoding"))
        encoding_ = std::get<std::string_view>(dict.at("encoding").value);

    const auto info = std::get<Bencode::Dictionary>(dict.at("info").value);

    piece_length_ = std::get<i64>(info.at("piece length").value);

    const auto pieces = std::get<std::string_view>(info.at("pieces").value);
    if (pieces.size() % 20 != 0)
        throw std::runtime_error(fmt::format("Invalid pieces length: {}", pieces.size()));
    pieces_.reserve(pieces.size() / 20);
    for (size_t i = 0; i < pieces.size(); i += 20)
    {
        SHA1 sha1;
        std::memcpy(sha1.data(), pieces.data() + i, 20);
        pieces_.push_back(sha1);
    }

    if (info.contains("files"))
    {
        const auto& files = std::get<Bencode::List>(info.at("files").value);
        MultiFile multi_file;
        multi_file.name = std::get<std::string_view>(info.at("name").value);
        for (const auto& file_bc : files)
        {
            const auto& file = std::get<Bencode::Dictionary>(file_bc.value);
            TorrentFile torrent_file;
            torrent_file.length = std::get<i64>(file.at("length").value);
            for (const auto& path = std::get<Bencode::List>(file.at("path").value); const auto& part : path)
                torrent_file.path /= std::get<std::string_view>(part.value);
            multi_file.files.push_back(torrent_file);
        }
        file_info_ = multi_file;
    }
    else
    {
        TorrentFile torrent_file;
        torrent_file.length = std::get<i64>(info.at("length").value);
        torrent_file.path = std::get<std::string_view>(info.at("name").value);
        file_info_ = torrent_file;
    }
}
