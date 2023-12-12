#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h" // include the common header file

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#include "glew.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "glut.h"
#include "keytime.h"


#include "setmaterial.cpp"
//#include "setlight.cpp"
//#include "osusphere.cpp"
//#include "osucone.cpp"
//#include "osutorus.cpp"
#include "bmptotexture.cpp"
//#include "loadobjfile.cpp"
#include "keytime.cpp"
//#include "glslprogram.cpp"

int currentLightColor = 0; // Declare currentLightColor as a global variable
int currentLightType = 0;
int isSpotLight = 0; // 0 for point light, 1 for spot light
GLuint POINT_LIGHT = 0;
GLuint SPOT_LIGHT = 1;

int cameraMode = 0; // 0 = free camera, 1 = follow camera

// title of these windows:

const char *WINDOWTITLE = "CS450 Final Project - James Adelhelm";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE     = GL_LINEAR;
const GLfloat FOGDENSITY  = 0.30f;
const GLfloat FOGSTART    = 1.5f;
const GLfloat FOGEND      = 4.f;


// for animation:
const int MS_PER_CYCLE = 100000;		// 10000 milliseconds = 10 seconds; make this larger to make animation slower

// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	BoxList;				// object display list
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );
void 	DrawWindmillTower();
void 	DrawWindmillBlade();
void 	DrawWindmillBlades();
void 	DrawWindmill();
void 	DrawSkybox();
void 	DrawWindmillTowerTop();
void	DrawLand();
void 	DrawSun();
void 	DrawSunPosition();
void 	InitSunPosition();
void 	InitCloudPositions();
void 	InitClouds();
void 	InitBuildingPositions();
void 	DrawCloud();
void 	DrawClouds();
void 	DrawBeachAndOcean();
void 	DrawBuildings();

// Function prototype
unsigned char *BmpToTexture(char *filename, int *width, int *height);
void LoadSandTexture();

void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);


// Building Functions
typedef struct {
    float x, y, z; // Position of the building
    float width;   // Width of building
    float height;  // Height of the building
    int colorIndex; // Index to the color array used to assign colors to buildings
} Building;


Building buildings[100]; // Array to hold building data

// Define boundaries (example values, adjust according to scene)
float minBuildingSpawnX = -50.0f; // Minimum X coordinate for buildings to be spawned on
float maxBuildingSpawnX = 50.0f;  // Maximum X coordinate for buildings to be spawned on
float minBuildingSpawnY = 0.0f;   // Minimum Y coordinate (ground level) for buildings to be spawned on
float maxBuildingSpawnY = 10.0f;  // Maximum Y coordinate for buildings to be spawned on
float minBuildingSpawnZ = -100.0f;  // Minimum Z coordinate (behind the windmill) for buildings to be spawned on
float maxBuildingSpawnZ = -10.0f; // Maximum Z coordinate for buildings to be spawned on


// Building color code
typedef struct {
    float r, g, b;  // RGB color components for building color
} Color;

// The building colors used for the building drawings
Color floridaKeysColors[] = {
    { 0.0f, 0.6f, 0.8f }, // Ocean Blue
    { 0.2f, 0.8f, 0.8f }, // Turquoise
    { 0.1f, 0.7f, 0.7f }, // Teal
    { 0.2f, 0.5f, 0.7f }, // Cerulean
    { 0.3f, 0.9f, 0.9f },  // Sky Blue
	{ 0.8f, 0.4f, 0.6f }, // Carribean pink
	{ 0.9f, 0.8f, 0.6f }, // cream
	{ 0.9f, 0.9f, 0.9f }, // marble
	{ 0.8f, 0.8f, 0.8f }, // light grey
	{ 0.6f, 0.6f, 0.6f }, // dark grey
	{ 0.4f, 0.2f, 0.0f }, // dark brown
	{ 0.6f, 0.4f, 0.2f },// light brown
};

// Configures the locations of the buildings
void InitBuildingPositions() {
    // Define the maximum height of buildings relative to the windmill
    float maxBuildingHeight = 2.0f; // Assuming windmill height is 2.0 units
	int sizeArrColors = sizeof(floridaKeysColors);
	int sizeColor = sizeof(Color);
	int numColors = sizeArrColors / sizeColor;

	// replace i with buildingIndex
	int numBuildings = 100;
	for (int buildingIndex = 0; buildingIndex < numBuildings; buildingIndex++) {
		int xRange = maxBuildingSpawnX - minBuildingSpawnX;
		float randXVal = ((float)rand() / RAND_MAX);
        buildings[buildingIndex].x = minBuildingSpawnX + randXVal * xRange;
        buildings[buildingIndex].y = minBuildingSpawnY; // The Ground level is at 0
		int zRange = maxBuildingSpawnZ - minBuildingSpawnZ;
		float randZVal = ((float)rand() / RAND_MAX);
        buildings[buildingIndex].z = minBuildingSpawnZ + randZVal * zRange;
        // Sets the building height to a random value, but not one that excees the maxBuildingHeight
		float heightMin = 0.5f;
		int buildingHeight = (int)(maxBuildingHeight * 10);
		float randomBuildingHeight = (float)(rand() % buildingHeight);
        buildings[buildingIndex].height = randomBuildingHeight / 10.0f + heightMin; // Added 0.5 to ensure we don't have 0 height
        buildings[buildingIndex].width = (float)(rand() % 4 + 1);
		buildings[buildingIndex].colorIndex = rand() % numColors; // Assigns a random color index to give building random color
    }
}


