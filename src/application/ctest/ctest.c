/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ASSERT_VALUE(value, expected) \
	if (value == expected) { \
		puts("[\x1b[32mOK\x1b[39m]"); \
	} else { \
		puts("[\x1b[31mFAILED\x1b[39m]"); \
		return; \
	}

#define ASSERT_TRUE(value) \
	if (value) { \
		puts("[\x1b[32mOK\x1b[39m]"); \
	} else { \
		puts("[\x1b[31mFAILED\x1b[39m]"); \
		return; \
	}

#define ASSERT_FALSE(value) \
	ASSERT_VALUE(value, 0)

#define ASSERT_NOT_NULL(ptr) \
	if (ptr != NULL) { \
		puts("[\x1b[32mOK\x1b[39m]"); \
	} else { \
		puts("[\x1b[31mFAILED\x1b[39m]"); \
		return; \
	}

#define ASSERT_POSITIVE(value) \
	if (value >= 0) { \
		puts("[\x1b[32mOK\x1b[39m]"); \
	} else { \
		puts("[\x1b[31mFAILED\x1b[39m]"); \
		return; \
	}

#define ASSERT_FLOAT_VALUE(value, expected) \
	if (fabs(value - expected) < 0.00001) { \
		puts("[\x1b[32mOK\x1b[39m]"); \
	} else { \
		puts("[\x1b[31mFAILED\x1b[39m]"); \
		return; \
}

#ifdef HHUOS
const char *HELP_MESSAGE =
#include "generated/README.md"
;
#else
const char *HELP_MESSAGE =
	"An application to test various C standard library functionalities."
	"It is not a full-featured testing framework,"
    "but rather a simple tool to verify the behavior of certain C library functions.\n"
	"Usage: ctest [options]\n"
	"Options:\n"
	"  -h, --help: Show this help message";
#endif

char buffer[1024];
jmp_buf jumpBuffer;

void longjmpTest(void) {
	longjmp(jumpBuffer, 0);
}

int intCompare(const void *a, const void *b) {
	return *(int*)a - *(int*)b;
}

