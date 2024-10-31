#include "ProfileManager.h"

// TEMP to start off with defaults
void ProfileManager::addDefaultProfiles() {
    if (!createProfile("Default") || !createProfile("Player1")) {
        std::cerr << "Error: Failed to create default profiles." << std::endl;
        return;
    }
    if (!getActiveProfile()->createBeyblade("DefaultBeyblade1")) {
        std::cout << "Cannot create 1" << std::endl;
    }
    if (!getActiveProfile()->createBeyblade("DefaultBeyblade2")) {
        std::cout << "Cannot create 2" << std::endl;
    }
}


// SERVER: To replace with server requests for unique profileIds
bool ProfileManager::createProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    if (profiles.size() >= MAX_PROFILES) {
        std::cerr << "Error: Cannot add profile. Maximum number of profiles (" << MAX_PROFILES << ") reached." << std::endl;
        return false;
    }
    
    
    nextProfileID++;
    if (profiles.size() > 0 && getProfileIterator(nextProfileID) != profiles.end()) {
        return false;
    }
    auto profile = std::make_shared<Profile>(nextProfileID, name);
    profiles.push_back(profile);
    if (!activeProfileId.has_value()) {
        activeProfileId = profile->getId();  // Set newly added profile as active if none active
    }
    return true;
}

bool ProfileManager::addProfile(std::shared_ptr<Profile> profile) {
    std::lock_guard<std::mutex> lock(mtx);
    if (profiles.size() >= MAX_PROFILES) {
        return false;
    }
    if (getProfileIterator(profile->getId()) != profiles.end()) {
        return false;
    }
    profiles.push_back(profile);
    if (!activeProfileId.has_value()) {
        activeProfileId = profile->getId();
    }
    return true;
}

// Deletes a profile by ID
bool ProfileManager::deleteProfile(int profileId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (it == profiles.end()) {
        std::cerr << "Error: Profile with ID " << profileId << " not found." << std::endl;
        return false;
    }

    bool wasSelected = (activeProfileId && *activeProfileId == profileId);
    profiles.erase(it);
    if (wasSelected) {
        if (!profiles.empty()) {
            activeProfileId = profiles.front()->getId(); // Default to the first remaining profile
        }
        else {
            activeProfileId.reset();
        }
    }
    return true;
}

std::shared_ptr<Profile> ProfileManager::getProfile(int profileId) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (it == profiles.end()) return nullptr;
    return *it;
}

// Nullptr if no active profile
std::shared_ptr<Profile> ProfileManager::getActiveProfile() const {
    std::lock_guard<std::mutex> lock(mtx);
    if (!activeProfileId.has_value()) {
        return nullptr;
    }
    auto it = getProfileIterator(activeProfileId.value());
    if (it == profiles.end()) return nullptr;
    return *it;
}

// Sets a profile as the active profile by ID
bool ProfileManager::setActiveProfile(int profileId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (it != profiles.end()) {
        activeProfileId = profileId;
        return true;
    }
    return false;
}

const std::vector<std::shared_ptr<Profile>>& ProfileManager::getAllProfiles() const {
    std::lock_guard<std::mutex> lock(mtx);
    return profiles;
}
