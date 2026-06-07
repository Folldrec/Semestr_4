#include "provider_registry.h"
#include <algorithm>

ProviderRegistry& ProviderRegistry::instance() {
    static ProviderRegistry inst;
    return inst;
}

void ProviderRegistry::add(std::unique_ptr<IProvider> provider) {
    providers_.push_back(std::move(provider));
}

IProvider* ProviderRegistry::get(const std::string& name) const {
    for (const auto& p : providers_)
        if (p->name() == name) return p.get();
    return nullptr;
}

const std::vector<std::unique_ptr<IProvider>>& ProviderRegistry::all() const {
    return providers_;
}

std::vector<IProvider*> ProviderRegistry::configured() const {
    std::vector<IProvider*> out;
    for (const auto& p : providers_)
        if (p->isConfigured()) out.push_back(p.get());
    return out;
}

std::vector<std::string> ProviderRegistry::names() const {
    std::vector<std::string> out;
    for (const auto& p : providers_) out.push_back(p->name());
    return out;
}
