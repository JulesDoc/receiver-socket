/*
 * fastReceiverUDP.cpp

 *
 *  Created on: Apr 1, 2016
 *      Author: jcalvopi
 */



#define _GNU_SOURCE

#include <iostream>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "protoError.h"
#include "configSocket.h"


#define MTU_SIZE (2048-64*2)
#define MAX_NUM_MSG 512

struct state {
	int sd;

	volatile uint64_t pps;

	struct mmsghdr messages[MAX_NUM_MSG];
	char buffers[MAX_NUM_MSG][MTU_SIZE];
	struct iovec iovecs[MAX_NUM_MSG];


} __attribute__ ((aligned (64)));



struct state *state_init(struct state *s) {

	int i;
	for (i = 0; i < MAX_NUM_MSG; i++) {
		//char *buf = &s->buffers[i][0];

		struct iovec *iovec; // = &s->iovecs[i];
		struct mmsghdr *msg = &s->messages[i];

		msg->msg_hdr.msg_iov = &s->iovecs[i];
		msg->msg_hdr.msg_iovlen = 1;

		iovec->iov_base = &s->buffers[i][0];
		iovec->iov_len = MTU_SIZE;
	}
	return s;
}

static void thread_loop(void *userdata)
{
	struct state *state = userdata;

	while (true) {


		int num_msgs = recvmmsg(state->sd, &state->messages[0], MAX_NUM_MSG, MSG_WAITFORONE, NULL);

		//receive something...
		if (num_msgs <= 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
				continue;
			}
			PFATAL("recvmmsg()");
		}

		int i, bytes = 0;
		for (i = 0; i < num_msgs; i++) {
			struct mmsghdr *msg = &state->messages[i];
			/* char *buf = msg->msg_hdr.msg_iov->iov_base; */
			int len = msg->msg_len;
			msg->msg_hdr.msg_flags = 0;
			msg->msg_len = 0;
			bytes += len;
		}
		__atomic_fetch_add(&state->pps, num_msgs, 0);

	}
}

int main(int argc, const char *argv[])
{
	//localhost
	const char *listen_addr_str = "0.0.0.0:4321";
	int recv_buf_size = 4*1024;

	//better results with only one thread in here
	int thread_num = 1;

	//always 1 since we are receiving for two differents IPs.
	int reuseport = 1;

	struct net_addr listen_addr;
	parse_addr(&listen_addr, listen_addr_str);

	int main_sd = -1;
	if (reuseport == 0) {
		fprintf(stderr, "[*] Starting receiver on %s, recv buffer %iKiB\n",
			addr_to_str(&listen_addr), recv_buf_size / 1024);

		main_sd = net_bind_udp(&listen_addr, 0);
		net_set_buffer_size(main_sd, recv_buf_size, 0);
	}

	struct state *array_of_states = calloc(thread_num, sizeof(struct state));

	int t;
	for (t = 0; t < thread_num; t++) {
		struct state *state = &array_of_states[t];
		state_init(state);
		if (reuseport == 0) {
			state->sd = main_sd;
		} else {
			fprintf(stderr, "[*] Starting receiver on %s, recv buffer %iKiB\n",
				addr_to_str(&listen_addr), recv_buf_size / 1024);

			int sd = net_bind_udp(&listen_addr, 1);
			net_set_buffer_size(sd, recv_buf_size, 0);
			state->sd = sd;
		}
		thread_spawn(thread_loop, state);
	}

	uint64_t last_pps = 0;
	uint64_t last_bps = 0;

	while (true) {
		struct timeval timeout =
			NSEC_TIMEVAL(MSEC_NSEC(1000UL));
		while (1) {
			int r = select(0, NULL, NULL, NULL, &timeout);
			if (r != 0) {
				continue;
			}
			if (TIMEVAL_NSEC(&timeout) == 0) {
				break;
			}
		}

		uint64_t now_pps = 0;
		for (t = 0; t < thread_num; t++) {

			struct state *state = &array_of_states[t];
			now_pps += __atomic_load_n(&state->pps, 0);

		}

		double delta_pps = now_pps - last_pps;
		//double delta_bps = now_bps - last_bps;
		last_pps = now_pps;


		std::cout << "packets per second: " << delta_pps << std::endl;


	}

	return 0;
}
