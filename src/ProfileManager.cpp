#include "ProfileManager.h"

bool ProfileManager::addProfile(const std::string& name) {
    if (profiles.find(name) != profiles.end()) {
        return false; // Profile already exists
    }
    profiles[name] = std::make_shared<Profile>(name);
    return true;
}

bool ProfileManager::removeProfile(const std::string& name) {
    return profiles.erase(name) > 0;
}

std::shared_ptr<Profile> ProfileManager::getProfile(const std::string& name) const {
    auto it = profiles.find(name);
    if (it != profiles.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Profile>> ProfileManager::getAllProfiles() const {
    std::vector<std::shared_ptr<Profile>> result;
    for (const auto& pair : profiles) {
        result.push_back(pair.second);
    }
    return result;
}

bool ProfileManager::setActiveProfile(const std::string& name) {
    auto profile = getProfile(name);
    if (profile) {
        activeProfile = profile;
        return true;
    }
    return false;
}

std::shared_ptr<Profile> ProfileManager::getActiveProfile() const {
    return activeProfile;
}

std::vector<std::shared_ptr<Beyblade>>& ProfileManager::getBeybladesForActiveProfile() {
    return activeProfile->getBeybladesOwned();
}
