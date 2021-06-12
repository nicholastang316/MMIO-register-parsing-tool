from flask import Flask, render_template, redirect, url_for, request
import re
import c_pcie

app = Flask(__name__)
app.config['SECRET_KEY'] = "c88d30bec6ddaec23c146e85ecab6264"


@app.route("/", methods=['GET'])
def home():
    return render_template('home.html', output="This is the output")


@app.route("/", methods=['POST'])
def execute():
    device_id = request.form['device_id']
    regspec_file = request.form['regspec_file']
    op = request.form['ops']
    wr_value = request.form['wr_value']
    reg_offset = request.form['reg_offset']

    if op == "pci_cfg_space":
        output = c_pcie.dump_cfg_space(device_id)
    elif op == "read":
        output = c_pcie.read_reg(device_id, "0xf7914000", reg_offset)
    elif op == "write":
        output = c_pcie.write_reg(device_id, "0xf7914000", reg_offset, wr_value)

    output = re.match(r"b'(.*)'", str(output))
    output = output.group(1)
    output = output.replace("\\n", "\n")
    app.logger.info(f"Device ID: {device_id}")
    app.logger.info(f"Regspec_file: {regspec_file}")
    app.logger.info(f"Operation selected: {op}")
    app.logger.info(f"Register offset: {reg_offset}")
    app.logger.info(f"Write value: {wr_value}")
    app.logger.info(f"Output: {output}")

    return render_template('home.html', output=output)
