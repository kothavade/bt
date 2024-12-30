#pragma once

#include <string>

namespace bt {

class Tracker {
public:
    Tracker() = default;
    Tracker(const std::string& url) : url{url} {}

    auto get_url() const -> const std::string& { return url; }

private:
    std::string url;
};

class HTTPTracker : public Tracker {
public:
    HTTPTracker() = default;
    HTTPTracker(const std::string& url) : Tracker{url} {}
};

}