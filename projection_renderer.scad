module projection_renderer(render_index = 0, kerf_width = 0) {
    echo(num_components=$children);

    offset(delta=kerf_width/2)
        projection()
            children(render_index);
}
