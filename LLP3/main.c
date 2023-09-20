#include <stdio.h>
#include "message.pb.h"
#include <pb_encode.h>
#include <pb_decode.h>
#include <inttypes.h>
#include "XPATHParser.h"
#include "configuration.h"

#define BUFFER_SIZE 10

int main() {
    parse();
    return 0;
}
