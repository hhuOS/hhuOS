#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <setjmp.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/io/stream/InputStream.h"

int intComp(const void* a, const void* b) {
	return (*(int*)a) - (*(int*)b);
}


jmp_buf jbuf;
void longjmp_test() {
	longjmp(jbuf, 0);
}

int32_t main() {
	bool t;
	
	int bw;
	
	
	//Test stdio
	char stdioBuf[1024];
	puts("----- stdio.h\n");
	puts("fopen, setvbuf, fputs, fflush, freopen, fgets, puts:\n");
	FILE * tf = fopen("ctest.txt", "w");
	setvbuf(tf, NULL, _IOFBF, BUFSIZ);
	fputs("Hello World\n", tf);
	fflush(tf);
	tf = freopen("ctest.txt", "r", tf);
	fgets(stdioBuf, 200, tf);
	puts(stdioBuf);	
	fclose(tf);
	
	
	tf = fopen("ctest.txt", "r");
	puts("fseek 10, SEEK_END: ");
	fseek(tf, 10, SEEK_END);
	fgets(stdioBuf, 200, tf);
	puts(stdioBuf);
	puts("rewind: ");
	rewind(tf);
	fgets(stdioBuf, 200, tf);
	puts(stdioBuf);
	Util::System::out << "ftell: "<< ftell(tf) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	fread(stdioBuf, 200, 1, tf);
	Util::System::out << "feof, ferror: "<< feof(tf)<<" "<<ferror(tf) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	perror("perrno");
	fclose(tf);
	
	puts("ungetc AB: ");
	tf = fopen("ctest.txt", "r");
	ungetc('B', tf);
	ungetc('A', tf);
	fgets(stdioBuf, 200, tf);
	puts(stdioBuf);
	fclose(tf);
	
	puts("append mode, fread:\n");
	tf = fopen("ctest.txt", "a");
	fputs("Test\n",tf);
	fclose(tf);
	
	tf = fopen("ctest.txt", "r");
	fread(stdioBuf, 200, 1,tf);
	puts(stdioBuf);
	fclose(tf);
	
	remove("ctest.txt"); 
	
	
	puts("rename ctest.txt, abc.txt:\n");
	rename("ctest.txt", "abc.txt");
	
	tf = fopen("abc.txt", "r");
	fread(stdioBuf, 200, 1,tf);
	puts(stdioBuf);
	fclose(tf);
	
	remove("abc.txt");
	
	tf = fopen("/bin", "w");
	Util::System::out << "Errno after trying to fopen /bin: "<< errno << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	
	
	puts("printf:\n");
	printf("Hello %c %s\n", 'A', "Test");
	printf("%10.2#o%10.2d%+-10.2d%+10.2d%10.2#x%10.2d\n", 115, -115, 16, 1234, 1234, 1);
	
	printf("%f %8.2f %F %e %g %G\n%n", 5.343, -21.23456, 1.0/0.0, 123456.0, 123456.0, 0.0235, &bw);
	printf("Bytes in previous line: %d, addr %p %p\n\n", bw, &bw, &main);
	
	puts("scanf:\n");
	int i1, i2, i3, i4, i5;
	double f = 0; 
	i5 = sscanf("1234 -0x13 100 ffAA -3.414e2\n", "%d %i %o %X %g\n", &i1, &i2, &i3, &i4, &f);
	printf("%i %i %i %i %g Argument count: %i\n", i1, i2, i3, i4, f, i5);
	
	char c1[1024], c2[1024], c3[1024], c4[1024];
	i5 = sscanf("abc defghij\n 012563abcdef\n", "%3c %s %[0-9]%[^\n]%n\n", c1, c2, c3, c4, &i4);
	c1[6] = '\0';
	printf("%s ; %s ; %s ; %s Argument count: %i, Bytes read: %i\n", c1, c2, c3, c4, i5, i4);
	
	puts("wchar printf/scanf Euro signs: ");
	wchar_t ws[100];
	const char * ts = "€€€€€€€€";
	sscanf(ts, "%ls", ws);
	printf("%ls\n", ws);
	
	printf("temp file name: %s\n", tmpnam(NULL));
	tf = tmpfile();
	fprintf(tf, "Write/Read to tmpfile: AAAA\n");
	rewind(tf);
	memset(stdioBuf, 0, 200);
	fread(stdioBuf, 200, 1,tf);
	puts(stdioBuf);
	fclose(tf);
	
	
	
	
	puts("\ngets: ");
	fflush(stdout);
	gets(stdioBuf);
	puts(stdioBuf);
	puts("\n\n");
	
	
	
	
	
	//Test time
	time_t testTime;
	Util::System::out << "----- time.h"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "time: "<< time(&testTime) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "clock: "<< clock() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	struct tm * tm_ptr = gmtime(&testTime);
	Util::System::out << "asctime(gmtime(time)): " << asctime(tm_ptr) << Util::Io::PrintStream::flush;
	testTime = mktime(tm_ptr);
	Util::System::out << "ctime(mktime(tm_ptr)): " << ctime(&testTime) << Util::Io::PrintStream::flush;
	
	char timeBuf[1024];
	strftime(timeBuf, 1024, "strftime:\nYear: (%Y %y) Month: (%b %B %m) Week: (%U %W)\nDay (%j %d) Weekday (%a %A %w) Time (%H %I %M %S)\nC-Time (%c)\nOther (%% %x %X %p %Z)", tm_ptr);
	
	Util::System::out << timeBuf << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	//Test locale
	Util::System::out << "----- locale.h"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "Current monetary name/symbol: " <<localeconv()->currency_symbol<<"/"<<localeconv()->int_curr_symbol<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "Current decimal seperator: " <<localeconv()->decimal_point<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	//Test setjmp 
	
	Util::System::out << "----- setjmp.h"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	if (setjmp(jbuf) == 0) {
		Util::System::out << "Calling setjmp"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
		longjmp_test();
	} else {
		Util::System::out << "Returned from longjmp"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	}
	
	Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	//Test sys/stat 
	mkdir("/ctest", 0);
	
	Util::System::out << "----- sys/stat.h\nmkdir: Created /ctest"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	
	//Test assert 
	printf("----- assert.h\n");
	assert(0);
	printf("\n");
	
	Util::System::in.readLine(t);
	
	//Test math
	Util::System::out << "----- math.h"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "fabs -4: "<< fabs(-4) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "fmod 3.7 0.5: "<< fmod(3.7, 0.5) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "log 4: "<< log(4) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "log10 4: "<< log10(4) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "pow 2, 2.5: "<< pow(2, 2.5)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "exp -5: "<< exp(-5)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "sqrt 2: "<< sqrt(2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "sin 2: "<< sin(2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "cos 2: "<< cos(2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "tan 2: "<< tan(2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "asin 0.5: "<< asin(0.5)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "acos 0.5: "<< acos(0.5)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "atan 0.5: "<< atan(0.5)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "atan2 0.5, 4: "<< atan2(0.5, 4)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "sinh 2: "<< sinh(2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "cosh 2: "<< cosh(2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "tanh 2: "<< tanh(2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "ceil 2.5: "<< ceil(2.5)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "floor 2.5: "<< floor(2.5)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	int ex;
	double r = frexp(13.245, &ex);
	double r2;
	Util::System::out << "frexp 13.245: "<< r<<", "<<ex<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "ldexp 2.5, 3: "<< ldexp(2.5, 3)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	r = modf(13.245, &r2);
	Util::System::out << "modf 13.245: "<< r2 <<" "<<r<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	Util::System::out << "isnan 1, 1/0: "<< isnan(1) <<", "<<isnan(1.0/0.0)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	Util::System::in.readLine(t);
	
	
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
	const char * wstr = "€$Ä";
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
	
	Util::System::in.readLine(t);
	
	//Test string
	
	const char * str1 = "Hello World!";
	const char * str2 = "Test";
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
	
	Util::System::out << "strdup: " << strdup(str1) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	
	Util::System::out << "strlen('Hello World!'): " << (uint32_t)strlen(str1) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strcmp('AAB', 'AAC') (HW,HW): " << strcmp("AAB", "AAC")<<" "<<strcmp(str1, str1) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strncmp('AAB', 'AAC', 2): " << strncmp("AAB", "AAC", 2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strcasecmp('AAB', 'aAb'): " << strcasecmp("AAB", "aAb")<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strncasecmp('AAB', 'AaC', 2): " << strncasecmp("AAB", "AaC", 2)<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
	Util::System::out << "strcoll('AAB', 'AAC'): " << strcoll("AAB", "AAC")<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
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