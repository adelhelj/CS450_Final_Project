#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"

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
    float x, y, z; // Position
    float width;   // Width of building
    float height;  // Height of the building
    int colorIndex; // Index to the color array
} Building;


Building buildings[100]; // Array to hold building data

// Define boundaries (example values, adjust according to scene)
float minX = -50.0f; // Minimum X coordinate
float maxX = 50.0f;  // Maximum X coordinate
float minY = 0.0f;   // Minimum Y coordinate (ground level)
float maxY = 10.0f;  // Maximum Y coordinate
float minZ = -100.0f;  // Minimum Z coordinate (behind the windmill)
float maxZ = -10.0f; // Maximum Z coordinate


// Building color code

typedef struct {
    float r, g, b;  // RGB color components
} Color;


Color floridaKeysColors[] = {
    { 0.0f, 0.6f, 0.8f }, // Ocean Blue
    { 0.2f, 0.8f, 0.8f }, // Turquoise
    { 0.1f, 0.7f, 0.7f }, // Teal
    { 0.2f, 0.5f, 0.7f }, // Cerulean
    { 0.3f, 0.9f, 0.9f },  // Sky Blue
	{ 0.8f, 0.4f, 0.6f },// Carribean pink
	 // cream
	{ 0.9f, 0.8f, 0.6f },
	// marble
	{ 0.9f, 0.9f, 0.9f },
	// light grey
	{ 0.8f, 0.8f, 0.8f },
	// dark grey
	{ 0.6f, 0.6f, 0.6f },
	// dark brown
	{ 0.4f, 0.2f, 0.0f },
	// light brown
	{ 0.6f, 0.4f, 0.2f },
};






void InitBuildingPositions() {
    // Define the maximum height of buildings relative to the windmill
    float maxBuildingHeight = 2.0f; // Assuming windmill height is 2.0 units
	int numColors = sizeof(floridaKeysColors) / sizeof(Color);

    for (int i = 0; i < 100; i++) {
        buildings[i].x = minX + ((float)rand() / RAND_MAX) * (maxX - minX);
        buildings[i].y = minY; // Ground level
        buildings[i].z = minZ + ((float)rand() / RAND_MAX) * (maxZ - minZ);
        // Set the building height to be a random value, but not more than maxBuildingHeight
        buildings[i].height = (float)(rand() % (int)(maxBuildingHeight * 10)) / 10.0f + 0.5f; // Added 0.5 to ensure we don't have 0 height
        buildings[i].width = (float)(rand() % 4 + 1);
		buildings[i].colorIndex = rand() % numColors; // Assign a random color index

    }
}



void DrawBuildings() {
    

    for (int i = 0; i < 100; i++) {
		Color color = floridaKeysColors[buildings[i].colorIndex];
        glPushMatrix();
        // Translate to the building's location
        glTranslatef(buildings[i].x, buildings[i].y + buildings[i].height * 0.5, buildings[i].z);
        // Scale the cube to the building's width and height
        glScalef(buildings[i].width, buildings[i].height, buildings[i].width);
		// set building color to grey white
		glColor3f(color.r, color.g, color.b); // Set the building color
        // Draw the main building cube
        glutSolidCube(1.0f);

        // Set roof color
        glColor3f(0.5f, 0.5f, 0.5f); // RGB for grey

        // Draw the roof
        // Translate up by half the height of the building plus half the roof thickness
        glTranslatef(0.0f, 0.5f + 0.05f, 0.0f);
        // Scale the roof to be slightly larger than the building, and flatter
        glScalef(1.1f, 0.1f, 1.1f);
        // Draw the roof cube
        glutSolidCube(1.0f);
        glPopMatrix();
    }
}





// My beach and ocean functions
// Global texture ID for the beach sand
GLuint beachTextureId;

void LoadSandTexture() {
    // The path to your BMP texture file
    char sandTextureFile[] = "sand.bmp";
    
    int width, height;
    unsigned char *sandTextureData = BmpToTexture(sandTextureFile, &width, &height);
    if (sandTextureData == NULL) {
        fprintf(stderr, "Cannot load sand texture\n");
        exit(1);
    }

    // Generate texture ID and bind it
    glGenTextures(1, &beachTextureId);
    glBindTexture(GL_TEXTURE_2D, beachTextureId);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Send texture data to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, sandTextureData);

    // Clean up
    delete[] sandTextureData;
}

// land / grass texture id and texture code
GLuint grassTextureId;

