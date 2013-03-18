int gen(int a) {
	return a + 10;
}

struct test 
{ 
	char a;
	int *p; 
};

int main() {
	int c=3; 
	struct test pro = {'b',&c};/*注释*/ 
}
