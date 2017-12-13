/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: lab12
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with VAOs & VBOs using a
 *	MD5 model.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2017
 *
 *
 */

//******************************************************************************

#include <GL/glew.h>
#include <GLFW/glfw3.h> // include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL/SOIL.h> // for image loading

#include <stdio.h>  // for printf functionality
#include <stdlib.h> // for exit functionality

#include <vector> // for vector

#include <CSCI441/FramebufferUtils3.hpp>
#include <CSCI441/modelLoader3.hpp>
#include <CSCI441/objects3.hpp>
#include <CSCI441/ShaderProgram3.hpp>

#include "../include/MD5/md5model.h" // for our MD5 Model
#include "FountainParticleSystem.h"

//******************************************************************************
//
// Global Parameters

struct VertexTextured
{
	float x, y, z;
	float s, t;
};

int windowWidth, windowHeight;
bool controlDown = false;
bool leftMouseDown = false;
glm::vec2 mousePosition(-9999.0f, -9999.0f);

glm::vec3 cameraAngles(1.82f, 2.01f, 25.0f);
glm::vec3 eyePoint(3.0f, 3.0f, 3.0f);
glm::vec3 lookAtPoint(0.0f, 0.0f, 0.0f);
glm::vec3 upVector(0.0f, 1.0f, 0.0f);

GLuint platformVAOd;
GLuint platformTextureHandle;

GLuint skyboxVAOds[6];   // all of our skybox VAOs
GLuint skyboxHandles[6]; // all of our skybox handles

CSCI441::ShaderProgram *textureShaderProgram, *hellknightShaderProgram, *hellknightSkeletonShaderProgram = NULL;
GLint uniform_modelMtx_loc, uniform_viewProjetionMtx_loc, uniform_tex_loc, uniform_color_loc;
GLint attrib_vPos_loc, attrib_vTextureCoord_loc;

CSCI441::ShaderProgram *modelPhongShaderProgram = NULL;
GLint uniform_phong_mv_loc, uniform_phong_v_loc, uniform_phong_p_loc, uniform_phong_norm_loc;
GLint uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_ma_loc, uniform_phong_s_loc;
GLint uniform_phong_txtr_loc, attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc;

CSCI441::ModelLoader *model = NULL;

GLuint texturedQuadVAO;

GLfloat platformSize = 20.0f;

CSCI441::ModelLoader *fountain = NULL;

vector<ParticleSystem*> systems;
vector<CSCI441::ShaderProgram *> particleShaderPrograms;
vector<GLuint> particleTextureHandles;
vector<GLint> modelview_uniform_locations;
vector<GLint> projection_uniform_locations;
vector<GLint> vpos_attrib_locations;
GLuint pointsVAO;
GLuint pointsVBO;

int NUM_POINTS = 99999000;

float masterTime = 0;

// Hellknight vars
md5_model_t md5model;
md5_anim_t md5animation;

md5_joint_t *skeleton = NULL;
anim_info_t animInfo;

bool animated = false;
bool displaySkeleton = false;
bool displayWireframe = false;
bool displayMesh = true;

GLuint vaoHellknight;

GLuint vbodsHellknight[2];

GLint pt_model_location, pt_view_location, pt_projection_location, pt_texture_location;

//******************************************************************************
//
// Helper Functions

// convertSphericalToCartesian() ///////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraAngles is updated.
//
////////////////////////////////////////////////////////////////////////////////
void convertSphericalToCartesian()
{
	eyePoint.x = cameraAngles.z * sinf(cameraAngles.x) * sinf(cameraAngles.y);
	eyePoint.y = cameraAngles.z * -cosf(cameraAngles.y);
	eyePoint.z = cameraAngles.z * -cosf(cameraAngles.x) * sinf(cameraAngles.y);
}

bool registerOpenGLTexture(unsigned char *textureData,
						   unsigned int texWidth, unsigned int texHeight,
						   GLuint &textureHandle)
{
	if (textureData == 0)
	{
		fprintf(stderr, "Cannot register texture; no data specified.");
		return false;
	}

	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

	return true;
}

//******************************************************************************
//
// Event Callbacks

// error_callback() ////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
////////////////////////////////////////////////////////////////////////////////
static void error_callback(int error, const char *description)
{
	fprintf(stderr, "[ERROR]: %s\n", description);
}

