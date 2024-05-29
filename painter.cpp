#include <iostream>
#include <SDL2\SDL.h>
#include <string>
#include <vector>



//constants
const int WIDTH=1000, HEIGHT=1000, FRAME_DELAY = 1, CANVASTOP = 60;
const char *windowTitle="Painter";

//window and renderer
SDL_Window *window=nullptr;
SDL_Renderer *renderer = nullptr;
//color struct
struct _color{
    int R ,G ,B;
    void Set(int r, int g, int b){
        R = r;
        G = g; 
        B = b;
    }
};

//
std::pair<int, int> prevPos;
std::pair<int, int> mousePos;



//pen attributes
_color penColor;
//bools
bool pressed = false;
bool wasPressed = false;

class _box{
    private:
        std::pair<int, int> pos;
        int w,h;
    public:
        std::string Stats(){
            std::string s ="";
            s+=std::to_string(pos.first);
            s+="    ";
            s+=std::to_string(pos.second);
            s+="    ";
            s+=std::to_string(w);
            s+="    ";
            s+=std::to_string(h);
            s+="\n";
            return s;

        }
        void SetBox(int x, int y, int W, int H){
            pos.first = x;
            pos.second = y;
            w = W;
            h = H;
        }
        void DrawBox(int R, int G, int B){
            SDL_Rect r;
            r.x = pos.first;
            r.y = pos.second;
            r.w = w;
            r.h = h;
            SDL_SetRenderDrawColor(renderer, R, G, B, 255);
            SDL_RenderFillRect(renderer, &r);
            SDL_RenderPresent(renderer);
             
        }
        bool CheckInbound(int x, int y){
            if(pos.first<x&&pos.second<y&&pos.first+w>x&&pos.second+h>y){
                return true;
            }
            return false;
        }
        
};
class _surface{
    public:
        _box hitBox;
        _color bgColor;
        void DrawSurface(){
            hitBox.DrawBox(bgColor.R, bgColor.G, bgColor.B);
        }
};
_surface canvas;
_surface ui;
void DrawCircle(int r, int x, int y){
    for(int i=x-r+1; i<x+r; i++){
        for(int j = y-r+1; j<y+r; j++){
            if(canvas.hitBox.CheckInbound(i,j)&&(i-x)*(i-x)+(j-y)*(j-y)<=r*r){
                SDL_RenderDrawPoint(renderer, i, j);
            }
        }
    }
}
class _brush{
    private:   
        int brushSize=1;
        
        _color brushColor;
    public:
        void SetColor(_color c){
            brushColor = c;
        }
        void ChangeSize(int c){
            brushSize+=c;
            if(brushSize<=0){
                brushSize=1;
            }
        }
        void Draw(){
            if(canvas.hitBox.CheckInbound(mousePos.first, mousePos.second)){
                SDL_SetRenderDrawColor(renderer, brushColor.R, brushColor.G, brushColor.B, 255);
                DrawCircle(brushSize, mousePos.first, mousePos.second);
                SDL_RenderPresent(renderer);
            }
        }
        
};
_brush brush;
class _button{
    public:
        _surface surface;
        void Press(int x, int y){
            if(surface.hitBox.CheckInbound(x,y)){
                brush.SetColor(surface.bgColor);
            }
        }     
};



std::vector<_button> buttons;

void AddButtons(){
    srand(time(0));
    for(int i = 0; i<10; i++){
        _button newButt;
        newButt.surface.hitBox.SetBox(i*CANVASTOP+20, CANVASTOP/2, CANVASTOP/3, CANVASTOP/3);
        newButt.surface.bgColor.Set(rand()%255, rand()%255, rand()%255);
        buttons.push_back(newButt);
    }
}


void Click(){
    if(ui.hitBox.CheckInbound(mousePos.first, mousePos.second)){
        for(_button b: buttons){
            b.Press(mousePos.first, mousePos.second);
        }
    }
}
void CanvasAndUISetup(){
    canvas.hitBox.SetBox(0, CANVASTOP, WIDTH, HEIGHT-CANVASTOP);
    ui.hitBox.SetBox(0, 0, WIDTH, CANVASTOP);
    canvas.bgColor.Set(255,255,255);
    ui.bgColor.Set(200,200,200);
    canvas.DrawSurface();
    ui.DrawSurface();
    AddButtons();
    for (_button b: buttons)
    {
        b.surface.DrawSurface();
    }
}
void WindowSetup(){
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_RenderSetScale(renderer , 1, 1);
    SDL_SetWindowTitle(window, windowTitle);
    
}
void FrameUpdate(){
    
    while(true){
        SDL_Event event;
        if(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                break;
            }
            switch(event.type){
                case SDL_MOUSEMOTION:{
                    int x, y;
                    prevPos = mousePos;
                    SDL_GetMouseState(&x, &y);
                    mousePos = {x, y};
                    if(event.button.button==SDL_BUTTON_LMASK){
                        brush.Draw();
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:{
                    if(event.button.button==SDL_BUTTON_LMASK){
                        Click();
                    }
                    break;
                }
                case SDL_KEYDOWN:{
                    switch(event.key.keysym.sym){
                        case SDLK_e:{
                            brush.SetColor(canvas.bgColor);
                            break;
                        }
                        case SDLK_r:{
                            canvas.DrawSurface();
                            break;
                        }
                        case SDLK_EQUALS:{
                            brush.ChangeSize(1);
                            break;
                        }
                        case SDLK_MINUS:{
                            brush.ChangeSize(-1);
                            break;
                        }
                        default:{
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}
int main(int argc, char*argv[]){
    WindowSetup();
    CanvasAndUISetup();
    brush.SetColor(ui.bgColor);
    FrameUpdate();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;

}