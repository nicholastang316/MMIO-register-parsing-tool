from subprocess import Popen, PIPE
import re


def dump_cfg_space(device_id):
    p = Popen(["./devtool", "--pci", "-d", device_id], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate(b"input data that is passed to subprocess' stdin")
    rc = p.returncode
    return output


def read_reg(device_id, base_addr, offset):
    p = Popen(["./devtool", "--mmio", "-d", device_id, "-b", base_addr, "-o", offset, "-r"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate(b"input data that is passed to subprocess' stdin")
    rc = p.returncode
    return output


def write_reg(device_id, base_addr, offset, wr_value):
    p = Popen(["./devtool", "--mmio", "-d", device_id, "-b", base_addr, "-o", offset, "-w", wr_value], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    output, err = p.communicate(b"input data that is passed to subprocess' stdin")
    rc = p.returncode
    return output


def regspec_dump(device_id, base_addr, regspec_file):
    p = Popen(["./devtool", "--mmio", "-d", device_id, "-b", base_addr, "-s", regspec_file], stdin=PIPE, stdout=PIPE,
               stderr=PIPE)
    output, err = p.communicate(b"input data that is passed to subprocess' stdin")
    rc = p.returncode
    return output


def regspec_dump_file(device_id, base_addr, regspec_file):
    p = Popen(["./devtool", "--mmio", "-d", device_id, "-b", base_addr, "-s", regspec_file, "-f", "regspec_dump.txt"], stdin=PIPE, stdout=PIPE,
               stderr=PIPE)
    output, err = p.communicate(b"input data that is passed to subprocess' stdin")
    rc = p.returncode

    # output = re.match(r"b'(.*)'", str(output))
    # output = output.group(1)
    # output = output.replace("\\n", "\n")
    #
    # output_file = open("regspec_dump.txt", "w")
    # output_file.write(output)

    output = "b'Press Download to download the file'"
    return output
