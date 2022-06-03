# Python
#
# Copyright 2019-2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

# This script combines multiple files into a single file, by placing them at specific addresses and padding the output with 0xFF bytes.
# Usage: `python combine_binaries.py output-filename [input-addr input-filename]*`
# Example: `python combine_binaries.py output.bin 0x1000 input1.bin 0x2000 input2.bin 0x8000 input3.bin`

import sys

output_file = open(sys.argv[1], "wb")
output_size = 0

arg_is_addr = True
for i in range(2, len(sys.argv)):
	if arg_is_addr:
		next_addr = int(sys.argv[i], 16)
		fill_count = next_addr - output_size
		if fill_count >= 0:
			output_file.write(bytearray(b'\xFF') * fill_count)
		else:
			exit("Invalid offset")
		output_size = next_addr
	else:
		input_file = open(sys.argv[i], "rb")
		input_data = input_file.read()
		input_file.close()
		output_file.write(input_data)
		output_size += len(input_data)
	arg_is_addr = not arg_is_addr

output_file.close()
