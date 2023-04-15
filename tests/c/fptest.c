float my_helper_float_func(float x, float y) {
	return x + y * 5;
}

int main() {
	union raw_fp
	{
		float f;
		unsigned int i;
	} x;
	
	x.f = 3.2f;

	x.f = my_helper_float_func(x.f, 6);

	x.i += 1;

	return (int)x.f;
}