void LoadGrassTexture(){
	// path to grass texture file
	char grassTextureFile[] = "dark_grass.bmp";

	int width, height;
	unsigned char *grassTextureData = BmpToTexture(grassTextureFile, &width, &height);
	if (grassTextureData == NULL){
		fprintf(stderr, "Cannot load grass texture\n");
		exit(1);
	}

	// generate texture id and bind it
	glGenTextures(1, &grassTextureId);
	glBindTexture(GL_TEXTURE_2D, grassTextureId);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat texture in S direction
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repeat texture in T direction
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear mag filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear min filter

	// send texture data to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, grassTextureData);

	// clean up
	delete[] grassTextureData;
}




const int BEACH_WIDTH = 200; // Width of the beach plane
const int BEACH_DEPTH = 12; // Reduced depth of the beach plane so it doesn't extend too far
const float BEACH_START_Z = 5.0f; // Start Z position of the beach just past the windmill
const float WATER_LEVEL = 0.1f; // Height of the water relative to the land

void DrawBeachAndOcean() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

	// Number of times to repeat the texture
    const float textureRepeatWidth = 30.0f;
    const float textureRepeatDepth = 300.0f;

    // Bind the sand texture
    glBindTexture(GL_TEXTURE_2D, beachTextureId);
    glEnable(GL_TEXTURE_2D);

    // Set the beach color to sandy yellow and bind the texture
    glColor3f(0.9f, 0.8f, 0.5f); // sandy yellow color

    // Draw the beach quad with texture coordinates
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-BEACH_WIDTH / 2, WATER_LEVEL, BEACH_START_Z);
        glTexCoord2f(textureRepeatWidth, 0.0f); glVertex3f(BEACH_WIDTH / 2, WATER_LEVEL, BEACH_START_Z);
        glTexCoord2f(textureRepeatWidth, textureRepeatDepth); glVertex3f(BEACH_WIDTH / 2, WATER_LEVEL, BEACH_START_Z + BEACH_DEPTH);
        glTexCoord2f(0.0f, textureRepeatDepth); glVertex3f(-BEACH_WIDTH / 2, WATER_LEVEL, BEACH_START_Z + BEACH_DEPTH);
    glEnd();

    glDisable(GL_TEXTURE_2D);


    // Set the ocean color to a nice shade of blue
    glColor3f(0.0f, 0.5f, 0.8f); // blue color

    // Draw the ocean quad, it should start where the land ends
    glBegin(GL_QUADS);
        glVertex3f(-BEACH_WIDTH / 2, WATER_LEVEL, BEACH_START_Z + BEACH_DEPTH);
        glVertex3f(BEACH_WIDTH / 2, WATER_LEVEL, BEACH_START_Z + BEACH_DEPTH);
        glVertex3f(BEACH_WIDTH / 2, WATER_LEVEL, 1000.0f);
        glVertex3f(-BEACH_WIDTH / 2, WATER_LEVEL, 1000.0f);
    glEnd();
	
	glEnable(GL_LIGHTING);   // Re-enable lighting
    glPopMatrix();
}

// Sun functions
// Create three instances of Keytimes for each axis
Keytimes *SunPosX = new Keytimes();
Keytimes *SunPosY = new Keytimes();
Keytimes *SunPosZ = new Keytimes();

// Create three instances of Keytimes for each sun RGB color component
Keytimes *SunRed = new Keytimes();
Keytimes *SunGreen = new Keytimes();
Keytimes *SunBlue = new Keytimes();


