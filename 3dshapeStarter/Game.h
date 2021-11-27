#ifndef GAME_H
#define GAME_H
/*
* if the resolution changes then update the projection matrix
*/
void OnResize(int screenWidth, int screenHeight, MyD3D& d3d);
/*
* Monitor for the escape key and quit
*/
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//render three shapes on screen
void Render(float dTime);
//modify the matrices to animate teh shapes
void Update(float dTime);
//create the geometry we'll need, startup the shader system
//initialise world, view and projection matrices
void InitGame();
//release all the d3d objects we had made
void ReleaseGame();

#endif

