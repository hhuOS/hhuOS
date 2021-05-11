/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip.c,v 1.65 2006/06/11 21:46:39 adam Exp $
 *
 */

#include "IPModule.h"
#include "IPOptions.h"

#include <cstring>

#define DEBUG_PRINTF(...) /*printf(__VA_ARGS__)*/

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */

/**
 * The IP TTL (time to live) of IP packets sent by uIP.
 *
 * This should normally not be changed.
 */
#define UIP_TTL         64

/**
 * The maximum time an IP fragment should wait in the reassembly
 * buffer before it is dropped.
 *
 */
#define UIP_REASS_MAXAGE 40

/**
 * Maximum number of TCP connections.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_CONNECTIONS 40

/**
 * Maximum number of listening TCP ports.
 *
 * \hideinitializer
 */
#define UIP_CONF_MAX_LISTENPORTS 40

/**
 * uIP buffer size.
 *
 * \hideinitializer
 */
#define UIP_CONF_BUFFER_SIZE     420

/**
 * CPU byte order.
 *
 * \hideinitializer
 */
#define UIP_CONF_BYTE_ORDER      LITTLE_ENDIAN

/**
 * Logging on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_LOGGING         1

/**
 * UDP support on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP             0

/**
 * UDP checksums on or off
 *
 * \hideinitializer
 */
#define UIP_CONF_UDP_CHECKSUMS   1

/*---------------------------------------------------------------------------*/
/* Variable definitions. */

uip_ipaddr_t uip_hostaddr, uip_draddr, uip_netmask;

static const uip_ipaddr_t all_ones_addr =
  {0xffff,0xffff};
static const uip_ipaddr_t all_zeroes_addr =
  {0x0000,0x0000};


struct uip_eth_addr uip_ethaddr = {{0,0,0,0,0,0}};

#ifndef UIP_CONF_EXTERNAL_BUFFER
uint8_t uip_buf[UIP_BUFSIZE + 2];   /* The packet buffer that contains
				    incoming packets. */
#endif /* UIP_CONF_EXTERNAL_BUFFER */

void *uip_appdata;               /* The uip_appdata pointer points to
				    application data. */
void *uip_sappdata;              /* The uip_appdata pointer points to
				    the application data which is to
				    be sent. */

uint16_t uip_len, uip_slen;
                             /* The uip_len is either 8 or 16 bits,
				depending on the maximum packet
				size. */

static uint16_t ipid;           /* Ths ipid variable is an increasing
				number that is used for the IP ID
				field. */

void uip_setipid(uint16_t id) { ipid = id; }

/* Temporary variables. */
static uint8_t c, opt;

/* Structures and definitions. */
#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO       8

/* Macros. */
#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define FBUF ((struct uip_tcpip_hdr *)&uip_reassbuf[0])
#define ICMPBUF ((struct uip_icmpip_hdr *)&uip_buf[UIP_LLH_LEN])

unsigned char
uiplib_ipaddrconv(char *addrstr, unsigned char *ipaddr)
{
    unsigned char tmp;
    char c;
    unsigned char i, j;

    tmp = 0;

    for(i = 0; i < 4; ++i) {
        j = 0;
        do {
            c = *addrstr;
            ++j;
            if(j > 4) {
                return 0;
            }
            if(c == '.' || c == 0) {
                *ipaddr = tmp;
                ++ipaddr;
                tmp = 0;
            } else if(c >= '0' && c <= '9') {
                tmp = (tmp * 10) + (c - '0');
            } else {
                return 0;
            }
            ++addrstr;
        } while(c != '.' && c != 0);
    }
    return 1;
}

static uint16_t
chksum(uint16_t sum, const uint8_t *data, uint16_t len)
{
  uint16_t t;
  const uint8_t *dataptr;
  const uint8_t *last_byte;

  dataptr = data;
  last_byte = data + len - 1;
  
  while(dataptr < last_byte) {	/* At least two more bytes */
    t = (dataptr[0] << 8) + dataptr[1];
    sum += t;
    if(sum < t) {
      sum++;		/* carry */
    }
    dataptr += 2;
  }
  
  if(dataptr == last_byte) {
    t = (dataptr[0] << 8) + 0;
    sum += t;
    if(sum < t) {
      sum++;		/* carry */
    }
  }

  /* Return sum in host byte order. */
  return sum;
}
/*---------------------------------------------------------------------------*/
uint16_t
uip_chksum(uint16_t *data, uint16_t len)
{
  return htons(chksum(0, (uint8_t *)data, len));
}
/*---------------------------------------------------------------------------*/
void
uip_init(void)
{
  /* IPv4 initialization. */
  uip_hostaddr[0] = uip_hostaddr[1] = 0;
}
/*---------------------------------------------------------------------------*/
/* XXX: IP fragment reassembly: not well-tested. */

