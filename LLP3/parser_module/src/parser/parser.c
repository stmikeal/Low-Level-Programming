#include "parser.h"

enum states next_do(char **string, struct list_level *level) {
    if (**string == '/' && *(*string)++) return S_NAME;
    return S_ERROR;
};

enum states name_do(char **string, struct list_level *level) {
    uint8_t negative = 0, any = 0;
    if (**string == '!' && *(*string)++) negative = 1;
    if (**string == '*' && *(*string)++) any = 1;
    if (!any) {
        if (**string && **string > '0' && **string < '9') {
            char *temp_p = *string;
            size_t len = 0;
            while (temp_p[len] >= '0' && temp_p[len] <= '9') len++;
            temp_p = *string;
            int64_t id = 0;
            for (size_t iter = 0; iter < len; iter++) {
                id = 10 * id + temp_p[iter] - '0';
            }
            level->value = create_list_element(id);
            level->negative = negative;
            *string = temp_p + len;
        } else {
            return S_ERROR;
        }
    } else {
        level->any = 1;
    }
    if (**string == '[' || **string == '!') {
        return S_ATTRIBUTE;
    } else {
        return S_NEXT;
    }

};

struct operator *read_operator(char **string){
    uint8_t number = 0;
    if (**string && **string > '0' && **string < '9') number = 1;
    char *temp_p = *string;
    size_t len = 0;
    struct operator *op;
    if (number) {
        double value = 0;
        uint8_t real = 0;
        while (temp_p[len] && (temp_p[len] >= '0' && temp_p[len] <= '9' || temp_p[len] == '.')) {

            if (temp_p[len] == '.') {
                len++;
                if (real) return NULL;
                real = 1;
                continue;
            }
            if (real) {
                double exp = 0.1;
                for(size_t iter = 1; iter < real; iter++) exp *= 0.1;
                real++;
                value = value +  exp * (temp_p[len] - '0');
            } else {
                value = 10 * value + temp_p[len] - '0';
            }
            len++;
        }
        union types t;
        if (real) {
            t.real = value;
            op = create_operator(!number, FLOAT_TYPE, t);
        } else {
            t.integer = (int64_t) value;
            op = create_operator(!number, INTEGER_TYPE, t);
        }

    } else {
        uint8_t field = 1;
        if (**string == '\'' && *(*string)++) field = 0;
        temp_p = *string;
        while (temp_p[len] && (temp_p[len] >= 'a' && temp_p[len] <= 'z' || temp_p[len] >= 'A' && temp_p[len] <= 'Z')){
            len++;
        }
        char *value = test_malloc(sizeof(char) * len);
        temp_p = *string;
        for (size_t iter = 0; iter < len; iter++){
            value[iter] = temp_p[iter];
        }
        struct field *f = create_field(len, value);
        union types t;
        t.string = f;
        op = create_operator(!number, STRING_TYPE, t);
        op->field = field;
    }
    *string += len;
    return op;
}

struct filter_list *create_wrap_filter_list(uint8_t negative){
    struct filter *f = create_filter(negative);
    struct filter_list *fl = create_filter_list();
    fl->value = f;
    return fl;
}

struct comparator *read_filter(char **string){
    struct comparator *comp = create_comparator();
    if (**string && **string=='!' && *(*string)++) comp->negative = 1;
    if (**string && **string=='@' && *(*string)++) comp->is_true = 1;
    else {
        comp->op1 = read_operator(string);
        comp->operation = (enum compare) *(*string)++;
        comp->op2 = read_operator(string);
    }
    return comp;
}

void append_comp(struct filter_list *fl, struct comparator *comp){
    struct comparator_list *cl = create_comparator_list();
    cl->value = comp;
    cl->next = fl->value->comparators;
    fl->value->comparators = cl;
}

enum states attribute_do(char **string, struct list_level *level) {
    uint8_t negative = 0;
    struct comparator *comp;
    if (**string && **string=='!' && *(*string)++) negative++;
    if (**string =='['){
        struct filter_list *fl = create_wrap_filter_list(negative);
        fl->next = level->filters;
        level->filters = fl;
        while(**string != ']' && *(*string)++) {
            comp = read_filter(string);
            append_comp(level->filters, comp);
            if (**string != '|' && **string != ']' && *(*string)++) return S_ERROR;
        }
        (*string)++;
        if (**string == '[' || **string == '!') return S_ATTRIBUTE;
        else return S_NEXT;
    } else {
        return S_ERROR;
    }

};

enum states error_do(char **string, struct list_level *level) {
    return S_ERROR;
};

enum states (*op_table[4])(char **, struct list_level *) = {
        [S_NEXT] = next_do,
        [S_NAME] = name_do,
        [S_ATTRIBUTE] = attribute_do,
        [S_ERROR] = error_do
};

struct view *parse_operation(char op_char){
    if (op_char != '-' && op_char != '+' && op_char != '?' && op_char != '=') return NULL;
    return create_view(op_char);
}

enum states parse_state(enum states cur_state, char **string, struct list_level *level){
    return op_table[cur_state](string, level);
}
