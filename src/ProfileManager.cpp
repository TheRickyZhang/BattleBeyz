#include <iostream>
#include <fstream>
#include <json.hpp>
#include <filesystem>

#include "ProfileManager.h"

#include "Beyblade.h"
#include "MessageLog.h"

using namespace std;
using namespace nlohmann;

// TEMP to start off with defaults
void ProfileManager::addDefaultProfiles() {
    if (!createProfile("Default") || !createProfile("Player1")) {
        cerr << "Error: Failed to create default profiles." << endl;
        return;
    }
    if (!getActiveProfile()->createBeyblade("DefaultBeyblade1")) {
        cout << "Cannot create 1" << endl;
    }
    if (!getActiveProfile()->createBeyblade("DefaultBeyblade2")) {
        cout << "Cannot create 2" << endl;
    }
}


// SERVER: To replace with server requests for unique profileIds
bool ProfileManager::createProfile(const string& name) {
    lock_guard<mutex> lock(mtx);
    if (profiles.size() >= MAX_PROFILES) {
        cerr << "Error: Cannot add profile. Maximum number of profiles (" << MAX_PROFILES << ") reached." << endl;
        return false;
    }
    
    nextProfileID++;
    if (profiles.size() > 0 && getProfileIterator(nextProfileID) != profiles.end()) {
        return false;
    }
    auto profile = make_shared<Profile>(nextProfileID, name);
    profiles.push_back(profile);
    if (!activeProfileId.has_value()) {
        activeProfileId = profile->getId();  // Set newly added profile as active if none active
    }
    return true;
}

bool ProfileManager::addProfile(shared_ptr<Profile> profile) {
    lock_guard<mutex> lock(mtx);
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
    lock_guard<mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (profiles.size() <= 1 || it == profiles.end()) {
        // cerr << "Error: Profile with ID " << profileId << " not found." << endl;
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

shared_ptr<Profile> ProfileManager::getProfile(int profileId) const {
    lock_guard<mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (it == profiles.end()) return nullptr;
    return *it;
}

// Nullptr if no active profile
shared_ptr<Profile> ProfileManager::getActiveProfile() const {
    lock_guard<mutex> lock(mtx);
    if (!activeProfileId.has_value()) {
        return nullptr;
    }
    auto it = getProfileIterator(activeProfileId.value());
    if (it == profiles.end()) return nullptr;
    return *it;
}

// Sets a profile as the active profile by ID
bool ProfileManager::setActiveProfile(int profileId) {
    lock_guard<mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (it != profiles.end()) {
        activeProfileId = profileId;
        return true;
    }
    return false;
}

const vector<shared_ptr<Profile>>& ProfileManager::getAllProfiles() const {
    lock_guard<mutex> lock(mtx);
    return profiles;
}

void ProfileManager::saveProfilesToFile(const string& filePath) {
    MessageLog& ml = MessageLog::getInstance();
    json js;
    for (const auto& profile : profiles) {
        js["profiles"].push_back(profile->toJson()); // Save all relevant profile details
    }
    if (activeProfileId.has_value()) {
        js["activeProfileId"] = activeProfileId.value();
    }
    ofstream file(filePath);
    if (file.is_open()) {
        file << js.dump(4); // Pretty print with 4 spaces
        ml.addMessage("Profiles saved successfully to " + filePath);
    }
    else {
        ml.addMessage("Error: unable save profiles to " + filePath, MessageType::ERROR);
    }
}

void ProfileManager::loadProfilesFromFile(const string& filePath) {
    MessageLog& ml = MessageLog::getInstance();
    if (!filesystem::exists(filePath)) {
        ml.addMessage("Error: cannot load profiles from " + filePath, MessageType::ERROR);
        return;
    }

    ifstream file(filePath);
    if (!file.is_open()) {
        ml.addMessage("Error: Unable to open file for reading: " + filePath, MessageType::ERROR);
        return;
    }
    json js;
    file >> js;

    for (const auto& profileJson : js["profiles"]) {
        string name = profileJson["name"];
        int id = profileJson["id"];

        // Retrieve, populate, and add profile with ProfileManager
        auto profile = make_shared<Profile>(id, name);
        profile->fromJson(profileJson);

        cout << profile->getName() << endl;

        if (!addProfile(profile)) {
            cout << "FAILED TO ADD" << endl;
            ml.addMessage("Error: Failed to add profile during load. Name: ", MessageType::ERROR);
        }
        else {
            cout << " ADDED SUCCESSFULLY" << endl;
        }
    }

    // Set the active profile (optional: based on saved state)
    if (js.contains("activeProfileId")) {
        int activeId = js["activeProfileId"];
        if (!setActiveProfile(activeId)) {
            std::string loadedIds;
            for (const auto& profile : profiles) {
                loadedIds += std::to_string(profile->getId()) + " ";
            }
            ml.addMessage("Active profile ID " + std::to_string(activeId) + " does not match any loaded ID. Available IDs: [" + loadedIds + "]",
                MessageType::WARNING
            );
        }
    }
}


vector<shared_ptr<Profile>>::const_iterator ProfileManager::getProfileIterator(int profileId) const {
    return find_if(profiles.begin(), profiles.end(), [&](const shared_ptr<Profile> &p) {
        return p->getId() == profileId;
    });
}