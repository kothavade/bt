#pragma once

#include <ada.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <chrono>
#include <filesystem>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>

#include "Aliases.h"
#include "SHA1.h"

namespace Typhoon {
struct TorrentFile {
    fs::path path;
    i64 length;
};

struct MultiFile {
    std::string name;
    std::vector<TorrentFile> files;
};

class Torrent {
   public:
    explicit Torrent(std::string_view metainfo);
    static Torrent from_file(const fs::path &file);

    [[nodiscard]] URL announce() const { return announce_; }
    [[nodiscard]] std::string info_hash() const { return info_hash_; }
    [[nodiscard]] i64 piece_length() const { return piece_length_; }
    [[nodiscard]] const std::vector<std::string> &pieces() const { return pieces_; }
    [[nodiscard]] const std::variant<TorrentFile, MultiFile> &file_info() const {
        return file_info_;
    }

   private:
    /// SHA1 hash of the bencoded info key
    std::string info_hash_;
    /// Announce URL of the tracker
    URL announce_;
    /// List of lists of URLs, containing a list of tiers of announces
    std::optional<std::vector<std::vector<URL>>> announce_list_;
    /// The creation time of the torrent
    std::optional<std::chrono::time_point<std::chrono::system_clock>> creation_date_;
    /// Free-form textual comments of the author
    std::optional<std::string> comment_;
    /// Name and version of the program used to create the .torrent
    std::optional<std::string> created_by_;
    /// The string encoding format used to generate the pieces
    std::optional<std::string> encoding_;
    /// Number of bytes in each piece
    i64 piece_length_;
    /// SHA-1 hash values for each piece
    std::vector<std::string> pieces_;
    /// In single-file mode, the file to save to.
    /// In multiple-file mode, a list of files with paths and lengths.
    std::variant<TorrentFile, MultiFile> file_info_;
    /// If true, the client *must* publish its presence to get other peers
    /// *only* via the trackers explicitly described in the metainfo file
    std::optional<bool> private_;

    friend struct fmt::formatter<Torrent>;
};
}

template <>
struct fmt::formatter<Typhoon::Torrent> {
    static constexpr auto parse(const format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Typhoon::Torrent &t, format_context &ctx) const {
        auto out = ctx.out();

        // Format required fields
        out = format_to(out, "Torrent{{\n");
        out = format_to(out, "Info Hash: {}\n", t.info_hash_);
        out = format_to(out, "Announce: {}\n", t.announce_.to_string());
        out = format_to(out, "Piece Length: {} bytes\n", t.piece_length_);
        out = format_to(out, "Pieces: {} SHA1 hashes\n", t.pieces_.size());

        // Format optional fields
        if (t.announce_list_) {
            out = format_to(out, "Announce List:\n");
            for (const auto &tier : *t.announce_list_) {
                // TODO: fix fmt::join
                // out = format_to(out, " {}\n", join(tier, ", "));
                out = format_to(out, "  [");
                for (const auto &url : tier) {
                    out = format_to(out, "{}", url.to_string());
                    if (&url != &tier.back()) {
                        out = format_to(out, ", ");
                    }
                }
                out = format_to(out, "]\n");
            }
        }
        if (t.creation_date_) {
            out = format_to(out, "Created: {}\n", *t.creation_date_);
        }
        if (t.comment_) {
            out = format_to(out, "Comment: {}\n", *t.comment_);
        }
        if (t.created_by_) {
            out = format_to(out, "Created By: {}\n", *t.created_by_);
        }
        if (t.encoding_) {
            out = format_to(out, "Encoding: {}\n", *t.encoding_);
        }
        if (t.private_) {
            out = format_to(out, "Private: {}\n", *t.private_);
        }

        // Format file info
        if (std::holds_alternative<Typhoon::TorrentFile>(t.file_info_)) {
            const auto &[path, length] = std::get<Typhoon::TorrentFile>(t.file_info_);
            out = format_to(out, "Single File: {} ({} bytes)\n", path.string(), length);
        } else {
            const auto &[name, files] = std::get<Typhoon::MultiFile>(t.file_info_);
            out = format_to(out, "Multi File - Name: {}\nFiles:\n", name);
            for (const auto &[path, length] : files) {
                out = format_to(out, "  {} ({} bytes)\n", path.string(), length);
            }
        }

        out = format_to(out, "}}");

        return out;
    }
};
