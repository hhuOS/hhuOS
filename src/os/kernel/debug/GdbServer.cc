//
// Created by krakowski on 08.06.18.
//

#include <devices/Serial.h>
#include "GdbServer.h"

static const char *hexchars = "0123456789abcdef";

static const uint32_t BUFMAX = 400;

static char remcomInBuffer[BUFMAX];

static char remcomOutBuffer[BUFMAX];

/* Number of registers.  */
#define NUMREGS	16

/* Number of bytes of registers.  */
#define NUMREGBYTES (NUMREGS * 4)

static bool initialized = false;

GdbServer::~GdbServer() {

}

void putDebugChar(char value) {
    static Serial serialPort(Serial::COM1);
    serialPort.sendChar(value);
}

char getDebugChar() {
    static Serial serialPort(Serial::COM1);

    char value = serialPort.readChar();

    return value;
}

enum regnames {EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
    PC /* also known as eip */,
    PS /* also known as eflags */,
    CS, SS, DS, ES, FS, GS};

int hex (char ch) {
    if ((ch >= 'a') && (ch <= 'f'))
        return (ch - 'a' + 10);
    if ((ch >= '0') && (ch <= '9'))
        return (ch - '0');
    if ((ch >= 'A') && (ch <= 'F'))
        return (ch - 'A' + 10);
    return (-1);
}

void set_char (char *addr, int val) {
    *addr = val;
}

int get_char (char *addr) {
    return *addr;
}

int hexToInt (char **ptr, int *intValue) {
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hex (**ptr);
        if (hexValue >= 0)
        {
            *intValue = (*intValue << 4) | hexValue;
            numChars++;
        }
        else
            break;

        (*ptr)++;
    }

    return (numChars);
}

char *mem2hex (char *mem, char *buf, int count, int may_fault) {
    int i;
    unsigned char ch;

    for (i = 0; i < count; i++)
    {
        ch = get_char (mem++);
        *buf++ = hexchars[ch >> 4];
        *buf++ = hexchars[ch % 16];
    }
    *buf = 0;
    return (buf);
}


char *hex2mem (char *buf, char *mem, int count, int may_fault) {
    int i;
    unsigned char ch;

    for (i = 0; i < count; i++) {
        ch = hex (*buf++) << 4;
        ch = ch + hex (*buf++);
        set_char (mem++, ch);
    }

    return (mem);
}

char *getpacket(void) {
     char *buffer = &remcomInBuffer[0];
    unsigned char checksum;
    unsigned char xmitcsum;
    unsigned int count;
    char ch;

    while (1)
    {
        /* wait around for the start character, ignore all other characters */
        while ((ch = getDebugChar ()) != '$')
            ;

        retry:
        checksum = 0;
        xmitcsum = -1;
        count = 0;

        /* now, read until a # or end of buffer is found */
        while (count < BUFMAX - 1)
        {
            ch = getDebugChar ();
            if (ch == '$')
                goto retry;
            if (ch == '#')
                break;
            checksum = checksum + ch;
            buffer[count] = ch;
            count = count + 1;
        }
        buffer[count] = 0;

        if (ch == '#')
        {
            ch = getDebugChar ();
            xmitcsum = hex (ch) << 4;
            ch = getDebugChar ();
            xmitcsum += hex (ch);

            if (checksum != xmitcsum)
            {
                putDebugChar ('-');	/* failed checksum */
            }
            else
            {
                putDebugChar ('+');	/* successful transfer */

                /* if a sequence char is present, reply the sequence ID */
                if (buffer[2] == ':')
                {
                    putDebugChar (buffer[0]);
                    putDebugChar (buffer[1]);

                    return &buffer[3];
                }

                return &buffer[0];
            }
        }
    }
}

void putpacket (char *buffer) {
    unsigned char checksum;
    int count;
    char ch;

    /*  $<packet info>#<checksum>.  */
    do
    {
        putDebugChar ('$');
        checksum = 0;
        count = 0;

        while ((ch = buffer[count]))
        {
            putDebugChar (ch);
            checksum += ch;
            count += 1;
        }

        putDebugChar ('#');
        putDebugChar (hexchars[checksum >> 4]);
        putDebugChar (hexchars[checksum % 16]);

    }
    while (getDebugChar () != '+');
}

