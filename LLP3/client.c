#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "XPATHParser.h"
#include "message.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb.h"
#include "crud_tools/crud.h"

#include <arpa/inet.h>

#define PORT "3490"

#define BUFFER_SIZE 1024

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static int do_connection(char *addr, struct addrinfo hints) {
    int sockfd;
    struct addrinfo *serv_info, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if ((rv = getaddrinfo(addr, PORT, &hints, &serv_info)) != 0) {
        fprintf(stderr, "get address info: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = serv_info; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket\n");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect\n");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(serv_info);

    return sockfd;
}

Query_tree convert_to_message(struct view *view){
    Query_tree tree = {};
    Level level = {};
    Filter filter = {};
    Operator operator = {};
    tree.command = view->crud_operation;
    struct list_level *list_level = view->tree;
    struct filter_list *list_filter;
    struct comparator_list *comparator_list;
    while(list_level != NULL) {
        tree.level[tree.level_count] = level;
        tree.level[tree.level_count].negative = list_level->negative;
        tree.level[tree.level_count].any = list_level->any;
        tree.level[tree.level_count].parent = list_level->place;
        tree.level[tree.level_count].id = list_level->value->element;
        list_filter = list_level->filters;
        while(list_filter != NULL) {
            tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count] = filter;
            tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].negative = list_filter->value->negative;
            comparator_list = list_filter->value->comparators;
            while (comparator_list != NULL) {
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].negative = comparator_list->value->negative;
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].operation = comparator_list->value->operation;
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].is_true = comparator_list->value->is_true;
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op1 = operator;
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op1.field = comparator_list->value->op1->field;
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op1.value.type = comparator_list->value->op1->type;
                switch (comparator_list->value->op1->type){
                    case STRING_TYPE:
                        strcpy(tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op1.value.values.str, comparator_list->value->op1->value.string->value);
                        break;
                    case INTEGER_TYPE: tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op1.value.values.integer = comparator_list->value->op1->value.integer;
                        break;
                    case BOOLEAN_TYPE: tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                               .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op1.value.values.real = comparator_list->value->op1->value.real;
                        break;
                    default: break;
                }
                if (comparator_list->value->op1->field) {
                    strcpy(tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                   .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op1.value.name, comparator_list->value->op1->value.string->value);
                }
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op2 = operator;
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op2.field = comparator_list->value->op2->field;
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                        .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op2.value.type = comparator_list->value->op2->type;
                switch (comparator_list->value->op2->type){
                    case STRING_TYPE:
                        strcpy(tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                       .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op2.value.values.str, comparator_list->value->op2->value.string->value);
                        break;
                    case INTEGER_TYPE: tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                               .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op2.value.values.integer = comparator_list->value->op2->value.integer;
                        break;
                    case BOOLEAN_TYPE: tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                               .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op2.value.values.real = comparator_list->value->op2->value.real;
                        break;
                    default: break;
                }
                if (comparator_list->value->op2->field) {
                    strcpy(tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count]
                                   .comp[tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count].op2.value.name, comparator_list->value->op2->value.string->value);
                }
                tree.level[tree.level_count].filters[tree.level[tree.level_count].filters_count].comp_count++;
                comparator_list = comparator_list->next;
            }
            tree.level[tree.level_count].filters_count++;
            list_filter = list_filter->next;
        }
        tree.level_count++;
        list_level = list_level->next;
    }
    return tree;
}

uint64_t execute(int sockfd) {
    struct view *view;
    Query_tree tree;
    Return_code returnCode;
    Collection collection;
    pb_ostream_t ostream;
    pb_istream_t istream;
    uint8_t output_buffer[BUFFER_SIZE];
    uint8_t input_buffer[BUFFER_SIZE];

    view = parse();
    tree = convert_to_message(view);

    if (tree.command == CRUD_UPDATE) {
        char name[16];
        uint64_t value;
        printf("Enter name: \n");
        scanf("%s", name);
        strcpy(tree.name, name);
        printf("Enter value: \n");
        scanf("%ld", &value);
        tree.value = value;
    }

    ostream = pb_ostream_from_buffer(output_buffer, sizeof(output_buffer));
    pb_encode(&ostream, Query_tree_fields, &tree);

    if (send(sockfd, output_buffer, BUFFER_SIZE, 0) == -1) {
        perror("client: couldn't send message\n");
        close(sockfd);
    }

    if ((recv(sockfd, input_buffer, BUFFER_SIZE, 0)) == -1) {
        perror("client: couldn't receive message\n");
        close(sockfd);
    }
    istream = pb_istream_from_buffer(input_buffer, sizeof(input_buffer));

    switch (tree.command) {
        case CRUD_FIND:
            pb_decode(&istream, Return_code_fields, &collection);
            for(size_t i = 0; i < collection.tuples_count; i++){
                printf("--- TUPLE %d ---\n", collection.tuples[i].id);
                printf("name: %s\n", collection.tuples[i].data.values[0].values.str);
                printf("code: %d\n", collection.tuples[i].data.values[0].values.integer);
            }
            returnCode.code = 0;break;
        default:
            pb_decode(&istream, Return_code_fields, &returnCode); break;
    }

    return returnCode.code;
}

int main(int argc, char *argv[])
{
    int sockfd;
    uint64_t code;
    struct addrinfo hints;

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    while(1) {
        sockfd = do_connection(argv[1], hints);
        printf("client: print your request:\n");
        code = execute(sockfd);
        if (!code) {
            printf("client: successfully executed\n");
        } else {
            printf("client: error %ld was occurred\n", code);
            break;
        }
        close(sockfd);
    }
    return 0;
}