void DrawBuildings() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disables lighting
    for (int buildingIndex = 0; buildingIndex < 100; buildingIndex++) {
		Color color = floridaKeysColors[buildings[buildingIndex].colorIndex];
        glPushMatrix();
        // Translates to the building's location
		float buildingX = buildings[buildingIndex].x;
		float buildingY = buildings[buildingIndex].y;
		float buildingZ = buildings[buildingIndex].z;
        glTranslatef(buildingX, buildingY + buildings[buildingIndex].height * 0.5, buildingZ);
        // Scales the cube to the building's width and height
		float buildingWidth = buildings[buildingIndex].width;
		float buildingIndHeight = buildings[buildingIndex].height;
        glScalef(buildingWidth, buildingIndHeight, buildingWidth);
		// set building color to grey white
		float redColor = color.r;
		float greenColor = color.g;
		float blueColor = color.b;
		glColor3f(redColor, greenColor, blueColor); // Sets the building color
        // Draw the main building cube
        glutSolidCube(1.0f);

        // Sets the roof color RGB values to the random color of next buildingIndex
		float roofRed = color.r + 0.5f;
		float roofGreen = color.g + 0.5f;
		float roofBlue = color.b + 0.5f;
	
        glColor3f(roofRed, roofGreen, roofBlue); // RGB for roof colors

        // Draws the roof
        // Translates up by half the height of the building plus half the roof thickness
        glTranslatef(0.0f, 0.5f + 0.05f, 0.0f);
        // Scales the roof to be slightly larger than the building, and flatter
		float scaleVal = 1.1f;
		float flatScaleVal = 0.1f;
        glScalef(scaleVal, flatScaleVal, scaleVal);
        // Draws the roof cube
        glutSolidCube(1.0f);
        glPopMatrix();
    }
	glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}


// My beach and ocean functions
// Global texture ID for the beach sand
GLuint beachTextureId;

void LoadSandTexture() {
    // The path to the sand BMP texture file
    char sandTextureFile[] = "sand.bmp";
    int width, height;
    unsigned char *sandTextureData = BmpToTexture(sandTextureFile, &width, &height);
    if (sandTextureData == NULL) {
        fprintf(stderr, "Unable to load the sand texture\n");
        exit(1);
    }
    // Generates the texture ID and binds it
    glGenTextures(1, &beachTextureId);
    glBindTexture(GL_TEXTURE_2D, beachTextureId);
    
	// Sets the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
	// Sends the sand texture data to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, sandTextureData);
    
	// Clean up
    delete[] sandTextureData;
}

// land / grass texture id and texture code
GLuint grassTextureId;

void LoadGrassTexture(){
	// path to grass texture file
	char grassTextureFile[] = "dark_grass.bmp";

	int grassWidth, grassHeight;
	unsigned char *grassTextureData = BmpToTexture(grassTextureFile, &grassWidth, &grassHeight);
	if (grassTextureData == NULL){
		fprintf(stderr, "Unable to load the grass texture\n");
		exit(1);
	}

	// generates the texture id and binds it
	glGenTextures(1, &grassTextureId);
	glBindTexture(GL_TEXTURE_2D, grassTextureId);

	// sets the texture parameters for the grass texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat the texture in S direction
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repeat the texture in T direction
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear mag filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear min filter

	// sends the grass texture data to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassWidth, grassHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, grassTextureData);

	// clean up
	delete[] grassTextureData;
}


const int beach_quad_width = 200; // Width of the beach plane/quad
const int beach_quad_y_value = 12; // the depth of the beach
const float beach_beginning_z_coord = 5.0f; // the start Z position of the beach just that is just past the windmill
const float ocean_y_val = 0.1f; // the height of the ocean water relative to the land

