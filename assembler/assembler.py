import sys
import lookups
from instructions import jmp_call
from bitstring import CreationError
import os.path

input_path = sys.argv[1]

out_folder = sys.argv[2] if len(sys.argv) >= 3 else ""

def is_reg_operand(operand):
  return operand.startswith("%") and operand in lookups.REGS


code_lines = []
label_locations = {}
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
      label_locations[line.rstrip(" :")] = index
    
    # Line of code
    else:
      code_lines.append(line)
      index += 1

# print("Lines of code found: ", code_lines)
# print("Labels and corresponding addresses found: ", labels)
# print("Registered all labels. Parsing instructions into bytes now")

# Now to actually assemble
instructions = []
for (index, code_line) in enumerate(code_lines):
  toks = code_line.split(" ")
  op = toks[0].lower()
  instruction_class = lookups.INSTRUCTION_TO_PARSE_FN[op]
  if instruction_class == jmp_call and toks[1] in label_locations:
    # this is a jump pointing to a valid label, meaning we must replace it with the offset
    line = op + " " + str(label_locations[toks[1]] - index)
  else:
    line = code_line.lower()
  try:
    instructions.append(instruction_class.parse(line))
  except CreationError as exception:
    print("Error parsing instruction %d: %s. \n\t%s" % (index, code_line, str(exception.msg)))
    exit(0)
  except Exception as exception:
    print("Other error occurred: \n\t%s" % str(exception))
    exit(0)
    



# print("All instructions parsed. Writing to file %s now" % "out.pj")
# print(instructions)
filename = os.path.basename(input_path)
(filename, ext) = os.path.splitext(filename)
out_filename = os.path.join(out_folder, filename)
with open(out_filename, "wb") as file:
  for instr in instructions:
    file.write(instr.bytes)

      