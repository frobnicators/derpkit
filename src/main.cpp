#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "derpkit/version.hpp"
#include "css/css.hpp"
#include <cstdio>
#include <cstdlib>

int main(int argc, char * argv[]) {
	printf("derpkit library version: %s\n", derpkit::version_string());

	if(argc != 2) {
		printf("Requires exactly one argument: %s css-file\n", argv[0]);
		exit(1);
	}
	CSS * css = CSS::from_file(argv[1]);
	css->print();
}
