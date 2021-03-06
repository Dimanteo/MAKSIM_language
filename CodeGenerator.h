#ifndef PROGRAMMING_LANGUAGE_CODEGENERATOR_H
#define PROGRAMMING_LANGUAGE_CODEGENERATOR_H


#include "My_Headers/txt_files.h"
#include "Tree_t/Tree.cpp"
#include "Node.h"
#include "Commons.h"
#include "Derivative.h"
#include "Maksim.h"

#define VERIFY_CONTEXT __FILE__, __PRETTY_FUNCTION__, __LINE__

const size_t MAX_STRING_SIZE     = 40;
const size_t MAX_VARIABLES_COUNT = 2000;
const size_t MAX_FUNCTION_COUNT  = 2000;
const int    NOT_SET             = -2;
int          RAM_POINTER         = 0;
int          GLOBAL              = -1;
const char   BASE_REG[]          = "bx";
const char   ADRESS_REG[]        = "ax";
int          CONDITION_COUNTER   = 0;
int          CYCLE_COUNTER       = 0;
int          STACK_FRAME         = 0;

//Переменные
struct Variable {
    char* ID     = nullptr;  //имя
    int   adress = NOT_SET;  //адрес в RAM
    int   level  = NOT_SET;  //область видимости переменной
};
Variable VARIABLES[MAX_VARIABLES_COUNT];
int VARIABLE_COUNT = 0;

//Функции
struct Function {
    char* ID   = nullptr;  //имя
    int   varc = 0;        //количество локальных переменных
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
            fprintf(file, "%s RAM[%d] lvl[%s]",
                    VARIABLES[value.code].ID,
                    VARIABLES[value.code].adress,
                    (VARIABLES[value.code].level == GLOBAL) ? "GLOBAL" : FUNCTIONS[VARIABLES[value.code].level].ID);
            break;
        case Node::OPERATION_TYPE:
            fprintf(file, "%s varc[%d]", FUNCTIONS[value.code].ID, FUNCTIONS[value.code].varc);
            break;
        case Node::SPECIAL_SYMBOLS:
            fprintf(file, "%s", STD_ObjList[value.code].ID);
    }
}

template<>
void Tree<Node>::valueDestruct() {
    value.~Node();
}

Node makeNode(char *value, Tree<Node> *node);

Tree<Node>* parseToAST(char** ast, Tree<Node>* node);

int makeSTDNode(char* value);

void translate(FILE* assembler, Tree<Node>* node);

void functionHandler(FILE* assembler, Tree<Node>* node);

void variableHandler(FILE* assembler, Tree<Node>* node);

void specialSymbolHandler(FILE* assembler, Tree<Node>* node);

void makeAssembler(const char filename[], Tree<Node>* tree);

void makeDump(const char textDump[], const char graphDump[], Tree<Node>* tree);

Tree<Node> * makeAST(const char input[]);


///////////////////////DEFINITIONS///////////////////////////

