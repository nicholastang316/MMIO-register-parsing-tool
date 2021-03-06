from flask import Flask, render_template, redirect, url_for, request, send_file
import re
import c_pcie

app = Flask(__name__)
app.config['SECRET_KEY'] = "c88d30bec6ddaec23c146e85ecab6264"
set_base_addr = ""
device_id = ""


@app.route("/", methods=['GET'])
def home():
    return render_template('home.html', output="---   Sample output   ---", set_base_addr=set_base_addr, device_id=device_id)


@app.route("/", methods=['POST'])
def execute():
    global set_base_addr
    global device_id

    device_id = request.form['device_id']
    regspec_file = request.files['regspec_file']
    op = request.form['ops']
    wr_value = request.form['wr_value']
    reg_offset = request.form['reg_offset']
    # app.logger.info(f"Set base address: {set_base_addr}")
    if set_base_addr:
        base_addr = set_base_addr
    else:
        base_addr = request.form['base_addr']
    # app.logger.info(f"Base address: {base_addr}")

    if op == "pci_cfg_space":
        output = c_pcie.dump_cfg_space(device_id)
    elif op == "read":
        output = c_pcie.read_reg(device_id, base_addr, reg_offset)
    elif op == "write":
        output = c_pcie.write_reg(device_id, base_addr, reg_offset, wr_value)
    elif op == "regspec_dump":
        regspec_file.save(regspec_file.filename)
        output = c_pcie.regspec_dump(device_id, base_addr, regspec_file.filename)
    elif op == "regspec_dump_file":
        regspec_file.save(regspec_file.filename)
        output = c_pcie.regspec_dump_file(device_id, base_addr, regspec_file.filename)
    elif op == "set_base":
        set_base_addr = request.form['base_addr']
        output = f"b'Base address is set to {set_base_addr}'"

    output = re.match(r"b'(.*)'", str(output))
    output = output.group(1)
    output = output.replace("\\n", "<br>")

    if op == "regspec_dump_file":
        return render_template('download.html')
    else:
        return render_template('home.html', output=output, set_base_addr=set_base_addr, device_id=device_id)


@app.route("/download")
def download_file():
    return send_file("regspec_dump.txt", as_attachment=True)