void DrawBeachAndOcean() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

	// Number of times to repeat the texture based on width and depth
    const float numTimesRepeatTextureWidth = 30.0f;
    const float numTimesRepeatTextureDepth = 300.0f;

    // Bind the sand texture
    glBindTexture(GL_TEXTURE_2D, beachTextureId);
    glEnable(GL_TEXTURE_2D);

    // Sets the beach color to a sandy yellow and binds the texture
    glColor3f(0.9f, 0.8f, 0.5f); // sandy yellow color

    // Draws the beach quad with texture coordinates
    glBegin(GL_QUADS);
		float BeachQuadX1 = -beach_quad_width / 2;
		float BeachQuadY1 = ocean_y_val;
		float BeachQuadZ1 = beach_beginning_z_coord;
        glTexCoord2f(0.0f, 0.0f); glVertex3f(BeachQuadX1, BeachQuadY1, BeachQuadZ1);

		float BeachQuadX2 = beach_quad_width / 2;
		float BeachQuadY2 = ocean_y_val;
		float BeachQuadZ2 = beach_beginning_z_coord; 
        glTexCoord2f(numTimesRepeatTextureWidth, 0.0f); glVertex3f(BeachQuadX2, BeachQuadY2, BeachQuadZ2);

		float BeachQuadX3 = beach_quad_width / 2;
		float BeachQuadY3 = ocean_y_val;
		float BeachQuadZ3 = beach_beginning_z_coord + beach_quad_y_value; 
        glTexCoord2f(numTimesRepeatTextureWidth, numTimesRepeatTextureDepth); glVertex3f(BeachQuadX3, BeachQuadY3, BeachQuadZ3);
        
		float BeachQuadX4 = -beach_quad_width / 2;
		float BeachQuadY4 = ocean_y_val;
		float BeachQuadZ4 = beach_beginning_z_coord + beach_quad_y_value; 
		glTexCoord2f(0.0f, numTimesRepeatTextureDepth); glVertex3f(BeachQuadX4, BeachQuadY4, BeachQuadZ4);
    glEnd();

    glDisable(GL_TEXTURE_2D);


    // Sets the ocean color to a nice shade of blue
    glColor3f(0.0f, 0.5f, 0.8f); // blue color

    // Draw the ocean quad, it should start where the land ends
    glBegin(GL_QUADS);

		float OceanQuadX1 = -beach_quad_width / 2;
		float OceanQuadY1 = ocean_y_val;
		float OceanQuadZ1 = beach_beginning_z_coord + beach_quad_y_value;

		float OceanQuadX2 = beach_quad_width / 2;
		float OceanQuadY2 = ocean_y_val;
		float OceanQuadZ2 = beach_beginning_z_coord + beach_quad_y_value;

		float OceanQuadX3 = beach_quad_width / 2;
		float OceanQuadY3 = ocean_y_val;
		float OceanQuadZ3 = 1000.0f;

		float OceanQuadX4 = -beach_quad_width / 2;
		float OceanQuadY4 = ocean_y_val;
		float OceanQuadZ4 = 1000.0f;

        glVertex3f(OceanQuadX1, OceanQuadY1, OceanQuadZ1);
        glVertex3f(OceanQuadX2, OceanQuadY2, OceanQuadZ2);
        glVertex3f(OceanQuadX3, OceanQuadY3, OceanQuadZ3);
        glVertex3f(OceanQuadX4, OceanQuadY4, OceanQuadZ4);
    glEnd();
	
	glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}


// Sun functions
Keytimes *SunPosX = new Keytimes();
Keytimes *SunPosY = new Keytimes();
Keytimes *SunPosZ = new Keytimes();

Keytimes *SunRed = new Keytimes();
Keytimes *SunGreen = new Keytimes();
Keytimes *SunBlue = new Keytimes();


void InitSunPosition() {
    float sunTravelDistVal = 100.0f; 
    // Initializes Keytimes for X-axis 
    SunPosX->AddTimeValue(0.0f, 0.0f); // Start position
    SunPosX->AddTimeValue(0.5f, 0.0f); // Mid position
    SunPosX->AddTimeValue(1.0f, 0.0f); // End position

    // Initializes Keytimes for Y-axis
    SunPosY->AddTimeValue(0.0f, 0.0f); // Start position 
    SunPosY->AddTimeValue(0.5f, sunTravelDistVal); // Middle position 
    SunPosY->AddTimeValue(1.0f, 0.0f); // End position 

    // Initialize Keytimes for Z-axis
    SunPosZ->AddTimeValue(0.0f, -sunTravelDistVal); // Start position 
    SunPosZ->AddTimeValue(0.5f, 0.0f); // Mid position 
    SunPosZ->AddTimeValue(1.0f, sunTravelDistVal); // End position 

	// Initialize Keytimes for sun color components 
	// Sun Red component
	SunRed->AddTimeValue(0.0f, 1.0f); // Sunrise
    SunRed->AddTimeValue(0.25f, 1.0f); 
    SunRed->AddTimeValue(0.5f, 1.0f); // Middle of the day
    SunRed->AddTimeValue(0.75f, 1.0f); 
    SunRed->AddTimeValue(1.0f, 1.0f); // Sunset 

    // Sun Green component 
    SunGreen->AddTimeValue(0.0f, 0.8f); // Sunrise 
    SunGreen->AddTimeValue(0.25f, 0.8f); 
    SunGreen->AddTimeValue(0.5f, 1.0f); // Middle of the day
    SunGreen->AddTimeValue(0.75f, 0.8f); 
    SunGreen->AddTimeValue(1.0f, 0.8f); // Sunset

    // Sun Blue component - is not used
    SunBlue->AddTimeValue(0.0f, 0.0f); 
    SunBlue->AddTimeValue(0.25f, 0.0f);
    SunBlue->AddTimeValue(0.5f, 0.0f); 
    SunBlue->AddTimeValue(0.75f, 0.0f); 
    SunBlue->AddTimeValue(1.0f, 0.0f); 
}


