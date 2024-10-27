#pragma once

#include "Profile.h"
#include "Beyblade.h"
#include <memory>
#include <string>

class ProfileManager {
public:
    static constexpr size_t MAX_PROFILES = 5;

    static ProfileManager& getInstance() {
        static ProfileManager instance;
        return instance;
    }

    ProfileManager(const ProfileManager&) = delete;
    ProfileManager& operator=(const ProfileManager&) = delete;

    void addDefaultProfiles();

    bool createProfile(const std::string& name);
    bool addProfile(std::shared_ptr<Profile> profile);
    bool deleteProfile(int profileId);
    std::shared_ptr<Profile> getProfile(int profileId) const;
    std::shared_ptr<Profile> getActiveProfile() const;
    bool setActiveProfile(int profileId);
    const std::vector<std::shared_ptr<Profile>>& getAllProfiles() const;
private:
    ProfileManager() = default;

    int currentTempId = 1;
    std::optional<int> activeProfileId;
    std::vector<std::shared_ptr<Profile>> profiles;
    mutable std::mutex mtx;

    std::vector<std::shared_ptr<Profile>>::const_iterator getProfileIterator(int profileId) const {
        return std::find_if(profiles.begin(), profiles.end(),
            [&](const std::shared_ptr<Profile>& p) { return p->getId() == profileId; });
    }
};