#define UIP_REASS_BUFSIZE (UIP_BUFSIZE - UIP_LLH_LEN)
static uint8_t uip_reassbuf[UIP_REASS_BUFSIZE];
static uint8_t uip_reassbitmap[UIP_REASS_BUFSIZE / (8 * 8)];
static const uint8_t bitmap_bits[8] = {0xff, 0x7f, 0x3f, 0x1f,
				    0x0f, 0x07, 0x03, 0x01};
static uint16_t uip_reasslen;
static uint8_t uip_reassflags;
#define UIP_REASS_FLAG_LASTFRAG 0x01
static uint8_t uip_reasstmr;

#define IP_MF   0x20

static uint8_t
uip_reass(void)
{
  uint16_t offset, len;
  uint16_t i;

  /* If ip_reasstmr is zero, no packet is present in the buffer, so we
     write the IP header of the fragment into the reassembly
     buffer. The timer is updated with the maximum age. */
  if(uip_reasstmr == 0) {
    memcpy(uip_reassbuf, &BUF->vhl, UIP_IPH_LEN);
    uip_reasstmr = UIP_REASS_MAXAGE;
    uip_reassflags = 0;
    /* Clear the bitmap. */
    memset(uip_reassbitmap, 0, sizeof(uip_reassbitmap));
  }

  /* Check if the incoming fragment matches the one currently present
     in the reasembly buffer. If so, we proceed with copying the
     fragment into the buffer. */
  if(BUF->srcipaddr[0] == FBUF->srcipaddr[0] &&
     BUF->srcipaddr[1] == FBUF->srcipaddr[1] &&
     BUF->destipaddr[0] == FBUF->destipaddr[0] &&
     BUF->destipaddr[1] == FBUF->destipaddr[1] &&
     BUF->ipid[0] == FBUF->ipid[0] &&
     BUF->ipid[1] == FBUF->ipid[1]) {

    len = (BUF->len[0] << 8) + BUF->len[1] - (BUF->vhl & 0x0f) * 4;
    offset = (((BUF->ipoffset[0] & 0x3f) << 8) + BUF->ipoffset[1]) * 8;

    /* If the offset or the offset + fragment length overflows the
       reassembly buffer, we discard the entire packet. */
    if(offset > UIP_REASS_BUFSIZE ||
       offset + len > UIP_REASS_BUFSIZE) {
      uip_reasstmr = 0;
      goto nullreturn;
    }

    /* Copy the fragment into the reassembly buffer, at the right
       offset. */
    memcpy(&uip_reassbuf[UIP_IPH_LEN + offset],
	   (char *)BUF + (int)((BUF->vhl & 0x0f) * 4),
	   len);
      
    /* Update the bitmap. */
    if(offset / (8 * 8) == (offset + len) / (8 * 8)) {
      /* If the two endpoints are in the same byte, we only update
	 that byte. */
	     
      uip_reassbitmap[offset / (8 * 8)] |=
	     bitmap_bits[(offset / 8 ) & 7] &
	     ~bitmap_bits[((offset + len) / 8 ) & 7];
    } else {
      /* If the two endpoints are in different bytes, we update the
	 bytes in the endpoints and fill the stuff inbetween with
	 0xff. */
      uip_reassbitmap[offset / (8 * 8)] |=
	bitmap_bits[(offset / 8 ) & 7];
      for(i = 1 + offset / (8 * 8); i < (offset + len) / (8 * 8); ++i) {
	uip_reassbitmap[i] = 0xff;
      }
      uip_reassbitmap[(offset + len) / (8 * 8)] |=
	~bitmap_bits[((offset + len) / 8 ) & 7];
    }
    
    /* If this fragment has the More Fragments flag set to zero, we
       know that this is the last fragment, so we can calculate the
       size of the entire packet. We also set the
       IP_REASS_FLAG_LASTFRAG flag to indicate that we have received
       the final fragment. */

    if((BUF->ipoffset[0] & IP_MF) == 0) {
      uip_reassflags |= UIP_REASS_FLAG_LASTFRAG;
      uip_reasslen = offset + len;
    }
    
    /* Finally, we check if we have a full packet in the buffer. We do
       this by checking if we have the last fragment and if all bits
       in the bitmap are set. */
    if(uip_reassflags & UIP_REASS_FLAG_LASTFRAG) {
      /* Check all bytes up to and including all but the last byte in
	 the bitmap. */
      for(i = 0; i < uip_reasslen / (8 * 8) - 1; ++i) {
	if(uip_reassbitmap[i] != 0xff) {
	  goto nullreturn;
	}
      }
      /* Check the last byte in the bitmap. It should contain just the
	 right amount of bits. */
      if(uip_reassbitmap[uip_reasslen / (8 * 8)] !=
	 (uint8_t)~bitmap_bits[uip_reasslen / 8 & 7]) {
	goto nullreturn;
      }

      /* If we have come this far, we have a full packet in the
	 buffer, so we allocate a pbuf and copy the packet into it. We
	 also reset the timer. */
      uip_reasstmr = 0;
      memcpy(BUF, FBUF, uip_reasslen);

      /* Pretend to be a "normal" (i.e., not fragmented) IP packet
	 from now on. */
      BUF->ipoffset[0] = BUF->ipoffset[1] = 0;
      BUF->len[0] = uip_reasslen >> 8;
      BUF->len[1] = uip_reasslen & 0xff;
      BUF->ipchksum = 0;
      BUF->ipchksum = ~(uip_ipchksum());

      return uip_reasslen;
    }
  }

 nullreturn:
  return 0;
}
/*---------------------------------------------------------------------------*/