void GdbServer::handleInterrupt(InterruptFrame &frame) {
    int sigval, stepping;
    int addr, length;
    char *ptr;
    int newPC;

//  if (remote_debug)
//    {
//      printf ("vector=%d, sr=0x%x, pc=0x%x\n",
//	      exceptionVector, registers[PS], registers[PC]);
//    }

    /* reply to host that an exception has occurred */
    sigval = computeSignal (frame.interrupt);

    ptr = remcomOutBuffer;

    *ptr++ = 'T';			/* notify gdb with signo, PC, FP and SP */
    *ptr++ = hexchars[sigval >> 4];
    *ptr++ = hexchars[sigval & 0xf];

    *ptr++ = hexchars[ESP];
    *ptr++ = ':';
    ptr = mem2hex((char *)&frame.esp, ptr, 4, 0);	/* SP */
    *ptr++ = ';';

    *ptr++ = hexchars[EBP];
    *ptr++ = ':';
    ptr = mem2hex((char *)&frame.ebp, ptr, 4, 0); 	/* FP */
    *ptr++ = ';';

    *ptr++ = hexchars[PC];
    *ptr++ = ':';
    ptr = mem2hex((char *)&frame.eip, ptr, 4, 0); 	/* PC */
    *ptr++ = ';';

    *ptr = '\0';

    putpacket (&remcomOutBuffer[0]);

    stepping = 0;

    while (1 == 1)
    {
        remcomOutBuffer[0] = 0;
        ptr = getpacket ();

        switch (*ptr++)
        {
            case '?':
                remcomOutBuffer[0] = 'S';
                remcomOutBuffer[1] = hexchars[sigval >> 4];
                remcomOutBuffer[2] = hexchars[sigval % 16];
                remcomOutBuffer[3] = 0;
                break;
            case 'd':
                //remote_debug = !(remote_debug);	/* toggle debug flag */
                break;
            case 'g':		/* return the value of the CPU registers */
                mem2hex ((char *) &frame.gs, remcomOutBuffer, NUMREGBYTES, 0);
                break;
            case 'G':		/* set the value of the CPU registers - return OK */
                hex2mem (ptr, (char *) &frame.gs, NUMREGBYTES, 0);
                strcpy (remcomOutBuffer, "OK");
                break;
            case 'P':		/* set the value of a single CPU register - return OK */
            {
                int regno;

                if (hexToInt (&ptr, &regno) && *ptr++ == '=')
                    if (regno >= 0 && regno < NUMREGS)
                    {
                        hex2mem (ptr, (char *) ((int*)&frame)[regno], 4, 0);
                        strcpy (remcomOutBuffer, "OK");
                        break;
                    }

                strcpy (remcomOutBuffer, "E01");
                break;
            }

                /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
            case 'm':
                /* TRY TO READ %x,%x.  IF SUCCEED, SET PTR = 0 */
                if (hexToInt (&ptr, &addr))
                    if (*(ptr++) == ',')
                        if (hexToInt (&ptr, &length))
                        {
                            ptr = 0;
                            mem2hex ((char *) addr, remcomOutBuffer, length, 1);
                        }

                if (ptr)
                {
                    strcpy (remcomOutBuffer, "E01");
                }
                break;

                /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
            case 'M':
                /* TRY TO READ '%x,%x:'.  IF SUCCEED, SET PTR = 0 */
                if (hexToInt (&ptr, &addr))
                    if (*(ptr++) == ',')
                        if (hexToInt (&ptr, &length))
                            if (*(ptr++) == ':')
                            {
                                hex2mem (ptr, (char *) addr, length, 1);
                                strcpy (remcomOutBuffer, "OK");

                                ptr = 0;
                            }
                if (ptr)
                {
                    strcpy (remcomOutBuffer, "E02");
                }
                break;

                /* cAA..AA    Continue at address AA..AA(optional) */
                /* sAA..AA   Step one instruction from AA..AA(optional) */
            case 's':
                stepping = 1;
            case 'c':
                /* try to read optional parameter, pc unchanged if no parm */
                if (hexToInt (&ptr, &addr))
                    frame.eip = addr;

                newPC = frame.eip;

                /* clear the trace bit */
                frame.esp &= 0xfffffeff;

                /* set the trace bit if we're stepping */
                if (stepping)
                    frame.esp |= 0x100;

                return;

                /* kill the program */
            case 'k':		/* do nothing */
#if 0
                /* Huh? This doesn't look like "nothing".
	     m68k-stub.c and sparc-stub.c don't have it.  */
	  BREAKPOINT ();
#endif
                break;
        }			/* switch */

        /* reply to the request */
        putpacket (remcomOutBuffer);
    }
}

uint8_t GdbServer::computeSignal(uint8_t interrupt) {
    int sigval;
    switch (interrupt)
    {
        case 0:
            sigval = 8;
            break;			/* divide by zero */
        case 1:
            sigval = 5;
            break;			/* debug exception */
        case 3:
            sigval = 5;
            break;			/* breakpoint */
        case 4:
            sigval = 16;
            break;			/* into instruction (overflow) */
        case 5:
            sigval = 16;
            break;			/* bound instruction */
        case 6:
            sigval = 4;
            break;			/* Invalid opcode */
        case 7:
            sigval = 8;
            break;			/* coprocessor not available */
        case 8:
            sigval = 7;
            break;			/* double fault */
        case 9:
            sigval = 11;
            break;			/* coprocessor segment overrun */
        case 10:
            sigval = 11;
            break;			/* Invalid TSS */
        case 11:
            sigval = 11;
            break;			/* Segment not present */
        case 12:
            sigval = 11;
            break;			/* stack exception */
        case 13:
            sigval = 11;
            break;			/* general protection */
        case 14:
            sigval = 11;
            break;			/* page fault */
        case 16:
            sigval = 7;
            break;			/* coprocessor error */
        default:
            sigval = 7;		/* "software generated" */
    }
    return (sigval);
}

bool GdbServer::isInitialized() {
    return initialized;
}

void GdbServer::initialize() {
    initialized = true;
}
