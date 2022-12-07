#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <type_traits>
#include <vector>
#include <random>
#include <cmath>
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_video.h"
#include "Vector2f.hpp"

#define PARTICLE_NUM 6
#define PI 3.14159265359
#define SPEED 1
#define CONTINUOUS_COLLISION
#define SHOW_PATH
//#define SHOW_FPS

const int width = 1280;
const int height = 720;

SDL_Window* window = SDL_CreateWindow("Particles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

SDL_Texture* loadTexture(const char* filepath){
	SDL_Texture* texture;
	SDL_Surface* surf = IMG_Load(filepath);
	texture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	return texture;
}

struct Particle{
	SDL_Texture* _texture;
	SDL_Rect _src, _dest;
	Vector2f _position;
	Vector2f _velocity;
	Vector2f _acceleration;
	float _speed;
	int _radius;
	int _rgb[3];
	std::vector<Vector2f> _path;

	Particle(const char* filepath, float p_x, float p_y, float v_x, float v_y, float a_y, std::mt19937& rng){
		_texture = loadTexture(filepath);
		_position = Vector2f(p_x, p_y);
		_velocity = Vector2f(v_x, v_y);
		_acceleration = Vector2f(0, a_y);
		//_angle = atan2(v_y, v_x);
		_src.x = _src.y = 0;
		_src.h = _src.w = 32;
		_dest.w = _dest.h = 64;
		_radius = 32;
		std::uniform_int_distribution<> rgb_dist(50, 255);
		_rgb[0] = rgb_dist(rng);
		_rgb[1] = rgb_dist(rng);
		_rgb[2] = rgb_dist(rng);
	}
};

bool collides_ball(Particle*& p1, Particle*& p2){
	return sqrt(pow(p2->_position._x - p1->_position._x, 2) + pow(p2->_position._y - p1->_position._y, 2)) <= p1->_radius + p2->_radius - 16;
}

void continuous_collision_detection(Particle*& start_p, Vector2f& end_p){
	static auto tc_func = [&](int b, float start, float end) -> double {
		return (b - end)/(start - end);
	};
	double tc = 0;
	if(end_p._x + start_p->_dest.w >= width){
		tc = tc_func(width - start_p->_dest.w, end_p._x, start_p->_position._x);
		start_p->_velocity._x = -start_p->_velocity._x;
	}
	if(end_p._x <= 0){
		tc = tc_func(0, start_p->_position._x, end_p._x);
		start_p->_velocity._x = -start_p->_velocity._x;
	}
	if(end_p._y + start_p->_dest.w >= height){
		tc = tc_func(height - start_p->_dest.w, end_p._y, start_p->_position._y);
		start_p->_velocity._y = -start_p->_velocity._y;
	}
	if(end_p._y <= 0){
		tc = tc_func(0, start_p->_position._y, end_p._y);
		start_p->_velocity._y = -start_p->_velocity._y;
	}

	if(tc != 0){	
		end_p._x = tc*end_p._x + (1-tc)*start_p->_position._x;
		end_p._y = tc*end_p._y + (1-tc)*start_p->_position._y;
	}
}

void ball_collision(std::vector<Particle*>& particles, int index_p){
	for(int i{index_p+1}; i < PARTICLE_NUM; ++i){
		if(collides_ball(particles[index_p], particles[i])){
			std::swap(particles[i]->_velocity, particles[index_p]->_velocity);	
		}
	}
}

void borderCollision(Particle*& p){
	if(p->_position._x + p->_dest.w >= width || p->_position._x <= 1.0f){
		p->_velocity._x *= -1;
	}

	if(p->_position._y + p->_dest.h >= height || p->_position._y <= 1.0f){
		p->_velocity._y *= -1;
	}
}	

void drawParticles(std::vector<Particle*>& particles){
	for(int i{}; i < PARTICLE_NUM; ++i)
		SDL_RenderCopy(renderer, particles[i]->_texture, &particles[i]->_src, &particles[i]->_dest);
	
	#ifdef SHOW_PATH
		for(int i{}; i < PARTICLE_NUM; ++i){
			SDL_SetRenderDrawColor(renderer, particles[i]->_rgb[0], particles[i]->_rgb[1], particles[i]->_rgb[2], SDL_ALPHA_OPAQUE);
			for(int j{}; j < particles[i]->_path.size(); ++j)
				SDL_RenderDrawPointF(renderer, particles[i]->_path[j]._x + 32, particles[i]->_path[j]._y + 32);
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	#endif
}

void manageParticles(std::vector<Particle*>& particles){
	for(int i{}; i < PARTICLE_NUM; ++i){	
		ball_collision(particles, i);
		#ifdef CONTINUOUS_COLLISION
			Vector2f end_p  = Vector2f(particles[i]->_position._x + particles[i]->_velocity._x*SPEED, particles[i]->_position._y + particles[i]->_velocity._y*SPEED);
			continuous_collision_detection(particles[i], end_p);
			particles[i]->_position._x = end_p._x;
			particles[i]->_position._y = end_p._y;
		#else
			borderCollision(particles[i]);
			particles[i]->_position._x += particles[i]->_velocity._x*SPEED;
			particles[i]->_position._y += particles[i]->_velocity._y*SPEED;
		#endif
		particles[i]->_dest.x = particles[i]->_position._x;
		particles[i]->_dest.y = particles[i]->_position._y;
		particles[i]->_velocity._x = (particles[i]->_velocity._x + particles[i]->_acceleration._x * 1/59.0);
		particles[i]->_velocity._y = (particles[i]->_velocity._y + particles[i]->_acceleration._y * 1/59.0);
		#ifdef SHOW_PATH 
			particles[i]->_path.push_back(particles[i]->_position);
		#endif
	}
}

int main(){	
	std::vector<Particle*> particles(PARTICLE_NUM);
	SDL_Init(SDL_INIT_EVERYTHING);

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<> dist_vx(2, 6);
	std::uniform_real_distribution<> dist_vy(-6, -2);
	std::uniform_real_distribution<> dist_ay(0, 7);
	std::uniform_real_distribution<> dist_pos(100, 600);

	for(int i{}; i < PARTICLE_NUM; ++i){
		particles[i] = new Particle("./circle.png", dist_pos(rng), dist_pos(rng), dist_vx(rng), dist_vy(rng), dist_ay(rng), rng);
	}
	
	bool running = true;
	SDL_Event event;

	int a = SDL_GetTicks();
	int b = SDL_GetTicks();
	double delta = 0;
	while(running){
		SDL_PollEvent(&event);
		switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			default:
				break;
		}
		a = SDL_GetTicks();
		delta  = a - b;
		if(delta > 1000/59.0){
			SDL_RenderClear(renderer);
			manageParticles(particles);
			drawParticles(particles);
			SDL_RenderPresent(renderer);
#ifdef SHOW_FPS	
			std::cout << "fps: " << 1000/delta << "\n";
#endif
			b = a;
		}
	}

	for(int i{}; i < PARTICLE_NUM; ++i)
		delete particles[i];

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	return 0;
}