void main(const int argc, char *argv[]) {
	setlocale(LC_ALL, "en_US.UTF-8");
	setbuf(stdout, NULL);

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("%s\n", HELP_MESSAGE);
			return;
		}
	}

	puts("----- string.h -----");

	printf("strcpy: ");
	const char *str1 = "Hello World!";
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, str1);
	ASSERT_FALSE(strcmp(buffer, str1));

	printf("strncpy: ");
	const char *str2 = "Test";
	strncpy(buffer, str2, 3);
	ASSERT_FALSE(strcmp(buffer, "Teslo World!"));

	printf("strcat: ");
	strcat(buffer, str2);
	ASSERT_FALSE(strcmp(buffer, "Teslo World!Test"));

	printf("strncat: ");
	strncat(buffer, str1, 7);
	ASSERT_FALSE(strcmp(buffer, "Teslo World!TestHello W"));

	printf("strdup: ");
	char *duplicatedStr = strdup(str1);
	ASSERT_FALSE(strcmp(duplicatedStr, str1));
	free(duplicatedStr);

	printf("strlen(\"Hello World!\"): ");
	ASSERT_VALUE(strlen(str1), 12);

	printf("strcmp(\"AAB\", \"AAC\"): ");
	ASSERT_TRUE(strcmp("AAB", "AAC"));

	printf("strcmp(\"HW\", \"HW\"): ");
	ASSERT_FALSE(strcmp(str1, str1));

	printf("strncmp(\"AAB\", \"AAC\", 2): ");
	ASSERT_FALSE(strncmp("AAB", "AAC", 2));

	printf("strcasecmp(\"AAB\", \"aAb\"): ");
	ASSERT_FALSE(strcasecmp("AAB", "aAb"));

	printf("strncasecmp(\"AAB\", \"aAb\", 2): ");
	ASSERT_FALSE(strncasecmp("AAB", "AaC", 2));

	printf("strcoll(\"AAB\", \"AAC\"): ");
	ASSERT_TRUE(strcoll("AAB", "AAC"));

	printf("strchr(\"Hello\", 'l'): ");
	str1 = "Hello";
	ASSERT_TRUE(strchr(str1, 'l') == str1 + 2);

	printf("strchr(\"Hello\", 'a'): ");
	ASSERT_TRUE(strchr(str1, 'a') == NULL);

	printf("strrchr(\"Hello\", 'l'): ");
	ASSERT_TRUE(strrchr(str1, 'l') == str1 + 3);

	printf("strrchr(\"Hello\", 'a'): ");
	ASSERT_TRUE(strrchr(str1, 'a') == NULL);

	printf("strspn(\"Hello\", \"leH\"): ");
	ASSERT_VALUE(strspn(str1, "leH"), 4);

	printf("strspn(\"Hello\", \"le\"): ");
	ASSERT_VALUE(strspn(str1, "le"), 0);

	printf("strcspn(\"Hello\", \"obc\"): ");
	ASSERT_VALUE(strcspn(str1, "obc"), 4);

	printf("strcspn(\"Hello\", \"Helo\"): ");
	ASSERT_VALUE(strcspn(str1, "Helo"), 0);

	printf("strpbrk(\"Hello\", \"ol\"): ");
	ASSERT_TRUE(strpbrk(str1, "ol") == str1 + 2);

	printf("strpbrk(\"Hello\", \"xyz\"): ");
	ASSERT_TRUE(strpbrk(str1, "abc") == NULL);

	printf("strstr(\"Hello World\", \"World\"): ");
	str1 = "Hello World";
	ASSERT_TRUE(strstr(str1, "World") == str1 + 6);

	printf("strstr(\"Hello World\", \"Test\"): ");
	ASSERT_TRUE(strstr(str1, "Test") == NULL);

	printf("strtok(\"Hello;World;from;hhuOS\", \";\"): ");
	char tokStr[] = "Hello;World;from;hhuOS";
	char *tokens[16] = {NULL};
	size_t tokenCount = 0;
	char *token = strtok(tokStr, ";");
	while (token && tokenCount < 16) {
		tokens[tokenCount++] = token;
		token = strtok(NULL, ";");
	}
	ASSERT_TRUE(
		tokenCount == 4 &&
		strcmp(tokens[0], "Hello") == 0 &&
		strcmp(tokens[1], "World") == 0 &&
		strcmp(tokens[2], "from") == 0 &&
		strcmp(tokens[3], "hhuOS") == 0);

	printf("memchr(\"Hello World\", 'o', 12): ");
	str1 = "Hello World";
	ASSERT_TRUE(memchr(str1, 'o', 8) == str1 + 4);

	printf("memchr(\"Hello World\", 'o', 3): ");
	ASSERT_TRUE(memchr(str1, 'o', 3) == NULL);

	printf("memchr(\"Hello World\", 'a', 12): ");
	ASSERT_TRUE(memchr(str1, 'a', 12) == NULL);

	printf("memcmp(\"AAB\", \"AAC\", 2): ");
	ASSERT_FALSE(memcmp("AAB", "AAC", 2));

	printf("memcmp(\"AAB\", \"AAC\", 3): ");
	ASSERT_TRUE(memcmp("AAB", "AAC", 3));

	printf("memset: ");
	memset(buffer, 'A', 4);
	ASSERT_FALSE(strncmp(buffer, "AAAA", 4));

	printf("memcpy: ");
	memcpy(buffer, str2, 4);
	ASSERT_FALSE(strncmp(buffer, "Test", 4));

	printf("memmove: ");
	strcpy(buffer, "Hello World!");
	memmove(&(buffer[2]), buffer, 16);
	ASSERT_FALSE(strcmp(buffer, "HeHello World!"));

	puts("\n----- stdlib.h -----");

	printf("rand: ");
	srand(12345);
	const int randValues1[5] = { rand(), rand(), rand(), rand(), rand() };
	srand(12345);
	const int randValues2[5] = { rand(), rand(), rand(), rand(), rand() };
	srand(123456);
	const int randValues3[5] = { rand(), rand(), rand(), rand(), rand() };
	ASSERT_TRUE(
		randValues1[0] == randValues2[0] &&
		randValues1[1] == randValues2[1] &&
		randValues1[2] == randValues2[2] &&
		randValues1[3] == randValues2[3] &&
		randValues1[4] == randValues2[4] &&
		(randValues1[0] != randValues3[0] ||
		randValues1[1] != randValues3[1] ||
		randValues1[2] != randValues3[2] ||
		randValues1[3] != randValues3[3] ||
		randValues1[4] != randValues3[4]));

	printf("malloc: ");
	void *a = malloc(15);
	ASSERT_NOT_NULL(a);

	printf("realloc: ");
	void *re = realloc(a, 25);
	if (re != NULL) {
		a = re;
	}
	ASSERT_NOT_NULL(a);
	free(a);

	printf("calloc: ");
	a = calloc(3, 5);
	ASSERT_NOT_NULL(a);
	free(a);

	printf("qsort: ");
	int sortTest[] = {5, 16, 2, 8, 12, 1, 0, 1, 24};
	qsort(sortTest, 9, sizeof(int), &intCompare);
	ASSERT_TRUE(
		sortTest[0] == 0 &&
		sortTest[1] == 1 &&
		sortTest[2] == 1 &&
		sortTest[3] == 2 &&
		sortTest[4] == 5 &&
		sortTest[5] == 8 &&
		sortTest[6] == 12 &&
		sortTest[7] == 16 &&
		sortTest[8] == 24);

	printf("bsearch: ");
	const int searchFor = 5;
	const int *found = bsearch(&searchFor, sortTest, 9, sizeof(int), intCompare);
	ASSERT_TRUE(found != NULL && *found == 5);

	printf("abs(-123): ");
	ASSERT_VALUE(abs(-123), 123);

	printf("labs(-123456790): ");
	ASSERT_VALUE(labs(-123456790), 123456790);

	printf("div(124, 7): ");
	const div_t d1 = div(124, 7);
	ASSERT_TRUE(d1.quot == 17 && d1.rem == 5);

	printf("ldiv(124444444, 777): ");
	const ldiv_t d2 = ldiv(124444444, 777);
	ASSERT_TRUE(d2.quot == 160160 && d2.rem == 124);

	printf("mblen euro sign: ");
	ASSERT_VALUE(mblen("€", MB_LEN_MAX), 3);

	printf("mbtowc euro sign: ");
	wchar_t wideChar;
	const int wideCharLen = mbtowc(&wideChar, "€", MB_LEN_MAX);
	ASSERT_TRUE(
		wideCharLen == 3 &&
		wideChar == 0x20ac);

	printf("wctomb euro sign: ");
	char multibyteChar[MB_LEN_MAX];
	const int multibyteCharLen = wctomb(multibyteChar, wideChar);
	ASSERT_TRUE(
		multibyteCharLen == 3 &&
		memcmp(multibyteChar, "€", 3) == 0);

	printf("mbstowcs: ");
	const char *multiByteStr = "€$Ä";
	wchar_t wideStr[16];
	const size_t wideStrLen = mbstowcs(wideStr, multiByteStr, 20);
	ASSERT_TRUE(
		wideStrLen == 3 &&
		wideStr[0] == 0x20ac &&
		wideStr[1] == 0x24 &&
		wideStr[2] == 0xc4);

	printf("wcstombs: ");
	char multiByteStrOut[16];
	const size_t multiByteStrLen = wcstombs(multiByteStrOut, wideStr, 16);
	ASSERT_TRUE(
		multiByteStrLen == 6 &&
		memcmp(multiByteStrOut, multiByteStr, 6) == 0);

	puts("\n----- math.h -----");

	printf("fabs(-4): ");
	ASSERT_VALUE(fabs(-4.0), 4.0);

	printf("fmod(3.7, 0.5): ");
	ASSERT_FLOAT_VALUE(fmod(3.7, 0.5), 0.2);

	printf("log(4): ");
	ASSERT_FLOAT_VALUE(log(4.0), 1.3862943611198906);

	printf("log10(4): ");
	ASSERT_FLOAT_VALUE(log10(4.0), 0.6020599913279624);

	printf("pow(2, 2.5): ");
	ASSERT_FLOAT_VALUE(pow(2.0, 2.5), 5.656854249492381);

	printf("exp(-5): ");
	ASSERT_FLOAT_VALUE(exp(-5.0), 0.006737946999085467);

	printf("sqrt(2): ");
	ASSERT_FLOAT_VALUE(sqrt(2.0), 1.4142135623730951);

	printf("sin(2): ");
	ASSERT_FLOAT_VALUE(sin(2.0), 0.9092974268256817);

	printf("cos(2): ");
	ASSERT_FLOAT_VALUE(cos(2.0), -0.4161468365471424);

	printf("tan(2): ");
	ASSERT_FLOAT_VALUE(tan(2.0), -2.185039863261519);

	printf("asin(0.5): ");
	ASSERT_FLOAT_VALUE(asin(0.5), 0.5235987755982989);

	printf("acos(0.5): ");
	ASSERT_FLOAT_VALUE(acos(0.5), 1.0471975511965979);

	printf("atan(0.5): ");
	ASSERT_FLOAT_VALUE(atan(0.5), 0.4636476090008061);

	printf("atan2(0.5, 4): ");
	ASSERT_FLOAT_VALUE(atan2(0.5, 4.0), 0.12435499454676144);

	printf("sinh(2): ");
	ASSERT_FLOAT_VALUE(sinh(2.0), 3.626860407847019);

	printf("cosh(2): ");
	ASSERT_FLOAT_VALUE(cosh(2.0), 3.7621956910836314);

	printf("tanh(2): ");
	ASSERT_FLOAT_VALUE(tanh(2.0), 0.9640275800758169);

	printf("round(2.6): ");
	ASSERT_FLOAT_VALUE(round(2.6), 3.0);

	printf("round(2.4): ");
	ASSERT_FLOAT_VALUE(round(2.4), 2.0);

	printf("ceil(2.5): ");
	ASSERT_FLOAT_VALUE(ceil(2.5), 3.0);

	printf("floor(2.5): ");
	ASSERT_FLOAT_VALUE(floor(2.5), 2.0);

	printf("trunc(2.5): ");
	ASSERT_FLOAT_VALUE(trunc(2.5), 2.0);

	printf("frexp(13.245): ");
	int exponent;
	const double fraction = frexp(13.245, &exponent);
	const double recomposed = fraction * pow(2.0, exponent);
	ASSERT_FLOAT_VALUE(recomposed, 13.245);

	printf("ldexp(2.5, 3): ");
	ASSERT_FLOAT_VALUE(ldexp(2.5, 3), 20.0);

	printf("modf(13.245): ");
	double intpart;
	const double fracpart = modf(13.245, &intpart);
	ASSERT_FLOAT_VALUE(intpart + fracpart, 13.245);

	printf("isinf(1.0): ");
	ASSERT_FALSE(isinf(1.0));

	printf("isinf(1.0 / 0.0): ");
	ASSERT_TRUE(isinf(1.0 / 0.0));

	printf("isnan(1.0): ");
	ASSERT_FALSE(isnan(1.0));

	printf("isnan(0.0 / 0.0): ");
	ASSERT_TRUE(isnan(0.0 / 0.0));

	puts("\n----- time.h -----");

	printf("time: ");
	ASSERT_POSITIVE(time(NULL));

	printf("clock: ");
	ASSERT_POSITIVE(clock());

	printf("gmtime: ");
	const time_t testTime = 1767608660; // Mon Jan 5 10:24:20 2026
	const struct tm *time = gmtime(&testTime);
	ASSERT_TRUE(
		time->tm_year == 126 &&
		time->tm_mon == 0 &&
		time->tm_mday == 5 &&
		time->tm_hour == 10 &&
		time->tm_min == 24 &&
		time->tm_sec == 20 &&
		time->tm_wday == 1 &&
		time->tm_yday == 4);

	printf("mktime: ");
	ASSERT_POSITIVE(mktime((struct tm*)time));

	printf("asctime: ");
	const char *timeStr = asctime(time);
	ASSERT_FALSE(strcmp(timeStr, "Mon Jan  5 10:24:20 2026\n"));

	puts("\n----- setjmp.h -----");

	printf("setjmp: ");
	const int jmpRet = setjmp(jumpBuffer);
	if (jmpRet == 0) {
		printf("[\x1b[32mOK\x1b[39m]\n");
		printf("longjmp: ");
		longjmpTest();
	} else {
		ASSERT_VALUE(jmpRet, 1);
	}

	puts("\n----- stdio.h -----");

	printf("fopen: ");
	FILE *file = fopen("ctest.txt", "w");
	ASSERT_NOT_NULL(file);

	printf("setvbuf: ");
	ASSERT_FALSE(setvbuf(file, NULL, _IOFBF, BUFSIZ));

	printf("fputs: ");
	const char *fputsStr = "Hello World\n";
	ASSERT_POSITIVE(fputs(fputsStr, file));

	printf("fflush: ");
	ASSERT_FALSE(fflush(file));

	printf("freopen: ");
	file = freopen("ctest.txt", "r", file);
	ASSERT_NOT_NULL(file);

	printf("fgets: ");
	memset(buffer, 0, sizeof(buffer));
	char *fgetsRet = fgets(buffer, sizeof(buffer), file);
	ASSERT_TRUE(
		fgetsRet != NULL &&
		strcmp(buffer, fputsStr) == 0);

	printf("fseek: ");
	const int ret = fseek(file, -10, SEEK_END);
	memset(buffer, 0, sizeof(buffer));
	fgets(buffer, sizeof(buffer), file);
	ASSERT_TRUE(
		ret == 0 &&
		strcmp(buffer, "llo World\n") == 0);

	printf("rewind: ");
	rewind(file);
	memset(buffer, 0, sizeof(buffer));
	fgets(buffer, sizeof(buffer), file);
	ASSERT_FALSE(strcmp(buffer, fputsStr));

	printf("ftell: ");
	ASSERT_VALUE(ftell(file), (long) strlen(fputsStr));

	printf("fread: ");
	rewind(file);
	ASSERT_VALUE(fread(buffer, 10, 1, file), 1);

	printf("fread content check: ");
	ASSERT_FALSE(strncmp(buffer, fputsStr, 10));

	printf("feof before end of file: ");
	ASSERT_FALSE(feof(file));

	printf("feof at end of file: ");
	fread(buffer, sizeof(buffer), 1, file);
	ASSERT_TRUE(feof(file));

	printf("fclose: ");
	ASSERT_FALSE(fclose(file));

	printf("remove: ");
	ASSERT_FALSE(remove("ctest.txt"));

	printf("tmpnam: ");
	const char *tmpName = tmpnam(NULL);
	ASSERT_NOT_NULL(tmpName);

	printf("tmpfile: ");
	file = tmpfile();
	ASSERT_NOT_NULL(file);

	printf("Write/Read temporary file: ");
	fputs("Test write to temporary file\n", file);
	rewind(file);
	memset(buffer, 0, sizeof(buffer));
	fgets(buffer, sizeof(buffer), file);
	fclose(file);
	remove(tmpName);
	ASSERT_FALSE(strcmp(buffer, "Test write to temporary file\n"));

	printf("fprintf(\"Hello %%c %%s\"): ");
	file = fopen("ctest.txt", "w+");
	fprintf(file, "Hello %c %s", 'A', "Test");
	rewind(file);
	memset(buffer, 0, sizeof(buffer));
	fgets(buffer, sizeof(buffer), file);
	fclose(file);
	remove("ctest.txt");
	ASSERT_FALSE(strcmp(buffer, "Hello A Test"));

	printf("fprintf(\"%%10.2#o%%10.2d%%+-10.2d%%+10.2d%%10.2#x%%10.2d\"): ");
	file = fopen("ctest.txt", "w+");
	fprintf(file, "%#10.2o%10.2d%+-10.2d%+10.2d%#10.2x%10.2d", 115, -115, 16, 1234, 1234, 1);
	rewind(file);
	memset(buffer, 0, sizeof(buffer));
	fgets(buffer, sizeof(buffer), file);
	fclose(file);
	remove("ctest.txt");
	ASSERT_FALSE(strcmp(buffer, "      0163      -115+16            +1234     0x4d2        01"));

	printf("fprintf(\"%%f %%8.2f %%F %%e %%g %%G%%n\"): ");
	file = fopen("ctest.txt", "w+");
	int bytesWritten;
	fprintf(file, "%f %8.2f %F %e %g %G%n", 5.343, -21.23456, 1.0/0.0, 123456.0, 123456.0, 0.0235, &bytesWritten);
	rewind(file);
	memset(buffer, 0, sizeof(buffer));
	fgets(buffer, sizeof(buffer), file);
	fclose(file);
	remove("ctest.txt");
	ASSERT_FALSE(strcmp(buffer, "5.343000   -21.23 INF 1.234560e+05 123456 0.0235"));

	printf("printf %%n check: ");
	ASSERT_VALUE(bytesWritten, 48);

	printf("scanf(\"1234 -0x13 100 ffAA -3.414e2\", \"%%d %%i %%o %%X %%g\"): ");
	int i1, i2, i3, i4;
	float f;
	int itemsRead = sscanf("1234 -0x13 100 ffAA -3.414e2", "%d %i %o %X %g", &i1, &i2, &i3, &i4, &f);
	ASSERT_TRUE(
		i1 == 1234 &&
		i2 == -0x13 &&
		i3 == 0100 &&
		i4 == 0xffaa &&
		f > -341.5 && f < -341.3 &&
		itemsRead == 5);

	printf("scanf(\"abc defghij\\n 012563abcdefg\", \"%%3c %%s %%[0-9]%%[^g]%%n\"): ");
	char c1[1024] = {0};
	char c2[1024] = {0};
	char c3[1024] = {0};
	char c4[1024] = {0};
	itemsRead = sscanf("abc defghij\n 012563abcdefg", "%3c %s %[0-9]%[^g]%n", c1, c2, c3, c4, &i4);
	ASSERT_TRUE(
		strncmp(c1, "abc", 3) == 0 &&
		strcmp(c2, "defghij") == 0 &&
		strcmp(c3, "012563") == 0 &&
		strcmp(c4, "abcdef") == 0 &&
		i4 == 25 &&
		itemsRead == 4);

	printf("sscanf euro signs wide char: ");
	wchar_t ws[100];
	itemsRead = sscanf("€€€", "%ls", ws);
	ASSERT_TRUE(
		ws[0] == 0x20ac &&
		ws[1] == 0x20ac &&
		ws[2] == 0x20ac &&
		itemsRead == 1);
}