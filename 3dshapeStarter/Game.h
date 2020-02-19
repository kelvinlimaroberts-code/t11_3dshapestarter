#ifndef GAME_H
#define GAME_H

void OnResize(int screenWidth, int screenHeight, MyD3D& d3d);
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void Render(float dTime);
void Update(float dTime);
void InitGame();
void ReleaseGame();

#endif

