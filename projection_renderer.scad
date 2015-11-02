module projection_renderer(render_index = 0, kerf_width = 0) {
    echo(num_components=$children);

    offset(delta=kerf_width/2) {
        projection() {
            children(render_index);
        }
    }

    // Registration marks (need 2, otherwise openscad changes how a single mark is rendered if it's the only path)
    translate([-50,-50])
    square([1,1]);
    translate([-48,-48])
    square([1,1]);
}
