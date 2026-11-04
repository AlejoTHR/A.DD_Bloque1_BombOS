/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/


#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "rlgl.h"       // Required to define vertex data (immediate-mode style)

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color); // Draw cube textured
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color); // Draw cube with a region of a texture


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// DEFINES
#define DELIMITER ';'
#define FORMAT "BOS"
#define VERSION "0.5"
#define TITLE "TITLE"
#define TEXTURES "TEXTURES"
#define SIZE "SIZE"
#define FLOOR "FLOOR"
#define MAP "MAP"
#define COLLISIONS "COLLISIONS"
#define OBJECTS "OBJECTS"
#define NO_TEXTURE '0'

// GLOBALS FOR STORAGE
std::string Version;
std::string Title;
std::string TexturesAmount;
int TexturesAmountINT;

// TEXTURAS
std::map<char, std::string> textures_files;

std::map<char, Texture2D> textures_loaded;


// LEVEL SIZE
int level_w = 0;
int level_h = 0;

// LEVEL LAYERS
char** floor_map;
char** map_map;
char** collisions_map;
char** objetcs_map;




void HeaderCheck(std::fstream& Level, std::string Reader, char Delimiter, std::string Format, std::string Errormsg, int NumError) {
	getline(Level, Reader, Delimiter);
	if (Reader != Format) {
		std::cout << Errormsg;
		Level.close();
		exit(NumError);
	}
	std::cout << Reader << std::endl;
}

// READ ALL TEXTURES
void CreateTexturesArray(std::fstream& Level, std::map<char, std::string>& textures_files, int& TexturesAmountINT,std::string ReaderTMP, char Delimiter) {
	for (size_t i = 0; i < TexturesAmountINT; i++) {

		// LEE ID
		getline(Level, ReaderTMP, Delimiter);
		// PASA ID A CHAR
		char tmp_char = ReaderTMP[0];

		// LEE NOMBRE
		getline(Level, ReaderTMP, Delimiter);

		// INSERTA ID Y NOMBRE
		textures_files.insert({ tmp_char, ReaderTMP });
		// SALTA LINEA
		getline(Level, ReaderTMP);
	}
}

void CreateCharPntrPntrArray(char**& ARRAY, int levelH, int levelW) {
	// CREATE Y char*
	ARRAY = new char* [levelH];
	// FOR EACH Y, INSERT X char
	for (int i = 0; i < levelH; i++) {
		ARRAY[i] = new char[levelW];
	}
}

void FillCharPntrPntrArray(std::fstream& Level, std::string ReaderTMP, char**& ARRAY, int levelH, int levelW ,char Delimiter) {
	// FILL char**
	for (int i = 0; i < levelH; i++)
	{
		for (int j = 0; j < levelW; j++)
		{
			getline(Level, ReaderTMP, Delimiter);
			ARRAY[i][j] = ReaderTMP[0];
		}
		getline(Level, ReaderTMP); // ALREADY JUMPS TO THE NEXT LINE WHEN FINISHED
	}
}


