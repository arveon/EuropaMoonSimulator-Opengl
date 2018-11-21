/* by Aleksejs Loginovs - October 2018 */

#pragma warning( disable: 4244 )


#pragma once
#define VALUES_PER_VERT 3
#define VALUES_PER_COLOR 4
#define VALUES_PER_NORMAL 3
#define NUM_TRIANGLE_VERTS 3
#define NUM_SQR_VERTS 6
#define NUM_CUBE_VERTS 8
#define NUM_CUBE_INDICES 36

#define NUM_LATS_SPHERE 20
#define NUM_LONGS_SPHERE 20

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define NUM_DRAWMODES 2

#define BASIC_SHADER 1
#define LIT_TEXTURED_SHADER 2
#define SKYBOX_SHADER 3

#define CAMERA_MOVEMENT_SPEED 10.f
#define CAMERA_STRAFE_SPEED 5.f

#define LIGHT_MOVEMENT_SPEED 0.01f