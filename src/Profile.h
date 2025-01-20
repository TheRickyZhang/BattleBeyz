#pragma once

#include <algorithm>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <json.hpp>

#include "Settings.h"

struct Layer; struct Disc; struct Driver;
class Beyblade;
class Stadium;

/*
* Contains information about beyblades, stats, and settings for a user's profile. 
* 
* Query by ID
*/
class Profile {
public:
    static constexpr size_t MAX_BEYBLADES_PER_PROFILE = 50;
    static constexpr size_t MAX_STADIUMS_PER_PROFILE = 10;

    // TODO: Flesh out settings
    Settings settings;

    // This id is ALSO globally unique, should be handled by profileManager
    Profile() : id(-1), name("") {}
    Profile(int id, const std::string& name) : id(id), name(name), activeBeybladeId(std::nullopt) {}

    int getId() const { return id; }
    std::string getName() const { return name; }

    nlohmann::json toJson() const;
    bool fromJson(const nlohmann::json& json);


    // CRUD (Create, Delete, Get, Set) Operations
    // When using a server, user serverAPI.createBeyblade(profileId, name) to create a new beyblade
    
    // BEYBLADE
    bool createBeyblade(const std::string& name, bool isTemplate=false);
    bool deleteBeyblade(int beybladeId);
    std::shared_ptr<Beyblade> getBeyblade(int beybladeId) const;
    bool setActiveBeyblade(int beybladeId);

    const std::vector<std::shared_ptr<Beyblade>>& getAllBeyblades() const;
    std::shared_ptr<Beyblade> getActiveBeyblade() const;  // Thread-safe for retrieval process, but the returned pointer is not thread-safe

    // STADIUM
    bool createStadium(const std::string& name);
    bool deleteStadium(int stadiumId);
    std::shared_ptr<Stadium> getStadium(int stadiumId) const;
    bool setActiveStadium(int stadiumId);

    const std::vector<std::shared_ptr<Stadium>>& getAllStadiums() const;
    std::shared_ptr<Stadium> getActiveStadium() const;  // Thread-safe for retrieval process, but the returned pointer is not thread-safe

    std::optional<int> activeStadiumId;
private:
    mutable std::mutex mtx;

    // TODO: Set up basic server sync
    int id; // This is PROFILE ID! NOT any of the ids it generates.
    std::vector<std::shared_ptr<Beyblade>> beybladesOwned{};   // List of Beyblades owned by the profile
    std::vector<std::shared_ptr<Stadium>> stadiumsOwned{};     // Stadiums owned by the profile
    std::optional<int> activeBeybladeId;
    std::string name{};

    // IDs start from 1
    int nextBeybladeId = 0;
    int nextStadiumID = 0;


    // Helper functions for implementing CRUD functionality
    bool addBeyblade(std::shared_ptr<Beyblade> beyblade);
    std::vector<std::shared_ptr<Beyblade>>::const_iterator getBeybladeIterator(int beybladeId) const;

    bool addStadium(std::shared_ptr<Stadium> stadium);
    std::vector<std::shared_ptr<Stadium>>::const_iterator getStadiumIterator(int stadiumId) const;
};
