#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <time.h>
#include <stdlib.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
#include "main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define TRUE 1
#define FALSE 0
#define CARSPEED 120
#define CARY 295
#define BOXHEIGHT 140
#define BOXLENGHT 120.0
#define LEFTBORDER 240
#define RIGHTBORDER 120
#define TICKRATE 0.5
#define CARFORWARDSPEED 600
#define CARPASSIVESPEED 300
#define CARBACKWRADSPEED 100
#define DIFICULTYLVL 160
#define ROADJUMP 40
#define DEADZONE 80

// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
// charset to bitmapa 128x128 zawierajπca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt úrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	if (x > 0 && x < SCREEN_WIDTH && y>0 && y < SCREEN_HEIGHT) {
		int bpp = surface->format->BytesPerPixel;
		Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
		*(Uint32*)p = color;
	}
};

// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

// rysowanie prostokπta o d≥ugoúci bokÛw l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

void Destructor(SDL_Surface* car, SDL_Surface* tree, SDL_Surface* paus, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_FreeSurface(screen);
	SDL_FreeSurface(car);
	SDL_FreeSurface(tree);
	SDL_FreeSurface(paus);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Interfacae(SDL_Surface* screen, double worldTime, int* points, SDL_Renderer* renderer, SDL_Texture* scrtex, SDL_Surface* charset, int czerwony, int niebieski)
{
	char text[128];
	// tekst informacyjny / info text
	DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
	DrawRectangle(screen, 4, SCREEN_HEIGHT - 42, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
	//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"

	//sprintf(text, "Szablon drugiego zadania, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);

	sprintf(text, "Jakub Grunwald 193346");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);

	//	      "Esc - exit, \030 - faster, \031 - slower"

	//sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
	sprintf(text, "Czas trwania = %.1lf s Punkty = %d ", worldTime, *points);
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

	sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie, \032 \033 - ruch");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 35, text, charset);
	sprintf(text, "L - wczytaj, S - zapisz, P - pauza,  N - nowa gra,  (a,b,c,d,e,f,g,h,i)");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 20, text, charset);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	//		SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

struct GreenBox {
	double x;
	double y;
	double lenght;
	double height;
};

struct Car {
	double lb;
	double pb;
};

void CarStart(struct Car carp[1]) {
	carp[0].pb = SCREEN_WIDTH / 2 - 16;
	carp[0].lb = SCREEN_WIDTH / 2 + 16;
};

void RoadStart(struct GreenBox box[10])
{
	int position = 0;
	for (int i = 0; i < 4; i++)
	{
		box[i].x = 0;
		box[i].y = 0 + position;
		box[i].lenght = BOXLENGHT;
		box[i].height = BOXHEIGHT;
		position += BOXHEIGHT - 10;
	}
	box[4].x = 0;
	box[4].y = position;
	box[4].lenght = BOXLENGHT;
	box[4].height = 0;
	position = 0;
	for (int i = 5; i < 10; i++)
	{
		box[i].x = SCREEN_WIDTH - BOXLENGHT;
		box[i].y = 0 + position;
		box[i].lenght = BOXLENGHT;
		box[i].height = BOXHEIGHT;
		position += BOXHEIGHT - 10;
	}
	box[9].x = SCREEN_WIDTH - BOXLENGHT;
	box[9].y = position;
	box[9].lenght = BOXLENGHT;
	box[9].height = 0;
}

void Road(double distance, SDL_Surface* screen, int czerwony, int zielony, struct GreenBox box[10]) {
	double size = 0;
	for (int i = 0; i < 10; i++)
	{
		if (box[i].height < 0)
		{
			if (i < 5)
			{
				if (i == 4)
				{
					int r = rand();
					if (r % 2 == 0) {
						box[i].lenght += ROADJUMP;
						if (box[i].lenght >= LEFTBORDER)box[i].lenght = LEFTBORDER;
					}
					else if (r % 2 == 1) {
						box[i].lenght -= ROADJUMP;
						if (box[i].lenght < RIGHTBORDER)box[i].lenght = RIGHTBORDER;
					}
					else {
						box[i].lenght = box[i].lenght;
					}
				}
				else {
					box[i].lenght = box[4].lenght;
				}
				box[i].height = 10;
				box[i].y = -10;
			}
			else {
				box[i].lenght = box[i - 5].lenght;
				box[i].height = box[i - 5].height;
				box[i].y = -10;
				box[i].x = SCREEN_WIDTH - box[i].lenght;
			}
		}
		else if (box[i].y + BOXHEIGHT > SCREEN_HEIGHT)
		{
			box[i].height -= distance;
			box[i].y += distance;
		}
		else if (box[i].height < BOXHEIGHT)
		{
			box[i].y = 0;
			box[i].height += distance;
		}
		else {
			box[i].y += distance;
		}
		DrawRectangle(screen, box[i].x, box[i].y, box[i].lenght, box[i].height + 10, zielony, zielony);
	}
};

void Pause(SDL_Surface* screen, double worldTime, SDL_Surface* paus, SDL_Renderer* renderer, SDL_Texture* scrtex, SDL_Surface* charset, int czerwony, int niebieski) {
	char text[128];
	DrawRectangle(screen, 118, 193, 405, 94, czerwony, niebieski);

	DrawSurface(screen, paus, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void Colision(struct Car carp[1], struct GreenBox box[10], int* points, int carSpeed, double* carSpeedw, double* mov) {
	int flag = TRUE;
	for (int i = 0; i < 10; i++) {
		if ((box[i].y + box[i].height > CARY - 32) && box[i].y < CARY + 32) {
			if ((box[i].x + box[i].lenght - DEADZONE > carp[0].lb) && i < 5) {
				*points -= 500;
				*carSpeedw = CARBACKWRADSPEED;
				*mov = box[i].lenght + DEADZONE;
				return;
			}
			else if (box[i].x + DEADZONE < carp[0].pb && i > 5) {
				*points -= 500;
				*carSpeedw = CARBACKWRADSPEED;
				*mov = box[i].x - DEADZONE;
				return;
			}

			if ((box[i].x + box[i].lenght - 1 > carp[0].lb) && i < 5) {
				flag = FALSE;
			}
			else if (box[i].x + 1 < carp[0].pb && i > 5) {
				flag = FALSE;
			}

		}
	}
	if (carSpeed == CARFORWARDSPEED && flag == TRUE)
	{
		*points += 100;
	}
	else if (carSpeed == CARPASSIVESPEED && flag == TRUE)
	{
		*points += 50;
	}
	else {

	}

}

void Carpostion(struct Car carp[1], double* mov) {
	carp[0].lb = *mov - 32 + 16;
	carp[0].pb = *mov + 32 - 16;
}

int Checker(SDL_Surface* car, SDL_Surface* tree, SDL_Surface* paus, SDL_Surface* picture, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen) {
	if (picture == NULL) {
		printf("SDL_LoadBMP(car2.bmp) error: %s\n", SDL_GetError());
		Destructor(car, tree, paus, screen, scrtex, window, renderer);
		SDL_Quit();
		return 1;
	};
}

void Trees(SDL_Surface* screen, SDL_Surface* tree, struct GreenBox box[10], SDL_Surface* car) {
	DrawSurface(screen, tree, 40, box[3].y);
	DrawSurface(screen, tree, 40, box[1].y);
	DrawSurface(screen, tree, 40, box[4].y);
	DrawSurface(screen, tree, SCREEN_WIDTH - 40, box[7].y);
	DrawSurface(screen, tree, SCREEN_WIDTH - 40, box[5].y);
	DrawSurface(screen, tree, SCREEN_WIDTH - 40, box[9].y);

}

void savefile(int* points, double* timegame, double* mov, struct GreenBox box[10], char savenumber)
{
	FILE* plik;
	time_t now = time(0);
	char filename[20] = { savenumber };
	char filedate[50] = { 0 };
	strftime(filedate, sizeof(filedate), "%#c", localtime(&now));
	plik = fopen(filename, "w");
	if (plik == NULL) {
		printf("I bet you saw THAT coming.\n");
		return;
	}
	fwrite(filedate, sizeof(char), strlen(filedate), plik);
	fwrite(points, sizeof(int), 1, plik);
	fwrite(timegame, sizeof(double), 1, plik);
	for (int i = 0; i < 10; i++)
	{
		fwrite(&box[i].x, sizeof(double), 1, plik);
		fwrite(&box[i].y, sizeof(double), 1, plik);
		fwrite(&box[i].lenght, sizeof(double), 1, plik);
		fwrite(&box[i].height, sizeof(double), 1, plik);
	}
	fwrite(mov, sizeof(double), 1, plik);
	fclose(plik);
}

void loadfile(int* points, double* timegame, double* mov, struct GreenBox box[10], char savenumber)
{
	FILE* plik;
	char filename[20] = { savenumber };
	char filedate[50] = { 0 };
	plik = fopen(filename, "r");
	if (plik == NULL) {
		printf("I bet you saw THAT coming.\n");
		return;
	}
	fread(filedate, sizeof(char) * 33, 1, plik);
	fread(points, sizeof(int), 1, plik);
	fread(timegame, sizeof(double), 1, plik);
	for (int i = 0; i < 10; i++)
	{
		fread(&box[i].x, sizeof(double), 1, plik);
		fread(&box[i].y, sizeof(double), 1, plik);
		fread(&box[i].lenght, sizeof(double), 1, plik);
		fread(&box[i].height, sizeof(double), 1, plik);
	}
	fread(mov, sizeof(double), 1, plik);
	fclose(plik);
}

void restart(int* points, double* timegame, double* mov) {
	*points = 0;
	*timegame = 0;
	*mov = (SCREEN_WIDTH / 2);
}

void SaveScreen(SDL_Surface* screen, double worldTime, SDL_Renderer* renderer, SDL_Texture* scrtex, SDL_Surface* charset, int czerwony, int niebieski) {
	char text[128];
	DrawRectangle(screen, 118, 193, 405, 94, czerwony, niebieski);

	sprintf(text, "Wybierz miejsce zapisu");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 200, text, charset);

	sprintf(text, "Miejsce Zapisu 1");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 220, text, charset);

	sprintf(text, "Miejsce Zapisu 2");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 235, text, charset);

	sprintf(text, "Miejsce Zapisu 3");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 250, text, charset);

	sprintf(text, "Miejsce Zapisu 4");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 265, text, charset);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void LoadScreen(SDL_Surface* screen, double worldTime, SDL_Renderer* renderer, SDL_Texture* scrtex, SDL_Surface* charset, int czerwony, int niebieski) {
	int savenumber = 65;
	char text[128];
	char filedate[50] = { 0 };
	int b = 0;
	DrawRectangle(screen, 118, 193, 405, 94, czerwony, niebieski);
	sprintf(text, "Wybierz miejsce z ktorego chcesz wczytac");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 200, text, charset);
	for (int j = 0; j < 4; j++)
	{
		char filename[20] = { savenumber };
		FILE* plik;
		plik = fopen(filename, "r");
		if (plik == NULL) {
			printf("Wrong file name try again");
			sprintf(text, filename);
		}
		else {
			fread(filedate, sizeof(char) * 33, 1, plik);
			fclose(plik);
			sprintf(text, filedate);
		}
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 220 + b, text, charset);
		savenumber++;
		b += 15;
	}

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	int t1, t2, rc, quit, pointss;
	double delta, worldTime, distance, roadSpeed, movement, carSpeed, timer, scorehandler;
	int L, R;
	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Surface* car; SDL_Surface* tree; SDL_Surface* paus;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;

	struct Car carp[1]; CarStart(carp);
	struct GreenBox box[10]; RoadStart(box);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "Spy Hunter");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	charset = SDL_LoadBMP("./cs8x8.bmp");
	Checker(charset, charset, charset, charset, scrtex, window, renderer, screen);
	SDL_SetColorKey(charset, true, 0x000000);

	car = SDL_LoadBMP("./car2.bmp");
	Checker(charset, charset, charset, car, scrtex, window, renderer, screen);

	paus = SDL_LoadBMP("./pause.bmp");
	Checker(charset, charset, charset, paus, scrtex, window, renderer, screen);

	tree = SDL_LoadBMP("./tree.bmp");
	Checker(charset, charset, charset, tree, scrtex, window, renderer, screen);

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0, 128, 0);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	quit = 0;
	worldTime = 0; double* time = &worldTime;
	distance = 0;
	roadSpeed = CARPASSIVESPEED; double* carSpeedw = &roadSpeed;
	carSpeed = 0;
	L = FALSE; R = TRUE;
	pointss = 0; int* points = &pointss;
	timer = 0;
	movement = SCREEN_WIDTH / 2; double* mov = &movement;

	while (!quit) {
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		worldTime += delta;
		if (timer < worldTime)
		{
			Colision(carp, box, points, roadSpeed, carSpeedw, mov);
			timer += TICKRATE;
		}
		distance += roadSpeed * delta;
		movement += carSpeed * delta;
		SDL_FillRect(screen, NULL, czarny);
		Road(distance, screen, czerwony, zielony, box);
		Trees(screen, tree, box, car);
		Carpostion(carp, mov);
		DrawSurface(screen, car, movement, CARY);
		Interfacae(screen, worldTime, points, renderer, scrtex, charset, czerwony, niebieski);
		while (SDL_PollEvent(&event)) {
			int flag = FALSE;
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_UP) roadSpeed = CARFORWARDSPEED;
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					R = FALSE;
					carSpeed = CARSPEED;
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					L = FALSE;
					carSpeed = -CARSPEED;
				}
				else if (event.key.keysym.sym == SDLK_DOWN) roadSpeed = CARBACKWRADSPEED;
				else if (event.key.keysym.sym == SDLK_p) flag == TRUE;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_RIGHT) {
					R = TRUE;
					if ((R == TRUE) && (L == TRUE)) carSpeed = 0;
					else if ((R == TRUE) && (L == FALSE)) carSpeed = -CARSPEED;
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					L = TRUE;
					if ((R == TRUE) && (L == TRUE)) carSpeed = 0;
					else if ((R == FALSE) && (L == TRUE)) carSpeed = CARSPEED;
				}
				else if (event.key.keysym.sym == SDLK_n) {
					restart(points, time, mov);
					RoadStart(box);
					Carpostion(carp, mov);
					timer = 0;
				}
				else if (event.key.keysym.sym == SDLK_UP) roadSpeed = CARPASSIVESPEED;
				else if (event.key.keysym.sym == SDLK_DOWN) roadSpeed = CARPASSIVESPEED;
				else if (event.key.keysym.sym == SDLK_p) {
					Pause(screen, worldTime, paus, renderer, scrtex, charset, czerwony, niebieski);
					while (flag != TRUE) {
						while (SDL_PollEvent(&event)) {
							switch (event.type) {
							case SDL_KEYUP:
								if (event.key.keysym.sym == SDLK_p) {
									flag = TRUE;
									break;
								}
							}
						}
					}
					L = TRUE;
					R = TRUE;
					//carSpeed = 0;
					t2 = SDL_GetTicks();
					t1 = t2;
				}
				else if (event.key.keysym.sym == SDLK_l) {
					LoadScreen(screen, worldTime, renderer, scrtex, charset, czerwony, niebieski);
					while (flag != TRUE) {
						while (SDL_PollEvent(&event)) {
							switch (event.type) {
							case SDL_KEYUP:
								if (event.key.keysym.sym == SDLK_1) {
									loadfile(points, time, mov, box, 65);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_2) {
									loadfile(points, time, mov, box, 66);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_3) {
									loadfile(points, time, mov, box, 67);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_4) {
									loadfile(points, time, mov, box, 68);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_l) {
									flag = TRUE;
									break;
								}
							}
						}
					}
					L = TRUE;
					R = TRUE;
					roadSpeed = CARBACKWRADSPEED;
					//carSpeed = 0;
					t2 = SDL_GetTicks();
					t1 = t2;
					timer -= worldTime;
				}
				else if (event.key.keysym.sym == SDLK_s) {
					SaveScreen(screen, worldTime, renderer, scrtex, charset, czerwony, niebieski);
					while (flag != TRUE) {
						while (SDL_PollEvent(&event)) {
							switch (event.type) {
							case SDL_KEYUP:
								if (event.key.keysym.sym == SDLK_1) {
									savefile(points, time, mov, box, 65);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_2) {
									savefile(points, time, mov, box, 66);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_3) {
									savefile(points, time, mov, box, 67);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_4) {
									savefile(points, time, mov, box, 68);
									flag = TRUE;
									break;
								}
								else if (event.key.keysym.sym == SDLK_s) {
									flag = TRUE;
									break;
								}
							}
						}
					}
					L = TRUE;
					R = TRUE;
					roadSpeed = CARBACKWRADSPEED;
					t2 = SDL_GetTicks();
					t1 = t2;
					//carSpeed = 0;
				}
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
		distance = 0;
	};
	SDL_FreeSurface(charset); Destructor(car, tree, paus, screen, scrtex, window, renderer); SDL_Quit();
	return 0;
};
