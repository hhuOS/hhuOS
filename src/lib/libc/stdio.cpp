#include "lib/libc/stdio.h"
#include "lib/util/io/file/File.h"
#include "lib/libc/errno.h"
#include "lib/libc/string.h"
#include "lib/libc/stdlib.h"
#include "lib/interface.h"
#include "lib/util/base/String.h"

using namespace Util::Io;

FILE * stdin;
FILE * stdout;
FILE * stderr;

void _init_stdio() {
	stdin = new FileStream(STANDARD_INPUT, true, false);
	setvbuf(stdin, NULL, _IOLBF, BUFSIZ);
	
	stdout = new FileStream(STANDARD_OUTPUT, false, true);
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	
	stderr = new FileStream(STANDARD_ERROR, false, true);
}


FILE * fopen(const char* filename, const char* mode) {
	FileStream::FileMode fmode = FileStream::FileMode::READ;
	
	for (;*mode != '\0'; mode++) {
		switch (*mode) {
			case 'r':
				fmode = FileStream::FileMode::READ;
				break;
			case 'w':
				fmode = FileStream::FileMode::WRITE;
				break;
			case 'a':
				fmode = FileStream::FileMode::APPEND;
				break;
			case '+':
				if (fmode == FileStream::FileMode::READ) fmode = FileStream::FileMode::READ_EXTEND;
				if (fmode == FileStream::FileMode::WRITE) fmode = FileStream::FileMode::WRITE_EXTEND;
				if (fmode == FileStream::FileMode::APPEND) fmode = FileStream::FileMode::APPEND_EXTEND;
				break;
		}
	}
	
	FILE * ret = new FileStream(filename, fmode);
	return ret->isOpen() ? ret : NULL;
}

FILE * freopen(const char * filename, const char * mode, FILE * stream) {
	if (stream) delete stream;
	return fopen(filename, mode);
}

int fclose(FILE * stream) {
	delete stream;
	return 0;
}

int fflush(FILE * stream) {
	return stream->fflush();
}

void setbuf(FILE * stream, char * buf) {
	if (buf) {
		setvbuf(stream, buf, _IOFBF, BUFSIZ);
	} else {
		setvbuf(stream, NULL, _IONBF, 0);
	}
}

int setvbuf (FILE * stream, char * buf, int mode, size_t size) {
	FileStream::BufferMode bmode;
	
	switch (mode) {
		case _IOFBF:
			bmode = FileStream::BufferMode::FULL;
			break;
			
		case _IOLBF:
			bmode = FileStream::BufferMode::LINE;
			break;
			
		case _IONBF:
		default:
			bmode = FileStream::BufferMode::NONE;
			break;
	}
	
	return stream->setBuffer(buf, bmode, size);
}



size_t fread(void * buffer, size_t size, size_t count, FILE * stream) {
	return stream->read((uint8_t*) buffer, 0, size * count);
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE * stream) {
	stream->write((uint8_t *)buffer, 0, size * count);
	return size * count;
}

int fgetc(FILE * stream) {
	return stream->read();
}

int getc(FILE * stream) {
	return stream->read();
}

char * fgets(char * str, int count, FILE * stream) {
	char * ret = str;
	for (int i=0; i<count; i++, str++) {
		int c = getc(stream);
		if (c == EOF) break;
		*str = c;
		if (c == '\n') break;
	}
	
	if (stream->isError()) {
		return NULL;
	}
	
	*(str+1) = '\0';
	
	return ret;
}

int fputc(int ch, FILE * stream) {
	return stream->fputc(ch);
}


int putc(int ch, FILE * stream) {
	return stream->fputc(ch);
}

int fputs(const char * str, FILE* stream) {
	for (;*str != '\0'; str++) {
		if (stream->fputc(*str) == EOF) {
			return EOF;
		}
	}
	return 0;
}

int getchar() {
	return fgetc(stdin);
}

char * gets(char * str) {
	char * ret = str;
	while(1) {
		int next = getchar();
		if (next == '\n' || next == EOF) {
			*str = '\0';
			if (next == EOF) return NULL;
			return ret; 
		}
		*(str++) = next;
	}
}

int putchar(int ch) {
	return fputc(ch, stdout);
}

int puts(const char * str) {
	return fputs(str, stdout);
}

int ungetc(int ch, FILE * stream) {
	return stream->ungetChar(ch);
}


long ftell(FILE * stream) {
	return stream->getPos();
}

int fgetpos(FILE * stream, fpos_t * pos) {
	*pos = stream->getPos();
	return 0;
}

int fseek(FILE * stream, long offset, int origin) {
	FileStream::SeekMode smode;
	
	switch (origin) {
		case SEEK_SET:
		default:
			smode = FileStream::SeekMode::SET;
			break;
			
		case SEEK_CUR:
			smode = FileStream::SeekMode::CURRENT;
			break;
			
		case SEEK_END:
			smode = FileStream::SeekMode::END;
			break;
	}
	
	stream->setPos(offset, smode);
	return 0;
}

int fsetpos(FILE * stream, const fpos_t * pos) {
	stream->setPos(*pos, FileStream::SeekMode::SET);
	return 0;
}

void rewind(FILE * stream) {
	stream->setPos(0, FileStream::SeekMode::SET);
	stream->clearError();
}

void clearerr(FILE * stream) {
	stream->clearError();
}

int feof(FILE * stream) {
	return stream->isEOF() ? 1 : 0;
}

int ferror(FILE * stream) {
	return stream->isError() ? 1 : 0;
}

void perror(const char * s) {
	if (s && *s) {
		fputs(s, stderr);
		fputs(": ", stderr);
	}
	
	fputs(strerror(errno), stderr);
	fputc('\n', stderr);
}

int remove(const char * pathname) {
	return Util::Io::File(Util::String(pathname)).remove()? 0 : -1;
}

int rename(const char * old_filename, const char * new_filename) {
	FILE * nf = fopen(new_filename, "w");
	FILE * of = fopen(old_filename, "r");
	
	if (!nf || !of) return -1;
	
	while (1) {
		int c = fgetc(of);
		if (c == EOF) break;
		fputc(c, nf);
	}
	remove(old_filename);
	return 0;
}


int lastTmp = 0;

char tmpFileName[L_tmpnam];

char * tmpnam(char * filename) {
	if (lastTmp >= TMP_MAX) return NULL;
	if (!filename) filename = tmpFileName;
	
	sprintf(filename, "/temp/tmp%d", lastTmp++);
	return filename;
}

FILE * tmpfile() {
	for (char * tmpNam = tmpnam(NULL); tmpNam != NULL ;  tmpNam = tmpnam(NULL))	 {
		if (!Util::Io::File(Util::String(tmpNam)).exists()) {
			return fopen(tmpNam, "wb+");
		}
	}
	return NULL;
}