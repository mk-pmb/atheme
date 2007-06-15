/*
 * atheme-services: A collection of minimalist IRC services
 * packet.c: IRC packet handling.
 *
 * Copyright (c) 2005-2007 Atheme Project (http://www.atheme.org)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "atheme.h"
#include "uplink.h"
#include "datastream.h"

/* bursting timer */
#if HAVE_GETTIMEOFDAY
struct timeval burstime;
#endif

static void irc_recvq_handler(connection_t *cptr)
{
	boolean_t wasnonl;
	char parsebuf[BUFSIZE + 1];
	int count;

	wasnonl = cptr->flags & CF_NONEWLINE ? TRUE : FALSE;
	count = recvq_getline(cptr, parsebuf, sizeof parsebuf - 1);
	if (count <= 0)
		return;
	cnt.bin += count;
	/* ignore the excessive part of a too long line */
	if (wasnonl)
		return;
	me.uplinkpong = CURRTIME;
	if (parsebuf[count - 1] == '\n')
		count--;
	if (count > 0 && parsebuf[count - 1] == '\r')
		count--;
	parsebuf[count] = '\0';
	parse(parsebuf);
}

static void ping_uplink(void *arg)
{
	unsigned int diff;

	if (me.connected)
	{
		ping_sts();

		diff = CURRTIME - me.uplinkpong;

		if (diff >= 600)
		{
			slog(LG_INFO, "ping_uplink(): uplink appears to be dead, disconnecting");
			sts("ERROR :Closing Link: 127.0.0.1 (Ping timeout: %d seconds)", diff);
			sendq_flush(curr_uplink->conn);
			if (me.connected)
			{
				errno = 0;
				connection_close(curr_uplink->conn);
			}
		}
	}

	if (!me.connected)
		event_delete(ping_uplink, NULL);
}

static void irc_handle_connect(void *vptr)
{
	connection_t *cptr = vptr;

	/* add our server */

	if (cptr == curr_uplink->conn)
	{
		cptr->flags = CF_UPLINK;
		cptr->recvq_handler = irc_recvq_handler;
		me.connected = TRUE;
		/* no SERVER message received */
		me.recvsvr = FALSE;

		slog(LG_INFO, "irc_handle_connect(): connection to uplink established");

		server_login();

#ifdef HAVE_GETTIMEOFDAY
		/* start our burst timer */
		s_time(&burstime);
#endif

		/* done bursting by this time... */
		ping_sts();

		/* ping our uplink every 5 minutes */
		event_delete(ping_uplink, NULL);
		event_add("ping_uplink", ping_uplink, NULL, 300);
		me.uplinkpong = time(NULL);
	}
}

void init_ircpacket(void)
{
	hook_add_event("connected");
	hook_add_hook("connected", irc_handle_connect);
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
