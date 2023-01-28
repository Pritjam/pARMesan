import sys
import lookups
import os.path

input_path = sys.argv[1]
out_folder = sys.argv[2]

def is_reg_operand(operand):
  return operand.startswith("%") and operand in lookups.REGS


code_lines = []
labels = {}
with open(input_path) as file:
  index = 0
  for line in file:
    line = line.split(";")[0]
    line = line.strip()
    
    # skip empty lines
    if line == "":
      continue
    
    # Labels
    if line.startswith("."):
      labels[line.rstrip(" :")] = index
    
    # Line of code
    else:
      code_lines.append(line)
      index += 1

# print("Lines of code found: ", code_lines)
# print("Labels and corresponding addresses found: ", labels)
# print("Registered all labels. Parsing instructions into bytes now")

# Now to actually assemble
instructions = []
for line in code_lines:
  toks = line.split(" ")
  op = toks[0]
  instruction_class = lookups.INSTRUCTION_TO_PARSE_FN[op]
  instructions.append(instruction_class.parse(line))


# print("All instructions parsed. Writing to file %s now" % "out.pj")
# print(instructions)
filename = os.path.basename(input_path)
out_filename = os.path.join(out_folder, filename[:-3])
with open(out_filename, "wb") as file:
  for instr in instructions:
    file.write(instr.bytes)

      