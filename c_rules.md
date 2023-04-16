1. DON'T use malloc
2. DON'T rely on initializers for global structs. Initialize them inside some kind of function that you call manually.
3. DO put __attribute__((section(".text"))) before main (and also put main at the end of the file?)
4. DO declare ALL memory mapped variables as `volatile`