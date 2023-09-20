/*
** server.c - a stream socket server demo
*/#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "message.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb.h"
#include "view.h"
#include "basic_types/list.h"
#include "ui/console_tools.h"

#define BUFFER_SIZE 1024

#define PORT "3490"  // порт, на который будут приходить соединения
#define BACKLOG 10     // как много может быть ожидающих соединений

void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// получаем адрес сокета, ipv4 или ipv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

Collection get_collection(struct result_list_tuple *list){
    Collection collection = {};
    Tuple tuple = {};
    while (list != NULL) {
        collection.tuples[collection.tuples_count] = tuple;
        collection.tuples[collection.tuples_count].id = list->id;
        collection.tuples[collection.tuples_count].parent_id = list->value->header.parent;
        collection.tuples[collection.tuples_count].data.values_count = 2;
        collection.tuples[collection.tuples_count].data.values[0].type = STRING_TYPE;
        strcpy(collection.tuples[collection.tuples_count].data.values[0].values.str, (char *)list->value->data[0]);
        collection.tuples[collection.tuples_count].data.values[1].type = INTEGER_TYPE;
        collection.tuples[collection.tuples_count].data.values[1].values.integer = list->value->data[1];
        collection.tuples_count++;
        list = list->next;
    }
    return collection;
}

void do_connection(int new_fd, char *filename) {

    Query_tree tree;
    Collection collection;
    Return_code returnCode = {52};
    pb_ostream_t ostream;
    pb_istream_t istream;
    uint8_t output_buffer[BUFFER_SIZE];
    uint8_t input_buffer[BUFFER_SIZE];

    if ((recv(new_fd, input_buffer, BUFFER_SIZE, 0)) == -1) {
        perror("server: couldn't receive message\n");
    }

    istream = pb_istream_from_buffer(input_buffer, sizeof(input_buffer));
    pb_decode(&istream, Query_tree_fields, &tree);

    struct result_list_tuple *list;

    uint64_t code = parse_request(filename, tree, &list);

    if (tree.command == CRUD_FIND) {
        collection = get_collection(list);
        ostream = pb_ostream_from_buffer(output_buffer, sizeof(output_buffer));
        pb_encode(&ostream, Collection_fields, &collection);
    } else {
        returnCode.code = code;
        ostream = pb_ostream_from_buffer(output_buffer, sizeof(output_buffer));
        pb_encode(&ostream, Return_code_fields, &returnCode);
    }

    if (send(new_fd, output_buffer, BUFFER_SIZE, 0) == -1) {
        perror("server: couldn't send message\n");
    }


}



int main(int argc, char *argv[]) {
    int sockfd, new_fd;  // слушаем на sock_fd, новые соединения - на new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // информация об адресе клиента
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server:socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server:bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server:failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // всё, что можно, с этой структурой мы сделали

    if (listen(sockfd, BACKLOG) == -1) {
        perror("server: listen\n");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("server: sigaction\n");
        exit(1);
    }

    printf("server: waiting for connections…\n");

    while (1) {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) continue;

        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *) &their_addr),s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) {
            close(sockfd);
            do_connection(new_fd, argv[2]);
            close(new_fd);
            exit(1);
        }
        close(new_fd);
    }
}