// key_callback() //////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's keypress callback.
//	Responds to key presses and key releases
//
////////////////////////////////////////////////////////////////////////////////
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_ESCAPE || key == 'Q') && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
		displaySkeleton = !displaySkeleton;
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
		displayWireframe = !displayWireframe;
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		displayMesh = !displayMesh;
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//		We will register this function as GLFW's mouse button callback.
//	Responds to mouse button presses and mouse button releases.  Keeps track if
//	the control key was pressed when a left mouse click occurs to allow
//	zooming of our arcball camera.
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		leftMouseDown = true;
		controlDown = (mods & GLFW_MOD_CONTROL);
	}
	else
	{
		leftMouseDown = false;
		mousePosition.x = -9999.0f;
		mousePosition.y = -9999.0f;
		controlDown = false;
	}
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's cursor movement callback.
//	Responds to mouse movement.  When active motion is used with the left
//	mouse button an arcball camera model is followed.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
	// make sure movement is in bounds of the window
	// glfw captures mouse movement on entire screen

	if (xpos > 0 && xpos < windowWidth)
	{
		if (ypos > 0 && ypos < windowHeight)
		{
			// active motion
			if (leftMouseDown)
			{
				if ((mousePosition.x - -9999.0f) < 0.001f)
				{
					mousePosition.x = xpos;
					mousePosition.y = ypos;
				}
				else
				{
					if (!controlDown)
					{
						cameraAngles.x += (xpos - mousePosition.x) * 0.005f;
						cameraAngles.y += (ypos - mousePosition.y) * 0.005f;

						if (cameraAngles.y < 0)
							cameraAngles.y = 0.0f + 0.001f;
						if (cameraAngles.y >= M_PI)
							cameraAngles.y = M_PI - 0.001f;
					}
					else
					{
						double totChgSq = (xpos - mousePosition.x) + (ypos - mousePosition.y);
						cameraAngles.z += totChgSq * 0.01f;

						if (cameraAngles.z <= 2.0f)
							cameraAngles.z = 2.0f;
						if (cameraAngles.z >= 35.0f)
							cameraAngles.z = 35.0f;
					}
					convertSphericalToCartesian();

					mousePosition.x = xpos;
					mousePosition.y = ypos;
				}
			}
			// passive motion
			else
			{
			}
		}
	}
}

// scroll_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's scroll wheel callback.
//	Responds to movement of the scroll where.  Allows zooming of the arcball
//	camera.
//
////////////////////////////////////////////////////////////////////////////////
static void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
	double totChgSq = yOffset;
	cameraAngles.z += totChgSq * 0.2f;

	// if (cameraAngles.z <= 2.0f)
	// 	cameraAngles.z = 2.0f;
	// if (cameraAngles.z >= 35.0f)
	// 	cameraAngles.z = 35.0f;

	convertSphericalToCartesian();
}

//******************************************************************************
//
// Setup Functions

void setupSkybox() {

}

void setupParticleSystems() {
	// main fountain particle system
	GLint handle = CSCI441::TextureUtils::loadAndRegisterTexture("textures/snowflake.png");
	FountainParticleSystem *fountainSystem = new FountainParticleSystem(FOUNTAIN, glm::vec3(0, 0, 0), 0.2f, 0.1f, 10, 100, -9.81, 1000, handle);
	systems.push_back(fountainSystem);
} 

void updateParticleSystems() {
	// printf("%f\n", masterTime);
	for (ParticleSystem* s : systems) {
		s->updateParticles(masterTime);
	}
}

// loadMD5Model() //////////////////////////////////////////////////////////////
//
//      Load in the MD5 Model
//
////////////////////////////////////////////////////////////////////////////////
void loadMD5Model()
{
	/* Load MD5 model file */
	if (!ReadMD5Model("models/monsters/hellknight/mesh/hellknight.md5mesh", &md5model))
		exit(EXIT_FAILURE);

	AllocVertexArrays(); // allocate memory for arrays and create VAO for MD5 Model

	/* Load MD5 animation file */
	if (!ReadMD5Anim("models/monsters/hellknight/animations/idle2.md5anim", &md5animation))
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		// successful loading...set up animation parameters
		animInfo.curr_frame = 0;
		animInfo.next_frame = 1;

		animInfo.last_time = 0;
		animInfo.max_time = 1.0 / md5animation.frameRate;

		/* Allocate memory for animated skeleton */
		skeleton = (md5_joint_t *)
			malloc(sizeof(md5_joint_t) * md5animation.num_joints);
	}

	printf("\n");
}

