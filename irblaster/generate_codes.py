"""
A python program to generate a codedb.h file from all the "POWER" codes in the codes.ir file, using the flipper zero's .ir format.
These files are redily avalable on the internet in huge collections.
Multiple files .ir can be concated together to generate a signgle codedb.h file
"""

added = set()
counter = 0

print('// Do not edit this file! Instead use generate_codes.py to generate a new file with the desired data.')
print('const union Code codes[] PROGMEM = {')

file = open("codes.ir")

name = None
proto = None
addr = None
command = None

def ir_hex_to_c(data, size):
	split = data.split(' ')
	return "0x" + "".join(split[:size][::-1])	

for line in file.readlines():
	if line.startswith("name: "): name = line[6:].rstrip()
	if line.startswith("protocol: "): proto = line[10:].rstrip()
	if line.startswith("address: "): addr = line[9:].rstrip()
	if line.startswith("command: "): command = line[9:].rstrip()
	if name and proto and addr and command:
		if "POWER" in name.upper():
			# Deduplicate commands
			if (addr, command) in added:
				continue;
			else:
				added.add((addr, command))
			# Add them
			if proto == "NEC":
				counter = counter + 1
				print("\t{.nec = { .type = CODE_NEC, .address = " + ir_hex_to_c(addr, 1) + ", .command = " + ir_hex_to_c(command, 1) + " }},")
			elif proto == "NECext":
				counter = counter + 1
				print("\t{.necext = { .type = CODE_NECEXT, .address = " + ir_hex_to_c(addr, 2) + ", .command = " + ir_hex_to_c(command, 2) + " }},")
			elif proto == "RC5":
				counter = counter + 1
				print("\t{.rc5 = { .type = CODE_RC5, .address = " + ir_hex_to_c(addr, 1) + ", .command = " + ir_hex_to_c(command, 1) + " }},")
			elif proto == "Samsung32":
				counter = counter + 1
				print("\t{.sam32 = { .type = CODE_SAM32, .address = " + ir_hex_to_c(addr, 2) + ", .command = " + ir_hex_to_c(command, 2) + " }},")
			else:
				print("// Unkown type: ", proto)
		# Reset data
		name = None
		proto = None
		addr = None
		command = None

print("};")
print(f"const uint32_t numcodes = {counter};")