void DrawSunPosition(float time) {
    // Converts linear time to ping-pong time for sun movemenbts
	float pingPongVal = fabsf(2.0f * time - 1.0f);
    float pingPongTime = 1.0f - pingPongVal;

    // Gets keytime values for sun position
    float sunPosX = SunPosX->GetValue(pingPongTime);
    float sunPosY = SunPosY->GetValue(pingPongTime);
    float sunPosZ = SunPosZ->GetValue(pingPongTime);

    // Applies translation to move the sun according to keytimes values
    glTranslatef(sunPosX, sunPosY, sunPosZ);
}


void DrawSun(float time) {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disables lighting
    // sets the sun color to keytimes vals
	glColor3f(SunRed->GetValue(time), SunGreen->GetValue(time), SunBlue->GetValue(time)); // RGB for sun color
    glPushMatrix();
    DrawSunPosition(Time);
    glutSolidSphere(5.0f, 20, 20); // Draws the sun as a sphere
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
	glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}


// Cloud functions
// Structure for clouds
// Array holds cloud positions
typedef struct CloudPosition {
    float x, y, z;
} CloudPosition;

const int numClouds = 100;
CloudPosition cloudPositions[numClouds]; // Array to hold cloud positions

// Initializes cloud positions
void InitCloudPositions() {
	int numClouds = 100;
    for (int cloudIndex = 0; cloudIndex < numClouds; cloudIndex++) {
        cloudPositions[cloudIndex].x = (float)(rand() % 200 - 100);
        cloudPositions[cloudIndex].y = (float)(rand() % 100 + 40);
        cloudPositions[cloudIndex].z = (float)(rand() % 200 - 100);
    }
}

// Structure for each part of the puffy clouds (individual sphere)
typedef struct CloudPuff {
    float offsetX, offsetY, offsetZ; // Offset positions for each sphere/cloud puff
    float size;                      // Size of each cloud puff that makes up all the clouds
} CloudPuff;

// Structure for a cloud
typedef struct Cloud {
    CloudPuff puffs[7]; // Each cloud has up to 7 puffs to create puffy effect
    int numPuffs;       // Actual number of puffs in this cloud
} Cloud;

Cloud clouds[numClouds]; // Array holds all the clouds

void InitClouds() {
    int numClouds = 100;
    for (int cloudIndex = 0; cloudIndex < numClouds; cloudIndex++) {
        // Randomly places the cloud positions
        clouds[cloudIndex].numPuffs = 3 + rand() % 5;
        for (int puffIndex = 0; puffIndex < clouds[cloudIndex].numPuffs; puffIndex++) {
            float puffXRandVal = (rand() % 20 - 10);
			clouds[cloudIndex].puffs[puffIndex].offsetX = puffXRandVal * 0.1f;
			float puffYRandVal = (rand() % 20 - 10);
            clouds[cloudIndex].puffs[puffIndex].offsetY = puffYRandVal * 0.1f;
			float puffZRandVal = (rand() % 20 - 10);
            clouds[cloudIndex].puffs[puffIndex].offsetZ = puffZRandVal * 0.1f;
			float puffSizeRandVal = (rand() % 100) / 50.0;
            clouds[cloudIndex].puffs[puffIndex].size = 1.0f + puffSizeRandVal;
        }
    }
}

void DrawCloud(Cloud cloud) {
    for (int puffIndex = 0; puffIndex < cloud.numPuffs; puffIndex++) {
        glPushMatrix();
        
        // Make puffSize larger to make clouds puffier
        float puffSize = cloud.puffs[puffIndex].size * 2.0f; 
        
        // Slightly randomizes the position of each puff within the cloud by using the offset value
        float offsetX = cloud.puffs[puffIndex].offsetX * 2.0f; // Changing this factor of 2.0 will alter the randomness
        float offsetY = cloud.puffs[puffIndex].offsetY * 2.0f;
        float offsetZ = cloud.puffs[puffIndex].offsetZ * 2.0f;
        
        glTranslatef(offsetX, offsetY, offsetZ);
        glutSolidSphere(puffSize, 20, 20);
        glPopMatrix();
    }
}


void DrawClouds() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disables lighting
	SetMaterial(1.0f, 1.0f, 1.0f, 100.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // White color for clouds
    for (int cloudIndex = 0; cloudIndex < 100; cloudIndex++) {
        glPushMatrix();
		float cloudX = cloudPositions[cloudIndex].x; 
		float cloudY = cloudPositions[cloudIndex].y; 
		float cloudZ = cloudPositions[cloudIndex].z; 
        glTranslatef(cloudX, cloudY, cloudZ);
        DrawCloud(clouds[cloudIndex]);
        glPopMatrix();
    }
	glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}




