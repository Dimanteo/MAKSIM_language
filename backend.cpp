#include <mem.h>
#include "My_Headers/txt_files.h"
#include "Tree_t/Tree.cpp"
#include "Node.h"
#include "Commons.h"

#define VERIFY_CONTEXT __FILE__, __PRETTY_FUNCTION__, __LINE__

const size_t MAX_STRING_SIZE = 40;
const size_t MAX_VARIABLES_COUNT = 2000;
const size_t MAX_FUNCTION_COUNT = 2000;
const int NOT_SET = -2;
int RAM_POINTER = 0;
int GLOBAL = -1;
const char BASE_REG[] = "bx";

//Переменные
struct Variable {
    char* ID = nullptr;   //имя
    int adress = NOT_SET; //адрес в RAM
    int level = NOT_SET;       //область видимости переменной
};
Variable VARIABLES[MAX_VARIABLES_COUNT];
int VARIABLE_COUNT = 0;

//Функции
struct Function {
    char* ID = nullptr;
    int varc = 0;
};
Function FUNCTIONS[MAX_FUNCTION_COUNT];
int FUNCTION_COUNT = 0;

template <>
void Tree<Node>::valuePrint(FILE *file) {
    switch (value.type) {
        case Node::NUMBER_TYPE:
            fprintf(file, "%lf", value.num);
            break;
        case Node::VARIABLE_TYPE:
            fprintf(file, "%s RAM[%d] lvl[%d]", VARIABLES[value.code].ID, VARIABLES[value.code].adress, VARIABLES[value.code].level);
            break;
        case Node::OPERATION_TYPE:
            fprintf(file, "%s varc[%d]", FUNCTIONS[value.code].ID, FUNCTIONS[value.code].varc);
            break;
        case Node::SPECIAL_SYMBOLS:
            fprintf(file, "%s", STD_ObjList[value.code].ID);
    }
}


Node makeNode(char *value, Tree<Node> *parent);

Tree<Node>* parseToAST(char** ast, Tree<Node>* parent);

int makeSTDNode(char* value);

void translate(FILE* assembler, Tree<Node>* node);

void functionHandler(FILE* assembler, Tree<Node>* node);

void variableHandler(FILE* assembler, Tree<Node>* node);

void specialSymbolHandler(FILE* assembler, Tree<Node>* node);

void makeAssembler(const char filename[], Tree<Node>* tree);

int main() {
    size_t size = 0;
    char* data = read_file_to_buffer_alloc("../Maksim.txt", "rb", &size);
    char* data_copy = data;
    Tree<Node>* tree = parseToAST(&data_copy, nullptr);
    printf("data: %s", data_copy);
    assert(tree);
    free(data);

    FILE* dump = fopen("Tree.log", "wb");
    Tree<Node>** sequence = tree->allocTree();
    tree->inorder(sequence);
    tree->treeDump(dump, OK_STATE, "main", VERIFY_CONTEXT, sequence);
    tree->graphDump("AST.png", sequence);

    free(sequence);
    fclose(dump);

    for (int i = 0; i < FUNCTION_COUNT; ++i) {
        free(FUNCTIONS[i].ID);
    }
    for (int i = 0; i < VARIABLE_COUNT; ++i) {
        free(VARIABLES[i].ID);
    }
}

void makeAssembler(const char *filename, Tree<Node> *tree) {
    FILE* assembler = fopen(filename, "wb");
    fprintf(assembler, "\tpush %d\n"
                       "\tpop %s\n"
                       "jmp $main", RAM_POINTER, BASE_REG);//TODO makeAsm. END point, and pretty header
}

int makeSTDNode(char *value) {
    assert(value);
    for (int i = 0; i < STD_FUNC_COUNT; ++i) {
        if (strcmp(value, STD_ObjList[i].ID) == 0) {
            return STD_ObjList[i].code;
        }
    }
    return -1;
}

