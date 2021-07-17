var all_fields = ["regspec_file", "reg_offset", "wr_value", "base_addr"];
function updateHome() {
    // Change all input fields back to normal first everytime an operation is selected
    for (var i = 0; i < all_fields.length; i++) {
        document.getElementById(all_fields[i]).required = false;
        change(all_fields[i].concat("_label"), "normal")
    }

    var op = document.getElementById("ops");
    var selected_op = op.options[op.selectedIndex].text;

    if (selected_op == "Dump PCI config space") {
    } else if (selected_op == "Dump MMIO registers and value in regspec.txt" || selected_op == "Dump MMIO registers and value in regspec.txt on an output file") {
        change("regspec_file_label", "required");
        document.getElementById("regspec_file").required = true;
    } else if (selected_op == "Read a MMIO register") {
        change("reg_offset_label", "required");
        document.getElementById("reg_offset").required = true;
    } else if (selected_op == "Write a MMIO register") {
        change("reg_offset_label", "required");
        document.getElementById("reg_offset").required = true;

        change("wr_value_label", "required");
        document.getElementById("wr_value").required = true;
    } else {
        change("base_addr_label", "required");
        document.getElementById("base_addr").required = true;
    }
}

function change(id, newClass) {
    identity=document.getElementById(id);
    identity.className=newClass;
}