void InitSunPosition() {
    float hugeNumber = 100.0f; // Adjust this value as needed

    // Initialize Keytimes for X-axis (if the sun moves along X-axis)
    SunPosX->AddTimeValue(0.0f, 0.0f); // Start position
    SunPosX->AddTimeValue(0.5f, 0.0f); // Mid position
    SunPosX->AddTimeValue(1.0f, 0.0f); // End position

    // Initialize Keytimes for Y-axis
    SunPosY->AddTimeValue(0.0f, 0.0f); // Start position (below the horizon)
    SunPosY->AddTimeValue(0.5f, hugeNumber); // Mid position (above the horizon)
    SunPosY->AddTimeValue(1.0f, 0.0f); // End position (back below the horizon)

    // Initialize Keytimes for Z-axis
    SunPosZ->AddTimeValue(0.0f, -hugeNumber); // Start position (far away)
    SunPosZ->AddTimeValue(0.5f, 0.0f); // Mid position (closest point)
    SunPosZ->AddTimeValue(1.0f, hugeNumber); // End position (far away again)

	// Initialize Keytimes for sun color components to have sun red orange in beginning and end and yellow at top
	// Sun Red component
	SunRed->AddTimeValue(0.0f, 1.0f); // Start position (yellow - high red)
	SunRed->AddTimeValue(0.5f, 1.0f); // Mid position (yellow-orange - higher red)
	SunRed->AddTimeValue(1.0f, 1.0f); // End position (yellow - return to high red)

	// Sun Green component
	SunGreen->AddTimeValue(0.0f, 1.0f); // Start position (yellow - high green)
	SunGreen->AddTimeValue(0.5f, 0.8f); // Mid position (yellow-orange - slightly less green)
	SunGreen->AddTimeValue(1.0f, 1.0f); // End position (yellow - return to high green)

	// Sun Blue component
	SunBlue->AddTimeValue(0.0f, 0.0f); // Start position (yellow - no blue)
	SunBlue->AddTimeValue(0.5f, 0.0f); // Mid position (yellow-orange - no blue)
	SunBlue->AddTimeValue(1.0f, 0.0f); // End position (yellow - no blue)


}

void DrawSunPosition(float time) {
	
    // Convert linear time to ping-pong time
    float pingPongTime = 1.0f - fabsf(2.0f * time - 1.0f);

    // Get interpolated values for each axis using ping-pong time
    float sunPosX = SunPosX->GetValue(pingPongTime);
    float sunPosY = SunPosY->GetValue(pingPongTime);
    float sunPosZ = SunPosZ->GetValue(pingPongTime);

    // Apply the translation to move the sun
    glTranslatef(sunPosX, sunPosY, sunPosZ);
}



void DrawSun(float time) {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting
    // set sun color to keytimes vals
	glColor3f(SunRed->GetValue(time), SunGreen->GetValue(time), SunBlue->GetValue(time)); // RGB for sun color
    glPushMatrix();
    DrawSunPosition(Time);
    glutSolidSphere(5.0f, 20, 20); // Draw the sun as a sphere
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
	glEnable(GL_LIGHTING);   // Re-enable lighting
    glPopMatrix();
}


// Cloud functions
// Structure for cloud puffs
// Define an array to hold cloud positions
typedef struct CloudPosition {
    float x, y, z;
} CloudPosition;

CloudPosition cloudPositions[100]; // Array to hold cloud positions

// Initialize cloud positions
void InitCloudPositions() {
    for (int i = 0; i < 100; i++) {
        cloudPositions[i].x = (float)(rand() % 200 - 100);
        cloudPositions[i].y = (float)(rand() % 100 + 40);
        cloudPositions[i].z = (float)(rand() % 200 - 100);
    }
}

typedef struct CloudPuff {
    float offsetX, offsetY, offsetZ; // Offset positions for each puff
    float size;                      // Size of each puff
} CloudPuff;

// Structure for a cloud
typedef struct Cloud {
    CloudPuff puffs[7]; // Each cloud can have up to 7 puffs
    int numPuffs;       // Actual number of puffs in this cloud
} Cloud;

Cloud clouds[100]; // Array to hold all clouds

void InitClouds() {
    for (int i = 0; i < 100; i++) {
        // Randomize cloud position
        clouds[i].numPuffs = 3 + rand() % 5;
        for (int n = 0; n < clouds[i].numPuffs; n++) {
            clouds[i].puffs[n].offsetX = (rand() % 20 - 10) * 0.1f;
            clouds[i].puffs[n].offsetY = (rand() % 20 - 10) * 0.1f;
            clouds[i].puffs[n].offsetZ = (rand() % 20 - 10) * 0.1f;
            clouds[i].puffs[n].size = 1.0f + (rand() % 100) / 50.0f;
        }
    }
}

void DrawCloud(Cloud cloud) {
    for (int n = 0; n < cloud.numPuffs; n++) {
        glPushMatrix();
        
        // Increase the size of each puff to make the cloud puffier
        float puffSize = cloud.puffs[n].size * 2.0f; // You can adjust the factor as needed
        
        // Slightly randomize the position of each puff within the cloud
        float offsetX = cloud.puffs[n].offsetX * 2.0f; // Adjust this factor for more randomness
        float offsetY = cloud.puffs[n].offsetY * 2.0f;
        float offsetZ = cloud.puffs[n].offsetZ * 2.0f;
        
        glTranslatef(offsetX, offsetY, offsetZ);
        glutSolidSphere(puffSize, 20, 20);
        glPopMatrix();
    }
}


