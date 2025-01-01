#include <iostream>

#include "Profile.h"

#include "Beyblade.h"
#include "MessageLog.h"

using namespace std;
using namespace nlohmann;

// SERVER: (need globally unique beybladeIds across multiple different profiles)
bool Profile::createBeyblade(const string& name, bool isTemplate) {
    MessageLog::getInstance().addMessage("Attempting to create Beyblade: "+name, MessageType::NORMAL);
    lock_guard<mutex> lock(mtx);
    if (beybladesOwned.size() >= MAX_BEYBLADES_PER_PROFILE) {
        cerr << "Error: Cannot add Beyblade. Maximum number of beyblades (" << MAX_BEYBLADES_PER_PROFILE << ") reached." << endl;
        return false;
    }

    nextBladeID++;
    if (!beybladesOwned.empty() && getBeybladeIterator(nextBladeID) != beybladesOwned.end()) {
        cerr << "Error: Beyblade with ID " << nextBladeID << " already exists." << endl;
        return false;
    }

    shared_ptr<Beyblade> beyblade = isTemplate
        ? make_shared<Beyblade>(nextBladeID, name, true)
        : make_shared<Beyblade>(nextBladeID, name);
    return addBeyblade(beyblade);

    // May be unnecessary if the addBeyblade call is logically equivalent
    
    //shared_ptr<Beyblade> beyblade;
    //if (isTemplate) {
    //    beyblade = make_shared<Beyblade>(nextBladeID, name, true);
    //}
    //else {
    //    beyblade = make_shared<Beyblade>(nextBladeID, name);
    //}

    //beybladesOwned.push_back(beyblade);
    //if (!activeBeybladeId.has_value()) {
    //    activeBeybladeId = beyblade->getId();
    //}
    //MessageLog::getInstance().addMessage("Beyblade created successfully with ID: " + beyblade->getId(), MessageType::NORMAL);
    //return true;
}


// Return false if beyblade already exists or max beyblades reached.
bool Profile::addBeyblade(shared_ptr<Beyblade> beyblade) {
    lock_guard<mutex> lock(mtx);
    if (getBeybladeIterator(beyblade->getId()) != beybladesOwned.end()) {
        MessageLog::getInstance().addMessage("Error: Already exists beyblade with ID " + beyblade->getId(), MessageType::WARNING);
        return false;
    }
    if (beybladesOwned.size() >= MAX_BEYBLADES_PER_PROFILE) {
        MessageLog::getInstance().addMessage("Max beyblades per profile reached", MessageType::WARNING);
        return false;
    }
    beybladesOwned.push_back(beyblade);
    if (!activeBeybladeId.has_value()) {
        activeBeybladeId = beyblade->getId();
    }
    MessageLog::getInstance().addMessage("Beyblade added successfully with ID: " + beyblade->getId());
    return true;
}


bool Profile::deleteBeyblade(int beybladeId) {
    lock_guard<mutex> lock(mtx);
    auto it = getBeybladeIterator(beybladeId);
    if (it == beybladesOwned.end()) {
        cerr << "Error: Beyblade with ID " << beybladeId << " not found in profile " << id << "." << endl;
        return false;
    }

    // Erase the Beyblade first to ensure safe access
    bool wasSelected = (activeBeybladeId && *activeBeybladeId == beybladeId);
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
    lock_guard<mutex> lock(mtx);
    auto it = getBeybladeIterator(beybladeId);
    if (it == beybladesOwned.end()) return nullptr;
    return *it;
}

// Take an optional id, or uses active one by default.
// Returns the beyblade pointer or nullptr if not found
shared_ptr<Beyblade> Profile::getActiveBeyblade() const {
    lock_guard<mutex> lock(mtx);
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

bool Profile::setActiveBeyblade(int beybladeId) {
    lock_guard<mutex> lock(mtx);
    auto it = getBeybladeIterator(beybladeId);
    if (it != beybladesOwned.end()) {
        activeBeybladeId = beybladeId;
        return true;
    }
    return false;
}

const vector<shared_ptr<Beyblade>>& Profile::getAllBeyblades() const {
    lock_guard<mutex> lock(mtx);
    return beybladesOwned;
}

json Profile::toJson() const {
    json js;
    js["id"] = id;
    js["name"] = name;
    js["settings"] = settings.toJson();
    for (const auto& beyblade : beybladesOwned) {
        js["beyblades"].push_back(beyblade->toJson());
    }
    return js;
}

void Profile::fromJson(const json& js) {
    id = js["id"];
    name = js["name"];
    settings.fromJson(js["settings"]);
    if (js.contains("beyblades")) {
        for (const auto& beybladeJson : js["beyblades"]) {
            shared_ptr<Beyblade> beyblade = make_unique<Beyblade>(Beyblade::fromJson(beybladeJson));
            if (!addBeyblade(beyblade)) {
                MessageLog::getInstance().addMessage("Failed to add beyblade " + beyblade->getName(), MessageType::ERROR);
            }
            else {
                cout << "   ADDED " << beyblade->getName() << " SUCCESSFULLY" << endl;
            }
        }
    }
}

// Can convert to map, but with <= 50 beys per profile lookup time is negligible
vector<shared_ptr<Beyblade>>::const_iterator Profile::getBeybladeIterator(int id) const {
  return find_if(
      beybladesOwned.begin(), beybladesOwned.end(),
      [&](const shared_ptr<Beyblade> &b) { return b->getId() == id; });
}