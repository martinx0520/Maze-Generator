#include <iostream>
#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

//GUI
//Screen dimension constants
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 960;

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
    //Initialization flag
    bool success = true;
    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        printf("Warning: Linear texture filtering not enabled!");
    }

    //Create window
    gWindow = SDL_CreateWindow("maze", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    //Create renderer for window
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    //Initialize renderer color
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    return success;
}

void close()
{
    //Destroy window	
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}
//Global Var
typedef pair<int, int> pi;
typedef pair<int, pair<int, int>> pii;
#define f first
#define s second
int r,c, di[4][2] = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} },di2[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} }, dis[101][101];
pi st,st2, ed, pa, pre[101][101];bool vis[101][101], vis2[101][101]; 
vector<pi> visn; vector<pi> unvisn;vector<pi> route;

int getneib(int x, int y) {
    int viscnt = 0;
    for (int i = 0; i < 4; i++) {
        int nr = x + di[i][0], nc = y + di[i][1];
        if (nr > 0 && nr <= r && nc > 0 && nc <= c) {
            if (vis[nr][nc]) {
                visn.push_back({ nr,nc });
                viscnt++;
            }
            else {
                unvisn.push_back({ nr,nc });
            }
        }
    }
    return viscnt;
}
void connect(pi a, pi b){
    if (a.f == b.f) {
        if (a.s < b.s) {
            for (int i = a.s++; i <= b.s; i++) {
                vis[a.f][i] = 1;
            }
        }
        else {
            for (int i = a.s--; i >= b.s; i--) {
                vis[a.f][i] = 1;
            }
        }
    
    }
    else if (a.s == b.s) {
        if (a.f < b.f) {
            for (int i = a.f++; i <= b.f; i++) {
                vis[i][a.s] = 1;
            }
        }
        else {
            for (int i = a.f--; i >= b.f; i--) {
                vis[i][a.s] = 1;
            }
        }
    }
}
void prim(pi st) {
    vector<pi> path;
    path.push_back(st);
    vis[st.first][st.second] = 1;
    while (!path.empty()) {
        srand((unsigned)time(0));
        int ind = rand() % path.size();
        pi cell = path[ind];
        int vcnt = getneib(cell.f, cell.s);
        path.erase(remove(path.begin(), path.end(), cell), path.end());
        if (!visn.empty()) {
            srand((unsigned)time(0));
            int nind = rand() % visn.size();
            connect(cell, visn[nind]);
        }
        for (pi p : unvisn) {
            path.push_back(p);
        }
        visn.clear();unvisn.clear();
    }
}
int eudis(pi a, pi b) {
    return abs(b.f - a.f) + abs(b.s - a.s);
}
void astar(pi st, pi ed) {
    priority_queue<pii, vector<pii>, greater<pii>> q;
    q.push({ 0, {st.f , st.s} });
    vis2[st.f][st.s] = 1; dis[st.f][st.s] = 0;
    pre[st.f][st.s] = { -1,-1 };
    while (!q.empty()) {
        int x = q.top().s.f, y = q.top().s.s; q.pop();
        if (x == ed.f && y == ed.s) { return; }
        for (int i = 0; i < 4; i++) {
            int nr = x + di2[i][0], nc = y + di2[i][1];
            if (nr > 0 && nr <= r && nc > 0 && nc <= c && !vis2[nr][nc] && vis[nr][nc]) {
                vis2[nr][nc] = 1;dis[nr][nc] = dis[x][y] + 1;
                pre[nr][nc] = { x,y };
                q.push({ dis[nr][nc] + eudis({nr,nc},{ed.f,ed.s}),{nr,nc} });
            }
        }

    }
}
int main( int argc, char* args[] )
{
    cin.sync_with_stdio(0);cin.tie(0);cout.tie(0);
    memset(vis, false, sizeof(vis));
    cin >> r >> c;
    if (r % 2 == 0 || c % 2 == 0) { return 0;}
    cin >> st.f >> st.s;
    prim(st);
    init();
    //Main loop flag
    bool quit = false;

    //Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0x64, 0x95, 0xED, 0xFF);
    SDL_RenderClear(gRenderer);

    SDL_Event e;

    int x = 0, y = 0;
    for (int i = 1; i <= r; i++) {
        for (int j = 1; j <= c; j++) {
            if (!vis[i][j]) {
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_Rect square2 = { x, y, SCREEN_WIDTH / c, SCREEN_HEIGHT / r };
                SDL_RenderFillRect(gRenderer, &square2);
            }
            else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_Rect square = { x, y, SCREEN_WIDTH / c, SCREEN_HEIGHT / r };
                SDL_RenderFillRect(gRenderer, &square);
            }
            x += SCREEN_WIDTH / c;
        }
        x = 0;
        y += SCREEN_HEIGHT / r;
    }

    //Update the surface
    SDL_RenderPresent(gRenderer);

    cin >> st2.f >> st2.s >> ed.f >> ed.s;
    astar(st2, ed);
    pi u = ed;
    while (u.first > 0) {
        route.push_back({ u.first,u.second });
        u = pre[u.first][u.second];
    }
    reverse(route.begin(), route.end());
    SDL_SetRenderDrawColor(gRenderer, 0x64, 0x95, 0xED, 0xFF);
    SDL_RenderClear(gRenderer);
    x = 0, y = 0;
    for (int i = 1; i <= r; i++) {
        for (int j = 1; j <= c; j++) {
            pi cor = { i,j };
            if (find(route.begin(), route.end(), cor) != route.end()) {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
                SDL_Rect square3 = { x, y, SCREEN_WIDTH / c, SCREEN_HEIGHT / r };
                SDL_RenderFillRect(gRenderer, &square3);
            }
            else if (!vis[i][j]) {
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_Rect square2 = { x, y, SCREEN_WIDTH / c, SCREEN_HEIGHT / r };
                SDL_RenderFillRect(gRenderer, &square2);
            }
            else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_Rect square = { x, y, SCREEN_WIDTH / c, SCREEN_HEIGHT / r };
                SDL_RenderFillRect(gRenderer, &square);
            }
            x += SCREEN_WIDTH / c;
        }
        x = 0;
        y += SCREEN_HEIGHT / r;
    }
    //Update the surface
    SDL_RenderPresent(gRenderer);

    //While application is running
    while (!quit)
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
    }
    close();

    return 0;
}

