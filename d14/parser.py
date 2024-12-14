import re

def read_input() -> str:
  with open("p1_input.txt", "r") as f:
    result = ""
    for line in f.readlines():
      result += line

  return result

def process(txt: str):
  # p=38,76 v=-61,88
  match = re.findall(r"p=([0-9]+),([0-9]+) v=(\-?[0-9]+),(\-?[0-9]+)", txt)
  result = list()
  for (cap1, cap2, cap3, cap4) in match:
    result.append((cap1, cap2, cap3, cap4))
  
  return result

def write_output(result):
  with open("p1_input_parsed.txt", "w") as f:
    for case in result:
      raw_text = ' '.join(case)
      print(raw_text, file=f)


def main():
  inp = read_input()
  result = process(inp)
  # print(list(result))
  write_output(result)
  pass

if __name__ == '__main__':
  main()
