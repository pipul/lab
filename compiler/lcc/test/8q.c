
struct vector {
	int len;
	char *data;
	int addressed;
	int falseortrue:4;
};

typedef struct vector Vector;

int up[15], down[15], rows[8], x[8];

static int baidu = 30;
unsigned google = 40;

struct vector new_vector() {
	int len;
	struct vector v;
	len = 3;
	v.addressed++;
	v.len = len + v.len;
	switch (v.len) {
		case 1: v.len = 1; break;
		case 2: v.len = 2; break;
		case 3: v.len = 4; break;
		default: v.len = 0; break;
	}
	v.len++;
	return v;
}

int sum(int a, int b) {
	return a + b;
}

int print_vector(struct vector v) {
	printf("%d\n", sum(v.len, 1));
	return 0;
}

int queens(int c) {
        int r;

        for (r = 0; r < 8; r++)
                if (rows[r] && up[r-c+7] && down[r+c]) {
                        rows[r] = up[r-c+7] = down[r+c] = 0;
                        x[c] = r;
                        if (c == 7)
                                print();
                        else
                                queens(c + 1);
                        rows[r] = up[r-c+7] = down[r+c] = 1;
                }
        return r;
}

int print() {
        int k;

        for (k = 0; k < 8; k++)
                printf("%c ", x[k]+'1');
        printf("\n");
        return(0);
}


int main() {
	int i;
	struct vector v;

	i = 35;
	v.len = 21;
	print_vector(v);
	v.addressed = i < 1 ? 1 : 0;
	for (i = 0; i < 15; i++)
		up[i] = down[i] = 1;
	for (i = 0; i < 8; i++)
		rows[i] = 1;
	queens(0);
	return(0);
}
