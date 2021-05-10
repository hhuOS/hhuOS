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

/*
 * uIP is a small implementation of the IP, UDP and TCP protocols (as
 * well as some basic ICMP stuff). The implementation couples the IP,
 * UDP, TCP and the application layers very tightly. To keep the size
 * of the compiled code down, this code frequently uses the goto
 * statement. While it would be possible to break the uip_process()
 * function into many smaller functions, this would increase the code
 * size because of the overhead of parameter passing and the fact that
 * the optimier would not be as efficient.
 *
 * The principle is that we have a small buffer, called the uip_buf,
 * in which the device driver puts an incoming packet. The TCP/IP
 * stack parses the headers in the packet, and calls the
 * application. If the remote host has sent data to the application,
 * this data is present in the uip_buf and the application read the
 * data from there. It is up to the application to put this data into
 * a byte stream if needed. The application will not be fed with data
 * that is out of sequence.
 *
 * If the application whishes to send data to the peer, it should put
 * its data into the uip_buf. The uip_appdata pointer points to the
 * first available byte. The TCP/IP stack will calculate the
 * checksums, and fill in the necessary header fields and finally send
 * the packet back to the peer.
*/

#include "IPModule.h"
#include "IPOptions.h"
#include "IPArchitecture.h"

#include <string.h>

#define DEBUG_PRINTF(...) /*printf(__VA_ARGS__)*/

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

uint8_t uip_flags;     /* The uip_flags variable is used for
				communication between the TCP/IP stack
				and the application program. */
struct uip_conn *uip_conn;   /* uip_conn always points to the current
				connection. */

struct uip_conn uip_conns[UIP_CONNS];
                             /* The uip_conns array holds all TCP
				connections. */
uint16_t uip_listenports[UIP_LISTENPORTS];
                             /* The uip_listenports list all currently
				listning ports. */

static uint16_t ipid;           /* Ths ipid variable is an increasing
				number that is used for the IP ID
				field. */

void uip_setipid(uint16_t id) { ipid = id; }

static uint8_t iss[4];          /* The iss variable is used for the TCP
				initial sequence number. */

#if UIP_ACTIVE_OPEN
static uint16_t lastport;       /* Keeps track of the last port used for
				a new connection. */
#endif /* UIP_ACTIVE_OPEN */

/* Temporary variables. */
uint8_t uip_acc32[4];
static uint8_t c, opt;
static uint16_t tmp16;

/* Structures and definitions. */
#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO       8

/* Macros. */
#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define FBUF ((struct uip_tcpip_hdr *)&uip_reassbuf[0])
#define ICMPBUF ((struct uip_icmpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

#if ! UIP_ARCH_CHKSUM
/*---------------------------------------------------------------------------*/
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
#ifndef UIP_ARCH_IPCHKSUM
uint16_t
uip_ipchksum(void)
{
  uint16_t sum;

  sum = chksum(0, &uip_buf[UIP_LLH_LEN], UIP_IPH_LEN);
  DEBUG_PRINTF("uip_ipchksum: sum 0x%04x\n", sum);
  return (sum == 0) ? 0xffff : htons(sum);
}
#endif
/*---------------------------------------------------------------------------*/
static uint16_t
upper_layer_chksum(uint8_t proto)
{
  uint16_t upper_layer_len;
  uint16_t sum;
  
  upper_layer_len = (((uint16_t)(BUF->len[0]) << 8) + BUF->len[1]) - UIP_IPH_LEN;

  /* First sum pseudoheader. */
  
  /* IP protocol and length fields. This addition cannot carry. */
  sum = upper_layer_len + proto;
  /* Sum IP source and destination addresses. */
  sum = chksum(sum, (uint8_t *)&BUF->srcipaddr[0], 2 * sizeof(uip_ipaddr_t));

  /* Sum TCP header and data. */
  sum = chksum(sum, &uip_buf[UIP_IPH_LEN + UIP_LLH_LEN],
	       upper_layer_len);
    
  return (sum == 0) ? 0xffff : htons(sum);
}
#endif /* UIP_ARCH_CHKSUM */
/*---------------------------------------------------------------------------*/
void
uip_init(void)
{
  for(c = 0; c < UIP_LISTENPORTS; ++c) {
    uip_listenports[c] = 0;
  }

  /* IPv4 initialization. */
  uip_hostaddr[0] = uip_hostaddr[1] = 0;

}
/*---------------------------------------------------------------------------*/
/* XXX: IP fragment reassembly: not well-tested. */

#if UIP_REASSEMBLY
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
#endif /* UIP_REASSEMBLY */
/*---------------------------------------------------------------------------*/

void
uip_process(uint8_t flag)
{
  uip_sappdata = uip_appdata = &uip_buf[UIP_IPTCPH_LEN + UIP_LLH_LEN];

//  /* Check if we were invoked because of the perodic timer fireing. */
//  if(flag == UIP_TIMER) {
//#if UIP_REASSEMBLY
//    if(uip_reasstmr != 0) {
//      --uip_reasstmr;
//    }
//#endif /* UIP_REASSEMBLY */
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

  if(uip_ipaddr_cmp(uip_hostaddr, all_zeroes_addr)) {
    /* If we are configured to use ping IP address configuration and
       hasn't been assigned an IP address yet, we accept all ICMP
       packets. */

  } else {
    /* If IP broadcast support is configured, we check for a broadcast
       UDP packet, which may be destined to us. */
#if UIP_BROADCAST
    DEBUG_PRINTF("UDP IP checksum 0x%04x\n", uip_ipchksum());
    if(BUF->proto == UIP_PROTO_UDP &&
       uip_ipaddr_cmp(BUF->destipaddr, all_ones_addr)
       /*&&
	 uip_ipchksum() == 0xffff*/) {
      goto udp_input;
    }
#endif /* UIP_BROADCAST */
    
    /* Check if the packet is destined for our IP address. */
    if(!uip_ipaddr_cmp(BUF->destipaddr, uip_hostaddr)) {
      goto drop;
    }
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
