#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <SDL_ttf.h>
#include <time.h>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

//GUI
//Screen dimension constants
int SCREEN_WIDTH = 1000;
int SCREEN_HEIGHT = 1000;

bool init();
void close();

SDL_Surface* loadSurface(std::string path);
SDL_Window* gWindow = NULL;
SDL_Window* mWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Surface* gScreenSurface = NULL;
bool init()
{
    bool success = true;
    int imgFlags = IMG_INIT_JPG;
    gScreenSurface = SDL_GetWindowSurface(gWindow);
    gWindow = SDL_CreateWindow("maze", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    return success;
}
void close()
{
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    SDL_Quit();
}
SDL_Surface* loadSurface(std::string path)
{

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    return loadedSurface;
}

//Global Var
typedef pair<int, int> pi;
typedef pair<int, pair<int, int>> pii;
#define f first
#define s second
int r,c,w,h,di[4][2] = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} },di2[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} }, dis[1001][1001];
pi st,st2, ed, pa, pre[1001][1001];bool vis[1001][1001], vis2[1001][1001]; 
vector<pi> visn; vector<pi> unvisn;vector<pi> route;

// finds all the neighbours of the given cell
void getneib(int x, int y) {
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
}
// connect the given cell to the maze
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
// Prim's Algorithm
void prim(pi st) {
    vector<pi> path;
    path.push_back(st);
    vis[st.first][st.second] = 1;
    while (!path.empty()) {
        srand((unsigned)time(0));
        int ind = rand() % path.size();
        pi cell = path[ind];
        vis[cell.first][cell.second] = 1;
        getneib(cell.f, cell.s);
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
//Manhattan Distance
int mandis(pi a, pi b) {
    return abs(b.f - a.f) + abs(b.s - a.s);
}
//A* Search ALgorithm
void astar(pi st, pi ed) {
    priority_queue<pii, vector<pii>, greater<pii>> q;
    q.push({ 0, {st.f , st.s} });
    vis2[st.f][st.s] = 1; dis[st.f][st.s] = 0;
    pre[st.f][st.s] = { -1,-1 };
    SDL_Rect squarest = { (st.s - 1) * w, (st.f - 1) * h, w, h };
    SDL_RenderFillRect(gRenderer, &squarest);
    SDL_RenderPresent(gRenderer);
    SDL_Delay(2);
    while (!q.empty()) {
        int x = q.top().s.f, y = q.top().s.s; q.pop();
        if (x == ed.f && y == ed.s) { return; }
        for (int i = 0; i < 4; i++) {
            int nr = x + di2[i][0], nc = y + di2[i][1];
            if (nr > 0 && nr <= r && nc > 0 && nc <= c && !vis2[nr][nc] && vis[nr][nc]) {
                vis2[nr][nc] = 1;dis[nr][nc] = dis[x][y] + 1;
                pre[nr][nc] = { x,y };
                q.push({ dis[nr][nc] + mandis({nr,nc},{ed.f,ed.s}),{nr,nc} });
                SDL_Rect square4 = { (nc - 1) * w, (nr - 1) * h, w, h };
                SDL_RenderFillRect(gRenderer, &square4);
            }
        }
        SDL_RenderPresent(gRenderer);
        SDL_Delay(2);
    }
}
int main( int argc, char* args[] )
{
    cin.sync_with_stdio(0);cin.tie(0);cout.tie(0);
    memset(vis, false, sizeof(vis));
    cout << "Input the Dimension of the Maze: ";
    cin >> r >> c;
    if (r % 2 == 0 || c % 2 == 0) { return 0;}
    w = SCREEN_WIDTH / c, h = SCREEN_HEIGHT / r;
    srand((unsigned)time(0));
    st.f = ((2 * rand()) + 1) % (r+1), st.s = ((2 * rand()) + 1) % (c + 1);
    cout << st.first << " " << st.second << endl;
    prim(st);
    //GUI starts here
    init();
    bool quit = false;

    SDL_Event e;

    SDL_SetRenderDrawColor(gRenderer, 0x64, 0x95, 0xED, 0xFF);
    SDL_RenderClear(gRenderer);
    int x = 0, y = 0;
    for (int i = 1; i <= r; i++) {
        for (int j = 1; j <= c; j++) {
            if (!vis[i][j]) {
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_Rect square2 = { x, y, w, h };
                SDL_RenderFillRect(gRenderer, &square2);
            }
            else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_Rect square = { x, y, w, h};
                SDL_RenderFillRect(gRenderer, &square);
            }
            x += w;
        }
        if (i == r) { SCREEN_WIDTH = x; }
        x = 0;
        y += h;
    }
    SCREEN_HEIGHT = y;
    SDL_RenderPresent(gRenderer);
    cout << "\nInput the starting Point: ";
    cin >> st2.f >> st2.s;
    cout << "\nInput the ending Point: ";
    cin >> ed.f >> ed.s;
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF);
    astar(st2, ed);
    pi u = ed;
    while (u.first > 0) {
        route.push_back({ u.first,u.second });
        u = pre[u.first][u.second];
    }
    reverse(route.begin(), route.end());
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
    for (pi p: route) {
        SDL_Rect square3 = {(p.s - 1) * w, (p.f - 1) * h, w, h };
        SDL_RenderFillRect(gRenderer, &square3);
        SDL_RenderPresent(gRenderer);
        SDL_Delay(2);
    }
    SDL_RenderPresent(gRenderer);

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
    }
    close();

    return 0;
}

