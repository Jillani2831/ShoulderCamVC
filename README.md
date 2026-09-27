# ShoulderCamVC

A small GTA Vice City 1.0 ASI camera plugin intended to give the classic game a modern
over-the-shoulder presentation, with Tommy positioned on the RIGHT side of the screen.

## Files

- `ShoulderCamVC.asi` - compiled plugin (created by the build)
- `ShoulderCamVC.ini` - camera settings
- `source/Main.cpp` - source
- `premake5.lua` - build configuration

## Compatibility

- GTA Vice City 1.0 / 10EN
- Requires an ASI loader such as Ultimate ASI Loader.

## Settings

`ShoulderOffset=0.65` is the important setting.

- Positive values put Tommy on the RIGHT.
- Negative values put Tommy on the LEFT.
- `0.45` is subtle.
- `0.65` is a good starting point.
- `0.80` is a stronger over-the-shoulder look.

`Distance=0` keeps the game's current camera distance.
`FOV=0` keeps the game's current FOV.

## Important

This plugin is designed as a small standalone camera offset. It does not replace
Classic Axis and should be tested without another mod that also rewrites the follow
camera. If both are installed and both modify the same camera every frame, their
effects can stack.

## Build

The repository's GitHub Actions workflow downloads Plugin-SDK, builds its VC library,
generates the project, then builds this plugin.
