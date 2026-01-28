# glennergy
Hämtar spotpris och optimerar elförbrukning










test meteo:
gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -ILibs -ILibs/API -ILibs/Utils main_meteotest.c Libs/API/Meteo.c Libs/Fetcher.c -o meteo_test -lcurl -ljansson -lm