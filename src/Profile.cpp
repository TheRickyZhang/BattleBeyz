#include <iostream>

#include "Profile.h"

#include "Beyblade.h"
#include "Stadium.h"
#include "MessageLog.h"

using namespace std;
using namespace nlohmann;

// JSON SERIALIZATION

/*
* Converts the Profile instance to a JSON representation.
* 
* @return [out] A json object containing: id, name, settings, beyblades { }
*/
json Profile::toJson() const {
    json js;
    js["id"] = id;
    js["name"] = name;
    js["settings"] = settings.toJson();
    for (const auto& beyblade : beybladesOwned) {
        js["beyblades"].push_back(beyblade->toJson());
    }
    for (const auto& stadium : stadiumsOwned) {
        js["stadiums"].push_back(stadium->toJson());
    }
    return js;
}

/**
 * Populates this profile object given a JSON object.
 *
 * @param [in] js A JSON object containing: id, name, settings, beyblades {}
 */
bool Profile::fromJson(const json& js) {
    try {
        id = js.at("id");
        name = js.at("name");
        if (js.contains("settings")) {
            settings.fromJson(js["settings"]);
        }
        if (js.contains("beyblades")) {
            for (const auto& beybladeJson : js["beyblades"]) {
                auto beyblade = make_shared<Beyblade>(Beyblade::fromJson(beybladeJson));
                if (!addBeyblade(beyblade)) {
                    MessageLog::getInstance().addMessage("Failed to add beyblade " + beyblade->getName(), MessageType::ERROR);
                }
            }

        }
        if (js.contains("stadiums")) {
            for (const auto& stadiumJson : js["stadiums"]) {
                auto stadium = make_shared<Stadium>(Stadium::fromJson(stadiumJson));
                if (!addStadium(stadium)) {
                    MessageLog::getInstance().addMessage("Failed to add stadium " + stadium->getName(), MessageType::ERROR);
                }
            }

        }
    }
    catch (const json::exception& e) {
        MessageLog::getInstance().addMessage("Error parsing JSON: " + std::string(e.what()), MessageType::ERROR);
        return false;
    }
    return true;
}

/*
* -----------------------------BEYBLADE LOGIC------------------------------------
*/

// SERVER: (need globally unique beybladeIds across multiple different profiles)
bool Profile::createBeyblade(const string& name, bool isTemplate) {
    if (beybladesOwned.size() >= MAX_BEYBLADES_PER_PROFILE) {
        MessageLog::getInstance().addMessage("Max beyblade count (" + to_string(MAX_BEYBLADES_PER_PROFILE) + ") reached.", MessageType::ERROR);
        return false;
    }
    while (getBeybladeIterator(nextBeybladeId) != beybladesOwned.end()) {
        nextBeybladeId++;
    }
    if (!beybladesOwned.empty() && getBeybladeIterator(nextBeybladeId) != beybladesOwned.end()) {
        MessageLog::getInstance().addMessage("Error: Beyblade with ID " + to_string(nextBeybladeId) + " already exists.", MessageType::ERROR);
        return false;
    }
    shared_ptr<Beyblade> beyblade = isTemplate
        ? make_shared<Beyblade>(nextBeybladeId, name, true)
        : make_shared<Beyblade>(nextBeybladeId, name);
    return addBeyblade(beyblade);
}

/*
* Deletes a Beyblade with the given beybladeId. 
* 
* @param  [in]  a beyblade Id
* @return [out] a boolean indicating whether the operation was successful. 
*/
bool Profile::deleteBeyblade(int beybladeId) {
    auto it = getBeybladeIterator(beybladeId);
    if (it == beybladesOwned.end()) {
        return false;
    }
    bool wasSelected = (activeBeybladeId && *activeBeybladeId == beybladeId);

    // Erase the Beyblade first to ensure safe access
    beybladesOwned.erase(it);
    if (wasSelected) {
        activeBeybladeId.reset();
        if (!beybladesOwned.empty()) {
            activeBeybladeId = beybladesOwned.front()->getId(); // Default to the first remaining Beyblade
        }
    }
    return true;
}

shared_ptr<Beyblade> Profile::getBeyblade(int beybladeId) const {
    auto it = getBeybladeIterator(beybladeId);
    if (it == beybladesOwned.end()) return nullptr;
    return *it;
}

