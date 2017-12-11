#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <cstdio>

#define ES_REPORT(op) if(auto const reported_value = op) \
	printf("Fail | " #op " -> %d\n", reported_value ); \
else  \
	printf("Success | " #op "\n")

extern "C"
int main(int argc, char *argv[])
{
	ES_REPORT(SDL_Init(SDL_INIT_EVERYTHING));

	SDL_Quit();

	printf("JOBS DONE\n");
	getchar();
	return 0;
}