// setupGLFW() /////////////////////////////////////////////////////////////////
//
//		Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
////////////////////////////////////////////////////////////////////////////////
GLFWwindow *setupGLFW()
{
	// set what function to use when registering errors
	// this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
	// all other GLFW calls must be performed after GLFW has been initialized
	glfwSetErrorCallback(error_callback);

	// initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "[ERROR]: Could not initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
	else
	{ 
		fprintf(stdout, "[INFO]: GLFW initialized\n");
	}

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // request forward compatible OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // request OpenGL Core Profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);				   // request OpenGL 3.x context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);				   // request OpenGL 3.3 context

	// create a window for a given size, with a given title
	GLFWwindow *window = glfwCreateWindow(640, 480, "Lab12: FBOs", NULL, NULL);
	if (!window)
	{ // if the window could not be created, NULL is returned
		fprintf(stderr, "[ERROR]: GLFW Window could not be created\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	else
	{
		fprintf(stdout, "[INFO]: GLFW Window created\n");
	}

	glfwMakeContextCurrent(window); // make the created window the current window
	glfwSwapInterval(1);			// update our screen after at least 1 screen refresh

	glfwSetKeyCallback(window, key_callback);				   // set our keyboard callback function
	glfwSetMouseButtonCallback(window, mouse_button_callback); // set our mouse button callback function
	glfwSetCursorPosCallback(window, cursor_callback);		   // set our cursor position callback function
	glfwSetScrollCallback(window, scroll_callback);			   // set our scroll wheel callback function

	return window; // return the window that was created
}

// setupOpenGL() ///////////////////////////////////////////////////////////////
//
//      Used to setup everything OpenGL related.
//
////////////////////////////////////////////////////////////////////////////////
void setupOpenGL()
{
	glEnable(GL_DEPTH_TEST); // enable depth testing
	glDepthFunc(GL_LESS);	// use less than depth test

	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);								   // enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use one minus blending equation

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the frame buffer to black
}

// setupGLEW() /////////////////////////////////////////////////////////////////
//
//      Used to initialize GLEW
//
////////////////////////////////////////////////////////////////////////////////
void setupGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum glewResult = glewInit();

	/* check for an error */
	if (glewResult != GLEW_OK)
	{
		printf("[ERROR]: Error initalizing GLEW\n");
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "[ERROR]: %s\n", glewGetErrorString(glewResult));
		exit(EXIT_FAILURE);
	}
	else
	{
		fprintf(stdout, "[INFO]: GLEW initialized\n");
		fprintf(stdout, "[INFO]: Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}

	if (!glewIsSupported("GL_VERSION_2_0"))
	{
		printf("[ERROR]: OpenGL not version 2.0+.  GLSL not supported\n");
		exit(EXIT_FAILURE);
	}
}

// setupTextures() /////////////////////////////////////////////////////////////
//
//      Load and register all the tetures for our program
//
////////////////////////////////////////////////////////////////////////////////
void setupTextures()
{
	platformTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture("textures/ground.png");
	
	// and get handles for our full skybox
	printf("[INFO]: registering skybox...\n");
	fflush(stdout);
	skyboxHandles[0] = CSCI441::TextureUtils::loadAndRegisterTexture("textures/skybox/DOOM16BK.png");
	skyboxHandles[3] = CSCI441::TextureUtils::loadAndRegisterTexture("textures/skybox/DOOM16LF.png");
	skyboxHandles[2] = CSCI441::TextureUtils::loadAndRegisterTexture("textures/skybox/DOOM16FT.png");
	skyboxHandles[1] = CSCI441::TextureUtils::loadAndRegisterTexture("textures/skybox/DOOM16RT.png");
	skyboxHandles[4] = CSCI441::TextureUtils::loadAndRegisterTexture("textures/skybox/DOOM16DN.png");
	skyboxHandles[5] = CSCI441::TextureUtils::loadAndRegisterTexture("textures/skybox/DOOM16UP.png");
	printf("[INFO]: skybox textures read in and registered!\n\n");
}

void setupShaders()
{
	textureShaderProgram = new CSCI441::ShaderProgram("shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl");
	uniform_modelMtx_loc = textureShaderProgram->getUniformLocation("modelMtx");
	uniform_viewProjetionMtx_loc = textureShaderProgram->getUniformLocation("viewProjectionMtx");
	uniform_tex_loc = textureShaderProgram->getUniformLocation("tex");
	uniform_color_loc = textureShaderProgram->getUniformLocation("color");
	attrib_vPos_loc = textureShaderProgram->getAttributeLocation("vPos");
	attrib_vTextureCoord_loc = textureShaderProgram->getAttributeLocation("vTextureCoord");

	modelPhongShaderProgram = new CSCI441::ShaderProgram("shaders/texturingPhong.v.glsl", "shaders/texturingPhong.f.glsl");
	uniform_phong_mv_loc = modelPhongShaderProgram->getUniformLocation("modelviewMtx");
	uniform_phong_v_loc = modelPhongShaderProgram->getUniformLocation("viewMtx");
	uniform_phong_p_loc = modelPhongShaderProgram->getUniformLocation("projectionMtx");
	uniform_phong_norm_loc = modelPhongShaderProgram->getUniformLocation("normalMtx");
	uniform_phong_md_loc = modelPhongShaderProgram->getUniformLocation("materialDiffuse");
	uniform_phong_ms_loc = modelPhongShaderProgram->getUniformLocation("materialSpecular");
	uniform_phong_ma_loc = modelPhongShaderProgram->getUniformLocation("materialAmbient");
	uniform_phong_s_loc = modelPhongShaderProgram->getUniformLocation("materialShininess");
	uniform_phong_txtr_loc = modelPhongShaderProgram->getUniformLocation("txtr");
	attrib_phong_vpos_loc = modelPhongShaderProgram->getAttributeLocation("vPos");
	attrib_phong_vnorm_loc = modelPhongShaderProgram->getAttributeLocation("vNormal");
	attrib_phong_vtex_loc = modelPhongShaderProgram->getAttributeLocation("vTexCoord");

	hellknightShaderProgram = new CSCI441::ShaderProgram("shaders/hellknightShader.v.glsl", "shaders/hellknightShader.f.glsl");
	pt_model_location = hellknightShaderProgram->getUniformLocation("model");
	pt_view_location = hellknightShaderProgram->getUniformLocation("view");
	pt_projection_location = hellknightShaderProgram->getUniformLocation("projection");
	pt_texture_location = hellknightShaderProgram->getUniformLocation("tex");

	// TODO add particle system shader programs
	CSCI441::ShaderProgram* program = NULL;
	GLuint modelview_uniform_location = -1;
	GLuint projection_uniform_location = -1;
	GLint vpos_attrib_location = -1;

	for (ParticleSystem* s : systems) {
		switch(s->type) { 
			case FOUNTAIN:
				program = new CSCI441::ShaderProgram("shaders/billboardQuadShader.v.glsl", 
													"shaders/billboardQuadShader.g.glsl", 
													"shaders/billboardQuadShader.f.glsl");

				modelview_uniform_location = program->getUniformLocation("mvMatrix");
				projection_uniform_location = program->getUniformLocation("projMatrix");
				vpos_attrib_location = program->getAttributeLocation("vPos");

				particleShaderPrograms.push_back(program);
				modelview_uniform_locations.push_back(modelview_uniform_location);
				projection_uniform_locations.push_back(projection_uniform_location);
				vpos_attrib_locations.push_back(vpos_attrib_location);
				break;
			case FIREWORK:
				printf("COMING SOON......");
				break;
		}
		
	}

	glGenVertexArrays(1, &pointsVAO);
	glBindVertexArray(pointsVAO);

	glGenBuffers(1, &pointsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glBufferData(GL_ARRAY_BUFFER, NUM_POINTS * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(vpos_attrib_location);
	glVertexAttribPointer(vpos_attrib_location, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
}

// setupBuffers() //////////////////////////////////////////////////////////////
//
//      Create our VAOs & VBOs. Send vertex data to the GPU for future rendering
//
////////////////////////////////////////////////////////////////////////////////
void setupBuffers()
{
	
	// setupSkybox();
	
	//////////////////////////////////////////
	//
	// Model

	model = new CSCI441::ModelLoader();
	model->loadModelFile("models/medstreet/medstreet.obj");

	//////////////////////////////////////////
	//
	// PLATFORM

	VertexTextured platformVertices[4] = {
		{-platformSize, 0.0f, -platformSize, 0.0f, 0.0f}, // 0 - BL
		{platformSize, 0.0f, -platformSize, 1.0f, 0.0f},  // 1 - BR
		{-platformSize, 0.0f, platformSize, 0.0f, -1.0f}, // 2 - TL
		{platformSize, 0.0f, platformSize, 1.0f, -1.0f}   // 3 - TR
	};

	unsigned short platformIndices[4] = {0, 1, 2, 3};

	glGenVertexArrays(1, &platformVAOd);
	glBindVertexArray(platformVAOd);

	GLuint vbods[2];
	glGenBuffers(2, vbods);

	glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(platformVertices), platformVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attrib_vPos_loc);
	glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);

	glEnableVertexAttribArray(attrib_vTextureCoord_loc);
	glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(platformIndices), platformIndices, GL_STATIC_DRAW);

	//////////////////////////////////////////
	//
	// SKYBOX

	unsigned short skyboxIndices[4] = {0, 1, 2, 3};

	GLfloat skyboxDim = 40.0f;
	VertexTextured skyboxVertices[6][4] = {
		{
			// back
			{-skyboxDim, -skyboxDim, -skyboxDim, 0.0f, 0.0f}, // 0 - BL
			{-skyboxDim, -skyboxDim, skyboxDim, -1.0f, 0.0f}, // 1 - BR
			{-skyboxDim, skyboxDim, -skyboxDim, 0.0f, 1.0f},  // 2 - TL
			{-skyboxDim, skyboxDim, skyboxDim, -1.0f, 1.0f}   // 3 - TR
		},

		{
			// right
			{-skyboxDim, -skyboxDim, skyboxDim, 0.0f, 0.0f}, // 0 - BL
			{skyboxDim, -skyboxDim, skyboxDim, -1.0f, 0.0f}, // 1 - BR
			{-skyboxDim, skyboxDim, skyboxDim, 0.0f, 1.0f},  // 2 - TL
			{skyboxDim, skyboxDim, skyboxDim, -1.0f, 1.0f}   // 3 - TR
		},

		{
			// front
			{skyboxDim, -skyboxDim, -skyboxDim, 0.0f, 0.0f}, // 0 - BL
			{skyboxDim, -skyboxDim, skyboxDim, 1.0f, 0.0f},  // 1 - BR
			{skyboxDim, skyboxDim, -skyboxDim, 0.0f, 1.0f},  // 2 - TL
			{skyboxDim, skyboxDim, skyboxDim, 1.0f, 1.0f}	// 3 - TR
		},

		{
			// left
			{-skyboxDim, -skyboxDim, -skyboxDim, 0.0f, 0.0f}, // 0 - BL
			{skyboxDim, -skyboxDim, -skyboxDim, 1.0f, 0.0f},  // 1 - BR
			{-skyboxDim, skyboxDim, -skyboxDim, 0.0f, 1.0f},  // 2 - TL
			{skyboxDim, skyboxDim, -skyboxDim, 1.0f, 1.0f}	// 3 - TR
		},

		{
			// bottom
			{-skyboxDim, -skyboxDim, -skyboxDim, 0.0f, 0.0f}, // 0 - BL
			{skyboxDim, -skyboxDim, -skyboxDim, 0.0f, 1.0f},  // 1 - BR
			{-skyboxDim, -skyboxDim, skyboxDim, 1.0f, 0.0f},  // 2 - TL
			{skyboxDim, -skyboxDim, skyboxDim, 1.0f, 1.0f}	// 3 - TR
		},

		{
			// top
			{-skyboxDim, skyboxDim, -skyboxDim, -1.0f, 1.0f}, // 0 - BL
			{skyboxDim, skyboxDim, -skyboxDim, -1.0f, 0.0f},  // 1 - BR
			{-skyboxDim, skyboxDim, skyboxDim, 0.0f, 1.0f},   // 2 - TL
			{skyboxDim, skyboxDim, skyboxDim, 0.0f, 0.0f}	 // 3 - TR
		}};

	glGenVertexArrays(6, skyboxVAOds);

	for (int i = 0; i < 6; i++)
	{
		glBindVertexArray(skyboxVAOds[i]);
		glGenBuffers(2, vbods);
		glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices[i]), skyboxVertices[i], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(attrib_vPos_loc);
		glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);
		glEnableVertexAttribArray(attrib_vTextureCoord_loc);
		glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));
	}

	//////////////////////////////////////////
	//
	// TEXTURED QUAD

	// LOOKHERE #1

	VertexTextured texturedQuadVerts[4] = {
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f}, // 0 - BL
		{1.0f, -1.0f, 0.0f, 1.0f, 0.0f},  // 1 - BR
		{-1.0f, 1.0f, 0.0f, 0.0f, 1.0f},  // 2 - TL
		{1.0f, 1.0f, 0.0f, 1.0f, 1.0f}	// 3 - TR
	};

	unsigned short texturedQuadIndices[4] = {0, 1, 2, 3};

	glGenVertexArrays(1, &texturedQuadVAO);
	glBindVertexArray(texturedQuadVAO);
	glGenBuffers(2, vbods);
	glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturedQuadVerts), texturedQuadVerts, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texturedQuadIndices), texturedQuadIndices, GL_STATIC_DRAW);
}