// My land functions - draws the land with grass texture
void DrawLand(){
    glPushMatrix();
    glDisable(GL_LIGHTING);  // Disablse lighting

    // Number of times to repeat the grass texture
    const float textureRepeatX = 50.0f; // Repeats grass texture 50 times along the X-axis
    const float textureRepeatZ = 50.0f; // Repeat grass texture 50 times along the Z-axis

    // Binds the grass texture
    glBindTexture(GL_TEXTURE_2D, grassTextureId);
    glEnable(GL_TEXTURE_2D);

    // Sets the grass color to green and binds the texture
    glColor3f(0.0f, 0.5f, 0.0f); // green color

    // Draws the land quad with repeated texture coordinates
    glBegin(GL_QUADS);
		float landVertexMin = -1000.0f;
		float landVertexMax = 1000.0f;
        glTexCoord2f(0.0f, 0.0f); glVertex3f(landVertexMin, 0.0f, landVertexMax);
        glTexCoord2f(textureRepeatX, 0.0f); glVertex3f(landVertexMax, 0.0f, landVertexMax);
        glTexCoord2f(textureRepeatX, textureRepeatZ); glVertex3f(landVertexMax, 0.0f, landVertexMin);
        glTexCoord2f(0.0f, textureRepeatZ); glVertex3f(landVertexMin, 0.0f, landVertexMin);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_TEXTURE_2D); // Disables texturing
    glEnable(GL_LIGHTING);    // Re-enables lighting
    glPopMatrix();
}

// function draws a rainbox out over the ocean
// The colors of the rainbow (Red, Orange, Yellow, Green, Blue, Violet)
GLfloat RainbowColors[6][3] = {
    { 1.0f, 0.0f, 0.0f }, // Red
    { 1.0f, 0.5f, 0.0f }, // Orange
    { 1.0f, 1.0f, 0.0f }, // Yellow
    { 0.0f, 1.0f, 0.0f }, // Green
    { 0.0f, 0.0f, 1.0f }, // Blue
    { 0.8f, 0.0f, 1.0f }  // Violet
};

void DrawLargeRainbow() {
	float twoPi = (2.0f * M_PI);
    float angleStep = twoPi / 100;  // Defines the segment resolution of the rainbow
    float rainbowRadius = 200.0f;  // Defines the radius of the rainbow
    float rainbowWidth = 2.0f;  // Defines the width of each of the rainbow's stripes
	
    glDisable(GL_LIGHTING);  // Disables lighting
    glEnable(GL_BLEND);      // Enable blending for realistic rainbow transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Sets blend function

    glPushMatrix();

    // Positions the rainbow
	float rainbowX = -170.0f;
	float rainbowY = 0.0f;
	float rainbowZ = 99.0f;
    glTranslatef(rainbowX, rainbowY, rainbowZ);  

	int numStripes = 6;
    for (int rainbowIndex = 0; rainbowIndex < numStripes; rainbowIndex++) {  // 6 rainbow colors
        glBegin(GL_TRIANGLE_STRIP);
		int loopIterationVal = 100;
        for (int rainbowInd2 = 0; rainbowInd2 <= loopIterationVal * 3/8 ; rainbowInd2++) {  // Half a circle for the arc
            float rainbowAngle = rainbowInd2 * angleStep;
            float rainbowInnerRadius = rainbowRadius - rainbowIndex * rainbowWidth;
            float outerRadius = rainbowRadius - (rainbowIndex + 1) * rainbowWidth;
			float alphaVal = 0.5f;

            // Inner vertex of the rainbow
            glColor4f(RainbowColors[rainbowIndex][0], RainbowColors[rainbowIndex][1], RainbowColors[rainbowIndex][2], alphaVal);  // Sets the alpha value to 0.5 for transparency
            float innerVertexCosFactor = cos(rainbowAngle) * rainbowInnerRadius;
			float innerVertexSinFactor = sin(rainbowAngle) * rainbowInnerRadius;
			glVertex3f(innerVertexCosFactor, innerVertexSinFactor, 0.0f);
            
            // Outer vertex of the rainbow
            glColor4f(RainbowColors[rainbowIndex][0], RainbowColors[rainbowIndex][1], RainbowColors[rainbowIndex][2], alphaVal);  // Same alpha val
            float outerVertexCosFactor = cos(rainbowAngle) * outerRadius;
			float outerVertexSinFactor = sin(rainbowAngle) * outerRadius;
			glVertex3f(outerVertexCosFactor, outerVertexSinFactor, 0.0f);
        }
        glEnd();
    }

    glPopMatrix();
    glDisable(GL_BLEND);     // Disables blending after drawing the rainbow
    glEnable(GL_LIGHTING);   // Re-enables lighting
}



// My Windmill functions

float bladeAngle = 0.0f; // Global variable for blade angle

