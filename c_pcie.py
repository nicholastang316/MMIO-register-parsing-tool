from subprocess import Popen, PIPE


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

