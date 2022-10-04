# Engine3D
 
Engine3D is an all purpose engine created for learning purposes during the 3D-Programming semester. 

## Deferred renderer
 - PBR
 - Phong
 - SSAO
 - SMAA
 - Bloom
 - Global Illumination
 - Depth of field
 - Volumetric Lighting
 - Volumetric Clouds
 - HDR + Tonemapping
 - GBuffer + ABuffer
 
## Custom Physics engine
 - Rotational physics and friction for rigid bodies
 - Heightmap and Convex Hull collision

## Grass Scene
<div style='float: center'>
  <img style='width: 600px' src="/Images/Grass Scene.png"></img>
</div>

## Sponza GI 0vs2 bounces

<div style='float: center'>
  <img style='width: 600px' src="/Images/Sponza0Bounce.png"></img>
</div>
<div style='float: center'>
  <img style='width: 600px' src="/Images/Sponza2Bounce.png"></img>
</div>

## Physics with terrain and B-Splines

<div style='float: center'>
  <img style='width: 600px' src="/Images/Physics40kBalls2.png"></img>
</div>
<div style='float: center'>
  <img style='width: 600px' src="/Images/PhysicsBSplines.png"></img>
</div>

# Installation

Requires windows 64-bit and openGL version 4.5+
This project runs on visual studio 2017+

1. Download and install QT 6.x+ with MSVC 
2. Install QT VS Tools extension. (Open visual studio->Extensions->Manage Extensions->Install QT VS Tools)
3. QT VS Tools will ask you to select a qt version. Click add QT installation, the version is located in C:/qt/6.x.x/msvc201x_64/bin/qmake.exe
4. Try to build the project in visual studio, if it asks you to select a qt version then go to project properties->QT project settings->QT installation->Select qt version 6.x
5. Build in Release 64 bit (Associated dll files are inside 3dprog22/Libs if needed)
