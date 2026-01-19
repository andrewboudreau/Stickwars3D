# Stickwars 3D

A 3D stick figure combat game ported from the original C++ OpenGL version (2003) to modern JavaScript using Babylon.js.

## [Play Now](https://andrewboudreau.github.io/Stickwars3D/)

## Controls

### Desktop
- **WASD** - Move
- **Mouse** - Look around
- **Shift** - Run
- **Click** - Lock pointer

### Mobile
- **Left Joystick** - Move
- **Swipe Right Side** - Look around
- **RUN Button** - Sprint

## Features

- Procedurally generated terrain with hills
- Skeletal animation system with quaternion interpolation (SLERP)
- Custom OBJ model loader
- Custom KFM keyframe animation format
- AI stick figures with team colors (Red vs Blue)
- Mobile touch controls with virtual joystick

## Technical Details

### Animation System
The game uses a custom keyframe animation format (.kfm) with quaternion rotations for smooth skeletal animation:

- **10 animated joints**: shoulders, elbows, hips, knees, upper/lower torso
- **7 body parts**: torso, bicep, forearm, hand, thigh, calf, foot
- **Animations**: running, stab, chop, sword swing, death, explosion

### Architecture
```
StickwarsGame
├── OBJLoader        - Parses Wavefront OBJ files
├── KFMLoader        - Parses keyframe animation files
├── RobotMeshManager - Manages body part mesh templates
├── Robot            - Skeletal hierarchy with animation
└── Terrain          - Procedural heightmap generation
```

## Original Game

The original Stickwars 3D was written in C++ using OpenGL and FLTK (2003-2006). Features included:
- First-person combat
- Multiple weapons (hammer, knife, bat, pistol, uzi, rifle, sniper, bazooka)
- Team-based AI with flag capture
- Destructible terrain (missile craters)
- Particle effects (smoke, debris)

## Building

No build step required - runs directly in the browser. Just serve the files with any static web server or open via GitHub Pages.

## License

Original C++ code by Michael Garland, 2003.