void renderParticleSystems(glm::mat4 viewMatrix, glm::mat4 projMatrix) {

	for (int i = 0; i < systems.size(); i++) {
		particleShaderPrograms[i]->useProgram(); 

		// precompute our MVP CPU side so it only needs to be computed once
		glm::mat4 modelMatrix = glm::mat4();

		glm::mat4 mvMtx = viewMatrix * modelMatrix;

		// send MVP to GPU
		glUniformMatrix4fv(modelview_uniform_locations[i], 1, GL_FALSE, &mvMtx[0][0]);
		glUniformMatrix4fv(projection_uniform_locations[i], 1, GL_FALSE, &projMatrix[0][0]);

		// sort!
		glm::vec3 view = glm::normalize(lookAtPoint - eyePoint);

		int NUM_POINT = systems[i]->particles.size();

		int orderedIndices[NUM_POINT];
		double distances[NUM_POINT];

		int count = 0;

		for (Particle *p : systems[i]->particles)
		{
		    glm::vec4 pos = modelMatrix * glm::vec4(p->position.x, p->position.y, p->position.z, 1);
		    glm::vec3 ep = glm::vec3(pos) - eyePoint;
		    double vp = glm::dot(view, ep);

		    orderedIndices[count] = count;
		    distances[count] = vp;
		    count++;
		}

		for (int i = 0; i < NUM_POINT; i++)
		{
		    for (int j = 0; j < NUM_POINT; j++)
		    {
		        if (distances[j] < distances[i])
		        {
		            double dist = distances[j];
		            distances[j] = distances[i];
		            distances[i] = dist;

		            int index = orderedIndices[j];
		            orderedIndices[j] = orderedIndices[i];
		            orderedIndices[i] = index;
		        }
		    }
		}
		
		systems[i]->draw(pointsVAO, pointsVBO, systems[i]->handle);
	}
}