//TODO standart dump
/*template <>
void Tree<Node>::genDot(Tree<Node> *node, FILE *file) {
    assert(file);
    assert(node);

    switch (node->getValue().type) {

        case Node::NUMBER_TYPE:
            fprintf(file, "\"%p\" [label=\"%g\","
                         "color=darkseagreen2,"
                         "style=filled,"
                         "shape=egg,"
                         "fontname=bold,"
                         "fontsize=20]\n", node, node->getValue().num);
            break;

        case Node::VARIABLE_TYPE:
            fprintf(file, "\"%p\" [label=\"%s\","
                          "color=darkolivegreen1,"
                          "shape=egg,"
                          "style=filled,"
                         "fontsize=20]\n", node, VARIABLES[node->getValue().code].ID);
            break;

        case Node::SPECIAL_SYMBOLS : {

            char shape[20] = "diamond";
            char color[20] = "gold";
            char style[20] = "filled";

            char shape1[20] = "pentagon";
            char color1[20] = "bisque";
            char style1[20] = "filled";
            switch (node->getValue().code) {

                case ADDITION:
                    strcpy(color, "gold");
                    fprintf(file, "\"%p\" [label=\"%c\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fixedsize=true"
                                  "fontname=bold"
                                  "fontsize=15]\n" , node, node->getValue().code, color, shape, style);
                    break;

                case MINUS:
                    strcpy(color, "orchid2");
                    fprintf(file, "\"%p\" [label=\"%c\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fixedsize=true"
                                  "fontname=bold"
                                  "fontsize=15]\n" , node, node->getValue().code, color, shape, style);
                    break;

                case MULTIPLY:
                    strcpy(color, "pink");
                    fprintf(file, "\"%p\" [label=\"%c\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fixedsize=true"
                                  "fontname=bold"
                                  "fontsize=15]\n" , node, node->getValue().code, color, shape, style);
                    break;

                case DIVIDE:
                    strcpy(color, "peachpuff");
                    fprintf(file, "\"%p\" [label=\"%c\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fixedsize=true"
                                  "fontname=bold"
                                  "fontsize=15]\n" , node, node->getValue().code, color, shape, style);
                    break;

                case POWER:
                    strcpy(color, "ivory");
                    fprintf(file, "\"%p\" [label=\"%c\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fixedsize=true"
                                  "fontname=bold"
                                  "fontsize=15]\n" , node, node->getValue().code, color, shape, style);
                    break;

                case DERIV:
                    strcpy(color, "gray78");
                    fprintf(file, "\"%p\" [label=\"%s\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fixedsize=true"
                                  "fontname=bold"
                                  "fontsize=15]\n" , node, Language_CMD[node->getValue().code], color, shape, style);
                    break;

                case WHILE:
                    strcpy(shape1, "pentagon");
                    fprintf(file, "\"%p\" [label=\"%s\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fontname=bold]\n", node, Language_CMD[node->getValue().code], color1, shape1, style1);
                    break;

                case IF:
                    strcpy(shape1, "hexagon");
                    fprintf(file, "\"%p\" [label=\"%s\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fontname=bold]\n", node, Language_CMD[node->getValue().code], color1, shape1, style1);
                    break;

                case IF_ELSE:
                    strcpy(shape1, "hexagon");
                    fprintf(file, "\"%p\" [label=\"%s\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fontname=bold]\n", node, Language_CMD[node->getValue().code], color1, shape1, style1);
                    break;

                case RET:
                    strcpy(shape1, "septagon");
                    fprintf(file, "\"%p\" [label=\"%s\","
                                  "color=%s,"
                                  "shape=%s,"
                                  "style=%s,"
                                  "fontname=bold]\n", node, Language_CMD[node->getValue().code], color1, shape1, style1);
                    break;

                case ASSIGN: {
                    char shape[20] = "circle";
                    strcpy(shape, "larrow");
                    fprintf(file, "\"%p\" [label=\"%c\","
                                  "color=black,"
                                  "style=bold,"
                                  "shape=%s,"
                                  "fontname=bold]\n", node, node->getValue().code, shape);
                    break;
                }

                default:
                    strcpy(shape, "parallelogram");
                    strcpy(color, "gray56");
            }

        }
            break;

        case Node::OPERATION_TYPE: {
            fprintf(file, "\"%p\" [label=\"%s\","
                          "color=bisque,"
                          "shape=box,"
                          "style=filled,"     \
                         "fontname=bold]\n", node, FUNCTIONS[node->getValue().code].ID);
        }
            break;
    }
}*/


