#pragma once

#include <iostream>
#include <string>

namespace cpu {

struct State {
  long long a;
  long long b;
  long long c;
  long long* literal_ptr;
  long long temporary; // if literal translates to a literal value, it will be
                       // filled here
  int pc;
  std::string out_buf;
  int quine_idx;
};

using PcStep = int;
constexpr PcStep ONE_INSTR = 2;
constexpr PcStep NO_STEP = 0;
constexpr PcStep FORCE_HALT = 9999;

void decode_combo_operand(State& state, char opcode) {
  switch (opcode) {
  case '0':
  case '1':
  case '2':
  case '3': {
    state.temporary = opcode - '0';
    state.literal_ptr = &state.temporary;
    break;
  }
  case '4': {
    state.literal_ptr = &state.a;
    break;
  }
  case '5': {
    state.literal_ptr = &state.b;
    break;
  }
  case '6': {
    state.literal_ptr = &state.c;
    break;
  }
  default: {
    std::cerr << "Invalid opcode " << opcode << '\n';
    assert(false);
  }
  }
}

void decode_literal_operand(State& state, char opcode) {
  assert('0' <= opcode && opcode <= '9');
  state.temporary = opcode - '0';
  state.literal_ptr = &state.temporary;
}

PcStep adv(State& state, std::string_view memory) {
  decode_combo_operand(state, memory.at(state.pc + 1));
  state.a >>= *state.literal_ptr;
  return ONE_INSTR;
}

PcStep bxl(State& state, std::string_view memory) {
  decode_literal_operand(state, memory.at(state.pc + 1));
  state.b ^= *state.literal_ptr;
  return ONE_INSTR;
}

PcStep bst(State& state, std::string_view memory) {
  decode_combo_operand(state, memory.at(state.pc + 1));
  state.b = (*state.literal_ptr) % 8;
  return ONE_INSTR;
}

PcStep jnz(State& state, std::string_view memory) {
  decode_literal_operand(state, memory.at(state.pc + 1));
  if (state.a == 0)
    return ONE_INSTR;
  state.pc = *state.literal_ptr;
  return NO_STEP;
}

PcStep bxc(State& state, std::string_view /*memory*/) {
  state.b ^= state.c;
  return ONE_INSTR;
}

PcStep out(State& state, std::string_view memory) {
  decode_combo_operand(state, memory.at(state.pc + 1));
  assert(*state.literal_ptr >= 0);
  uint8_t val = static_cast<uint8_t>(*state.literal_ptr % 8);
  state.out_buf.push_back(val + '0');
  return ONE_INSTR;
}

PcStep bdv(State& state, std::string_view memory) {
  decode_combo_operand(state, memory.at(state.pc + 1));
  state.b = state.a >> *state.literal_ptr;
  return ONE_INSTR;
}

PcStep cdv(State& state, std::string_view memory) {
  decode_combo_operand(state, memory.at(state.pc + 1));
  state.c = state.a >> *state.literal_ptr;
  return ONE_INSTR;
}

PcStep p2_out_quine_check(State& state, std::string_view memory) {
  decode_combo_operand(state, memory.at(state.pc + 1));
  uint8_t val = static_cast<uint8_t>(*state.literal_ptr % 8);
  if (state.quine_idx >= (int)memory.size()) {
    state.quine_idx = -1;
    return FORCE_HALT;
  }

  if (memory[state.quine_idx] == (val + '0')) {
    ++state.quine_idx;
    return ONE_INSTR;
  }
  return FORCE_HALT;
}

void execute(State& state, std::string_view mem, bool for_p2 = false) {
  const int mem_sz = mem.size();
  while (state.pc < mem_sz) {
    const char opcode = mem.at(state.pc);
    switch (opcode) {
    case '0':
      state.pc += adv(state, mem);
      break;
    case '1':
      state.pc += bxl(state, mem);
      break;
    case '2':
      state.pc += bst(state, mem);
      break;
    case '3':
      state.pc += jnz(state, mem);
      break;
    case '4':
      state.pc += bxc(state, mem);
      break;
    case '5':
      state.pc += for_p2 ? p2_out_quine_check(state, mem) : out(state, mem);
      break;
    case '6':
      state.pc += bdv(state, mem);
      break;
    case '7':
      state.pc += cdv(state, mem);
      break;
    default: {
      std::cerr << "Invalid opcode " << opcode << '\n';
      assert(false);
    }
    }
  }
}

} // namespace cpu