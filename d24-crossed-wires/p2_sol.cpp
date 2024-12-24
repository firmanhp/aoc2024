#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <span>
#include <sstream>
#include <thread>
#include <tuple>
#include <unordered_set>

#include "input.h"

namespace {

using Gate = input::Input::Gate;
using util::print;
using util::valid_spot;
using util::Vector2D;
typedef long long ll;
typedef std::pair<int, int> pii;

constexpr std::string INPUT = "p1_input.txt";
constexpr int MAX_N = 45;

enum class Op { AND, OR, XOR };

std::ostream& operator<<(std::ostream& o, const std::vector<std::string>& v) {
  for (int i = 0; i < (int)v.size(); ++i) {
    if (i > 0)
      o << ',';
    o << v[i];
  }
  return o;
}

void clean_inputs(input::Input& input) {
  for (input::Input::Gate& gate : input.gates) {
    if (gate.operand_1 > gate.operand_2)
      std::swap(gate.operand_1, gate.operand_2);
  }
  std::sort(
      input.gates.begin(), input.gates.end(),
      [](const auto& a, const auto& b) { return a.operand_1 < b.operand_1; });
}

void print(const input::Input& input) {
  for (const auto& [name, val] : input.initial) {
    std::cout << name << " " << val << '\n';
  }
  std::cout << '\n';
  for (const auto& gate : input.gates) {
    std::cout << gate.operand_1 << " " << gate.op << " " << gate.operand_2
              << " -> " << gate.operand_result << '\n';
  }
}

void print(const input::Input::Gate& gate) {
  std::cout << gate.operand_1 << " " << gate.op << " " << gate.operand_2
            << " -> " << gate.operand_result << '\n';
}

std::string reg_x(int idx) {
  assert(idx < 100);
  std::ostringstream o;
  o << "x" << std::setfill('0') << std::setw(2) << idx;
  return o.str();
}

std::string reg_y(int idx) {
  assert(idx < 100);
  std::ostringstream o;
  o << "y" << std::setfill('0') << std::setw(2) << idx;
  return o.str();
}

std::string reg_z(int idx) {
  assert(idx < 100);
  std::ostringstream o;
  o << "z" << std::setfill('0') << std::setw(2) << idx;
  return o.str();
}

const input::Input::Gate& find_gate(const input::Input& input,
                                    std::string_view op1, Op op,
                                    std::string_view op2) {
  std::string op_str;
  switch (op) {
  case Op::AND:
    op_str = "AND";
    break;
  case Op::OR:
    op_str = "OR";
    break;
  case Op::XOR:
    op_str = "XOR";
    break;
  }

  for (auto& gate : input.gates) {
    if (((gate.operand_1 == op1 && gate.operand_2 == op2) ||
         (gate.operand_1 == op2 && gate.operand_2 == op1)) &&
        (gate.op == op_str)) {
      return gate;
    }
  }

  std::cerr << "FATAL: Gate not found " << op1 << " " << op_str << " " << op2
            << '\n';
  assert("Gate not found" && false);
}

void swap_gate_result(input::Input& input, std::string res1, std::string res2) {
  for (auto& gate : input.gates) {
    if (gate.operand_result == res1) {
      gate.operand_result = res2;
      continue;
    }
    if (gate.operand_result == res2) {
      gate.operand_result = res1;
      continue;
    }
  }
}

void assert_temp_reg(std::string_view reg) {
  if (reg.front() == 'x' || reg.front() == 'y' || reg.front() == 'z') {
    std::cout << "ANOMALY: i/o register used as temporary: " << reg << '\n';
    assert(false);
  }
}

/**
The pattern:
  x00 XOR y00 -> z00
  x00 AND y00 -> C01_ktt

  x01 XOR y01 -> S01_rvb
  C01_ktt XOR S01_rvb -> z01
  C01_ktt AND S01_rvb -> kmb (c_gate1)
  x01 AND y01 -> kgp (c_gate2)
  kgp OR kmb -> C02_rkn (c_gate)

  x02 XOR y02 -> S02_ssq
  rkn XOR S02_ssq -> z02
  x02 AND y02 -> kwm
  rkn AND S02_ssq -> vsc
  kwm OR vsc -> C03_ntj

  x03 XOR y03 -> S03_fbk
  S03_fbk XOR C03_ntj -> z03
  x03 AND y03 -> jmr
  S03_fbk AND C03_ntj -> dps
  dps OR jmr -> C04_mpf
 */
ll solve(input::Input& input) {
  std::cout << "Test " << reg_x(5) << '\n';
  clean_inputs(input);

  // manual intervention
  std::string c = "ktt"; // from 0-th first bit
  std::string s;

  // store outputs here
  swap_gate_result(input, "djg", "z12");
  swap_gate_result(input, "sbg", "z19");
  swap_gate_result(input, "mcq", "hjm");
  swap_gate_result(input, "dsd", "z37");

  std::vector<std::string> res = {"djg", "z12", "sbg", "z19",
                                  "mcq", "hjm", "dsd", "z37"};

  for (int bit = 1; bit < MAX_N; ++bit) {
    const Gate& s_gate = find_gate(input, reg_x(bit), Op::XOR, reg_y(bit));
    s = s_gate.operand_result;
    print(s_gate);

    const Gate& sc_gate = find_gate(input, c, Op::XOR, s);
    print(sc_gate);
    if (sc_gate.operand_result != reg_z(bit)) {
      std::cout << "ANOMALY: unexpected SC_gate result: "
                << sc_gate.operand_result << " vs " << reg_z(bit) << '\n';
      assert(false);
    }

    const Gate& c_gate1 = find_gate(input, c, Op::AND, s);
    assert_temp_reg(c_gate1.operand_result);
    print(c_gate1);
    const Gate& c_gate2 = find_gate(input, reg_x(bit), Op::AND, reg_y(bit));
    assert_temp_reg(c_gate2.operand_result);
    print(c_gate2);
    const Gate& c_gate = find_gate(input, c_gate1.operand_result, Op::OR,
                                   c_gate2.operand_result);
    print(c_gate);
    c = c_gate.operand_result;
    if (bit < MAX_N - 1) {
      assert_temp_reg(c);
    } else {
      // the last bit, carry becomes the last result.
      if (c != reg_z(bit + 1)) {
        std::cout << "ANOMALY: unexpected C_gate result: " << c << " vs "
                  << reg_z(bit + 1) << '\n';
      }
    }

    std::cout << '\n';
  }

  std::cout << "OK!\n";
  std::sort(res.begin(), res.end());
  std::cout << res << '\n';
  return 0;
}

} // namespace

int main() {
  auto input = input::read_input(INPUT);
  std::cout << solve(input) << '\n';
  return 0;
}
