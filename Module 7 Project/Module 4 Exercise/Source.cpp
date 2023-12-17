#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnOpengl/camera.h> // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "CS330 Final Project JMORRIS"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];         // Handle for the vertex buffer object
        GLuint nVertices;    // Number of indices of the mesh
        GLuint nIndices;
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;

    // Define object meshes
    GLMesh gMesh;
    GLMesh lampMesh;
    GLMesh planeMesh;
    GLMesh boxMesh;
    GLMesh glassMesh;
    GLMesh glassMesh2;
    GLMesh plateMesh;
    GLMesh toastMesh;


    // Define Textures
    GLuint gTextureId;
    GLuint gBoxTexture;
    GLuint gGlassTexture;
    GLuint gGlass2Texture;
    GLuint gToastTexture;
    glm::vec2 gUVScale(1.0f, 1.0f);
    GLint gTexWrapMode = GL_CLAMP_TO_BORDER;

    // Shader programs to use
    GLuint shaderCeramic;
    GLuint shaderLight;
    GLuint shaderMatte;
    GLuint shaderSatin;
    GLuint shaderGlass;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 7.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Subject position and scale
    glm::vec3 gCubePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gCubeScale(1.0f);

    glm::vec3 gPlanePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gPlaneScale(1.0f);

    glm::vec3 gGlassPosition(3.0f, -1.2f, 1.0f);
    glm::vec3 gGlass2Position(2.5f, -1.2f, 0.0f);
    glm::vec3 gGlassScale(0.35f);

    glm::vec3 gPlatePosition(-3.0f, 0.0f, 0.0f);
    glm::vec3 gPlateScale(1.0f, 1.0f, 1.0f);

    glm::vec3 gBoxPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gBoxScale(1.0f);

    glm::vec3 gToastPosition(-3.0f, 0.0f, 0.0f);
    glm::vec3 gToastPosition2(-2.9f, 0.2f, -0.1f);
    glm::vec3 gToastScale(1.0f);



    // Cube and light color
    glm::vec3 gObjectColor(1.f, 0.2f, 0.0f);
    glm::vec3 gLightColorA(2.0f, 2.f, 1.6f);
    glm::vec3 gLightColor(0.6f, 0.5f, 0.4f);
    glm::vec3 gLightColor2(0.3f, 0.3f, 0.3f);

    // Light position and scale
    glm::vec3 gLightPosition(8.0f, 8.0f, 0.0f);
    glm::vec3 gLightPosition2(-8.0f, 7.0f, 2.0f);
    glm::vec3 gLightScale(0.5f);

    // Lamp animation
    bool gIsLampOrbiting = true;


    //Projection tracking
    bool isPerspective = true;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateBowl(GLMesh& mesh);
void UCreatePlane(GLMesh& mesh);
void UCreateBox(GLMesh& mesh);
void UCreateGlass(GLMesh& mesh);
void UCreateLamp(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void UCreatePlate(GLMesh& mesh);
void UCreateToast(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


//------------------------------------------------ Ceramic SHADERS -------------------------------------------------------------------------


/* Bowl Vertex Shader Source Code*/
const GLchar* cubeVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() //Ceramic SHADER VERTEX MAIN
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Ceramic Fragment Shader Source Code*/
const GLchar* cubeFragmentShaderSource = GLSL(440,

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightColor2;
uniform vec3 lightPos2;
uniform vec3 lightColorA;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()//Ceramic SHADER FRAGMENT MAIN
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.2f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColorA; // Generate ambient light color

    ///--------------------LIGHT ONE BOWL ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.0f; // Set specular light strength
    float highlightSize = 3.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    ///--------------------LIGHT TWO BOWL ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact2 = max(dot(norm, lightDirection2), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse2 = impact2 * lightColor2; // Generate diffuse light color

    //Calculate Specular lighting*/
    vec3 reflectDir2 = reflect(-lightDirection2, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize);
    vec3 specular2 = specularIntensity * specularComponent * lightColor2;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular + diffuse2 + specular2) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

//------------------------------------------------ GLASS SHADERS -------------------------------------------------------------------------


/* Glass Vertex Shader Source Code*/
const GLchar* glassVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() //Glass SHADER VERTEX MAIN
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


/* Glass Fragment Shader Source Code*/
const GLchar* glassFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightColor2;
uniform vec3 lightPos2;
uniform vec3 lightColorA;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()//GLASS SHADER FRAGMENT MAIN
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.2f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColorA; // Generate ambient light color

    ///--------------------LIGHT ONE GLASS ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 1.5f; // Set specular light strength
    float highlightSize = 2.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    ///--------------------LIGHT TWO GLASS ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact2 = max(dot(norm, lightDirection2), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse2 = impact2 * lightColor2; // Generate diffuse light color

    //Calculate Specular lighting*/
    vec3 reflectDir2 = reflect(-lightDirection2, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize);
    vec3 specular2 = specularIntensity * specularComponent * lightColor2;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular + diffuse2 + specular2) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);

//--------------------------------------------------Matte SHADERS-----------------------------------------------------------

