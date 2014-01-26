//these definitions can be expanded to make normal prototypes, or functionpointers and dlsym lines

BOBLIGHT_FUNCTION(void*,       boblight_init,             ());
BOBLIGHT_FUNCTION(void,        boblight_destroy,          (void* vpboblight));

BOBLIGHT_FUNCTION(int,         boblight_connect,          (void* vpboblight, const char* address, int port, int usectimeout));
BOBLIGHT_FUNCTION(int,         boblight_setpriority,      (void* vpboblight, int priority));
BOBLIGHT_FUNCTION(const char*, boblight_geterror,         (void* vpboblight));
BOBLIGHT_FUNCTION(int,         boblight_getnrlights,      (void* vpboblight));
BOBLIGHT_FUNCTION(const char*, boblight_getlightname,     (void* vpboblight, int lightnr));

BOBLIGHT_FUNCTION(int,         boblight_getnroptions,     (void* vpboblight));
BOBLIGHT_FUNCTION(const char*, boblight_getoptiondescript,(void* vpboblight, int option));
BOBLIGHT_FUNCTION(int,         boblight_setoption,        (void* vpboblight, int lightnr, const char* option));
BOBLIGHT_FUNCTION(int,         boblight_getoption,        (void* vpboblight, int lightnr, const char* option, const char** output));
BOBLIGHT_FUNCTION(void,        boblight_setscanrange,     (void* vpboblight, int width, int height));

BOBLIGHT_FUNCTION(int,         boblight_addpixel,         (void* vpboblight, int lightnr, int* rgb));
BOBLIGHT_FUNCTION(void,        boblight_addpixelxy,       (void* vpboblight, int x, int y, int* rgb));
BOBLIGHT_FUNCTION(void,        boblight_addbitmap,        (void* vpboblight, unsigned char* bmp, int xsize, int ysize, int delay));
BOBLIGHT_FUNCTION(int,         boblight_sendrgb,          (void* vpboblight, int sync, int* outputused, int cluster_leds));
BOBLIGHT_FUNCTION(int,         boblight_ping,             (void* vpboblight, int* outputused));
BOBLIGHT_FUNCTION(int,         boblight_setadjust,        (void* vpboblight, int* adjust));
BOBLIGHT_FUNCTION(void,        boblight_fillbuffer,       (void* vpboblight));
