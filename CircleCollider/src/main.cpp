#include<SDL.h>
#include<SDL_image.h>
#include<vector>
#include<string>
#include<string.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

struct Circle
{
	int x, y;
	int r;
};

class CustomTexture
{
public:
	SDL_Texture* mTexture;
	int mWidth, mHeight;

	CustomTexture()
	{
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}

	void render(int x, int y)
	{
		SDL_Rect dstRect = { x, y, mWidth, mHeight };
		SDL_RenderCopy(gRenderer, mTexture, NULL, &dstRect);
	}
	void free()
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
	}
};

CustomTexture gDotTexture;

void close()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_Quit();
	IMG_Quit();
}

int squaredDist(int x1, int y1, int x2, int y2)
{
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

bool checkcollision(Circle& c1, Circle& c2)
{
	return squaredDist(c1.x, c1.y, c2.x, c2.y) <= ((c1.r + c2.r) * (c1.r + c2.r));
}

bool checkcollision(Circle& a, SDL_Rect& b)
{
	//Co-ordinates of point on the rectangle closest to circle
	int cx = 0, cy = 0;
	if (a.x < b.x)
	{
		cx = b.x;
	}
	else if (a.x > b.x + b.w)
	{
		cx = b.x + b.w;
	}
	else
	{
		cx = a.x;
	}

	if (a.y < b.y)
	{
		cy = b.y;
	}
	else if (a.y > b.y + b.h)
	{
		cy = b.y + b.h;
	}
	else
	{
		cy = a.y;
	}
	return squaredDist(cx, cy, a.x, a.y) <= (a.r * a.r);
}

class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 3;

	//Initializes the variables
	Dot(int x, int y);

	//Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);


	//void move();  //without collision detection

	//Moves the dot and checks circular collision and wall collision
	void move(Circle& othercircleCollider, SDL_Rect& wall);

	//Shows the dot on the screen
	void render();

	//Gets the collision boxes
	Circle& getColliders();

private:
	//The X and Y offsets of the dot
	int mPosX, mPosY;

	//The velocity of the dot
	int mVelX, mVelY;

	//Dot's collision boxes
	//std::vector<SDL_Rect> mColliders;

	//Dot's circle collider
	Circle mCirc_collider;

	//Moves the collision boxes relative to the dot's offset
	void shiftColliders();
};

Dot::Dot(int x, int y)
{
	//Initialize the offsets
	mPosX = x;
	mPosY = y;

	//Create the necessary SDL_Rects
	//mColliders.resize(11);

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	//Initialize the dot's circular collider
	mCirc_collider.x = mPosX;
	mCirc_collider.y = mPosY;
	mCirc_collider.r = DOT_WIDTH / 2;

	shiftColliders();
}

void Dot::render()
{
	//Show the dot
	gDotTexture.render(mPosX - DOT_WIDTH / 2, mPosY - DOT_HEIGHT / 2);
}


void Dot::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}

	}
}


void Dot::shiftColliders()
{
	mCirc_collider.x = mPosX;
	mCirc_collider.y = mPosY;
}

void Dot::move(Circle& othercircleCollider, SDL_Rect& wall)
{
	mPosX += mVelX;
	shiftColliders();
	if ((mPosX - mCirc_collider.r < 0) || (mPosX + mCirc_collider.r > SCREEN_WIDTH) || checkcollision(mCirc_collider, othercircleCollider) || checkcollision(mCirc_collider, wall))
	{
		mPosX -= mVelX;
		shiftColliders();
	}
	mPosY += mVelY;
	shiftColliders();
	if ((mPosY - mCirc_collider.r < 0) || (mPosY + mCirc_collider.r > SCREEN_HEIGHT) || checkcollision(mCirc_collider, othercircleCollider) || checkcollision(mCirc_collider, wall))
	{
		mPosY -= mVelY;
		shiftColliders();
	}
}

Circle& Dot::getColliders()
{
	return mCirc_collider;
}


int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	gWindow = SDL_CreateWindow("pixel collision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	std::string basepath = std::string(SDL_GetBasePath())+"..\\..\\..\\CircleCollider\\assets\\dot.bmp";
	SDL_Surface* loaded_surface = IMG_Load(basepath.c_str());

	if (!loaded_surface)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		close();
		return 0;
	}

	SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, 0, 255, 255));
	gDotTexture.mTexture = SDL_CreateTextureFromSurface(gRenderer, loaded_surface);
	gDotTexture.mWidth = loaded_surface->w;
	gDotTexture.mHeight = loaded_surface->h;
	SDL_FreeSurface(loaded_surface);

	bool quit = false;
	SDL_Event e;

	Dot dot(10, 10);
	Dot otherdot(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);

	SDL_Rect wall = { 300, 40, 40, 400 };
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			dot.handleEvent(e);
		}

		//Move dot and check collision
		dot.move(otherdot.getColliders(), wall);
		//dot.move();

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);

		//Rendering the wall
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &wall);

		//Rendering the dot
		dot.render();
		otherdot.render();

		//Update Screen
		SDL_RenderPresent(gRenderer);
	}

	close();
	return 0;
}