/* Matte Vertex Shader Source Code*/
const GLchar* planeVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()//PLANE VERTEX MAIN
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


//PLANE FRAGMENT
const GLchar* planeFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightColor2;
uniform vec3 lightPos2;
uniform vec3 lightColorA;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()//Matte FRAGMENT MAIN
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.2f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColorA; // Generate ambient light color

    ///--------------------LIGHT ONE ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.2f; // Set specular light strength
    float highlightSize = 17.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    ///--------------------LIGHT TWO ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact2 = max(dot(norm, lightDirection2), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse2 = impact2 * lightColor2; // Generate diffuse light color

    //Calculate Specular lighting*/
    vec3 reflectDir2 = reflect(-lightDirection2, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize);
    vec3 specular2 = specularIntensity * specularComponent * lightColor2;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular + diffuse2 + specular2) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);//End of plane shader

//--------------------------------------------------Satin SHADERS-----------------------------------------------------------

/* Satin Vertex Shader Source Code*/
const GLchar* boxVertexShaderSource = GLSL(440,

layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()//BOX VERTEX MAIN
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);


//BOX FRAGMENT
const GLchar* boxFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightColor2;
uniform vec3 lightPos2;
uniform vec3 lightColorA;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()//BOX FRAGMENT MAIN
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.2f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColorA; // Generate ambient light color

    ///--------------------LIGHT ONE ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.5f; // Set specular light strength
    float highlightSize = 8.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    ///--------------------LIGHT TWO ------------------------------------------------------

    //Calculate Diffuse lighting*/
    vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact2 = max(dot(norm, lightDirection2), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse2 = impact2 * lightColor2; // Generate diffuse light color

    //Calculate Specular lighting*/
    vec3 reflectDir2 = reflect(-lightDirection2, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize);
    vec3 specular2 = specularIntensity * specularComponent * lightColor2;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular + diffuse2 + specular2) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


//----------------------------LAMP SHADERS------------------
/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()//LAMP VERTEX MAIN
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

    


