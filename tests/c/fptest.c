int main() {
	union raw_fp
	{
		float f;
		unsigned int i;
	} x;
	
	x.f = 3.2f;

	x.f *= 5;

	x.i += 1;

	return (int)x.f;
}
