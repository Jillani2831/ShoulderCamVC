#include "plugin.h"
#include "CCamera.h"
#include "CCam.h"
#include "CWorld.h"
#include "CPlayerPed.h"
#include "CPed.h"
#include "CVector.h"

#include <windows.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

using namespace plugin;

namespace ShoulderCam {

static bool enabled = true;
static float shoulder = 0.65f;   // Camera offset. Positive = camera moves left, putting player on right.
static float height = 0.05f;
static float distance = 0.0f;    // 0 = keep game's current camera distance.
static float fov = 0.0f;         // 0 = keep game's FOV.
static bool affectAiming = true;
static bool affectVehicles = false;

static std::string IniPath() {
    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::string s(path);
    auto slash = s.find_last_of("\\/");
    if (slash != std::string::npos) s.resize(slash + 1);
    return s + "ShoulderCamVC.ini";
}

static std::string Trim(std::string s) {
    auto notSpace = [](unsigned char c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

static bool ReadBool(const std::string& value, bool fallback) {
    std::string v = value;
    std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c){ return (char)std::tolower(c); });
    if (v == "1" || v == "true" || v == "yes" || v == "on") return true;
    if (v == "0" || v == "false" || v == "no" || v == "off") return false;
    return fallback;
}

static void LoadIni() {
    std::ifstream file(IniPath());
    if (!file) return;

    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
        if (line.front() == '[') continue;

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = Trim(line.substr(0, eq));
        std::string value = Trim(line.substr(eq + 1));

        try {
            if (key == "Enabled") enabled = ReadBool(value, enabled);
            else if (key == "ShoulderOffset") shoulder = std::stof(value);
            else if (key == "HeightOffset") height = std::stof(value);
            else if (key == "Distance") distance = std::stof(value);
            else if (key == "FOV") fov = std::stof(value);
            else if (key == "AffectAiming") affectAiming = ReadBool(value, affectAiming);
            else if (key == "AffectVehicles") affectVehicles = ReadBool(value, affectVehicles);
        } catch (...) {}
    }

    // Clamp dangerous values so an accidental INI typo doesn't launch the camera into orbit.
    shoulder = std::clamp(shoulder, -2.0f, 2.0f);
    height = std::clamp(height, -1.0f, 1.0f);
    distance = std::clamp(distance, 0.0f, 8.0f);
    fov = std::clamp(fov, 0.0f, 120.0f);
}

static CVector CameraRight(const CVector& front) {
    CVector up(0.0f, 0.0f, 1.0f);
    CVector right(
    front.y * up.z - front.z * up.y,
    front.z * up.x - front.x * up.z,
    front.x * up.y - front.y * up.x
);
    float len = right.Magnitude();
    if (len > 0.0001f) right /= len;
    return right;
}

static void Apply() {
    if (!enabled) return;

    CPlayerPed* player = FindPlayerPed();
    if (!player) return;

    CCam& cam = TheCamera.m_asCams[TheCamera.m_nActiveCam];
    if (!cam.m_pCamTargetEntity) return;

    if (cam.m_pCamTargetEntity != player)
        return;

    // Leave first-person and other non-follow situations alone.
    // The active camera already contains the game's mouse/controller rotation.
    CVector front = cam.m_vecFront;
    if (front.Magnitude() < 0.0001f) return;
   float frontLen = front.Magnitude();
if (frontLen > 0.0001f) {
    front.x /= frontLen;
    front.y /= frontLen;
    front.z /= frontLen;
}

    // This is the key: move the whole camera rig sideways.
    // Negative world-right moves the camera to the player's left,
    // making Tommy appear on the RIGHT side of the screen.
    CVector right = CameraRight(front);
    CVector shift = right * (-shoulder);
    shift.z = height;

    CVector oldSource = cam.m_vecSource;
    CVector oldTarget = cam.m_vecTargetCoorsForFudgeInter;
    float currentDistance = (oldSource - oldTarget).Magnitude();

    CVector newTarget = oldTarget + shift;
    CVector newSource = oldSource + shift;

    if (distance > 0.001f) {
        newSource = newTarget - front * distance;
    }

    // Basic collision correction for the shifted camera.
    CColPoint colPoint{};
    CEntity* entity = nullptr;
    CWorld::pIgnoreEntity = player;

    if (CWorld::ProcessLineOfSight(
        newTarget, newSource, colPoint, entity,
        true, true, false, true, false, false, false, false)) {
        newSource = colPoint.m_vecPoint;
    }

    CWorld::pIgnoreEntity = nullptr;

    cam.m_vecSource = newSource;
    cam.m_vecTargetCoorsForFudgeInter = newTarget;
    cam.m_vecFront = newTarget - newSource;
    float camFrontLen = cam.m_vecFront.Magnitude();
if (camFrontLen > 0.0001f) {
    cam.m_vecFront.x /= camFrontLen;
    cam.m_vecFront.y /= camFrontLen;
    cam.m_vecFront.z /= camFrontLen;
}

    if (fov > 0.001f)
        cam.m_fFOV = fov;
}

struct Main {
    Main() {
        LoadIni();
        Events::gameProcessEvent += [] {
            Apply();
        };
    }
} gMain;

} // namespace ShoulderCam
