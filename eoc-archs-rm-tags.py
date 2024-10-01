# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

#https://stackoverflow.com/questions/44977441/delete-line-from-text-file-if-line-contains-one-of-few-specified-strings-python

import os
import sys
import getopt

def StringIsFoundInLine(line, strings):
	return any([string in line for string in strings])

def main(argv):
	filepath=""
	opts, args = getopt.getopt(argv, "hf:")
	for opt, arg in opts:
		if opt == "-h":
			print("usage: eoc-archs-rm-tags.py -f <filepath>")
			sys.exit(0)
		elif opt in "-f":
			filepath = arg
		else:
			continue

	if not filepath:
		print("ERROR: No filepath provide in the command line arguments.")
		sys.exit(0)
	else:
		filepath = os.path.abspath(filepath.strip())

	newLines = []
	tags = ["XR_DOCS_TAG_"]

	file = open(filepath, "r")
	for line in file:
		if StringIsFoundInLine(line, tags):
			continue
		else:
			newLines.append(line)

	file.close()

	filepath_temp = filepath + ".tmp"
	w_file = open(filepath_temp, "w")
	w_file.writelines(newLines)
	w_file.close()

	os.remove(filepath)
	os.rename(filepath_temp, filepath)

if __name__ == "__main__":
	main(sys.argv[1:])
