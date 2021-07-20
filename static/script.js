var all_fields = ["regspec_file", "reg_offset", "wr_value", "base_addr"];
function update_required() {
    var op = document.getElementById("ops");
    var selected_op = op.options[op.selectedIndex].text;

    // Change all input fields back to normal first everytime an operation is selected
    for (var i = 0; i < all_fields.length; i++) {
        document.getElementById(all_fields[i]).required = false;
        change(all_fields[i].concat("_div"), "hidden");
    }

    // If base_addr is not set, makes base_addr a required field when it is not "Dump PCI config space"
    if (typeof set_base_addr === 'undefined' && selected_op != "Dump PCI config space") {
        change("base_addr_div", "required");
        document.getElementById("base_addr").required = true;
    }

    if (selected_op == "Dump PCI config space") {
    } else if (selected_op == "Dump MMIO registers and value in regspec.txt" || selected_op == "Dump MMIO registers and value in regspec.txt on an output file") {
        change("regspec_file_div", "required");
        document.getElementById("regspec_file").required = true;

    } else if (selected_op == "Read a MMIO register") {
        change("reg_offset_div", "required");
        document.getElementById("reg_offset").required = true;
    } else if (selected_op == "Write a MMIO register") {
        change("reg_offset_div", "required");
        document.getElementById("reg_offset").required = true;

        change("wr_value_div", "required");
        document.getElementById("wr_value").required = true;
    } else {
        change("base_addr_div", "required");
        document.getElementById("base_addr").required = true;
    }
}

function change(id, newClass) {
    identity=document.getElementById(id);
    identity.className=newClass;
}