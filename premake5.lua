workspace "ShoulderCamVC"
    configurations { "ReleaseVC" }
    platforms { "Win32" }

    location "project_files"

project "ShoulderCamVC"
    kind "SharedLib"
    language "C++"
    architecture "x86"
    targetname "ShoulderCamVC"
    targetextension ".asi"
    characterset "MBCS"
    staticruntime "On"
    cppdialect "C++latest"

    files {
        "source/**.h",
        "source/**.cpp"
    }

    includedirs {
        "source",
        "$(PLUGIN_SDK_DIR)/shared",
        "$(PLUGIN_SDK_DIR)/shared/game",
        "$(PLUGIN_SDK_DIR)/plugin_vc",
        "$(PLUGIN_SDK_DIR)/plugin_vc/game_vc",
        "$(PLUGIN_SDK_DIR)/plugin_vc/game_vc/enums",
        "$(PLUGIN_SDK_DIR)/plugin_vc/game_vc/rw"
    }

    libdirs {
        "$(PLUGIN_SDK_DIR)/output/lib"
    }

    defines {
        "GTAVC",
        "PLUGIN_SGV_10EN",
        "RW",
        "_CRT_NON_CONFORMING_SWPRINTFS"
    }

    filter "configurations:ReleaseVC"
        optimize "On"
        symbols "Off"
        links { "plugin_vc" }

    filter {}
