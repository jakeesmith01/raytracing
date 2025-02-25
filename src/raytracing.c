#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 1200
#define HEIGHT 900
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0x808080FF
#define COLOR_BLACK 0x00000000
#define COLOR_RAY 0xFFd43b
#define NUM_RAYS 500
#define RAY_THICKNESS 1
#define M_PI 3.14159265358979323846 // intellisense is convinced this isnt defined despite compiling just fine without this line


struct Circle{
    double x;                   // The x-coordinate of the center of the circle
    double y;                   // The y-coordinate of the center of the circle
    double r;                   // The radius of the circle
} Circle;

struct Ray{
    double x_start;             // The x-coordinate of the start of the ray
    double y_start;             // The y-coordinate of the start of the ray
    double angle;               // The angle of the ray in radians
    // double x_end;               // The x-coordinate of the end of the ray
    // double y_end;               // The y-coordinate of the end of the ray
} Ray;

/// @brief Fills in the pixels of a circle on a surface 
/// @param surface The surface to draw the circle on
/// @param circle The circle to draw 
/// @param color The color to the draw the pixels of the circle 
void FillCircle(SDL_Surface* surface, struct Circle circle, Uint32 color){
    double radius_squared = pow(circle.r,2);

    // Iterate over the enveloping square of the circle, and fill in the pixels of the circle if they are within the radius
    for(double x = circle.x - circle.r; x <= circle.x + circle.r; x++){
        for(double y = circle.y - circle.r; y <= circle.y + circle.r; y++){
            if((pow(x-circle.x,2) + pow(y-circle.y,2)) < radius_squared){
                SDL_Rect pixel = (SDL_Rect) { x, y, 1, 1 };
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

/// @brief Generates rays from a circle and stores them in an array
/// @param circle The circle to generate rays from
/// @param rays The array to store the rays in
void generate_rays(struct Circle circle, struct Ray rays[NUM_RAYS]){
    
    for(int i = 0; i < NUM_RAYS; i++){
        double angle = ((double)i / NUM_RAYS) * 2 * M_PI;
        struct Ray ray = { circle.x, circle.y, angle };
        rays[i] = ray;
    }

}

/// @brief Fills in rays on a surface with the specified color
/// @param surface The surface to draw the rays on
/// @param rays The rays to draw
/// @param color The color to draw the rays
/// @param object The object to check if the rays hit
void FillRays(SDL_Surface* surface, struct Ray rays[NUM_RAYS], Uint32 color, struct Circle object){
    
    double radius_squared = pow(object.r, 2);

    for(int i = 0; i < NUM_RAYS; i++){
        struct Ray ray = rays[i];

        // End Of Screen
        int EOS = 0;
        int object_hit = 0;

        double step = 1;
        double x_draw = ray.x_start;
        double y_draw = ray.y_start;
        while(!EOS && !object_hit){
            x_draw += step * cos(ray.angle);
            y_draw += step * sin(ray.angle);

            // Draw the pixels of the ray
            SDL_Rect pixel = (SDL_Rect) { x_draw, y_draw, RAY_THICKNESS, RAY_THICKNESS };
            SDL_FillRect(surface, &pixel, color);

            // Check if the ray has gone off the screen
            if(x_draw < 0 || x_draw > WIDTH || y_draw < 0 || y_draw > HEIGHT){
                EOS = 1;
            }

            double distance_squared = pow(x_draw - object.x, 2) + pow(y_draw - object.y, 2);

            if(distance_squared < radius_squared){
                object_hit = 1;
            }
        
        }
    }
}

/// @brief Raytracing with SDL2  
/// @return 
int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    // Surface is needed to draw on the window
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect erase_rect = { 0, 0, WIDTH, HEIGHT };
    struct Circle circle = { 200, 200, 40 };
    struct Circle shadow = { 600, 450, 140 };

    struct Ray rays[NUM_RAYS];
    generate_rays(circle, rays);

    // speed at which the secondary, 'obstacle' circle will move
    double obstacle_speed_y = 3;
    int running = 1;
    SDL_Event event; 
    while(running){

        // Polls for events, such as the window being closed 
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
            if(event.motion.type == SDL_MOUSEMOTION && event.motion.state & SDL_BUTTON_LMASK){ // If the left mouse button is pressed & the mouse is moved
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                generate_rays(circle, rays);
            }
        }

        // Erases the previous frame and draws the new one 
        SDL_FillRect(surface, &erase_rect, COLOR_BLACK); 

        // Draw the rays on the surface
        FillRays(surface, rays, COLOR_RAY, shadow);

        // Draws the circle on the surface
        FillCircle(surface, circle, COLOR_WHITE);
        FillCircle(surface, shadow, COLOR_WHITE);

        // move the obstacle circle for a more visually appealing effect 
        shadow.y += obstacle_speed_y;

        // Check if it's within boundaries and reverse speed as needed
        if(shadow.y + shadow.r > HEIGHT || shadow.y - shadow.r < 0){
            obstacle_speed_y *= -1;
        }

        // Updates the window with the new surface 
        SDL_UpdateWindowSurface(window);

        // 10 corresponds to 100 fps
        SDL_Delay(10); 
    }
}