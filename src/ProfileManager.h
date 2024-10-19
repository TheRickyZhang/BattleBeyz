#pragma once

#include "Profile.h"
#include "Beyblade.h"
#include <unordered_map>
#include <memory>
#include <string>

class ProfileManager {
public:
    // Get the singleton instance
    static ProfileManager& getInstance() {
        static ProfileManager instance;
        return instance;
    }

    // Deleted to prevent copying or assignment
    ProfileManager(const ProfileManager&) = delete;
    ProfileManager& operator=(const ProfileManager&) = delete;

    // Profile management methods
    bool addProfile(const std::string& name);
    bool removeProfile(const std::string& name);
    std::shared_ptr<Profile> getProfile(const std::string& name) const;
    std::vector<std::shared_ptr<Profile>> getAllProfiles() const;
    std::shared_ptr<Profile> getActiveProfile() const;
    std::vector<std::shared_ptr<Beyblade>>& getBeybladesForActiveProfile();

    bool setActiveProfile(const std::string& name);

private:
    ProfileManager() = default; // Private constructor
    std::unordered_map<std::string, std::shared_ptr<Profile>> profiles;
    std::shared_ptr<Profile> activeProfile;
};