void PrintArray(char**& ARRAY, int levelH, int levelW) {
	for (int i = 0; i < levelH; i++)
	{
		for (int j = 0; j < levelW; j++)
		{
			std::cout << ARRAY[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int READER()
{
	// OPEN FILE IN READING AND WRITING
	std::fstream Level;
	Level.open("level_1.bos");

	std::string tmp; // TEMPORAL FOR ANYTHING UNWORTHY

	// CHECK IF IT WAS OPEN
	if (!Level.is_open()) {
		std::cerr << "Error 1: File could not open" << std::endl;
		Level.close();
		return 1;
	}

	// READ FILE HEADER FOR FORMAT
	HeaderCheck(Level, tmp, DELIMITER, FORMAT, "Error 2: Wrong Format", 2);

	// WHEN HEADER IS CORRECT, READ VERSION
	HeaderCheck(Level, Version, DELIMITER, VERSION, "Error 3: Wrong Version", 3);

	// JUMP TO THE START OF NEXT LINE READING ";"
	getline(Level, tmp);

	// READ "TITLE" HEADER
	HeaderCheck(Level, tmp, DELIMITER, TITLE, "Error 4: TITTLE Missindicated", 4);

	// WHEN TITLE IS NOT EMPTY
	getline(Level, Title, DELIMITER);
	if (Title == " ") {
		std::cout << "Error 5: Title Undetected";
		Level.close();
		return 5;
	}
	std::cout << Title << std::endl;

	// JUMP TO THE START OF NEXT LINE
	getline(Level, tmp);

	// READ TEXTURES HEADER
	HeaderCheck(Level, tmp, DELIMITER, TEXTURES, "Error 6: TEXTURES Missindicated", 6);

	// WHEN TEXTURES HEADER IS CORRECT, READ TEXTURES AMOUNT
	getline(Level, TexturesAmount, DELIMITER);

	// TexturesAmount STRING TO INT
	TexturesAmountINT = std::stoi(TexturesAmount);
	// IF AMOUNT IS NEGATIVE
	if (TexturesAmountINT <= 0) {
		std::cout << "Error 7: TEXTURES (int)AMOUNT is Negative";
		Level.close();
		return 7;
	}
	std::cout << TexturesAmount << std::endl;

	// JUMP TO THE START OF NEXT LINE
	getline(Level, tmp);

	// READ ALL TEXTURES
	CreateTexturesArray(Level, textures_files, TexturesAmountINT, tmp, DELIMITER);
	//PRINT ALL SAVED TEXTURES
	for (auto texture_file : textures_files) {
		std::cout << texture_file.first << "=>" << texture_file.second << std::endl;
	}

	// READ SIZE HEADER
	HeaderCheck(Level, tmp, DELIMITER, SIZE, "Error 8 = SIZE Missindicated", 8);
	// READ WIDTH
	getline(Level, tmp, DELIMITER);
	level_w = std::stoi(tmp);
	std::cout << level_w << std::endl;

	// READ HEIGHT
	getline(Level, tmp, DELIMITER);
	level_h = std::stoi(tmp);
	std::cout << level_h << std::endl;

	// JUMP TO THE START OF NEXT LINE
	getline(Level, tmp);

	// READ FLOOR HEADER
	HeaderCheck(Level, tmp, DELIMITER, FLOOR, "Error 9 = MAP Missindicated", 9);

	// JUMP TO THE START OF NEXT LINE
	getline(Level, tmp);

	// INSTANTIATE FLOOR MAP
	CreateCharPntrPntrArray(floor_map, level_h, level_w);

	// FILL char**
	FillCharPntrPntrArray(Level, tmp, floor_map, level_h, level_w, DELIMITER);

	//PRINT char**
	PrintArray(floor_map, level_h, level_w);

	// READ MAP HEADER
	HeaderCheck(Level, tmp, DELIMITER, MAP, "Error 10 = MAP Missindicated", 10);

	// JUMP TO THE START OF NEXT LINE
	getline(Level, tmp);

	// INSTANTIATE MAP MAP
	CreateCharPntrPntrArray(map_map, level_h, level_w);

	// FILL char**
	FillCharPntrPntrArray(Level, tmp, map_map, level_h, level_w, DELIMITER);

	//PRINT char**
	PrintArray(map_map, level_h, level_w);

	// READ COLLISIONS HEADER
	HeaderCheck(Level, tmp, DELIMITER, COLLISIONS, "Error 11 = COLLISIONS Missindicated", 11);

	// INSTANTIATE COLLISIONS MAP
	CreateCharPntrPntrArray(collisions_map, level_h, level_w);

	// FILL char**
	FillCharPntrPntrArray(Level, tmp, collisions_map,level_h, level_w ,DELIMITER);

	//PRINT char**
	PrintArray(collisions_map, level_h, level_w);

	// READ OBJECTS HEADER
	HeaderCheck(Level, tmp, DELIMITER, OBJECTS, "Error 12 = OBJECTS Missindicated", 12);

	// JUMP TO THE START OF NEXT LINE
	getline(Level, tmp);

	// INSTANTIATE OBJECTS MAP
	CreateCharPntrPntrArray(objetcs_map, level_h, level_w);

	// FILL char**
	FillCharPntrPntrArray(Level, tmp, objetcs_map, level_h, level_w, DELIMITER);

	//PRINT char**
	PrintArray(objetcs_map, level_h, level_w);

	Level.close(); // CLOSE OPENNED FILE
	return 0;
}

// DRAW MAP
void DrawFloorMap(std::map<char, Texture2D>& textures_loaded, char**& _map, int levelH, int levelW, float Depht, float offsetX, float offsetZ) {
	// DRAW FLOOR MAP
	for (int i = 0; i < level_h; i++)
	{
		for (int j = 0; j < level_w; j++){
			DrawCubeTexture(textures_loaded[_map[i][j]], { (float)i + offsetX, Depht, (float)j + offsetZ }, 1.0f, 1.0f, 1.0f, WHITE);
		}
	}
}
void DrawMapMap(std::map<char, Texture2D>& textures_loaded, char**& _map, int levelH, int levelW, float Depht, float offsetX, float offsetZ) {
	// DRAW FLOOR MAP
	for (int i = 0; i < level_h; i++)
	{
		for (int j = 0; j < level_w; j++){
			if (_map[j][i] != NO_TEXTURE)
				DrawCubeTexture(textures_loaded[_map[j][i]], { (float)i + offsetX, Depht, (float)j + offsetZ }, 1.0f, 1.0f, 1.0f, WHITE);
		}
	}
}


int main(void)
{
	// LECTOR DE NIVELES BombOS
	READER();


    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;


    InitWindow(screenWidth, screenHeight, "raylib [models] example - cubicmap rendering");


    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = { 0.0f, 10.0f, 5.0f };     // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };          // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                 // Camera projection type

	CheckCollisionBoxes((BoundingBox)Vector3 {}, (BoundingBox)Vector3 {});

	// LOAD TEXTURES
	for (auto const& it : textures_files) {

		std::string tmpName = "resources/" + it.second;
		const char* tmpSRC = tmpName.c_str();

		Image tmpIMAGE = LoadImage(tmpSRC);
		
		Texture2D tmpTEXTURE2D = LoadTextureFromImage(tmpIMAGE);

		textures_loaded.insert({ it.first, tmpTEXTURE2D});
	}

    Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };          // Set model position


	bool pause = false;     // Pause camera orbital rotation (and zoom)



	bool characterReceived = false;


	SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------
	
	
	// START
	float offsetX = -level_w / 2;
	if (level_w % 2 == 0) {
		offsetX += 0.5f;
	}

	float offsetZ = -level_h / 2;
	if (level_h % 2 == 0) {
		offsetZ += 0.5f;
	}


	Vector3 ballPosition = { 0 + offsetX, 0,  0 + offsetZ };
	bool Spawned = false;
	// SPAWN CHARACTER
	for (int i = 0; i < level_h; i++)
	{
		for (int j = 0; j < level_w; j++) {
			if (objetcs_map[j][i] == '1') {
				ballPosition = { i + offsetX, 0, j + offsetZ };
			}

		}
	}

	// UPDATE
	// Main game loop
	while (!WindowShouldClose())        // Detect window close button or ESC key
	{

		// Update
		//----------------------------------------------------------------------------------
		if (IsKeyPressed(KEY_P)) pause = !pause;




		//----------------------------------------------------------------------------------







		// Draw
		//----------------------------------------------------------------------------------
		
		
		
		
		
		
		
		
		BeginDrawing();

			ClearBackground(RAYWHITE);

			BeginMode3D(camera);

			// DRAW FLOOR MAP
			DrawFloorMap(textures_loaded, floor_map, level_h, level_w, 0.0f, offsetX, offsetZ);

			// DRAW MAP MAP
			DrawMapMap (textures_loaded, map_map, level_h, level_w, 1.0f, offsetX, offsetZ);



			if (IsKeyPressed(KEY_W)) {
				ballPosition.z -= 1.0f;
				GetKeyPressed();
			}
			else if (IsKeyPressed(KEY_S)) {
				ballPosition.z += 1.0f;
			}
			else if (IsKeyPressed(KEY_A)) {
				ballPosition.x -= 1.0f;
			}
			else if (IsKeyPressed(KEY_D)) {
				ballPosition.x += 1.0f;
			}
	
			DrawSphere(ballPosition, 1.0f, RED);


			DrawGrid(10, 1.0f);        // Draw a grid

			EndMode3D();

			DrawFPS(10, 10);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------


	CloseWindow();              // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}




// REPOSITORIO DE GERARD VERDU CFGS 2º DAMVIOD

//------------------------------------------------------------------------------------
// Custom Functions Definition
//------------------------------------------------------------------------------------
// Draw cube textured
// NOTE: Cube position is the center position
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;

	// Set desired texture to be enabled while drawing following vertex data
	rlSetTexture(texture.id);

	// Vertex data transformation can be defined with the commented lines,
	// but in this example we calculate the transformed vertex data directly when calling rlVertex3f()
	//rlPushMatrix();
		// NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
		//rlTranslatef(2.0f, 0.0f, 0.0f);
		//rlRotatef(45, 0, 1, 0);
		//rlScalef(2.0f, 2.0f, 2.0f);

	rlBegin(RL_QUADS);
	rlColor4ub(color.r, color.g, color.b, color.a);
	// Front Face
	rlNormal3f(0.0f, 0.0f, 1.0f);       // Normal Pointing Towards Viewer
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
	// Back Face
	rlNormal3f(0.0f, 0.0f, -1.0f);     // Normal Pointing Away From Viewer
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
	// Top Face
	rlNormal3f(0.0f, 1.0f, 0.0f);       // Normal Pointing Up
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	// Bottom Face
	rlNormal3f(0.0f, -1.0f, 0.0f);     // Normal Pointing Down
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	// Right face
	rlNormal3f(1.0f, 0.0f, 0.0f);       // Normal Pointing Right
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left Of The Texture and Quad
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left Of The Texture and Quad
	// Left Face
	rlNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
	rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left Of The Texture and Quad
	rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Right Of The Texture and Quad
	rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Right Of The Texture and Quad
	rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Of The Texture and Quad
	rlEnd();
	//rlPopMatrix();

	rlSetTexture(0);
}

// Draw cube with texture piece applied to all faces
void DrawCubeTextureRec(Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;
	float texWidth = (float)texture.width;
	float texHeight = (float)texture.height;

	// Set desired texture to be enabled while drawing following vertex data
	rlSetTexture(texture.id);

	// We calculate the normalized texture coordinates for the desired texture-source-rectangle
	// It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
	rlBegin(RL_QUADS);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// Front face
	rlNormal3f(0.0f, 0.0f, 1.0f);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

	// Back face
	rlNormal3f(0.0f, 0.0f, -1.0f);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

	// Top face
	rlNormal3f(0.0f, 1.0f, 0.0f);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

	// Bottom face
	rlNormal3f(0.0f, -1.0f, 0.0f);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

	// Right face
	rlNormal3f(1.0f, 0.0f, 0.0f);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

	// Left face
	rlNormal3f(-1.0f, 0.0f, 0.0f);
	rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
	rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
	rlTexCoord2f(source.x / texWidth, source.y / texHeight);
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

	rlEnd();

	rlSetTexture(0);
}