void
uip_process(uint8_t flag)
{
  uip_sappdata = uip_appdata = &uip_buf[UIP_IPTCPH_LEN + UIP_LLH_LEN];

//  /* Check if we were invoked because of the perodic timer fireing. */
//  if(flag == UIP_TIMER) {
//    if(uip_reasstmr != 0) {
//      --uip_reasstmr;
//    }
//  }

  /* This is where the input processing starts. */
  /* Start of IP input header processing code. */
  
  /* Check validity of the IP header. */
  if(BUF->vhl != 0x45)  { /* IP version and header length. */
    goto drop;
  }

  /* Check the size of the packet. If the size reported to us in
     uip_len is smaller the size reported in the IP header, we assume
     that the packet has been corrupted in transit. If the size of
     uip_len is larger than the size reported in the IP packet header,
     the packet has been padded and we set uip_len to the correct
     value.. */

  if((BUF->len[0] << 8) + BUF->len[1] <= uip_len) {
    uip_len = (BUF->len[0] << 8) + BUF->len[1];
  } else {
    goto drop;
  }

  /* Check the fragment flag. */
  if((BUF->ipoffset[0] & 0x3f) != 0 ||
     BUF->ipoffset[1] != 0) {
    uip_len = uip_reass();
    if(uip_len == 0) {
      goto drop;
    }
  }
    
    /* Check if the packet is destined for our IP address. */
    if(!uip_ipaddr_cmp(BUF->destipaddr, uip_hostaddr)) {
      goto drop;
    }

  if(uip_ipchksum() != 0xffff) { /* Compute and check the IP header
				    checksum. */
    goto drop;
  }

  /* ICMPv4 processing code follows. */
  if(BUF->proto != UIP_PROTO_ICMP) { /* We only allow ICMP packets from
					here. */
    goto drop;
  }

  /* ICMP echo (i.e., ping) processing. This is simple, we only change
     the ICMP type from ECHO to ECHO_REPLY and adjust the ICMP
     checksum before we return the packet. */
  if(ICMPBUF->type != ICMP_ECHO) {
    goto drop;
  }

  ICMPBUF->type = ICMP_ECHO_REPLY;

  if(ICMPBUF->icmpchksum >= HTONS(0xffff - (ICMP_ECHO << 8))) {
    ICMPBUF->icmpchksum += HTONS(ICMP_ECHO << 8) + 1;
  } else {
    ICMPBUF->icmpchksum += HTONS(ICMP_ECHO << 8);
  }

  /* Swap IP addresses. */
  uip_ipaddr_copy(BUF->destipaddr, BUF->srcipaddr);
  uip_ipaddr_copy(BUF->srcipaddr, uip_hostaddr);

  goto send;

  /* End of IPv4 input header processing code. */

 ip_send_nolen:

  BUF->vhl = 0x45;
  BUF->tos = 0;
  BUF->ipoffset[0] = BUF->ipoffset[1] = 0;
  ++ipid;
  BUF->ipid[0] = ipid >> 8;
  BUF->ipid[1] = ipid & 0xff;
  /* Calculate IP checksum. */
  BUF->ipchksum = 0;
  BUF->ipchksum = ~(uip_ipchksum());
  DEBUG_PRINTF("uip ip_send_nolen: chkecum 0x%04x\n", uip_ipchksum());

 send:
  DEBUG_PRINTF("Sending packet with length %d (%d)\n", uip_len,
	       (BUF->len[0] << 8) | BUF->len[1]);

  /* Return and let the caller do the actual transmission. */
  uip_flags = 0;
  return;
 drop:
  uip_len = 0;
  uip_flags = 0;
  return;
}
/*---------------------------------------------------------------------------*/
uint16_t
htons(uint16_t val)
{
  return HTONS(val);
}
/*---------------------------------------------------------------------------*/