Node makeNode(char *value, Tree<Node> *parent) {
    assert(value);
    int code = 0;
    if ((code = makeSTDNode(value)) != -1) {
        return Node(Node::SPECIAL_SYMBOLS, code);
    }
    if (char* name = strchr(value, '$')) {
        for (int i = 0; i < FUNCTION_COUNT; ++i) {
            if (strcmp(value, FUNCTIONS[i].ID) == 0)
                return Node(Node::OPERATION_TYPE, i);
        }
        FUNCTIONS[FUNCTION_COUNT].ID = strdup(name);
        return Node(Node::OPERATION_TYPE, FUNCTION_COUNT++);
    }
    double number = 0;
    if (sscanf(value, "%lf", &number)) {
        return Node(number);
    }

    //if value is Variable
    if (parent->getParent()->getValue().type == Node::SPECIAL_SYMBOLS && parent->getParent()->getValue().code == DOT_COMA) {
        for (int i = 0; i < VARIABLE_COUNT; ++i) {
            if (strcmp(value, VARIABLES[i].ID) == 0 && VARIABLES[i].level == GLOBAL)
                return Node(Node::VARIABLE_TYPE, i);
        }
        VARIABLES[VARIABLE_COUNT] = {strdup(value), RAM_POINTER++, GLOBAL};
        return Node(Node::VARIABLE_TYPE, VARIABLE_COUNT++);
    } else {
        Tree<Node>* function = parent;
        while (function->getValue().type != Node::OPERATION_TYPE) {
            function = function->getParent();
        }
        int current_level = function->getValue().code;
        for (int i = 0; i < VARIABLE_COUNT; ++i) {
            if (VARIABLES[i].level == current_level && strcmp(VARIABLES[i].ID, value) == 0) {
                return Node(Node::VARIABLE_TYPE, i);
            } else if (VARIABLES[i].level == GLOBAL && strcmp(VARIABLES[i].ID, value) == 0) {
                return Node(Node::VARIABLE_TYPE, i);
            }
        }
        VARIABLES[VARIABLE_COUNT] = {strdup(value), FUNCTIONS[current_level].varc, current_level};
        FUNCTIONS[current_level].varc++;
        return Node(Node::VARIABLE_TYPE, VARIABLE_COUNT++);
    }
}

Tree<Node>* parseToAST(char** ast, Tree<Node>* parent) {
    assert(ast);
    char buffer[MAX_STRING_SIZE] = "";
    int length = 0;
    sscanf(*ast, "{%[^{}@]%n", buffer, &length);
    printf("%s\n", buffer);
    char* value = strdup(buffer);
    Tree<Node>* node = new Tree<Node>(makeNode(value, parent));

    //{v{}{}} {v@@} {v{}@} {v@{}}
    (*ast) += length;
    while (**ast != '@' && **ast != '{' && **ast != '}') {
        (*ast)++;
    }
    switch (**ast) {
        case '{':
            node->connectSubtree(LEFT_CHILD, parseToAST(ast, node));
            break;
        case '}':
            (*ast)++;
            free(value);
            return node;
        case '@':
            (*ast)++;
            break;
    }
    while (**ast != '{' && **ast != '}') {
        (*ast)++;
    }
    if (**ast == '{')
        node->connectSubtree(RIGHT_CHILD, parseToAST(ast, node));
    (*ast)++;
    free(value);
    return node;
}

void translate(FILE *assembler, Tree<Node>* node) {
    switch (node->getValue().type) {
        case Node::NUMBER_TYPE:
            fprintf(assembler, "push %lf\n", node->getValue().num);
            break;
        case Node::VARIABLE_TYPE:
            variableHandler(assembler, node);
            break;
        case Node::OPERATION_TYPE:
            functionHandler(assembler, node);
            break;
        case Node::SPECIAL_SYMBOLS:
            specialSymbolHandler(assembler, node);
            break;
    }
}


#define postorder(code, token)\
    case code:\
        translate(assembler, node->getChild(LEFT_CHILD));\
        translate(assembler, node->getChild(RIGHT_CHILD));\
        fprintf(assembler, token);\
        break;

#define it_is(CHILD, TYPE, CODE) node->getChild(CHILD)->getValue().type == TYPE && node->getChild(CHILD)->getValue().code == CODE

int CONDITION_COUNTER = 0;
int CYCLE_COUNTER = 0;