void DrawClouds() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

	SetMaterial(1.0f, 1.0f, 1.0f, 100.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // White color for clouds
    for (int i = 0; i < 100; i++) {
        glPushMatrix();
        glTranslatef(cloudPositions[i].x, cloudPositions[i].y, cloudPositions[i].z);
        DrawCloud(clouds[i]);
        glPopMatrix();
    }

	glEnable(GL_LIGHTING);   // Re-enable lighting
    glPopMatrix();
}




// My land functions - draw the land with grass texture
void DrawLand(){
    glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

    // Number of times to repeat the texture
    const float textureRepeatX = 50.0f; // Repeat 50 times along the X-axis
    const float textureRepeatZ = 50.0f; // Repeat 50 times along the Z-axis

    // Bind the grass texture
    glBindTexture(GL_TEXTURE_2D, grassTextureId);
    glEnable(GL_TEXTURE_2D);

    // Set the grass color to green and bind the texture
    glColor3f(0.0f, 0.5f, 0.0f); // green color

    // Draw the land quad with repeated texture coordinates
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1000.0f, 0.0f, 1000.0f);
        glTexCoord2f(textureRepeatX, 0.0f); glVertex3f(1000.0f, 0.0f, 1000.0f);
        glTexCoord2f(textureRepeatX, textureRepeatZ); glVertex3f(1000.0f, 0.0f, -1000.0f);
        glTexCoord2f(0.0f, textureRepeatZ); glVertex3f(-1000.0f, 0.0f, -1000.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_TEXTURE_2D); // Disable texturing
    glEnable(GL_LIGHTING);    // Re-enable lighting
    glPopMatrix();
}

	

// My Windmill functions

float bladeAngle = 0.0f; // Global variable for blade angle

void DrawWindmillTowerTop() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

    float baseRadius = 0.3f; // Match this with the top width of the tower
    float topRadius = 0.05f; // Smaller radius for the top of the cone
    float height = 0.5f; // Height of the cone
    int slices = 20; // Number of subdivisions around the Z axis
    int stacks = 20; // Number of subdivisions along the Z axis

    // Cone base
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f); // Translate to the top of the tower
    glRotatef(-90, 1.0f, 0.0f, 0.0f); // Orient the cone correctly
	// Set the cone color to brown that contrasts cream white
	glColor3f(0.6f, 0.4f, 0.2f); // RGB for brown
	
    
    gluCylinder(quad, baseRadius, topRadius, height, slices, stacks);
    
    // Cone cap
    gluQuadricOrientation(quad, GLU_INSIDE);
    gluDisk(quad, 0.0f, baseRadius, slices, 1); // Draw top cap
    glPopMatrix();
    gluDeleteQuadric(quad);
	glEnable(GL_LIGHTING);   // Re-enable lighting
    glPopMatrix();
}


void DrawWindmillTower() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

    float baseWidth = 0.5f;
    float topWidth = 0.3f;
    float height = 2.0f;
    int numSides = 4; // Simple rectangular shape

    glBegin(GL_QUADS);
	// set windmill tower color to a dark grey blue
	glColor3f(0.0f, 0.0f, 0.2f); // RGB for dark grey blue
	
	
    for (int i = 0; i < numSides; ++i) {
        float angle = 2.0f * M_PI * i / numSides;
        float nextAngle = 2.0f * M_PI * (i + 1) / numSides;
        // Bottom vertices
        glVertex3f(baseWidth * cos(angle), baseWidth * sin(angle), 0.0f);
        glVertex3f(baseWidth * cos(nextAngle), baseWidth * sin(nextAngle), 0.0f);
        // Top vertices
        glVertex3f(topWidth * cos(nextAngle), topWidth * sin(nextAngle), height);
        glVertex3f(topWidth * cos(angle), topWidth * sin(angle), height);

    }
    glEnd();
	glEnable(GL_LIGHTING);   // Re-enable lighting
    glPopMatrix();
}


void DrawWindmillBlade() {
    float width = 0.1f;
    float length = 1.0f;

    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, width, 0.0f);
    glVertex3f(length, width, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);
    glEnd();
}


