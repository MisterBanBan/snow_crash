#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>

typedef union {
    uint8_t raw[8];
    struct {
        uint8_t  version : 3;
        uint8_t  state   : 2;
        uint8_t  cmd     : 3;
        uint32_t length;          /* payload length, little-endian  */
        uint16_t checksum;        /* simple XOR-16 over payload      */
    } __attribute__((packed)) fields;
} msg_header_t;

#define CMD_HELLO 0
#define CMD_ADMIN 2
#define CMD_QUIT  3

#define SOCK_PATH "/run/blacksun/blacksun.sock"
#define PROTOCOL_VER 1
#define MAGIC_HELLO 0xDEADU //Hello command payload

//Same checksum function as blacksun.c
static __attribute__((always_inline)) inline uint16_t
compute_checksum(const uint8_t * restrict buf, uint32_t len)
{
    uint16_t acc = 0;
    for (uint32_t i = 0; i < len; i++)
        acc ^= (uint16_t)buf[i] ^ (uint16_t)(i & 0xFFu);
    return acc;
}

int send_hello_cmd(int socket_fd)
{
	//Build header and send command with the hello payload.
	uint8_t buf[sizeof(msg_header_t) + 2] = {0};
	msg_header_t header = {0};
	uint16_t payload = MAGIC_HELLO;

	header.fields.cmd = CMD_HELLO;
	header.fields.version = PROTOCOL_VER;
	header.fields.length = 2;
	header.fields.checksum = compute_checksum((void*)&payload, 2);
	memcpy(buf, &header, sizeof(header));
	memcpy(buf + sizeof(msg_header_t), &payload, 2);
	if (write(socket_fd, &buf, sizeof(msg_header_t) + 2) == -1)
		return (1);
	return (2);
}

int send_cmd(int socket_fd, uint8_t cmd)
{
	//Build header and send command without payload.
	uint8_t buf[sizeof(msg_header_t)] = {0};
	msg_header_t header = {0};

	header.fields.cmd = cmd;
	header.fields.version = PROTOCOL_VER;
	memcpy(buf, &header, sizeof(header));
	if (write(socket_fd, &buf, sizeof(msg_header_t)) == -1)
		return (1);
	return (2);
}

int read_msg(int socket_fd, char* buffer, int buffer_len)
{
	int i = 0;

	//Read one character until a \n.
	while(i < buffer_len - 1)
	{
		if (read(socket_fd, &buffer[i], 1) != 1)
			return (1);
		if (buffer[i] == '\n')
			return (0);
		++i;
	}
	return (1);
}

int send_hello_seq(int socket_fd)
{
	//Send command
	if (send_hello_cmd(socket_fd) == 1)
		return (1);

	//Read answer.
	char buf[1024] = {0};
	if (read_msg(socket_fd, buf, 1024) == 1)
		return (1);
	printf("%s\n", buf);

	//Check if an error was sent.
	if (strcmp(buf, "ERR\n") == 0)
		return (1);
	return (0);
}

int send_admin_seq(int socket_fd)
{
	//Send admin command.
	if (send_cmd(socket_fd, CMD_ADMIN) == 1)
		return (1);

	//Read the first part of answer (either ACCESS GRANTED of error).
	char buf[1024] = {0};
	memset(buf, 0, sizeof(buf));
	if (read_msg(socket_fd, buf, 1024) == 1)
		return (1);
	printf("%s\n", buf);

	//Check if an error was sent.
	if (strcmp(buf, "ERR\n") == 0)
		return (1);
	memset(buf, 0, sizeof(buf));

	//Read second part of the answer (the flag).
	if (read_msg(socket_fd, buf, 1024) == 1)
		return (1);
	printf("%s\n", buf);
	return (0);
}

int main(void)
{
	int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_fd < 0)
		return (1);

	struct sockaddr_un addr = {0};
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

	//Connect to the blacksun socket.
	if (connect(socket_fd, (struct sockaddr*)&addr,sizeof(addr)) != 0)
	{
		close(socket_fd);
		return (1);
	}
	
	//Send hello command.
	if (send_hello_seq(socket_fd) == 1)
	{
		close(socket_fd);
		return (1);
	}

	//Send admin command.
	if (send_admin_seq(socket_fd) == 1)
	{
		close(socket_fd);
		return (1);
	}
	
	//Send quit command.
	if (send_cmd(socket_fd, CMD_QUIT) == 1)
	{
		close(socket_fd);
		return (1);
	}
	close(socket_fd);
	return (0);
}