void main()//LAMP FRAGMENT MAIN
{
    vec4 lampVec;
    lampVec.rgba = vec4(1.0f, 0.9f, 0.7f, 1.0f);
    fragmentColor = lampVec;
   // fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateBowl(gMesh); // Calls the function to create the Vertex Buffer Object
    UCreateLamp(lampMesh);
    UCreatePlane(planeMesh);
    UCreateBox(boxMesh);
    UCreateGlass(glassMesh);
    UCreatePlate(plateMesh);
    UCreateToast(toastMesh);

    // Create the shader programs
    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, shaderCeramic))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, shaderLight))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(planeVertexShaderSource, planeFragmentShaderSource, shaderMatte))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(boxVertexShaderSource, boxFragmentShaderSource, shaderSatin))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(glassVertexShaderSource, glassFragmentShaderSource, shaderGlass))
        return EXIT_FAILURE;

    // Load texture
    const char* texFilename = "../../resources/textures/scenetex2.png";

    const char* texFilename2 = "../../resources/textures/cerealbox1.png";

    const char* texFilename3 = "../../resources/textures/glasstexture1.png";

    const char* texFilename4 = "../../resources/textures/glasstexture2.png";

    const char* texFilename5 = "../../resources/textures/toast.png";

    

    if (!UCreateTexture(texFilename, gTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    if (!UCreateTexture(texFilename2, gBoxTexture))
    {
        cout << "Failed to load texture 2 " << texFilename << endl;
        return EXIT_FAILURE;
    }

    if (!UCreateTexture(texFilename3, gGlassTexture))
    {
        cout << "Failed to load texture 2 " << texFilename << endl;
        return EXIT_FAILURE;
    }


    if (!UCreateTexture(texFilename4, gGlass2Texture))
    {
        cout << "Failed to load texture 2 " << texFilename << endl;
        return EXIT_FAILURE;
    }

    if (!UCreateTexture(texFilename5, gToastTexture))
    {
        cout << "Failed to load texture 2 " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(shaderCeramic);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(shaderCeramic, "uTexture"), 0);

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);
    UDestroyMesh(lampMesh);
    UDestroyMesh(planeMesh);
    UDestroyMesh(boxMesh);
    UDestroyMesh(glassMesh);
    UDestroyMesh(plateMesh);
    UDestroyMesh(toastMesh);

    // Release texture
    UDestroyTexture(gTextureId);
    UDestroyTexture(gBoxTexture);
    UDestroyTexture(gGlassTexture);
    UDestroyTexture(gGlass2Texture);


    // Release shader programs
    UDestroyShaderProgram(shaderCeramic);
    UDestroyShaderProgram(shaderLight);
    UDestroyShaderProgram(shaderMatte);
    UDestroyShaderProgram(shaderSatin);
    UDestroyShaderProgram(shaderGlass);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UPWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWNWARD, gDeltaTime);



   

    // Pause and resume lamp orbiting
    static bool isLKeyDown = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !gIsLampOrbiting) {
        gIsLampOrbiting = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && gIsLampOrbiting) {
        gIsLampOrbiting = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        isPerspective = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        isPerspective = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        gLightColor.x = 0.6f;
        gLightColor.y = 0.5f;
        gLightColor.z = 0.4f;

        gLightColor2.x = 0.3f;
        gLightColor2.y = 0.3f;
        gLightColor2.z = 0.3f;
    }
    else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        gLightColor.x = 0.9f;
        gLightColor.y = 0.0f;
        gLightColor.z = 0.0f;

        gLightColor2.x = 0.0f;
        gLightColor2.y = 0.0f;
        gLightColor2.z = 0.9f;
    }
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        gLightColor.x = 0.0f;
        gLightColor.y = 0.9f;
        gLightColor.z = 0.0f;

        gLightColor2.x = 0.0f;
        gLightColor2.y = 0.0f;
        gLightColor2.z = 0.0f;
    }






}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Function called to render a frame
void URender()
{
    // Lamp orbits around the origin
    const float angularVelocity = glm::radians(45.0f);
    if (gIsLampOrbiting)
    {
        glm::vec4 newPosition = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gLightPosition, 1.0f);
        gLightPosition.x = newPosition.x;
        gLightPosition.y = newPosition.y;
        gLightPosition.z = newPosition.z;

        glm::vec4 newPosition2 = glm::rotate(angularVelocity * gDeltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(gLightPosition2, 1.0f);
        gLightPosition2.x = newPosition2.x;
        gLightPosition2.y = newPosition2.y;
        gLightPosition2.z = newPosition2.z;
    }

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //------------------------------- DRAW BOWL----------------------------------------
    glBindVertexArray(gMesh.vao);
    glUseProgram(shaderCeramic);

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = glm::translate(gCubePosition) * glm::scale(gCubeScale);

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    glm::mat4 projection;

    // Set the projection matrix to either perspective or ortho, depending on settings
    if (isPerspective == true) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f);
    }
    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(shaderCeramic, "model");
    GLint viewLoc = glGetUniformLocation(shaderCeramic, "view");
    GLint projLoc = glGetUniformLocation(shaderCeramic, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(shaderCeramic, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(shaderCeramic, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(shaderCeramic, "lightPos");

    GLint lightColorLocA = glGetUniformLocation(shaderCeramic, "lightColorA");

    GLint lightColorLoc2 = glGetUniformLocation(shaderCeramic, "lightColor2");
    GLint lightPositionLoc2 = glGetUniformLocation(shaderCeramic, "lightPos2");

    GLint viewPositionLoc = glGetUniformLocation(shaderCeramic, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(shaderCeramic, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);


    //------------------------------- DRAW PLANE----------------------------------------
    glBindVertexArray(planeMesh.vao);
    glUseProgram(shaderMatte);

    //// Model matrix: transformations are applied right-to-left order
    model = glm::translate(gPlanePosition) * glm::scale(gPlaneScale);



    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(shaderMatte, "model");
    viewLoc = glGetUniformLocation(shaderMatte, "view");
    projLoc = glGetUniformLocation(shaderMatte, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    objectColorLoc = glGetUniformLocation(shaderMatte, "objectColor");
    lightColorLoc = glGetUniformLocation(shaderMatte, "lightColor");
    lightPositionLoc = glGetUniformLocation(shaderMatte, "lightPos");

    lightColorLocA = glGetUniformLocation(shaderMatte, "lightColorA");

    lightColorLoc2 = glGetUniformLocation(shaderMatte, "lightColor2");
    lightPositionLoc2 = glGetUniformLocation(shaderMatte, "lightPos2");

    viewPositionLoc = glGetUniformLocation(shaderMatte, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    //const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(shaderMatte, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);


    //------------------------------- DRAW TOAST ONE----------------------------------------
    glBindVertexArray(toastMesh.vao);
    glUseProgram(shaderMatte);

    //// Model matrix: transformations are applied right-to-left order
    model = glm::translate(gToastPosition) * glm::scale(gToastScale);

     // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(shaderMatte, "model");
    viewLoc = glGetUniformLocation(shaderMatte, "view");
    projLoc = glGetUniformLocation(shaderMatte, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    objectColorLoc = glGetUniformLocation(shaderMatte, "objectColor");
    lightColorLoc = glGetUniformLocation(shaderMatte, "lightColor");
    lightPositionLoc = glGetUniformLocation(shaderMatte, "lightPos");

    lightColorLocA = glGetUniformLocation(shaderMatte, "lightColorA");

    lightColorLoc2 = glGetUniformLocation(shaderMatte, "lightColor2");
    lightPositionLoc2 = glGetUniformLocation(shaderMatte, "lightPos2");

    viewPositionLoc = glGetUniformLocation(shaderMatte, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    //const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(shaderMatte, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gToastTexture);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);

    //------------------------------- DRAW TOAST TWO----------------------------------------

    //// Model matrix: transformations are applied right-to-left order
    model = glm::translate(gToastPosition2) * glm::scale(gToastScale);

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(shaderMatte, "model");
    viewLoc = glGetUniformLocation(shaderMatte, "view");
    projLoc = glGetUniformLocation(shaderMatte, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    objectColorLoc = glGetUniformLocation(shaderMatte, "objectColor");
    lightColorLoc = glGetUniformLocation(shaderMatte, "lightColor");
    lightPositionLoc = glGetUniformLocation(shaderMatte, "lightPos");

    lightColorLocA = glGetUniformLocation(shaderMatte, "lightColorA");

    lightColorLoc2 = glGetUniformLocation(shaderMatte, "lightColor2");
    lightPositionLoc2 = glGetUniformLocation(shaderMatte, "lightPos2");

    viewPositionLoc = glGetUniformLocation(shaderMatte, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    //const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(shaderMatte, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gToastTexture);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);

    //------------------------------- DRAW BOX----------------------------------------
    glBindVertexArray(boxMesh.vao);
    glUseProgram(shaderSatin);

    //// Model matrix: transformations are applied right-to-left order
    model = glm::translate(gBoxPosition) * glm::scale(gBoxScale);


    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(shaderSatin, "model");
    viewLoc = glGetUniformLocation(shaderSatin, "view");
    projLoc = glGetUniformLocation(shaderSatin, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    objectColorLoc = glGetUniformLocation(shaderSatin, "objectColor");
    lightColorLoc = glGetUniformLocation(shaderSatin, "lightColor");
    lightPositionLoc = glGetUniformLocation(shaderSatin, "lightPos");

    lightColorLocA = glGetUniformLocation(shaderSatin, "lightColorA");

    lightColorLoc2 = glGetUniformLocation(shaderSatin, "lightColor2");
    lightPositionLoc2 = glGetUniformLocation(shaderSatin, "lightPos2");

    viewPositionLoc = glGetUniformLocation(shaderSatin, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    //const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(shaderSatin, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBoxTexture);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);


    //------------------------------- DRAW GLASS----------------------------------------
    glBindVertexArray(glassMesh.vao);
    glUseProgram(shaderGlass);

    //// Model matrix: transformations are applied right-to-left order
    model = glm::translate(gGlassPosition) * glm::scale(gGlassScale);


    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(shaderGlass, "model");
    viewLoc = glGetUniformLocation(shaderGlass, "view");
    projLoc = glGetUniformLocation(shaderGlass, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    objectColorLoc = glGetUniformLocation(shaderGlass, "objectColor");
    lightColorLoc = glGetUniformLocation(shaderGlass, "lightColor");
    lightPositionLoc = glGetUniformLocation(shaderGlass, "lightPos");

    lightColorLocA = glGetUniformLocation(shaderGlass, "lightColorA");

    lightColorLoc2 = glGetUniformLocation(shaderGlass, "lightColor2");
    lightPositionLoc2 = glGetUniformLocation(shaderGlass, "lightPos2");

    viewPositionLoc = glGetUniformLocation(shaderGlass, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    //const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(shaderGlass, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gGlassTexture);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);


    //------------------------------- DRAW SECOND GLASS----------------------------------------
    glBindVertexArray(glassMesh.vao);
    glUseProgram(shaderGlass);

    //// Model matrix: transformations are applied right-to-left order
    model = glm::translate(gGlass2Position) * glm::scale(gGlassScale);


    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(shaderGlass, "model");
    viewLoc = glGetUniformLocation(shaderGlass, "view");
    projLoc = glGetUniformLocation(shaderGlass, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    objectColorLoc = glGetUniformLocation(shaderGlass, "objectColor");
    lightColorLoc = glGetUniformLocation(shaderGlass, "lightColor");
    lightPositionLoc = glGetUniformLocation(shaderGlass, "lightPos");

    lightColorLocA = glGetUniformLocation(shaderGlass, "lightColorA");

    lightColorLoc2 = glGetUniformLocation(shaderGlass, "lightColor2");
    lightPositionLoc2 = glGetUniformLocation(shaderGlass, "lightPos2");

    viewPositionLoc = glGetUniformLocation(shaderGlass, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    //const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(shaderGlass, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gGlass2Texture);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);


    //------------------------------- DRAW PLATE----------------------------------------
    glBindVertexArray(plateMesh.vao);
    glUseProgram(shaderGlass);

    //// Model matrix: transformations are applied right-to-left order
    model = glm::translate(gPlatePosition) * glm::scale(gPlateScale);

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(shaderGlass, "model");
    viewLoc = glGetUniformLocation(shaderGlass, "view");
    projLoc = glGetUniformLocation(shaderGlass, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    objectColorLoc = glGetUniformLocation(shaderGlass, "objectColor");
    lightColorLoc = glGetUniformLocation(shaderGlass, "lightColor");
    lightPositionLoc = glGetUniformLocation(shaderGlass, "lightPos");

    lightColorLocA = glGetUniformLocation(shaderGlass, "lightColorA");

    lightColorLoc2 = glGetUniformLocation(shaderGlass, "lightColor2");
    lightPositionLoc2 = glGetUniformLocation(shaderGlass, "lightPos2");

    viewPositionLoc = glGetUniformLocation(shaderGlass, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    glUniform3f(lightColorLocA, gLightColorA.r, gLightColorA.g, gLightColorA.b);

    //const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    UVScaleLoc = glGetUniformLocation(shaderGlass, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gGlass2Texture);

    // Draws the triangles
    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);



   

    // LAMP: draw lamp
    //---------------------------------------------------------------------------------------------------------------------------
    glUseProgram(shaderLight);

    glBindVertexArray(lampMesh.vao);

    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition) * glm::scale(gLightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(shaderLight, "model");
    viewLoc = glGetUniformLocation(shaderLight, "view");
    projLoc = glGetUniformLocation(shaderLight, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);

    //LAMP 2: Draw the second lamp
     //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition2) * glm::scale(gLightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(shaderLight, "model");
    viewLoc = glGetUniformLocation(shaderLight, "view");
    projLoc = glGetUniformLocation(shaderLight, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL);


    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Creates the bowl mesh
void UCreateBowl(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //BOWL---------------------------------------- VERTS 0 to 25 -------------------------------------------------
        // Vertex Positions    // Colors (r,g,b)
        0.0f, -0.5f, 0.0f,   0.0f, -1.0f, 0.0f,   0.1f, 0.1f,// Bowl Center Vertex 0:

        //Base of bowl
        0.25f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.1f, 0.1f,// Bottom Right Front Vertex 1: Red
        -0.25f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  0.1f, 0.1f,// Bottom Left Front Vertex 2: Red
        0.5f, -0.5f, -0.25f,   0.0f, -1.0f, 0.0f,   0.1f, 0.1f,// Bottom Right Mid Vertex 3: Red
        -0.5f, -0.5f, -0.25f,   0.0f, -1.0f, 0.0f,  0.1f, 0.1f,// Bottom Left Mid Vertex 4: Red

         0.25f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,  0.1f, 0.1f,// Bottom Right Rear Vertex 5: Red
        -0.25f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,  0.1f, 0.1f,// Bottom Left Rear Vertex 6: Red
         0.5f, -0.5f, 0.25f,   0.0f, -1.0f, 0.0f,  0.1f, 0.1f,// Bottom Right Mid Vertex 7: Red
        -0.5f, -0.5f, 0.25f,   0.0f, -1.0f, 0.0f,  0.1f, 0.1f,// Bottom Left Mid Vertex 8: Red

        //First Ring (Milk Level)

        0.5f, 0.0f, -0.75f,   0.5f, 0.0f, -0.75f,   0.1f, 0.1f,  // Bottom Right Front Vertex 9: Red
        -0.5f, 0.0f, -0.75f,   -0.5f, 0.0f, -0.75f,  0.1f, 0.1f, // Bottom Left Front Vertex 10: Red
        0.75f, 0.0f, -0.5f,   0.75f, 0.0f, -0.5f,   0.1f, 0.1f,// Bottom Right Mid Vertex 11: Red
        -0.75f, 0.0f, -0.5f,   -0.75f, 0.0f, -0.5f,  0.1f, 0.1f,// Bottom Left Mid Vertex 12: Red

        0.5f, 0.0f, 0.75f,   0.5f, 0.0f, 0.75f,    0.1f, 0.1f,// Bottom Right Rear Vertex 13: Red
        -0.5f, 0.0f, 0.75f,   -0.5f, 0.0f, 0.75f,   0.1f, 0.1f,// Bottom Left Rear Vertex 14: Red
        0.75f, 0.0f, 0.5f,   0.75f, 0.0f, 0.5f,    0.1f, 0.1f,// Bottom Right Mid Vertex 15: Red
        -0.75f, 0.0f, 0.5f,   -0.75f, 0.0f, 0.5f,   0.1f, 0.1f,// Bottom Left Mid Vertex 16: Red

        0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.25f, 0.75f,// Milk Plane Center 17: White 

        //Second Ring - Bowl Edge
        0.5f, 0.2f, -0.75f,   0.5f, 0.2f, -0.75f,   0.1f, 0.1f,// Bottom Right Front Vertex 18: Red
        -0.5f, 0.2f, -0.75f,   -0.5f, 0.2f, -0.75f,  0.1f, 0.1f,// Bottom Left Front Vertex 19: Red
        0.75f, 0.2f, -0.5f,   0.75f, 0.2f, -0.5f,   0.1f, 0.1f,// Bottom Right Mid Vertex 20: Red
        -0.75f, 0.2f, -0.5f,   -0.75f, 0.2f, -0.5f,  0.1f, 0.1f,// Bottom Left Mid Vertex 21: Red

        0.5f, 0.2f, 0.75f,   0.5f, 0.2f, 0.75f,    0.1f, 0.1f,// Bottom Right Rear Vertex 22: Red
        -0.5f, 0.2f, 0.75f,   -0.5f, 0.2f, 0.75f,   0.1f, 0.1f,// Bottom Left Rear Vertex 23: Red
        0.75f, 0.2f, 0.5f,   0.75f, 0.2f, 0.5f,   0.1f, 0.1f,// Bottom Right Mid Vertex 24: Red
        -0.75f, 0.2f, 0.5f,   -0.75f, 0.2f, 0.5f,  0.1f, 0.1f,// Bottom Left Mid Vertex 25: Red


        //END BOWL------------------------------- VERTS 0 to 25 -------------------------------------------------------

        //PLANE --------------------------------- Verts 26 to 29 ------------------------------------------------------

        -5.0f, -0.5f, -3.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.5f,// Plane front right corner 26: Grey
        -5.0f, -0.5f, 3.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,// Plane rear right corner 27: Grey
        5.0f, -0.5f, -3.0f,   0.0f, 1.0f, 0.0f,   0.5f, 0.5f,// Plane front left corner 28: Grey
        5.0f, -0.5f, 3.0f,   0.0f, 1.0f, 0.0f,    0.5f, 1.0f,// Plane rear left corner 29: Grey

        //END PLANE------------------------------------------------------------------------------------------------

        //BOWL MILK --------------------  Verts 30 to 37 -----------------------------------------------------------------
         0.5f, 0.1f, -0.75f,   0.0f, 1.0f, 0.0f,  0.375f, 1.0f,// Bottom Right Front Vertex 30: Red
        -0.5f, 0.1f, -0.75f,   0.0f, 1.0f, 0.0f,  0.125f, 1.0f,// Bottom Left Front Vertex 31: Red
        0.75f, 0.1f, -0.5f,   0.0f, 1.0f, 0.0f,   0.5f, 0.875f,// Bottom Right Mid Vertex 32: Red
        -0.75f, 0.1f, -0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.875f,// Bottom Left Mid Vertex 33: Red

        0.5f, 0.1f, 0.75f,   0.0f, 1.0f, 0.0f,   0.375f, 0.5f,// Bottom Right Rear Vertex 34: Red
        -0.5f, 0.1f, 0.75f,   0.0f, 1.0f, 0.0f,  0.125f, 0.5f,// Bottom Left Rear Vertex 35: Red
        0.75f, 0.1f, 0.5f,   0.0f, 1.0f, 0.0f,   0.5f, 0.625f,// Bottom Right Mid Vertex 36: Red
        -0.75f, 0.1f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.625f,// Bottom Left Mid Vertex 37: Red



    };
    // Index data to share position data
    GLushort indices[] = {
        //BOWL-----------------------------------------------------------------
        //BASE
         0, 1, 2,
        0, 1, 3,
        0, 3, 7,
        0, 7, 5,
        0, 5, 6,
        0, 6, 8,
        0, 8, 4,
        0, 4, 2,

        //FIRST LAYER WALL
        1, 2, 9,
        9, 2, 10,

        1, 3, 9,
        9, 3, 11,

         3, 7, 15,
         3, 15, 11,

         7, 5, 15,
         15, 5, 13,

        5, 6, 13,
        13, 6, 14,

         6, 8, 14,
        14, 8, 16,

        8, 4, 16,
        16, 4, 12,

        4, 2, 12,
        12, 2, 10,

        //MILK PLANE
        //10, 9 , 17,
        //9, 11, 17,
        //11, 15, 17,
        //13, 15, 17,
        //14, 13, 17,
        //14, 16, 17,
        //12, 16, 17,
        //12, 10, 17,


        //Second Layer Wall
        10, 9, 18,
        18, 19, 10,

        9, 11, 20,
        18, 20, 9,

         11, 15, 24,
         20, 24, 11,

        13, 15, 24,
        22, 24, 13,

        14, 13, 22,
        23, 22, 14,

        16, 14, 23,
        25, 23, 16,

        12, 16, 25,
        21, 25, 12,

        12, 10, 21,
        21, 19, 10,

        //END BOWL---------------------------------------------------------------------

        //PLANE -----------------------------------------------------------------------

     //   26, 27, 28,
     //   28, 29, 27,

        //END PLANE -------------------------------------------------------

        //MILK
        31, 30, 17,
        30, 32, 17,
        32, 36, 17,
        34, 36, 17,
        35, 34, 17,
        35, 37, 17,
        33, 37, 17,
        33, 31, 17


    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


//Creates the mesh for the base plane   
void UCreatePlane(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)      //Textures
         -5.0f, -0.5f, -3.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.5f,// Plane front right corner 0: Grey
        -5.0f, -0.5f, 3.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,// Plane rear right corner 1: Grey
        5.0f, -0.5f, -3.0f,   0.0f, 1.0f, 0.0f,   0.5f, 0.5f,// Plane front left corner 2: Grey
        5.0f, -0.5f, 3.0f,   0.0f, 1.0f, 0.0f,    0.5f, 1.0f// Plane rear left corner 3: Grey
    };
    // Index data to share position data
    GLushort indices[] = {

        0, 1, 2,
        1, 3, 2


    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


//Creates the mesh for the box of cereal
void UCreateBox(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    // Normals           //Textures

        //Front
         -1.2f, -0.5f, -1.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,// Box Front Left Bot Corner 0
        1.2f, -0.5f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,// Box Front Right BotCorner 1
        -1.2f, 2.4f, -1.0f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,// Box Front Left Top Corner 2
        1.2f, 2.4f, -1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,// Box Front Right Top Corner 3


        //Back
        -1.2f, -0.5f, -2.0f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,// Box Rear Left Bot Corner 4
        1.2f, -0.5f, -2.0f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,// Box Rear Right BotCorner 5
        -1.2f, 2.4f, -2.0f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,// Box Rear Left Top Corner 6
        1.2f, 2.4f, -2.0f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,// Box Rear Right Top Corner 7


        //Top
         -1.2f, 2.4f, -1.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,// Box Front Left Top Corner 8
        1.2f, 2.4f, -1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,// Box Front Right Top Corner 9
        -1.2f, 2.4f, -2.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,// Box Rear Left Top Corner 10
        1.2f, 2.4f, -2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,// Box Rear Right Top Corner 11

        //Left
         -1.2f, -0.5f, -1.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,// Box Front Left Bot Corner 12
         -1.2f, 2.4f, -1.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,// Box Front Left Top Corner 13
         -1.2f, -0.5f, -2.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,// Box Rear Left Bot Corner 14
         -1.2f, 2.4f, -2.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,// Box Rear Left Top Corner 15

         //Right
          1.2f, -0.5f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,// Box Front Right BotCorner 16
          1.2f, 2.4f, -1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,// Box Front Right Top Corner 17
          1.2f, -0.5f, -2.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,// Box Rear Right BotCorner 18
          1.2f, 2.4f, -2.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,// Box Rear Right Top Corner 19





    };
    // Index data to share position data
    GLushort indices[] = {
        //Front
        0, 1, 2,
        1, 3, 2,
        //Back
        4, 5, 6,
        5, 7, 6,
        //Top
        8, 9, 10,
        9, 11, 10,
        //Left
        12, 13, 14,
        13, 15, 14,
        //Right
        16, 17, 18,
        17, 19, 18,




    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

//Creates the mesh used to render the glasses
void UCreateGlass(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    // Normals           //Textures

        //Bottom Circle
         0.0f, 2.0f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,// Bot Center 0

        -1.25f, 2.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,// Mid Bot Left Corner 1
        -0.5f, 2.0f, 1.1f,   -0.5f, 0.0f, 1.1f,  0.166f, 0.0f,// Front Left Bot Corner 2
        0.5f, 2.0f, 1.1f,   0.5f, 0.0f, 1.1f,   0.332f, 0.0f,//  Front Right Bot Corner 3
        1.25f, 2.0f, 0.0f,    1.0f, 0.0f, 0.0f,   0.498f, 0.0f,// Mid Bot Right Corner 4
        0.5f, 2.0f, -1.1f,   0.5f, 0.0f, -1.1f,   0.664f, 0.0f,//  Rear Right Bot Corner 5
        -0.5f, 2.0f, -1.1f,   -0.5f, 0.0f, -1.1f,  0.83f, 0.0f,// Rear Left Bot Corner 6
        -1.25f, 2.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,// Mid Bot Left Corner 7




        -1.25f, 7.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,// Mid Top Left Corner 8
        -0.5f, 7.0f, 1.1f,   -0.5f, 0.0f, 1.1f,  0.166f, 1.0f,// Front Top Bot Corner 9
        0.5f, 7.0f, 1.1f,   0.5f, 0.0f, 1.1f,   0.332f, 1.0f,//  Front Top Bot Corner  10
        1.25f, 7.0f, 0.0f,    1.0f, 0.0f, 0.0f,   0.498f, 1.0f,// Mid Top Top Corner 11
        0.5f, 7.0f, -1.1f,   0.5f, 0.0f, -1.1f,   0.664f, 1.0f,//  Rear Top Bot Corner 12
        -0.5f, 7.0f, -1.1f,   -0.5f, 0.0f, -1.1f,  0.83f, 1.0f,// Rear Left Top Corner 13
        -1.25f, 7.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,// Mid Top Left Corner 14

        //Juice
         0.0f, 4.5f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,// Top Center 15

        -1.25f, 4.5f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,// Mid Top Left Corner 16
        -0.5f, 4.5f, 1.1f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,// Front Top Bot Corner 17
        0.5f, 4.5f, 1.1f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,//  Front Top Bot Corner  18
        1.25f, 4.5f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,// Mid Top Top Corner 19
        0.5f, 4.5f, -1.1f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,//  Rear Top Bot Corner 20
        -0.5f, 4.5f, -1.1f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,// Rear Left Top Corner 21
















    };
    // Index data to share position data
    GLushort indices[] = {
      
       //bottom 
      // 0, 1, 2,
      // 0, 2, 3,
      // 0, 3, 4,
      // 0, 4, 5,
      // 0, 5, 6,
      // 0, 6, 7,



       //Sides
       1, 2, 9,
       8, 9, 1,

       2, 3, 10,
       9, 10, 2,

       3, 4, 11,
       11, 10, 3,

       4, 5, 12,
       12, 11, 4,

       5, 6, 13,
       13, 12, 5,

       6, 1, 8,
       8, 13, 6,


       //Juice
       15, 16, 17,
       15, 17, 18,
       15, 18, 19,
       15, 19, 20,
       15, 20, 21,
       15, 21, 16,





    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


//Creates a mesh used to render the plate
void UCreatePlate(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    // Normals           //Textures

        //Bottom Circle
         0.0f,-0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,// Bot Center 0

        -1.25f, -0.5f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,// Mid Bot Left Corner 1
        -0.5f, -0.5f, 1.1f,   -0.5f, 0.0f, 1.1f,  0.166f, 0.0f,// Front Left Bot Corner 2
        0.5f, -0.5f, 1.1f,   0.5f, 0.0f, 1.1f,   0.332f, 0.0f,//  Front Right Bot Corner 3
        1.25f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   0.498f, 0.0f,// Mid Bot Right Corner 4
        0.5f, -0.5f, -1.1f,   0.5f, 0.0f, -1.1f,   0.664f, 0.0f,//  Rear Right Bot Corner 5
        -0.5f, -0.5f, -1.1f,   -0.5f, 0.0f, -1.1f,  0.83f, 0.0f,// Rear Left Bot Corner 6
        -1.25f, -0.5f, 0.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,// Mid Bot Left Corner 7



        //Top Circle
        -1.25f,-0.35f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,// Mid Top Left Corner 8
        -0.5f, -0.35f, 1.1f,   0.0f, 1.0f, 0.0f,  0.166f, 1.0f,// Front Top Bot Corner 9
        0.5f, -0.35f, 1.1f,   0.0f, 1.0f, 0.0f,   0.332f, 1.0f,//  Front Top Bot Corner  10
        1.25f, -0.35f, 0.0f,    0.0f, 1.0f, 0.0f,   0.498f, 1.0f,// Mid Top Top Corner 11
        0.5f, -0.35f, -1.1f,   0.0f, 1.0f, 0.0f,   0.664f, 1.0f,//  Rear Top Bot Corner 12
        -0.5f, -0.35f, -1.1f,   0.0f, 1.0f, 0.0f,  0.83f, 1.0f,// Rear Left Top Corner 13
        -1.25f, -0.35f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,// Mid Top Left Corner 14

        //Juice
         0.0f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,// Top Center 15


















    };
    // Index data to share position data
    GLushort indices[] = {

        //bottom 
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 7,



        //Sides
        1, 2, 9,
        8, 9, 1,

        2, 3, 10,
        9, 10, 2,

        3, 4, 11,
        11, 10, 3,

        4, 5, 12,
        12, 11, 4,

        5, 6, 13,
        13, 12, 5,

        6, 1, 8,
        8, 13, 6,


        //Top
        15, 8, 9,
       15, 9, 10,
       15, 10, 11,
       15, 11, 12,
       15, 12, 13,
       15, 13, 14,






    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

//Creates the mesh used to render the slices of toast
void UCreateToast(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    // Normals           //Textures

        //Bottom Circle

        -0.9f, -0.35f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,// Mid Bot Left Corner 0
        0.0f, -0.35f, -0.9f,   0.0f, 0.0f, 0.0f,  1.0f, 0.0f,// Front Left Bot Corner 1
        0.9f, -0.35f, 0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 1.0f,//  Front Right Bot Corner 2
        0.0f, -0.35f, 0.9f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f,//  Front Right Bot Corner 3




        -0.9f, -0.15f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,// Mid Bot Left Corner 0
        0.0f, -0.15f, -0.9f,   0.0f, 0.0f, 0.0f,  1.0f, 0.0f,// Front Left Bot Corner 1
        0.9f, -0.15f, 0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 1.0f,//  Front Right Bot Corner 2
        0.0f, -0.15f, 0.9f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f,//  Front Right Bot Corner 3
       


















    };
    // Index data to share position data
    GLushort indices[] = {

        //bottom 
       0, 1, 3,
       1, 3, 2,


        //Top
        4, 5, 7,
        5, 6, 7,

        //Sides
        0, 1, 5,
        4, 5, 0,

        1, 2, 6,
        6, 5, 1,

        2, 3, 7,
        7, 6, 2,

        0, 1, 5,
        5, 4, 0,






    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

//Creates the pyramids used to represent the light sources
void UCreateLamp(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)      //Textures
         0.0f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,    0.5f, 0.5f,// Tip Vertex 0
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,// Bottom Right Front Vertex 1
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,// Bottom Left Front Vertex 2

         0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f,// Bottom Right Rear Vertex 3
        -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 0.0f,    0.0f, 1.0f// Bottom Left Rear Vertex 4
    };
    // Index data to share position data
    GLushort indices[] = {
        0, 1, 2,  // Front Face
        0, 1, 3,   // Right Face
        0, 2, 4,  // Left Face
        0, 3, 4,  // Rear Face
        1, 2, 4,  // Base Triangle 1
        1, 3, 4, // Base Triangle 2

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
