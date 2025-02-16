from colored_stl_exporter import ColoredStlExporter
from pathlib import Path

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('input_file', type=Path, help='flap.scad file')
    parser.add_argument('output_folder', type=Path, help='directory to place the colored flap STL files')

    args = parser.parse_args()

    for flap in range(0, 52):
        flap_path = args.output_folder / f"flap_{flap}"
        flap_path.mkdir(parents=True, exist_ok=True)
        openscad_variables = {
            "flap_number": flap,
            "gap": 0,
            "print_3d": True,
            "generate_second_half": False,
        }

        ColoredStlExporter(str(args.input_file), str(flap_path), openscad_variables=openscad_variables).run()