//******************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

// renderScene() ///////////////////////////////////////////////////////////////
//
//		This method will contain all of the objects to be drawn.
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, double deltaTime)
{

	updateParticleSystems();
	glm::mat4 m, vp = projectionMatrix * viewMatrix;

	hellknightShaderProgram->useProgram();

	//
	glm::mat4 mod = glm::rotate(glm::mat4(), -90.0f * 3.14159f / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	// glm::mat4 mod = glm::mat4();
	mod = glm::scale(mod, glm::vec3(0.04f, 0.04f, 0.04f));
	glUniformMatrix4fv(pt_model_location, 1, GL_FALSE, &mod[0][0]);
	glUniformMatrix4fv(pt_view_location, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(pt_projection_location, 1, GL_FALSE, &projectionMatrix[0][0]);

	/* Calculate current and next frames */
	Animate(&md5animation, &animInfo, deltaTime);

	/* Interpolate skeletons between two frames */
	InterpolateSkeletons (md5animation.skelFrames[animInfo.curr_frame],
												md5animation.skelFrames[animInfo.next_frame],
												md5animation.num_joints,
												animInfo.last_time * md5animation.frameRate,
												skeleton);

	/* Draw each mesh of the model */
	for( int i = 0; i < md5model.num_meshes; ++i ) {
			md5_mesh_t mesh = md5model.meshes[i];
			PrepareMesh( &mesh, skeleton );

			if( displayWireframe )
				glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

				DrawMesh( &mesh , pt_model_location, hellknightShaderProgram->getShaderProgramHandle()); 
	}

	// Use our texture shader program
	textureShaderProgram->useProgram();

	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &m[0][0]);
	glUniformMatrix4fv(uniform_viewProjetionMtx_loc, 1, GL_FALSE, &vp[0][0]);
	glUniform1ui(uniform_tex_loc, GL_TEXTURE0);

	glm::vec3 white(1, 1, 1);
	glUniform3fv(uniform_color_loc, 1, &white[0]);

	// draw the skybox
	for (unsigned int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, skyboxHandles[i]);
		glBindVertexArray(skyboxVAOds[i]);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void *)0);
	}

	// draw the platform
	glBindTexture(GL_TEXTURE_2D, platformTextureHandle);
	glBindVertexArray(platformVAOd);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void *)0);

	// translate the model up slightly to prevent depth fighting on the platform
	m = glm::translate(m, glm::vec3(4, 0.1, -platformSize));

	glm::mat4 mv = viewMatrix * m;
	glm::mat4 nMtx = glm::transpose(glm::inverse(mv));

	// use our textured phong shader program for the model
	modelPhongShaderProgram->useProgram();
	glUniformMatrix4fv(uniform_phong_mv_loc, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix4fv(uniform_phong_v_loc, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(uniform_phong_p_loc, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0]);
	glUniform1i(uniform_phong_txtr_loc, 0);

	// draw the buildings
	model->draw(attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc,
				uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc, uniform_phong_ma_loc,
				GL_TEXTURE0);
				
	m = glm::translate(m, glm::vec3(0, 0, platformSize * 2.0f));
	mv = viewMatrix * m;
	nMtx = glm::transpose(glm::inverse(mv));
	glUniformMatrix4fv(uniform_phong_mv_loc, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix4fv(uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0]);
	//draw the second set of buildings
	model->draw(attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc,
				uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc, uniform_phong_ma_loc,
				GL_TEXTURE0);

	renderParticleSystems(viewMatrix, projectionMatrix);

	masterTime += 1.0f/20.0f;
}

