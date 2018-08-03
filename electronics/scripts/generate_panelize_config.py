
ORIGIN = (20, 20)
BOARD_SIZE = (16.256, 16.256)

ROWS = 4
COLS = 3

VCUT_LINE_OUTSET = 2
VCUT_LABEL_OUTSET = (6, 3)

_HEADER = """
load sensor.kicad_pcb
set-title "Splitflap sensor board [PANELIZED]"
set-comment-1 https://github.com/scottbez1/splitflap

create-template
"""

_FOOTER = """
save panelized_sensor.kicad_pcb
"""


def run():
    print _HEADER

    print 'source-area {} {} {} {}'.format(
        ORIGIN[0],
        ORIGIN[1],
        ORIGIN[0] + BOARD_SIZE[0],
        ORIGIN[1] + BOARD_SIZE[1],
    )

    # Generate copies
    for r in range(ROWS):
        print '\n# Row {}'.format(r)
        for c in range(COLS):
            print 'copy {} {}'.format(
                ORIGIN[0] + c * BOARD_SIZE[0],
                ORIGIN[1] + r * BOARD_SIZE[1],
            )

    print ''

    # Draw board outline
    x_min = ORIGIN[0]
    x_max = ORIGIN[0] + COLS * BOARD_SIZE[0]
    y_min = ORIGIN[1]
    y_max = ORIGIN[1] + ROWS * BOARD_SIZE[1]
    draw_edge_line(x_min, y_min, x_max, y_min)
    draw_edge_line(x_max, y_min, x_max, y_max)
    draw_edge_line(x_max, y_max, x_min, y_max)
    draw_edge_line(x_min, y_max, x_min, y_min)

    # Draw v-cut lines
    for r in range(1, ROWS):
        draw_edge_line(
            ORIGIN[0] - VCUT_LINE_OUTSET,
            ORIGIN[1] + r * BOARD_SIZE[1],
            ORIGIN[0] + COLS * BOARD_SIZE[0] + VCUT_LINE_OUTSET,
            ORIGIN[1] + r * BOARD_SIZE[1],
        )
        print 'draw-text V-CUT {} {} 0 Edge.Cuts 3 2 0.25'.format(
            ORIGIN[0] + COLS * BOARD_SIZE[0] + VCUT_LINE_OUTSET + VCUT_LABEL_OUTSET[0],
            ORIGIN[1] + r * BOARD_SIZE[1],
        )

    for c in range(1, COLS):
        draw_edge_line(
            ORIGIN[0] + c * BOARD_SIZE[0],
            ORIGIN[1] - VCUT_LINE_OUTSET,
            ORIGIN[0] + c * BOARD_SIZE[0],
            ORIGIN[1] + ROWS * BOARD_SIZE[1] + VCUT_LINE_OUTSET,
        )
        print 'draw-text V-CUT {} {} 0 Edge.Cuts 3 2 0.25'.format(
            ORIGIN[0] + c * BOARD_SIZE[0],
            ORIGIN[1] + ROWS * BOARD_SIZE[1] + VCUT_LINE_OUTSET + VCUT_LABEL_OUTSET[1],
        )

    print _FOOTER


def draw_edge_line(x1, y1, x2, y2):
    print 'draw-line {} {} {} {} Edge.Cuts 0.15'.format(x1, y1, x2, y2)

if __name__ == '__main__':
    run()
