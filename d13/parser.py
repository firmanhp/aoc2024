import re

def read_input() -> str:
  with open("p1_input.txt", "r") as f:
    result = ""
    for line in f.readlines():
      result += line

  return result

def extract_xy(txt: str, r):
  match = re.findall(r, txt)
  result = list()
  for (cap1, cap2) in match:
    result.append((cap1, cap2))
  
  return result

def process(txt: str) -> int:
  button_a = extract_xy(txt, r"Button A: X\+([0-9]+), Y\+([0-9]+)")
  button_b = extract_xy(txt, r"Button B: X\+([0-9]+), Y\+([0-9]+)")
  prize = extract_xy(txt, r"Prize: X=([0-9]+), Y=([0-9]+)")
  return list(zip(button_a, button_b, prize))

def write_output(result):
  with open("p1_input_parsed.txt", "w") as f:
    for case in result:
      pair_str = map(lambda p: ' '.join(p), case)
      raw_text = ' '.join(pair_str)
      print(raw_text, file=f)


def main():
  inp = read_input()
  result = process(inp)
  # print(list(result))
  write_output(result)
  pass

if __name__ == '__main__':
  main()
