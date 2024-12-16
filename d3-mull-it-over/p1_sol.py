import re

def read_input() -> str:
  with open("p1_input.txt", "r") as f:
    result = ""
    for line in f.readlines():
      result += line

  return result

def process(txt: str) -> int:
  match = re.findall(r"mul\\(([0-9]+),([0-9]+)\\)", txt)
  result = 0
  for (cap1, cap2) in match:
    result += int(cap1) * int(cap2)
  return result

def main():
  inp = read_input()
  result = process(inp)
  print(result)
  pass

if __name__ == '__main__':
  main()