void DrawWindmillBlades() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting
    int numBlades = 4;
	// set windmill blade color to white
	glColor3f(1.0f, 1.0f, 1.0f);  // RGB for white
    for (int i = 0; i < numBlades; ++i) {
        glPushMatrix();
		// move blades a lil further out towards z
		glTranslatef(0.0f, 0.0f, 0.115f);
        glRotatef(bladeAngle, 0.0f, 0.0f, 1.0f); // Rotate the blades based on bladeAngle
        glRotatef(i * (360.0f / numBlades), 0.0f, 0.0f, 1.0f);
        DrawWindmillBlade();
        glPopMatrix();
    }
	glEnable(GL_LIGHTING);   // Re-enable lighting
    glPopMatrix();
}


void DrawWindmill() {
	
    // Draw Tower
    glPushMatrix();
    glRotatef(270.0f, 1.0f, 0.0f, 0.0f); // Rotate 270 degrees around the X-axis
    DrawWindmillTower();
    glPopMatrix();

    // Draw Blades
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.35f); // Adjust blade position to top of the tower
    DrawWindmillBlades();
    glPopMatrix();
	
}

void DrawSkybox() {
	glPushMatrix();
    glDisable(GL_LIGHTING);  // Disable lighting

    // Size of the skybox - should be large enough to encompass the whole scene
    float size = 100.0f; 

    // Disable depth writing
    glDepthMask(GL_FALSE);  

    // Start drawing the skybox
    glBegin(GL_QUADS);

    // Set the color to sky blue
    glColor3f(0.53f, 0.81f, 0.92f);  // RGB for sky blue

    // Top face
    glVertex3f(-size, size, -size);
    glVertex3f(-size, size, size);
    glVertex3f(size, size, size);
    glVertex3f(size, size, -size);

    // Bottom face
    glVertex3f(-size, -size, -size);
    glVertex3f(size, -size, -size);
    glVertex3f(size, -size, size);
    glVertex3f(-size, -size, size);

    // Four sides
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    glVertex3f(size, size, size);
    glVertex3f(-size, size, size);

    glVertex3f(size, -size, -size);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, size, -size);
    glVertex3f(size, size, -size);

    glVertex3f(-size, -size, -size);
    glVertex3f(-size, -size, size);
    glVertex3f(-size, size, size);
    glVertex3f(-size, size, -size);

    glVertex3f(size, -size, size);
    glVertex3f(size, -size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, size, size);

    glEnd();

    // Re-enable depth writing
    glDepthMask(GL_TRUE);

	glEnable(GL_LIGHTING);   // Re-enable lighting
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

float lightRadius = 10.0f;  // Radius of the circular path
float lightAngle = 0.0f;   // Current angle of the light source in radians
float angularSpeed = 0.01f;  // Angular speed for circular motion


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
        yCoord,
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

	// Define Light Properties
    GLfloat lightPosition[] = {0.0f, 10.0f, -1.0f, 50.0f};  // light 50 y units above the origin
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1000.0f};      // light color is white

	// Set Up Light Source
    glEnable(GL_LIGHT0);          // Use light source 0
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

	// create the axes:
	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
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

		case 'o':
        case 'O':
            NowProjection = ORTHO;
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

        case 'w':
            currentLightColor = 0; // White
            break;
        case 'r':
            currentLightColor = 1; // Red
            break;
        case 'g':
            currentLightColor = 2; // Green
            break;
        case 'b':
            currentLightColor = 3; // Blue
            break;
        case 'y':
            currentLightColor = 4; // Yellow
            break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	// Update the light source color based on the currentLightColor variable
    GLfloat lightColor[] = {1.0f, 0.0f, 0.0f, 1.0f}; // Default to white
    switch (currentLightColor)
    {
        case 0: // White
            lightColor[0] = 1.0f;
            lightColor[1] = 1.0f;
            lightColor[2] = 1.0f;
			glColor3f(1.0f, 1.0f, 1.0f);
            break;
        case 1: // Red
            lightColor[0] = 1.0f;
			lightColor[1] = 0.0f;
			lightColor[2] = 0.0f;
			glColor3f(1.0f, 0.0f, 0.0f);
            break;
        case 2: // Green
			lightColor[0] = 0.0f;
            lightColor[1] = 1.0f;
			lightColor[2] = 0.0f;
			glColor3f(0.0f, 1.0f, 0.0f);
            break;
        case 3: // Blue
			lightColor[0] = 0.0f;
			lightColor[1] = 0.0f;
            lightColor[2] = 1.0f;
			glColor3f(0.0f, 0.0f, 1.0f);
            break;
        case 4: // Yellow
            lightColor[0] = 1.0f;
            lightColor[1] = 1.0f;
			lightColor[2] = 0.0f;
			glColor3f(1.0f, 1.0f, 0.0f);
            break;
    }
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

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