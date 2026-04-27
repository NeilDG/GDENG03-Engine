# FPS Camera Controls - Quick Reference

## Controls

### Movement
- **W** - Move Forward
- **S** - Move Backward  
- **A** - Strafe Left
- **D** - Strafe Right
- **Space** - Move Up
- **Left Ctrl** - Move Down
- **Left Shift** - Sprint (2x speed)

### Look
- **Left Mouse Button + Drag** - Look around (yaw and pitch)
- **Scroll Wheel** - Adjust movement speed

### Other
- **Z** - Toggle IBL (Image-Based Lighting) on/off
- **Space** - Switch between PBR test scenes

## Tips

1. **Inspecting Objects:** Use slow speed (scroll down) for detailed inspection
2. **Moving Between Scenes:** Use sprint (Left Shift) for faster travel
3. **Vertical Movement:** Hold Space to ascend, Left Ctrl to descend
4. **Speed Adjustment:** Scroll wheel adjusts from 0.5 to 50 units/second
5. **Camera Rotation:** Hold left mouse button and drag to rotate the camera view

## Configuration

In code, you can adjust:
```cpp
fpsControl->setMovementSpeed(5.0f);      // Base speed
fpsControl->setLookSensitivity(0.1f);    // Mouse sensitivity
fpsControl->setSprintMultiplier(2.0f);   // Sprint speed multiplier
fpsControl->setInvertY(false);           // Invert Y-axis
```

## Current Settings

- **Movement Speed:** 5.0 units/second
- **Sprint Multiplier:** 2.0x (10.0 units/second)
- **Look Sensitivity:** 0.1
- **Y-Axis:** Normal (not inverted)
- **Pitch Range:** ±89° (prevents gimbal lock)
