
/*
 * A 2D wedge, from the origin with the specified radius and angle sweep.
 * n is the number of facets to draw as part of the wedge (i.e. it doesn't specify the number of facets of the full circle).
 */
module wedge(radius, angle, n) {
    points = [ for (i = [0 : n]) [cos(angle*i/n)*radius, sin(angle*i/n)*radius] ];
    polygon(concat([[0,0]], points));
}


