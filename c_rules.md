1. DON'T use malloc
2. DON'T rely on initializers for global structs. Initialize them inside some kind of function that you call manually.
3.