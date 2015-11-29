
module assert(truth, msg="(no messsage provided)") {
    if (!truth) {
        // Intentionally don't close the <div> tag to make the entire console red
        echo(str(
            "<div style='background-color:#FF0000'>",
            "<br><br>",
            "####################",
            "<br><br>",
            "<span style='font-size: x-large'>ASSERTION ERROR!</span>",
            "<br><br>",
            msg,
            "<br><br>",
            "####################",
            "<br><br>"));

        color("red") {
            cube([1000, 1000, 1000], center=true);
        }
    }
}

required_widgets = 24;
widgets = 12 + 3;

assert(widgets >= required_widgets, str("Not enough widgets! Required ", required_widgets, " but found ", widgets));


