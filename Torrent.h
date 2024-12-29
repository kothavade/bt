#pragma once

#include <string_view>
#include <vector>
#include <variant>
#include <optional>
#include <chrono>
#include <filesystem>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include "Aliases.h"


namespace fs = std::filesystem;

struct TorrentFile
{
    fs::path path;
    int length;
};

struct MultiFile
{
    std::string_view name;
    std::vector<TorrentFile> files;
};

using SHA1 = std::array<std::byte, 20>;

class Torrent
{
public:
    explicit Torrent(const std::string_view metainfo);

private:
    /// Announce URL of the tracker
    std::string_view announce_;
    /// List of lists of URLs, containing a list of tiers of announces
    std::optional<std::vector<std::vector<std::string_view>>> announce_list_;
    /// The creation time of the torrent
    std::optional<std::chrono::time_point<std::chrono::system_clock>> creation_date_;
    /// Free-form textual comments of the author
    std::optional<std::string_view> comment_;
    /// Name and version of the program used to create the .torrent
    std::optional<std::string_view> created_by_;
    /// The string encoding format used to generate the pieces
    std::optional<std::string_view> encoding_;
    /// Number of bytes in each piece
    u32 piece_length_;
    /// SHA-1 hash values for each piece
    std::vector<SHA1> pieces_;
    /// In single-file mode, the file to save to.
    /// In multiple-file mode, a list of files with paths and lengths.
    std::variant<TorrentFile, MultiFile> file_info_;
    /// If true, the client *must* publish its presence to get other peers
    /// *only* via the trackers explicitly described in the metainfo file
    std::optional<bool> private_;

    friend struct fmt::formatter<Torrent>;
};


template <>
struct fmt::formatter<Torrent>
{
    static constexpr auto parse(const format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const Torrent& t, format_context& ctx) const
    {
        auto out = ctx.out();

        // Format required fields
        out = format_to(out, "Torrent{{\n");
        out = format_to(out, "Announce: {}\n", t.announce_);
        out = format_to(out, "Piece Length: {} bytes\n", t.piece_length_);
        out = format_to(out, "Pieces: {} SHA1 hashes\n", t.pieces_.size());

        // Format optional fields
        if (t.announce_list_)
        {
            out = format_to(out, "Announce List: {}\n", join(*t.announce_list_, ", "));
        }
        if (t.creation_date_)
        {
            out = format_to(out, "Created: {}\n", *t.creation_date_);
        }
        if (t.comment_)
        {
            out = format_to(out, "Comment: {}\n", *t.comment_);
        }
        if (t.created_by_)
        {
            out = format_to(out, "Created By: {}\n", *t.created_by_);
        }
        if (t.encoding_)
        {
            out = format_to(out, "Encoding: {}\n", *t.encoding_);
        }
        if (t.private_)
        {
            out = format_to(out, "Private: {}\n", *t.private_);
        }

        // Format file info
        if (std::holds_alternative<TorrentFile>(t.file_info_))
        {
            const auto& [path, length] = std::get<TorrentFile>(t.file_info_);
            out = format_to(out, "Single File: {} ({} bytes)\n",
                            path.string(), length);
        }
        else
        {
            const auto& [name, files] = std::get<MultiFile>(t.file_info_);
            out = format_to(out, "Multi File - Name: {}\nFiles:\n", name);
            for (const auto& [path, length] : files)
            {
                out = format_to(out, "  {} ({} bytes)\n",
                                path.string(), length);
            }
        }

        out = format_to(out, "}}");

        return out;
    }
};
