#include <cstdint>

#include <stdlib.h>
#include <string.h>

#include "lib/util/base/System.h"

#include "lib/util/io/stream/PrintStream.h"

int intComp(const void* a, const void* b) {
	return (*(int*)a) - (*(int*)b);
}

int32_t main(int32_t argc, char *argv[]) {
	
	//Test stdlib 
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
    
	Util::System::out << "strtol -0x1A3bf: " << strtol(" -0x1A3bf", NULL, 0)  << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strtoul -1: " << (uint32_t)strtoul(" -1", NULL, 0)  << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strtod +24.343e-5: " << strtod("+24.343e-5", NULL) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	system("echo system: This is running echo");
	
	wchar_t ret;
	char buf[1024];
	char * wstr = "€$Ä";
	wchar_t wbuf[20];
	Util::System::out << "mblen euro sign: " << mblen("€", MB_LEN_MAX) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	mbtowc(&ret, "€", MB_LEN_MAX);
	Util::System::out << "mbtowc euro sign: " << ret <<Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	wctomb(buf, ret);
	Util::System::out << "mblen wctomb euro sign: " << mblen(buf, MB_LEN_MAX)<<Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	mbstowcs(wbuf, wstr,100);
	wcstombs(buf, wbuf, 100);
	Util::System::out << "wcstombs -> mbstowcs euro dollar AE: " <<Util::Io::PrintStream::hex;
	for (int i=0;i<4;i++) {
		Util::System::out << wbuf[i]<<" ";
	}
	Util::System::out <<Util::Io::PrintStream::dec << buf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	
	//Test string
	
	char * str1 = "Hello World!";
	char * str2 = "Test";
	Util::System::out << "----- string.h" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	strcpy(buf, str1);
	Util::System::out << "strcpy: " << buf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	strncpy(buf, str2, 3);
	Util::System::out << "strncpy: " << buf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	strcat(buf, str2);
	Util::System::out << "strcat: " << buf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	strncat(buf, str1, 7);
	Util::System::out << "strncat: " << buf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	strxfrm(buf, str1, 5);
	Util::System::out << "strxfrm: " << buf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	Util::System::out << "strlen('Hello World!'): " << (uint32_t)strlen(str1) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strcmp(''AAB', 'AAC') (HW,HW): " << strcmp("AAB", "AAC")<<" "<<strcmp(str1, str1) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strncmp(''AAB', 'AAC', 2): " << strncmp("AAB", "AAC", 2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strcoll(''AAB', 'AAC'): " << strcoll("AAB", "AAC")<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strchr(HW,'o'): " << strchr(str1, 'o')<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strrchr(HW,'o'): " << strrchr(str1, 'o')<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strspn(HW, 'leH'): " << (uint32_t)strspn(str1, "leH")<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strcspn(HW, 'No!'): " << (uint32_t)strcspn(str1, "No!")<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strpbrk(HW, ' !'): " << strpbrk(str1, " !")<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strstr(HW, 'World'): " << strstr(str1, "World")<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	Util::System::out << "strtok(HW, 'o'): "<< Util::Io::PrintStream::flush;
	strcpy(buf, str1);
	char* tokptr = strtok(buf,"o");
	while (tokptr) {
		Util::System::out << tokptr <<";"<< Util::Io::PrintStream::flush;
		tokptr = strtok(NULL,"o");
	}
	Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	strcpy(buf ,str1);
	Util::System::out << "memchr(HW, 'o', 8): " << (char*)memchr(buf,'o', 8)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "memcmp(AAB, AAC, 3): " << memcmp("AAB", "AAC", 3)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	memset(buf, 'A', 4);
	Util::System::out << "memset(HW, 'A', 4): " << buf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	memcpy(buf, str2, 4);
	Util::System::out << "memcpy(HW, Test, 4): " << buf<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	memmove(&(buf[2]), buf, 30);
	Util::System::out << "memmove(HW[2], HW, 30): " << buf<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strerror(1): " << strerror(1)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	
	return 0;
	
}