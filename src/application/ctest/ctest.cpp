#include <cstdint>

#include <stdlib.h>

#include "lib/util/base/System.h"
#include "lib/util/io/stream/PrintStream.h"

int intComp(const void* a, const void* b) {
	return (*(int*)a) - (*(int*)b);
}

int32_t main(int32_t argc, char *argv[]) {
	
	
	Util::System::out << "----- stdlib.h" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	srand(12345);
	Util::System::out << "rand: " << rand() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	void * a = malloc(15);
	void * b = calloc(3,5);
	a = realloc(a, 25);
	Util::System::out <<Util::Io::PrintStream::hex <<"malloc, calloc, realloc: " << a <<" "<< b<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	free(a);
	free(b);
	
	int sortTest[] = {5, 16, 2, 8, 12, 1, 0, 1, 24};
	qsort(sortTest, 9, sizeof(int), &intComp);
	Util::System::out << Util::Io::PrintStream::dec;
	Util::System::out << "qsort: ";
	for (int i=0;i<9; i++) {
		Util::System::out << sortTest[i]<<" ";
	}
	Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	int searchFor = 5;
	Util::System::out << "bsearch 5: " << *(int*)bsearch(&searchFor, sortTest, 9, sizeof(int), intComp) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

	Util::System::out << "abs, labs: " << abs(-123) << " " <<labs(-123456790)<<Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	div_t d1 = div(124, 7);
	ldiv_t d2 = ldiv(124444444, 777);
	Util::System::out << "div(124,7), ldiv(124444444444, 777): " << d1.quot <<" "<<d1.rem<<" "<<d2.quot<<" "<<d2.rem<<Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    return 0;
	
}