void DrawWindmillTowerTop() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

    float baseRadius = 0.3f; // This must match top width of the tower
    float topRadius = 0.05f; // Smaller radius at the top of the cone
    float heightCone = 0.5f; // Height of the cone
    int coneSlices = 20; // Number of subdivisions around the Z axis
    int coneStacks = 20; // Number of subdivisions along the Z axis

    // Cone base
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f); // Translate to the top of the tower
    glRotatef(-90, 1.0f, 0.0f, 0.0f); // Orients the cone on top of the tower
	glColor3f(0.6f, 0.4f, 0.2f); // RGB for brown
    gluCylinder(quad, baseRadius, topRadius, heightCone, coneSlices, coneStacks);
    
    // Cone cap
    gluQuadricOrientation(quad, GLU_INSIDE);
    gluDisk(quad, 0.0f, baseRadius, coneSlices, 1); // Draws the top cap
    glPopMatrix();
    gluDeleteQuadric(quad);
	glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}


void DrawWindmillTower() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disables lighting

    float windmillTowerBaseWidth = 0.5f;
    float windmillTowerTopWidth = 0.3f;
    float windmillTowerHeight = 2.0f;
    int numSides = 4; // Creates a rectangle shape

    glBegin(GL_QUADS);
	// set windmill tower color to a dark grey blue
	glColor3f(0.0f, 0.0f, 0.2f); // RGB for dark grey blue
	
	
    for (int towerIndex = 0; towerIndex < numSides; ++towerIndex) {
        float angle = 2.0f * M_PI * towerIndex / numSides;
        float nextAngle = 2.0f * M_PI * (towerIndex + 1) / numSides;
        // Bottom vertices
		float towerVertX1 = windmillTowerBaseWidth * cos(angle);
		float towerVertY1 = windmillTowerBaseWidth * sin(angle);
		float towerVertZ1 = 0.0f;
        glVertex3f(towerVertX1, towerVertY1, towerVertZ1);

		float towerVertX2 = windmillTowerBaseWidth * cos(nextAngle);
		float towerVertY2 = windmillTowerBaseWidth * sin(nextAngle);
		float towerVertZ2 = 0.0f;
        glVertex3f(towerVertX2, towerVertY2, towerVertZ2);
        
		// Top vertices
		float towerVertX3 = windmillTowerTopWidth * cos(nextAngle);
		float towerVertY3 = windmillTowerTopWidth * sin(nextAngle);
		float towerVertZ3 = windmillTowerHeight;
        glVertex3f(towerVertX3, towerVertY3, towerVertZ3);
        
		
		float towerVertX4 = windmillTowerTopWidth * cos(angle);
		float towerVertY4 = windmillTowerTopWidth * sin(angle);
		float towerVertZ4 = windmillTowerHeight;
		glVertex3f(towerVertX4, towerVertY4, towerVertZ4);

    }
    glEnd();
	glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}


void DrawWindmillBlade() {
    float bladeWidth = 0.1f;
    float bladeLength = 1.0f;

    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, bladeWidth, 0.0f);
    glVertex3f(bladeLength, bladeWidth, 0.0f);
    glVertex3f(bladeLength, 0.0f, 0.0f);
    glEnd();
}


void DrawWindmillBlades() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disables lighting
    int numBlades = 4;
	// sets windmill blade color to white
	glColor3f(1.0f, 1.0f, 1.0f);  // RGB for white
    for (int bladeIndex = 0; bladeIndex < numBlades; ++bladeIndex) {
        glPushMatrix();
		// move blades a lil further out towards z
		glTranslatef(0.0f, 0.0f, 0.115f);
        glRotatef(bladeAngle, 0.0f, 0.0f, 1.0f); // Rotates the blades based on bladeAngle
        glRotatef(bladeIndex * (360.0f / numBlades), 0.0f, 0.0f, 1.0f);
        DrawWindmillBlade();
        glPopMatrix();
    }
	glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}


void DrawWindmill() {
    // Draws Windmill Tower
    glPushMatrix();
    glRotatef(270.0f, 1.0f, 0.0f, 0.0f); // Rotate 270 degrees around the X-axis
    DrawWindmillTower();
    glPopMatrix();

    // Draws Windmill Blades
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.35f); // Adjusts blade position to top of the tower
    DrawWindmillBlades();
    glPopMatrix();
	
}

// skybox RGB keytimes values, starts at dark evening when sun is below xyz plane and works to cyan when sun is overhead
Keytimes *SkyRed = new Keytimes();
Keytimes *SkyGreen = new Keytimes();
Keytimes *SkyBlue = new Keytimes();

