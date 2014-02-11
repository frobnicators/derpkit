#include "css/css.hpp"
#include <cstdio>
#include <cstdlib>

int main(int argc, char * argv[]) {
	if(argc != 2) {
		printf("Requires exactly one argument: %s css-file\n", argv[0]);
		exit(1);
	}
	CSS * css = CSS::from_file(argv[1]);
}
