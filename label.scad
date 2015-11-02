
module roboto(text, size) {
    text(text=text, size=size, font="Roboto");
}
module text_label(lines) {
    text_height=3;
    module text_lines(lines, text_height, spacing=1.5) {
        for (i = [0 : len(lines)-1]) {
            translate([0, text_height * spacing * (len(lines)-1-i), 0]) {
                roboto(lines[i], text_height);
            }
        }
    }
    text_lines(lines, text_height);
}