void InitSkybox() {
    // Initialize Keytimes for sky color components to simulate sunrise, midday, sunset, and back to sunrise
    // Sky Red keytimes component values
    SkyRed->AddTimeValue(0.0f, 0.2f); // Sunrise
    SkyRed->AddTimeValue(0.25f, 0.53f); // Midday 
    SkyRed->AddTimeValue(0.5f, 0.2f); // Sunset 
    SkyRed->AddTimeValue(0.75f, 0.53f); // Midday
    SkyRed->AddTimeValue(1.0f, 0.2f); // Sunrise

    // Sky Green keytimes component values
    SkyGreen->AddTimeValue(0.0f, 0.3f); // Sunrise
    SkyGreen->AddTimeValue(0.25f, 0.81f); // Midday 
    SkyGreen->AddTimeValue(0.5f, 0.3f); // Sunset 
    SkyGreen->AddTimeValue(0.75f, 0.81f); // Midday 
    SkyGreen->AddTimeValue(1.0f, 0.3f); // Sunrise

    // Sky Blue keytimes component values
    SkyBlue->AddTimeValue(0.0f, 0.5f); // Sunrise
    SkyBlue->AddTimeValue(0.25f, 0.92f); // Midday 
    SkyBlue->AddTimeValue(0.5f, 0.5f); // Sunset 
    SkyBlue->AddTimeValue(0.75f, 0.92f); // Midday 
    SkyBlue->AddTimeValue(1.0f, 0.5f); // Back to sunrise 
}



void DrawSkybox() {
    glPushMatrix();
    glDisable(GL_LIGHTING);  // Disables lighting

    // Size of the skybox
    float skyboxSize = 100.0f; 

    // Disables depth writing
    glDepthMask(GL_FALSE);  

    // Starts drawing the skybox using a quad box shape
    glBegin(GL_QUADS);

    // Sets the color based on the keytime values at Time
    glColor3f(SkyRed->GetValue(Time), SkyGreen->GetValue(Time), SkyBlue->GetValue(Time)); // RGB for sky blue

    // Top face of skybox
    glVertex3f(-skyboxSize, skyboxSize, -skyboxSize);
    glVertex3f(-skyboxSize, skyboxSize, skyboxSize);
    glVertex3f(skyboxSize, skyboxSize, skyboxSize);
    glVertex3f(skyboxSize, skyboxSize, -skyboxSize);

    // Bottom face of skybox
    glVertex3f(-skyboxSize, -skyboxSize, -skyboxSize);
    glVertex3f(skyboxSize, -skyboxSize, -skyboxSize);
    glVertex3f(skyboxSize, -skyboxSize, skyboxSize);
    glVertex3f(-skyboxSize, -skyboxSize, skyboxSize);

    // Four sides of skybox
    glVertex3f(-skyboxSize, -skyboxSize, skyboxSize);
    glVertex3f(skyboxSize, -skyboxSize, skyboxSize);
    glVertex3f(skyboxSize, skyboxSize, skyboxSize);
    glVertex3f(-skyboxSize, skyboxSize, skyboxSize);

    glVertex3f(skyboxSize, -skyboxSize, -skyboxSize);
    glVertex3f(-skyboxSize, -skyboxSize, -skyboxSize);
    glVertex3f(-skyboxSize, skyboxSize, -skyboxSize);
    glVertex3f(skyboxSize, skyboxSize, -skyboxSize);

    glVertex3f(-skyboxSize, -skyboxSize, -skyboxSize);
    glVertex3f(-skyboxSize, -skyboxSize, skyboxSize);
    glVertex3f(-skyboxSize, skyboxSize, skyboxSize);
    glVertex3f(-skyboxSize, skyboxSize, -skyboxSize);

    glVertex3f(skyboxSize, -skyboxSize, skyboxSize);
    glVertex3f(skyboxSize, -skyboxSize, -skyboxSize);
    glVertex3f(skyboxSize, skyboxSize, -skyboxSize);
    glVertex3f(skyboxSize, skyboxSize, skyboxSize);

    glEnd();

    // Re-enables depth writing
    glDepthMask(GL_TRUE);

    glEnable(GL_LIGHTING);   // Re-enables lighting
    glPopMatrix();
}




