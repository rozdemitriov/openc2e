#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include <fstream>
#include "openc2e.h"
#include <iostream>
#include <dirent.h>

#include "World.h"
#include "caosVM.h"

SDL_Surface *screen;
SDL_Surface **backsurfs[20]; // todo: grab metaroom count, don't arbitarily define 20
int adjustx, adjusty;

void drawWorld() {
	MetaRoom *m = world.map.getCurrentMetaRoom();
	blkImage *test = m->backImage();
	for (unsigned int i = 0; i < (test->totalheight / 128); i++) {
		for (unsigned int j = 0; j < (test->totalwidth / 128); j++) {
			unsigned int whereweare = j * (test->totalheight / 128) + i;
			SDL_Rect destrect;
			destrect.x = (j * 128) - adjustx + m->x(); destrect.y = (i * 128) - adjusty + m->y();
			SDL_BlitSurface(backsurfs[m->id][whereweare], 0, screen, &destrect);
		}
	}
	for (std::vector<Room *>::iterator i = world.map.getCurrentMetaRoom()->rooms.begin();
			 i != world.map.getCurrentMetaRoom()->rooms.end(); i++) {
		// ceiling
		aalineColor(screen, (**i).x_left - adjustx, (**i).y_left_ceiling - adjusty, (**i).x_right - adjustx, (**i).y_right_ceiling - adjusty, 0xFF000077);
		// floor
		aalineColor(screen, (**i).x_left - adjustx, (**i).y_left_floor - adjusty, (**i).x_right - adjustx, (**i).y_right_floor - adjusty, 0xFF000077);
		// left side
		aalineColor(screen, (**i).x_left - adjustx, (**i).y_left_ceiling - adjusty, (**i).x_left - adjustx, (**i).y_left_floor - adjusty, 0xFF000077);
		// right side
		aalineColor(screen, (**i).x_right  - adjustx, (**i).y_right_ceiling - adjusty, (**i).x_right - adjustx, (**i).y_right_floor - adjusty, 0xFF000077);
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

extern "C" int main(int argc, char *argv[]) {
	std::vector<std::string> scripts;

	DIR *dirh;
	dirh = opendir("data/Bootstrap/001 World/");
	assert(dirh);
	for (dirent *dirp = readdir(dirh); dirp != NULL; dirp = readdir(dirh)) {
		// we assume everything except . and .. is a cos file
		if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, ".."))
			scripts.push_back(std::string("data/Bootstrap/001 World/") + dirp->d_name);
	}
	closedir(dirh);

	sort(scripts.begin(), scripts.end());
	for (std::vector<std::string>::iterator i = scripts.begin(); i != scripts.end(); i++) {
		std::ifstream script(i->c_str());
		assert(script.is_open());
		caosScript testscript(script);
		caosVM testvm(0);
		/* std::cout << "dump of script:\n";
		std::cout << testscript.dump(); */
		std::cout << "executing script " << *i << "...\n";
		std::cout.flush();
		std::cerr.flush();
		testvm.script = &testscript;
		testvm.runEntirely();
		std::cout.flush();
		std::cerr.flush();
	}

	Uint32 initflags = SDL_INIT_VIDEO;
	Uint8 video_bpp = 0;
	Uint32 videoflags = SDL_SWSURFACE + SDL_RESIZABLE;
	int done;
	SDL_Event event;

	if ( SDL_Init(initflags) < 0 ) {
		std::cout << "SDL init failed: " << SDL_GetError();
		return 1;
	}

	for (unsigned int j = 0; j < world.map.getMetaRoomCount(); j++) {
		world.map.SetCurrentMetaRoom(j);
		MetaRoom *m = world.map.getCurrentMetaRoom();
		blkImage *test = m->backImage();
		assert(test != 0);

		backsurfs[m->id] = new SDL_Surface *[test->numframes()];
		for (unsigned int i = 0; i < test->numframes(); i++) {
			backsurfs[m->id][i] = SDL_CreateRGBSurfaceFrom(test->data(i),
																							test->width(i),
																							test->height(i),
																							16, // depth
																							test->width(i) * 2, // pitch
																							0xF800, 0x07E0, 0x001F, 0); // RGBA mask
			assert(backsurfs[m->id][i] != 0);
		}
	}
	
	screen = SDL_SetVideoMode(640, 480, video_bpp, videoflags);
	assert(screen != 0);

	SDL_WM_SetCaption("openc2e - Creatures 3", "openc2e");

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	adjustx = world.map.getCurrentMetaRoom()->x();
	adjusty = world.map.getCurrentMetaRoom()->y();
	drawWorld();

	done = 0;
	while ( !done ) {
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, video_bpp, videoflags);
					assert(screen != 0);
					drawWorld();
					break;
				case SDL_MOUSEBUTTONDOWN:
					drawWorld();
					break;
				case SDL_KEYDOWN:
					if (event.key.type == SDL_KEYDOWN) {
						switch (event.key.keysym.sym) {
							case SDLK_LEFT:
								adjustx -= 20; break;
							case SDLK_RIGHT:
								adjustx += 20; break;
							case SDLK_UP:
								adjusty -= 20; break;
							case SDLK_DOWN:
								adjusty += 20; break;
							case SDLK_1:
								world.map.SetCurrentMetaRoom(world.map.getCurrentMetaRoom()->id - 1);
								adjustx = world.map.getCurrentMetaRoom()->x();
								adjusty = world.map.getCurrentMetaRoom()->y();
								break;
							case SDLK_2:
								world.map.SetCurrentMetaRoom(world.map.getCurrentMetaRoom()->id + 1);
								adjustx = world.map.getCurrentMetaRoom()->x();
								adjusty = world.map.getCurrentMetaRoom()->y();
								break;
							default:
								break;
						}
						drawWorld();
					}
					break;
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
	}
	
	SDL_Quit();

	return(0);
}
