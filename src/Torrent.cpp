#include <fmt/format.h>

#include "Torrent.h"
#include "Bencode.h"

Torrent::Torrent(const fs::path& file)
{
    if (!exists(file))
        throw std::runtime_error(fmt::format("File {} does not exist", file.string()));
    if (!is_regular_file(file))
        throw std::runtime_error(fmt::format("File {} is not a regular file", file.string()));
    if (file.extension() != ".torrent")
        throw std::runtime_error(fmt::format("File {} is not a .torrent file", file.string()));
    std::ifstream in(file);
    if (!in)
        throw std::runtime_error(fmt::format("Failed to open file {}", file.string()));
    const auto size = file_size(file);
    std::string torrent_file(size, '\0');
    in.read(torrent_file.data(), static_cast<std::streamsize>(size));
    in.close();
    *this = Torrent{std::string_view{torrent_file}};
}

Torrent::Torrent(const std::string_view metainfo)
{
    const auto bencode = Bencode::parse(metainfo);
    const auto dict = rva::get<Bencode::Dict>(bencode);
    announce_ = rva::get<std::string>(dict.at("announce"));
    if (dict.contains("announce-list"))
    {
        const auto& announce_list = rva::get<Bencode::List>(dict.at("announce-list"));
        std::vector<std::vector<std::string>> list;
        list.reserve(announce_list.size());
        for (const auto& tier_bc : announce_list)
        {
            const auto& tier = rva::get<Bencode::List>(tier_bc);
            std::vector<std::string> tier_list;
            tier_list.reserve(tier.size());
            for (const auto& url : tier)
                tier_list.push_back(rva::get<std::string>(url));
            list.push_back(tier_list);
        }
        announce_list_ = list;
    }
    if (dict.contains("creation date"))
    {
        const auto date = rva::get<i64>(dict.at("creation date"));
        creation_date_ = std::chrono::system_clock::time_point{std::chrono::seconds{date}};
    }
    if (dict.contains("comment"))
        comment_ = rva::get<std::string>(dict.at("comment"));
    if (dict.contains("created by"))
        created_by_ = rva::get<std::string>(dict.at("created by"));
    if (dict.contains("encoding"))
        encoding_ = rva::get<std::string>(dict.at("encoding"));

    const auto info = rva::get<Bencode::Dict>(dict.at("info"));

    piece_length_ = rva::get<i64>(info.at("piece length"));

    const auto pieces = rva::get<std::string>(info.at("pieces"));
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
        const auto& files = rva::get<Bencode::List>(info.at("files"));
        MultiFile multi_file;
        multi_file.name = rva::get<std::string>(info.at("name"));
        for (const auto& file_bc : files)
        {
            const auto& file = rva::get<Bencode::Dict>(file_bc);
            TorrentFile torrent_file;
            torrent_file.length = rva::get<i64>(file.at("length"));
            for (const auto& path = rva::get<Bencode::List>(file.at("path")); const auto& part : path)
                torrent_file.path /= rva::get<std::string>(part);
            multi_file.files.push_back(torrent_file);
        }
        file_info_ = multi_file;
    }
    else
    {
        TorrentFile torrent_file;
        torrent_file.length = rva::get<i64>(info.at("length"));
        torrent_file.path = rva::get<std::string>(info.at("name"));
        file_info_ = torrent_file;
    }
}