void specialSymbolHandler(FILE *assembler, Tree<Node> *node) {
    switch (node->getValue().code) {
        case RET:
            if (!node->childIsEmpty(LEFT_CHILD)) {
                translate(assembler, node->getChild(LEFT_CHILD));
            }
            fprintf(assembler, "ret\n");
            break;
        case DOT_COMA:
        case OP:
            if (!node->childIsEmpty(LEFT_CHILD))
                translate(assembler, node->getChild(LEFT_CHILD));
            if (!node->childIsEmpty(RIGHT_CHILD))
                translate(assembler, node->getChild(RIGHT_CHILD));
            break;
        case PUT:
            fprintf(assembler, "\tout\n");
            break;
        case GET:
            fprintf(assembler, "\tin\n");
            if (!node->childIsEmpty(LEFT_CHILD))
                translate(assembler, node->getChild(LEFT_CHILD));
            break;
        case ASSIGN:
            translate(assembler, node->getChild(RIGHT_CHILD));
            translate(assembler, node->getChild(LEFT_CHILD));
            break;
        case IF: {
            int if_number = CONDITION_COUNTER++;
            translate(assembler, node->getChild(LEFT_CHILD));
            fprintf(assembler, " if_%d\n", if_number);
            if (it_is(RIGHT_CHILD, Node::SPECIAL_SYMBOLS, IF_ELSE)) {
                translate(assembler, node->getChild(RIGHT_CHILD)->getChild(RIGHT_CHILD));
                fprintf(assembler, "\tjmp if_%d_end\nif_%d:\n", if_number, if_number);
                translate(assembler, node->getChild(RIGHT_CHILD)->getChild(LEFT_CHILD));
            } else {
                fprintf(assembler, "\tjmp if_%d_end\nif_%d:\n", if_number, if_number);
                translate(assembler, node->getChild(RIGHT_CHILD));
            }
            fprintf(assembler, "if_%d_end:\n", if_number);
            break;
        }
        case WHILE: {
            int cycle_number = CYCLE_COUNTER++;
            fprintf(assembler, "cycle_%d:", cycle_number);
            translate(assembler, node->getChild(RIGHT_CHILD));
            translate(assembler, node->getChild(LEFT_CHILD));
            fprintf(assembler, " cycle_%d\n", cycle_number);
            break;
        }
        postorder(MORE, "\tjb\n")
        postorder(LESS, "\tja\n")
        postorder(MORE_EQUAL, "\tjbe\n")
        postorder(LESS_EQUAL, "\tjae\n")
        postorder(EQUAL, "\tje\n")
        postorder(ADDITION, "\tadd\n")
        postorder(MINUS, "\tsub\n")
        postorder(DIVIDE, "\tdiv\n")
        postorder(MULTIPLY, "\tmul\n")
        postorder(POWER, "\tpow\n")
        //TODO дифференциатор
    }
}

void functionHandler(FILE *assembler, Tree<Node> *node) {
    Tree<Node>* node_copy = node;
    if (node->getParent()->getParent()->getValue().type == Node::SPECIAL_SYMBOLS
    && node->getParent()->getParent()->getValue().code == DOT_COMA) {
        fprintf(assembler, "%s:\n"
                           "\tpush %d\n"
                           "\tpush %s\n"
                           "\tadd\n"
                           "\tpop %s\n",
                           FUNCTIONS[node->getValue().code].ID, FUNCTIONS[node->getValue().code].varc, BASE_REG, BASE_REG);
        while (!node_copy->childIsEmpty(LEFT_CHILD)) {
            assert(it_is(LEFT_CHILD, Node::SPECIAL_SYMBOLS, COMA_PARAMETER));
            node_copy = node_copy->getChild(LEFT_CHILD);
            assert(node_copy->getChild(RIGHT_CHILD)->getValue().code == Node::VARIABLE_TYPE);
            fprintf(assembler, "\tpop [%s + %d]\n", BASE_REG, VARIABLES[node_copy->getChild(RIGHT_CHILD)->getValue().code].adress);
        }
        translate(assembler, node->getChild(RIGHT_CHILD));
    } else {
        while (!node_copy->childIsEmpty(LEFT_CHILD)) {
            assert(it_is(LEFT_CHILD, Node::SPECIAL_SYMBOLS, COMA_PARAMETER));
            node_copy = node_copy->getChild(LEFT_CHILD);
            translate(assembler, node_copy->getChild(RIGHT_CHILD));
        }
        fprintf(assembler, "\tcall %s\n"
                           "\tpush %s\n"
                           "\tpush %d\n"
                           "\tsub\n"
                           "\tpop %s\n",
                           FUNCTIONS[node->getValue().code].ID, BASE_REG, FUNCTIONS[node->getValue().code].varc, BASE_REG);
    }
}

void variableHandler(FILE *assembler, Tree<Node> *node) {
    Variable* var = &VARIABLES[node->getValue().code];
    if (node->getParent()->getValue().type == Node::SPECIAL_SYMBOLS
    && (node->getParent()->getValue().code == ASSIGN || node->getParent()->getValue().code == GET)) {
        if (var->level == GLOBAL) {
            fprintf(assembler, "pop [%d]", var->adress);
        } else {
            fprintf(assembler, "pop [%s + %d]", BASE_REG, var->adress);
        }
    } else {
        if (var->level == GLOBAL) {
            fprintf(assembler, "push [%d]", var->adress);
        } else {
            fprintf(assembler, "push [%s + %d]", BASE_REG, var->adress);
        }
    }
}

#undef postorder
#undef it_is