// main program:
int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );
	LoadSandTexture();
	LoadGrassTexture();
	

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );
	InitSkybox();
	InitBuildingPositions();
	InitSunPosition();
	InitCloudPositions();
	InitClouds();

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it
void
Animate( )
{
	 // Update windmill blade rotation
    bladeAngle += 1.0f; // Adjust rotation speed as needed
    if (bladeAngle > 360.0f) {
        bladeAngle -= 360.0f;
    }

	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


Keytimes EyeX;
Keytimes EyeY;
Keytimes EyeZ;

const int MSEC = 10000;		// 10000 milliseconds = 10 seconds


// draw the complete scene:

void
Display( )
{
	int eyemsec = glutGet( GLUT_ELAPSED_TIME )  %  MSEC;
	// turn that into a time in seconds:
	float eyenowTime = (float)eyemsec  / 1000.;

	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif


	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( NowProjection == ORTHO )
		glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector using keytimes variables for the positions that change over time
	if (cameraMode == 0){
		gluLookAt( EyeX.GetValue(eyenowTime), EyeY.GetValue(eyenowTime), EyeZ.GetValue(eyenowTime),     0.f, 0.f, 0.f,     0.f, 1.f, 0.f );

	} else {
		// normal camera 5 units y above but also viewing the windmill from the front
		gluLookAt( 0.f, 5.f, 15.f,     0.f, 0.f, 0.f,     0.f, 1.f, 0.f );

		
	}
	
	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );

	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[NowColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );

	// Set up the light source as either a point light or a spot light
    if (currentLightType == POINT_LIGHT)
    {
        glEnable(GL_LIGHT0); // Enable point light
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0); // Disable spot light effect
    }
    else if (currentLightType == SPOT_LIGHT)
    {
        glEnable(GL_LIGHT0); // Enable spot light
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0); // Set the spot light cutoff angle
    }

	// Update the light source position
	float yCoord = SunPosY->GetValue(Time) - 12.0f;
	float xCoord = SunPosX->GetValue(Time);
	float zCoord = SunPosZ->GetValue(Time);


    GLfloat lightPosition[] = {
        xCoord,
        yCoord - 6.0f,
        zCoord,
        1.0f
    };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	
	// draw the box object by calling up its display list:
	// glCallList( BoxList );

	DrawSun(Time);
	DrawLand();
	 // Set up the view for the lake and draw it
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); 
    DrawBeachAndOcean();
	DrawLargeRainbow();
    glPopMatrix();
	DrawBuildings();
	DrawClouds();
	DrawWindmill();
	DrawWindmillTowerTop();
	DrawSkybox();

#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.f,   0.f, 1.f, 0.f );
			glCallList( BoxList );
		glPopMatrix( );
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );
	EyeX.Init();
	EyeX.AddTimeValue(0.0, 0.0);
	EyeX.AddTimeValue(1.0, 10.0);
	EyeX.AddTimeValue(2.0, 20.0);
	EyeX.AddTimeValue(3.0, 30.0);
	EyeX.AddTimeValue(4.0, 40.0);
	EyeX.AddTimeValue(5.0, 50.0);
	EyeX.AddTimeValue(6.0, 40.0);
	EyeX.AddTimeValue(7.0, 30.0);
	EyeX.AddTimeValue(8.0, 20.0);
	EyeX.AddTimeValue(9.0, 10.0);
	EyeX.AddTimeValue(10.0, 0.0);

	EyeY.Init();
	EyeY.AddTimeValue(0.0, 5.0);
	EyeY.AddTimeValue(1.0, 15.0);
	EyeY.AddTimeValue(2.0, 25.0);
	EyeY.AddTimeValue(3.0, 35.0);
	EyeY.AddTimeValue(4.0, 40.0);
	EyeY.AddTimeValue(5.0, 50.0);
	EyeY.AddTimeValue(6.0, 40.0);
	EyeY.AddTimeValue(7.0, 35.0);
	EyeY.AddTimeValue(8.0, 25.0);
	EyeY.AddTimeValue(9.0, 15.0);
	EyeY.AddTimeValue(10.0, 5.0);

	EyeZ.Init();
	EyeZ.AddTimeValue(0.0, 15.0);
	EyeZ.AddTimeValue(1.0, 15.0);
	EyeZ.AddTimeValue(2.0, 15.0);
	EyeZ.AddTimeValue(3.0, 20.0);
	EyeZ.AddTimeValue(4.0, 20.0);
	EyeZ.AddTimeValue(5.0, 20.0);
	EyeZ.AddTimeValue(6.0, 25.0);
	EyeZ.AddTimeValue(7.0, 25.0);
	EyeZ.AddTimeValue(8.0, 20.0);
	EyeZ.AddTimeValue(9.0, 20.0);
	EyeZ.AddTimeValue(10.0, 15.0);

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	// Defines Light Properties
    GLfloat lightPosition[] = {0.0f, 10.0f, -1.0f, 50.0f};  // light 50 y units above the origin
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 10000.0f};      // light color is white

	// Sets Up Light Source
    glEnable(GL_LIGHT0);          // Uses light source 0
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

	// create the axes:
	// AxesList = glGenLists( 1 );
	// glNewList( AxesList, GL_COMPILE );
	// 	glLineWidth( AXES_WIDTH );
	// 		Axes( 1.5 );
	// 	glLineWidth( 1. );
	// glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		// cameraMode
		case 'c':
		case 'C':
			cameraMode = !cameraMode;
			break;

        case 'p':
        case 'P':
            currentLightType = POINT_LIGHT;
            isSpotLight = 0;
            break;

        case 's':
        case 'S':
            currentLightType = SPOT_LIGHT;
            isSpotLight = 1;
            break;
        case 'q':
        case 'Q':
        case ESCAPE:
            DoMainMenu(QUIT); // will not return here
            break;            // happy compiler



		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );

    // Force a call to Display():
    glutSetWindow(MainWindow);
    glutPostRedisplay();
	}
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist; 
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}