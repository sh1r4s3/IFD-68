#include "map.h"


void def_map()
{
  FILE *fMap = fopen("map.asc", "w");

  for (unsigned int i = 0; i < MAP_SIZE; i++)
  {
    char *FNmode = ifd_map_default[i].fnmod ? "FN_ON" : "FN_OFF";
    char *keyboardKey = ifd_map_default[i - (i/69)*69].name;
    char *key = ifd_map_default[i].name;
    fprintf(fMap, "%s %s %s\n", FNmode, keyboardKey, key);
  }

  fclose(fMap);
}


MAP *search_in_map(char *sym)
{
  for (unsigned int i = 0; i < MAP_SIZE; i++)
  {
    if (strncmp(ifd_map_default[i].name, sym, 7 /* maximum length of the key name according to the ifd_map_default */) == 0)
    {
      return &ifd_map_default[i];
    }
  }

  return 0;
}
