// TODO
// OK does not save -- should it?
//
// How to get error reasons during profile processing?
//
// Dropdown lists are difficult to read. Need different background or border.

const int MAX_PROFILE_NAME_LENGTH = 30;

static int activeBlade = 0;                 // Blade 0 or Blade 1
static int activeBladeOld = 0;              // Previous value
static std::vector<int> activeProfileItem;  // Index for Which profile is active in each blade.
static GameControl* gameControl = nullptr;
static char newProfileName[MAX_PROFILE_NAME_LENGTH];
static std::vector<std::string> profileItems;  // Names of selectable profiles
static bool showPromptForProfile = false;

// These are the configuraable values; there's one item per possible blade.

static std::vector<float> discMass;
static std::vector<float> discMomentOfInertia;
static std::vector<float> driverCoefficientOfFriction;
static std::vector<float> driverMass;
static std::vector<float> driverMomentOfInertia;
static std::vector<float> layerCoefficientOfRestitution;
static std::vector<float> layerMass;
static std::vector<float> layerMomentOfInertia;
static std::vector<float> layerRecoilDistributionMean;
static std::vector<float> layerRecoilDistributionStdDev;

// Data for information screen

static std::string runState = "Idle";
bool runStateIsError = false;

static void setupBackground(GLFWwindow *window, Texture &backgroundTexture) {
    auto *data = static_cast<GameControl *>(glfwGetWindowUserPointer(window));
    auto windowWidth = *data->windowWidth;
    auto windowHeight = *data->windowHeight;
    auto backgroundShader = data->backgroundShader;
    auto quadRenderer = data->quadRenderer;

    if (backgroundTexture.ID != 0) {
        glm::mat4 ortho = glm::ortho(0.0f, (float) windowWidth, 0.0f, (float) windowHeight, -1.0f, 1.0f);
        backgroundShader->use();
        backgroundShader->setUniform1f("time", (float)glfwGetTime());
        // Bind the background texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture.ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        backgroundShader->setInt("backgroundTexture", 0);
        quadRenderer->render();
    }
}

/**
* Initialize Custom Screen dialog before display.
* 
* This is called when you click the "customize" button.  We reset everthing to
* defaults.  This is currently might be overkill, but allows us in the future
* to restart the game, too.
*/

static void UICustomScreenInit(GameControl* gc)
{
    extern std::vector<int> activeProfileItem;
    int defaultProfileIndex = 0;
    extern GameControl* gameControl;
    extern  std::vector<std::string> profileItems;

    extern std::vector<float> discMass;
    extern std::vector<float> discMomentOfInertia;
    extern std::vector<float> driverCoefficientOfFriction;
    extern std::vector<float> driverMass;
    extern std::vector<float> driverMomentofInertia;
    extern std::vector<float> layerCoefficientOfRestitution;
    extern std::vector<float> layerMass;
    extern std::vector<float> layerMomentOfInertia;
    extern std::vector<float> layerRecoilDistributionMean;
    extern std::vector<float> layerRecoilDistributionStdDev;

    if (gameControl != nullptr) {
        return;  // Initialize only once
    }

    gameControl = gc;  // We'll need this later

    // At the moment this is a bit weird.  The incoming blade structures start with default values,
    // and we create profile structures also weith default values!

    if (gc->iniFile == nullptr) {
        // First time through here. Set up the ini file handling

        gc->iniFile = new mINI::INIFile("beyblade_profiles.ini");
        gc->iniData = new mINI::INIStructure();

        if (!gc->iniFile->read(*gc->iniData)) {
            gc->iniData->clear();  // Couldn't read, create a new setup
        }

        for (auto it = gc->iniData->begin(); it != gc->iniData->end(); it++) {
            auto const& section = it->first;
            profileItems.push_back(section);
        }

        // You MUST at least have a "Default" profile.

        if (!gc->iniData->has("Default")) {
            Disc disc;
            Driver driver;
            Layer layer;

            UIProfileCreateSectionFromDefaults("Default", disc, driver, layer);
            profileItems.clear();
            profileItems.push_back("Default");

            if (!gc->iniFile->write(*gc->iniData)) {
                std::cerr << "Failed to write profile file" << std::endl;  // TODO: Why did it fail?
            };
        }

        std::sort(profileItems.begin(), profileItems.end());  // Sort the items

        // Find the index of "Default"
        defaultProfileIndex = (int)std::distance(profileItems.begin(), std::find(profileItems.begin(), profileItems.end(), "Default"));

        // TODO: What if the mesh has other weird settings that aren't really
        // the default?
    }

    /*
    discMass.clear();
    discMomentOfInertia.clear();
    driverCoefficientOfFriction.clear();
    driverMass.clear();
    driverMomentOfInertia.clear();
    layerCoefficientOfRestitution.clear();
    layerMass.clear();
    layerMomentOfInertia.clear();
    layerRecoilDistributionMean.clear();
    layerRecoilDistributionStdDev.clear();
    */

    // There must be at least TWO blades!  You might have more in the future,
    // although the customization UI is probably incomplete.

    std::vector<Beyblade*> blades = gc->physicsWorld->getBeyblades();
    for (size_t i = 0; i < blades.size(); i++) {
        activeProfileItem.push_back(defaultProfileIndex);

        Beyblade* b = blades[i];
        BeybladeBody* bb = b->getRigidBody();

        discMass.push_back(bb->getDiscMass().value() * 1000.0f);
        driverMass.push_back(bb->getDriverMass().value() * 1000.0f);
        layerMass.push_back(bb->getLayerMass().value() * 1000.0f);

        discMomentOfInertia.push_back(bb->getDiscMomentOfInertia().value() * 1000.0f);
        driverMomentOfInertia.push_back(bb->getDriverMomentOfInertia().value() * 1000.0f);
        layerMomentOfInertia.push_back(bb->getLayerMomentOfInertia().value() * 1000.0f);

        driverCoefficientOfFriction.push_back(bb->getDriverCOF().value());
        layerCoefficientOfRestitution.push_back(bb->getLayerCOR().value());

        layerRecoilDistributionMean.push_back(bb->getLayerRecoilDistribution().getMean().value());
        layerRecoilDistributionStdDev.push_back(bb->getLayerRecoilDistribution().getStdDev().value());
    }
}