bool Profile::setActiveBeyblade(int beybladeId) {
    auto it = getBeybladeIterator(beybladeId);
    if (it != beybladesOwned.end()) {
        activeBeybladeId = beybladeId;
        return true;
    }
    return false;
}


// Take an optional id, or uses active one by default.
// Returns the beyblade pointer or nullptr if not found
shared_ptr<Beyblade> Profile::getActiveBeyblade() const {
    if (!activeBeybladeId.has_value()) {
        return nullptr;
    }
    auto it = getBeybladeIterator(activeBeybladeId.value());
    if (it != beybladesOwned.end()) {
        return *it;
    }
    else {
        return nullptr;
    }
}

const vector<shared_ptr<Beyblade>>& Profile::getAllBeyblades() const {
    return beybladesOwned;
}

/*
* -----------------------------STADIUM LOGIC------------------------------------
*/
bool Profile::createStadium(const string& name) {
    if (stadiumsOwned.size() >= MAX_STADIUMS_PER_PROFILE) return false;

    while (getStadiumIterator(nextStadiumID) != stadiumsOwned.end()) {
        nextStadiumID++;
    }
    if (!stadiumsOwned.empty() && getStadiumIterator(nextStadiumID) != stadiumsOwned.end()) {
        MessageLog::getInstance().addMessage("Error: Stadium with ID " + to_string(nextBeybladeId) + " already exists.", MessageType::ERROR);
        return false;
    }
    auto stadium = make_shared<Stadium>(nextStadiumID, name);
    return addStadium(stadium);
}

bool Profile::deleteStadium(int stadiumId) {
    auto it = getStadiumIterator(stadiumId);
    if (it == stadiumsOwned.end()) return false;

    stadiumsOwned.erase(it);
    return true;
}

shared_ptr<Stadium> Profile::getStadium(int stadiumId) const {
    auto it = getStadiumIterator(stadiumId);
    return it != stadiumsOwned.end() ? *it : nullptr;
}

bool Profile::setActiveStadium(int stadiumId)
{
    auto it = getStadiumIterator(stadiumId);
    if (it != stadiumsOwned.end()) {
        activeBeybladeId = stadiumId;
        return true;
    }
    return false;
}

const vector<shared_ptr<Stadium>>& Profile::getAllStadiums() const {
    return stadiumsOwned;
}

std::shared_ptr<Stadium> Profile::getActiveStadium() const
{
    if (!activeStadiumId.has_value()) {
        return nullptr;
    }
    auto it = getStadiumIterator(activeBeybladeId.value());
    if (it != stadiumsOwned.end()) {
        return *it;
    }
    else {
        return nullptr;
    }
}



/*
* -----------------------------HELPER LOGIC------------------------------------
*/


/*
* A helper function used in createBeyblade
*
* @param  [in]  a shared pointer to a Beyblade with an existing non-conflicting id
* @return [out] a boolean indicating whether the operation was successful.
*/
bool Profile::addBeyblade(shared_ptr<Beyblade> beyblade) {
    if (getBeybladeIterator(beyblade->getId()) != beybladesOwned.end()) {
        return false;
    }
    if (beybladesOwned.size() >= MAX_BEYBLADES_PER_PROFILE) {
        return false;
    }
    beybladesOwned.push_back(beyblade);
    if (!activeBeybladeId.has_value()) {
        activeBeybladeId = beyblade->getId();
    }
    return true;
}

// Can convert to map, but with <= 50 beys per profile lookup time is negligible
vector<shared_ptr<Beyblade>>::const_iterator Profile::getBeybladeIterator(int beybladeId) const {
  return find_if(
      beybladesOwned.begin(), beybladesOwned.end(),
      [&](const shared_ptr<Beyblade> &b) { return b->getId() == beybladeId; });
}


bool Profile::addStadium(shared_ptr<Stadium> stadium) {
    if (getStadiumIterator(stadium->getId()) != stadiumsOwned.end() ||
        stadiumsOwned.size() >= MAX_STADIUMS_PER_PROFILE) return false;

    stadiumsOwned.push_back(stadium);
    return true;
}

vector<shared_ptr<Stadium>>::const_iterator Profile::getStadiumIterator(int stadiumId) const {
    return find_if(stadiumsOwned.begin(), stadiumsOwned.end(),
        [&](const shared_ptr<Stadium>& s) { return s->getId() == stadiumId; });
}