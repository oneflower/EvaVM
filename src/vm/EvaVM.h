#ifndef __EvaVM_h
#define __EvaVM_h

#include <string>
#include <vector>
#include <array>

#include "../Logger.h"
#include "../bytecode/Opcode.h"
#include "EvaValue.h"

/**
 * Reads the current byte in the bytecode
 * and advances the ip pointer.
 */
#define READ_BYTE() *ip++

#define GET_CONST() constants[READ_BYTE()]
#define STACK_LIMIT 512

#define BINARY_OP(op) \
  do { \
    auto op2 = AS_NUMBER(pop()); \
    auto op1 = AS_NUMBER(pop()); \
    push(NUMBER(op1 op op2)); \
  } while (false)


class EvaVM {
    public:
    EvaVM() {}

    void push(const EvaValue& value) {
        if ((sp - stack.begin()) == STACK_LIMIT) {
            DIE << "push(): Stack overflow.\n";
        }
        *sp = value;
        sp++;
    }

    EvaValue pop() {
        if (stack.size() == 0) {
            DIE << "pop: empty stack.\n";
        }
        --sp;
        return *sp;
    }

    EvaValue exec(const std::string& program) {
        // 1. Parse the program
        // auto ast = parser->parse(program);

        // 2. Compile program to Eva bytecode
        // code = compiler->compiler(ast);

        // constants.push_back(NUMBER(10));
        // constants.push_back(NUMBER(3));
        // constants.push_back(NUMBER(10));

        constants.push_back(ALLOC_STRING("Hello, "));
        constants.push_back(ALLOC_STRING("world!"));

        // (- (* 10 3) 10)
        // code = {OP_CONST, 0, OP_CONST, 1, OP_MULT, OP_CONST, 2, OP_SUB, OP_HALT};

        // (+ "hello" "world")
        code = {OP_CONST, 0, OP_CONST, 1, OP_ADD, OP_HALT};
        ip = &code[0];

        // Init the stack:
        sp = &stack[0];

        return eval();
    }

    EvaValue eval() {
        for (;;) {
            auto opcode = READ_BYTE();
            //log(opcode);
            switch (opcode) {
                case OP_HALT:
                  return pop();

                case OP_CONST:
                  push(GET_CONST());
                  break;

                case OP_ADD: {
                  auto op2 = pop();
                  auto op1 = pop();

                  if (IS_NUMBER(op1) && IS_NUMBER(op2)) {
                      auto v1 = AS_NUMBER(op1);
                      auto v2 = AS_NUMBER(op2);
                      push(NUMBER(v1 + v2));
                  }

                  else if (IS_STRING(op1) && IS_STRING(op2)) {
                      auto s1 = AS_CPPSTRING(op1);
                      auto s2 = AS_CPPSTRING(op2);
                      push(ALLOC_STRING(s1 + s2));
                  }

                  break;
                }

                case OP_SUB: {
                  BINARY_OP(-);
                  break;
                }

                case OP_MULT: {
                  BINARY_OP(*);
                  break;
                }

                case OP_DIV: {
                  BINARY_OP(/);
                  break;
                }
                  
                default:
                  DIE << "Unknown opcode: " << std::hex << opcode;
            }
        }
    }

    uint8_t* ip;
    EvaValue* sp;

    std::array<EvaValue, STACK_LIMIT> stack;

    std::vector<EvaValue> constants;

    std::vector<uint8_t> code;
};

#endif