/**
* Prompt for a new profile name and save the profile when selected.
*/

static void UIPromptForProfile()
{
    extern char newProfileName[MAX_PROFILE_NAME_LENGTH];
    extern bool showPromptForProfile;

    if (showPromptForProfile) {
        OpenPopup("Save Profile##title");
    }

    // Create a popup window
    if (BeginPopupModal("Save Profile##title", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Display a text input field for the file name
        InputText("Profile Name##text", newProfileName, MAX_PROFILE_NAME_LENGTH);

        // Display buttons to confirm or cancel the input

        if (Button("OK", ImVec2(120, 0))) {
            if (newProfileName[0] != '\0') {  // [Re]write the profiles, and add or replace the screen data.
                //UIProfileCreateSectionFromScreen(newProfileName);

                if (std::find(profileItems.begin(), profileItems.end(), newProfileName) == profileItems.end()) {
                    profileItems.push_back(newProfileName);
                }

                std::sort(profileItems.begin(), profileItems.end());  // Sort the items

                int newItemIndex = (int)std::distance(profileItems.begin(), std::find(profileItems.begin(), profileItems.end(), newProfileName));
                activeProfileItem[activeBlade] = newItemIndex;

                newProfileName[0] = '\0';
            }

            showPromptForProfile = false;
            CloseCurrentPopup();
        }

        SameLine();

        if (Button("Cancel", ImVec2(120, 0))) {
            showPromptForProfile = false;
            CloseCurrentPopup();
        }

        EndPopup();
    }
}


/**
* Create a new section (i.e., a blade profile) in the ini data.
* 
* @param profileName                [in] Section name such as "Default".
* 
* @param disc                       [in] A disc object.
* 
* @param driver                     [in] A driver object.
* 
* @param layer                      [in] A layer object.
*/

static void UIProfileCreateSectionFromDefaults(const char* profileName, Disc& disc, Driver& driver, Layer& layer)
{
    extern GameControl* gameControl;

    mINI::INIStructure& i = *gameControl->iniData;  // Easy-to-type reference

    /**
    * Create a new section from the active screen values.  Rewrites the ini file.
    */
    if (!gameControl->iniFile->write(*gameControl->iniData)) {
        std::cerr << "Failed to write profile file" << std::endl;  // TODO: Why did it fail?
    }
    // 12/14/24" TODO: Migrate mINI or some other file system? gameControl is outdated
    //(*gameControl->iniData)[profileName].set({
    //    { "discMass", std::to_string(disc.mass) },
    //    { "discMomentOfInertia", std::to_string(disc.momentOfInertia) },
    //    { "driverCoefficientOfFriction",  std::to_string(driver.coefficientOfFriction) },
    //    { "driverMass",  std::to_string(driver.mass) },
    //    { "driverMomentOfInertia", std::to_string(driver.momentOfInertia) },
    //    { "layerCoefficientOfRestitution", std::to_string(layer.coefficientOfRestitution) },
    //    { "layerMass",  std::to_string(layer.mass) },
    //    { "layerMomentOfInertia", std::to_string(layer.momentOfInertia) },
    //    { "layerRecoilDistributionMean", std::to_string(layer.recoilDistribution.getMean()) },
    //    { "layerRecoilDistributionStdDev", std::to_string(layer.recoilDistribution.getStdDev()) }
    //    });
}

/**
* Handle profile selection change.
*/

static void UICustomScreenProfileSelected(int selectedItemIndex)
{
    extern int activeBlade;
    extern std::vector<int> activeProfileItem;

    activeProfileItem[activeBlade] = selectedItemIndex;
    UIUpdateBladeFromProfile();  // Change all values on the screen.
}

/**
* Update game state message in the information screen.
*/

void UISetRunState(bool isError, const std::string& msg)
{
    extern std::string runState;
    extern bool runStateIsError;

    runState = msg;
    runStateIsError = isError;
}

/**
* Update the screen for the active blade from the active profile.
*/

static void UIUpdateBladeFromProfile()
{
    extern int activeBlade;
    extern std::vector<int> activeProfileItem;
    extern std::vector<std::string> profileItems;
    extern GameControl* gameControl;

    std::string profileName = profileItems[activeProfileItem[activeBlade]];

    extern std::vector<float> discMass;
    extern std::vector<float> discMomentOfInertia;
    extern std::vector<float> driverCoefficientOfFriction;
    extern std::vector<float> driverMass;
    extern std::vector<float> driverMomentofInertia;
    extern std::vector<float> layerCoefficientOfRestitution;
    extern std::vector<float> layerMass;
    extern std::vector<float> layerMomentOfInertia;
    extern std::vector<float> layerRecoilDistributionMean;
    extern std::vector<float> layerRecoilDistributionStdDev;

    mINI::INIStructure& i = *gameControl->iniData;  // Easy-to-type reference

    // Easier way to do this?  I want the map for this section, but C++ won't deduce from the reference.

    discMass[activeBlade] = (float)(atof(i[profileName]["discMass"].c_str()) * 1000.0);
    discMomentOfInertia[activeBlade] = (float)(atof(i[profileName]["discMomentOfInertia"].c_str()) * 1000.0);
    
    driverCoefficientOfFriction[activeBlade] = (float)atof(i[profileName]["driverCoefficientOfFriction"].c_str());
    driverMass[activeBlade] = (float)(atof(i[profileName]["driverMass"].c_str()) * 1000.0);
    driverMomentOfInertia[activeBlade] = (float)(atof(i[profileName]["driverMomentOfInertia"].c_str()) * 1000.0);

    layerCoefficientOfRestitution[activeBlade] = (float)atof(i[profileName]["layerCoefficientOfRestitution"].c_str());
    layerMass[activeBlade] = (float)(atof(i[profileName]["layerMass"].c_str()) * 1000.0);
    layerMomentOfInertia[activeBlade] = (float)(atof(i[profileName]["layerMomentOfInertia"].c_str()) * 1000.0);
    layerRecoilDistributionMean[activeBlade] = (float)atof(i[profileName]["layerRecoilDistributionMean"].c_str());
    layerRecoilDistributionStdDev[activeBlade] = (float)atof(i[profileName]["layerRecoilDistributionStdDev"].c_str());
}


#include <vector> 
#include <memory>
#include <string>

class GameEngine;
class Profile;
class Beyblade;

class SaveManager {
public:
    static SaveManager& getInstance() {
        static SaveManager instance;
        return instance;
    }

    void saveProfiles(const std::vector<std::shared_ptr<Profile>>& profiles, const std::string& filePath);
    void saveBeyblades(const std::vector<std::shared_ptr<Beyblade>>& beyblades, const std::string& filePath);
    void saveAll(const GameEngine& engine, const std::string& directoryPath);

private:
    SaveManager() = default;
};