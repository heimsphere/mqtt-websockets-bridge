static int inc1 (int increment)
{
	static int i = 0;
	i += increment;
	return i;
}

static int inc2 (int increment )
{
	static int i = 0;
	i += increment;
	return i;
}

int getInc1(int i) {
	return inc1(i);
}

int getInc2(int i) {
	return inc2(i);
}

int getInc3(int x) {
	static int i = 0;
	i += x;
	return i;
}

int getInc4(int x) {
	static int i = 0;
	i += x;
	return i;
}