void makeAssembler(const char *filename, Tree<Node> *tree) {
    FILE* assembler = fopen(filename, "wb");
    time_t now = time(nullptr);
    fprintf(assembler, ";<--This is auto-generated file-->\n"
                       ";Generated %s\n"
                       "\tpush %d\n"
                       "\tpop %s\n"
                       "\tcall $main\n"
                       "end\n",
            ctime(&now), RAM_POINTER, BASE_REG);
    translate(assembler, tree);
    fclose(assembler);
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

Node makeNode(char *value, Tree<Node> *node) {
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
    if (node->getParent()->getValue().type == Node::SPECIAL_SYMBOLS && node->getParent()->getValue().code == DOT_COMA) {
        for (int i = 0; i < VARIABLE_COUNT; ++i) {
            if (strcmp(value, VARIABLES[i].ID) == 0 && VARIABLES[i].level == GLOBAL)
                return Node(Node::VARIABLE_TYPE, i);
        }
        VARIABLES[VARIABLE_COUNT] = {strdup(value), RAM_POINTER++, GLOBAL};
        return Node(Node::VARIABLE_TYPE, VARIABLE_COUNT++);
    } else {
        Tree<Node>* function = node->getParent();
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

Tree<Node>* parseToAST(char** ast, Tree<Node>* node) {
    assert(ast);
    char buffer[MAX_STRING_SIZE] = "";
    int length = 0;
    sscanf(*ast, "{%[^{}@]%n", buffer, &length);
    char* value = strdup(buffer);
    assert(node);
    node->setValue(makeNode(value, node));

    //{v{}{}} {v@@} {v{}@} {v@{}}
    (*ast) += length;
    while (**ast != '@' && **ast != '{' && **ast != '}') {
        (*ast)++;
    }
    switch (**ast) {
        case '{':
            node->growChild(LEFT_CHILD, Node());
            parseToAST(ast, node->getChild(LEFT_CHILD));
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
    if (**ast == '{') {
        node->growChild(RIGHT_CHILD, Node());
        parseToAST(ast, node->getChild(RIGHT_CHILD));
    }
    (*ast)++;
    free(value);
    return node;
}

void translate(FILE *assembler, Tree<Node>* node) {
    switch (node->getValue().type) {
        case Node::NUMBER_TYPE:
            fprintf(assembler, "\tpush %lf\n", node->getValue().num);
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

void specialSymbolHandler(FILE *assembler, Tree<Node> *node) {
    switch (node->getValue().code) {
        case RET:
            if (!node->childIsEmpty(LEFT_CHILD)) {
                fprintf(assembler, ";<--Return values-->\n");
                translate(assembler, node->getChild(LEFT_CHILD));
            }
            fprintf(assembler, ";Swap return adress and values\n"
                               "\tpop cx\n"
                               "\tpop dx\n"
                               "\tpush cx\n"
                               "\tpush dx\n"
                               "ret\n");
            break;
        case DOT_COMA:
        case OP:
            if (!node->childIsEmpty(LEFT_CHILD))
                translate(assembler, node->getChild(LEFT_CHILD));
            if (!node->childIsEmpty(RIGHT_CHILD))
                translate(assembler, node->getChild(RIGHT_CHILD));
            break;
        case PUT:
            translate(assembler, node->getChild(LEFT_CHILD));
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

                if (!node->getChild(RIGHT_CHILD)->childIsEmpty(RIGHT_CHILD))
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
            fprintf(assembler, "cycle_%d:\n", cycle_number);
            translate(assembler, node->getChild(RIGHT_CHILD));
            translate(assembler, node->getChild(LEFT_CHILD));
            fprintf(assembler, " cycle_%d\n", cycle_number);
            break;
        }

        postorder(MORE,      "\tjb")
        postorder(LESS,      "\tja")
        postorder(MORE_EQUAL,"\tjbe")
        postorder(LESS_EQUAL,"\tjae")
        postorder(EQUAL,     "\tje")
        postorder(NOT_EQUAL, "\tjne")
        postorder(ADDITION,  "\tadd\n")
        postorder(MINUS,     "\tsub\n")
        postorder(DIVIDE,    "\tdiv\n")
        postorder(MULTIPLY,  "\tmul\n")
        postorder(POWER,     "\tpow\n")

        case DERIV: {
            Differentiator dif(node->getChild(LEFT_CHILD)->getValue().code);

            Tree<Node>* copy = node->getChild(RIGHT_CHILD)->copySubtree();

            Tree<Node>* result = dif.diff(copy);

            delete(copy);

            dif.optimization(result);

            node->removeSubTree(RIGHT_CHILD);
            node->connectSubtree(RIGHT_CHILD, result);

            translate(assembler, node->getChild(RIGHT_CHILD));

            break;
        }
    }
}

void functionHandler(FILE *assembler, Tree<Node> *node) {
    Tree<Node>* node_copy = node;

    if (node->getParent()->getValue().type == Node::SPECIAL_SYMBOLS
        && node->getParent()->getValue().code == DOT_COMA) {

        fprintf(assembler, "%s:\n"
                           "\tpop %s; Save call adress to register\n"
                           ";<--Parameters initialization-->\n",
                FUNCTIONS[node->getValue().code].ID, ADRESS_REG);

        STACK_FRAME = FUNCTIONS[node->getValue().code].varc;

        while (!node_copy->childIsEmpty(LEFT_CHILD)) {
            assert(it_is(LEFT_CHILD, Node::SPECIAL_SYMBOLS, COMA_PARAMETER));

            node_copy = node_copy->getChild(LEFT_CHILD);

            assert(node_copy->getChild(RIGHT_CHILD)->getValue().type == Node::VARIABLE_TYPE);

            fprintf(assembler, "\tpop [%s + %d]\n", BASE_REG, VARIABLES[node_copy->getChild(RIGHT_CHILD)->getValue().code].adress);
        }

        fprintf(assembler, "\tpush %s; Push call adress to stack\n"
                           ";<--Body-->\n",
                ADRESS_REG);

        translate(assembler, node->getChild(RIGHT_CHILD));

        fprintf(assembler, "ret\n");
    } else {

        while (!node_copy->childIsEmpty(LEFT_CHILD)) {
            assert(it_is(LEFT_CHILD, Node::SPECIAL_SYMBOLS, COMA_PARAMETER));

            node_copy = node_copy->getChild(LEFT_CHILD);

            translate(assembler, node_copy->getChild(RIGHT_CHILD));
        }

        fprintf(assembler, ";<--RAM memory allocate-->\n"
                           "\tpush %s\n"
                           "\tpush %d\n"
                           "\tadd\n"
                           "\tpop %s\n"
                           "\tcall %s\n"
                           ";<--RAM memory deallocate-->\n"
                           "\tpush %s\n"
                           "\tpush %d\n"
                           "\tsub\n"
                           "\tpop %s\n",
                BASE_REG,
                STACK_FRAME,
                BASE_REG,
                FUNCTIONS[node->getValue().code].ID,
                BASE_REG,
                STACK_FRAME,
                BASE_REG);
    }
}

void variableHandler(FILE *assembler, Tree<Node> *node) {
    Variable* var = &VARIABLES[node->getValue().code];

    if (node->getParent()->getValue().type == Node::SPECIAL_SYMBOLS
        && (node->getParent()->getValue().code == ASSIGN || node->getParent()->getValue().code == GET)) {

        if (var->level == GLOBAL) {

            fprintf(assembler, "\tpop [%d]\n", var->adress);

        } else {
            fprintf(assembler, "\tpop [%s + %d]\n", BASE_REG, var->adress);
        }
    } else {
        if (var->level == GLOBAL) {
            fprintf(assembler, "\tpush [%d]\n", var->adress);
        } else {
            fprintf(assembler, "\tpush [%s + %d]\n", BASE_REG, var->adress);
        }
    }
}

void makeDump(const char *textDump, const char *graphDump, Tree<Node> *tree) {
    FILE* dump = fopen(textDump, "wb");

    Tree<Node>** sequence = tree->allocTree();

    tree->inorder(sequence);

    tree->treeDump(dump, OK_STATE, "main", VERIFY_CONTEXT, sequence);

    tree->graphDump(graphDump, sequence);

    free(sequence);
    fclose(dump);
}

Tree<Node> * makeAST(const char *input) {
    size_t size = 0;

    char* data = read_file_to_buffer_alloc(input, "rb", &size);

    char* data_copy = data;

    Tree<Node>* tree = new Tree<Node>(Node());

    parseToAST(&data_copy, tree);

    assert(tree);
    free(data);

    return tree;
}

#undef postorder
#undef it_is

#endif //PROGRAMMING_LANGUAGE_CODEGENERATOR_H
