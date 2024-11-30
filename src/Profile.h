#pragma once

#include "Beyblade.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <optional>
#include <mutex>

struct Layer; struct Disc; struct Driver;
class Profile {
public:
    static constexpr size_t MAX_BEYBLADES_PER_PROFILE = 50;

    // This id is ALSO globally unique, should be handled by profileManager
    Profile(int id, const std::string& name)
        : id(id), name(name), activeBeybladeId(std::nullopt) {}

    int getId() const { return id; }
    std::string getName() const { return name; }

    // When using a server, user serverAPI.createBeyblade(profileId, name) to create a new beyblade
    bool createBeyblade(const std::string& name, bool isTemplate=false);
    bool addBeyblade(std::shared_ptr<Beyblade> beyblade);
    bool deleteBeyblade(int beybladeId);

    std::shared_ptr<Beyblade> getBeyblade(int beybladeId) const;
    std::shared_ptr<Beyblade> getActiveBeyblade() const;  // Thread-safe for retrieval process, but the returned pointer is not thread-safe
    bool setActiveBeyblade(int beybladeId);

    // Retrieves the list of owned Beyblades
    const std::vector<std::shared_ptr<Beyblade>>& getAllBeyblades() const;

private:
    int id; // This is PROFILE ID!
    std::vector<std::shared_ptr<Beyblade>> beybladesOwned{};    // List of Beyblades owned by the profile
    std::optional<int> activeBeybladeId;
    std::string name{};
    int nextBladeID = 0;                    // Next blade id is #1.


    mutable std::mutex mtx;

    // Can convert to map, but with <= 50 beys per profile lookup time is negligible
    std::vector<std::shared_ptr<Beyblade>>::const_iterator getBeybladeIterator(int id) const {
        return std::find_if(beybladesOwned.begin(), beybladesOwned.end(),
            [&](const std::shared_ptr<Beyblade>& b) { return b->getId() == id; });
    }
};
