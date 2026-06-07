#include "cached_http.h"
#include <iostream>

CachedHttp::CachedHttp(std::shared_ptr<IHttp> inner, size_t maxSize)
    : inner_(std::move(inner)), maxSize_(maxSize) {}

std::optional<std::string> CachedHttp::get(const std::string& url,
                                             long timeout)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = cache_.find(url);
    if (it != cache_.end()) {
        hits_++;
        return it->second;
    }

    misses_++;
    auto result = inner_->get(url, timeout);

    if (result && !result->empty()) {
        if (cache_.size() >= maxSize_)
            cache_.erase(cache_.begin());

        cache_[url] = *result;
    }

    return result;
}

size_t CachedHttp::cacheSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_.size();
}

void CachedHttp::clearCache() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
    std::cout << "[Cache] Cleared\n";
}