///*****************************************************************************
//
// Our main function

// main() ///////////////////////////////////////////////////////////////
//
//		Really you should know what this is by now.
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	// GLFW sets up our OpenGL context so must be done first
	GLFWwindow *window = setupGLFW(); // initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();					  // initialize all of the OpenGL specific information
	setupGLEW();					  // initialize all of the GLEW specific information
	setupParticleSystems();
	setupShaders();					  // load our shaders into memory
	setupBuffers();					  // load all our VAOs and VBOs into memory
	setupTextures();				  // load all textures into memory
	loadMD5Model();					  // load the MD5 Model & animation if provided

	for (int i = 0; i < systems.size(); i++) {
		glValidateProgram(particleShaderPrograms[i]->getShaderProgramHandle());
		CSCI441_INTERNAL::ShaderUtils::printLog(particleShaderPrograms[i]->getShaderProgramHandle());
	}

	convertSphericalToCartesian(); // set up our camera position

	double current_time = 0, last_time = 0;

	CSCI441::setVertexAttributeLocations(attrib_vPos_loc, -1, attrib_vTextureCoord_loc);
	CSCI441::drawSolidSphere(1, 16, 16); // strange hack I need to make spheres draw - don't have time to investigate why..it's a bug with my library

	//  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
	//	until the user decides to close the window and quit the program.  Without a loop, the
	//	window will display once and then the program exits.
	while (!glfwWindowShouldClose(window))
	{ // check if the window was instructed to be closed
		// Get the size of our window framebuffer.  Ideally this should be the same dimensions as our window, but
		// when using a Retina display the actual window can be larger than the requested window.  Therefore
		// query what the actual size of the window we are rendering to is.

		last_time = current_time;	 // time the last frame was rendered
		current_time = glfwGetTime(); // time this frame is starting at

		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		glViewport(0, 0, windowWidth, windowHeight);

		// glViewport( 0, 0, windowWidth, windowHeight );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the current color contents and depth buffer in the framebuffer

		// set the projection matrix based on the window size
		// use a perspective projection that ranges
		// with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
		glm::mat4 projectionMatrix = glm::perspective(45.0f, windowWidth / (float)windowHeight, 0.001f, 1000.0f);

		// set up our look at matrix to position our camera
		glm::mat4 viewMatrix = glm::lookAt(eyePoint, lookAtPoint, upVector);

		// pass our view and projection matricesa
		renderScene(viewMatrix, projectionMatrix, current_time - last_time);

		glFlush();

		glfwSwapBuffers(window); // flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();		 // check for any events and signal to redraw screen
	}

	glfwDestroyWindow(window); // clean up and close our window
	glfwTerminate();		   // shut down GLFW to clean up our context

	return EXIT_SUCCESS; // exit our program successfully!
}
