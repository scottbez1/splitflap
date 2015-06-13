from xml.dom import minidom

REGISTRATION_MARK = 'M 1.10001,-1.10001 L -0.100006,-1.10001 L -0.100006,0.100006 L 1.10001,0.100006 z'

def process():
    cut_dom = minidom.parse('build/cut.svg')
    cut_svg = cut_dom.documentElement
    fix_dimens(cut_svg)

    # Set fill and stroke for laser cutting
    for path in cut_svg.getElementsByTagName('path'):
        apply_attributes(path, {
            'fill': 'none',
            'stroke': '#0000ff',
            'stroke-width': '0.01',
        })
        delete_registration_mark(path)

    etch_svg = minidom.parse('build/etch.svg').documentElement
    # Set fill and stroke for laser etching
    for path in etch_svg.getElementsByTagName('path'):
        apply_attributes(path, {
            'fill': '#000000',
            'stroke': 'none',
        })
        delete_registration_mark(path)
        
        output_node = cut_dom.importNode(path, True)
        cut_svg.appendChild(output_node)

    write_svg(cut_svg, 'build/output.svg')


def fix_dimens(svg_node):
    # Add mm units to the document dimensions
    svg_node.attributes['width'].value += 'mm'
    svg_node.attributes['height'].value += 'mm'

def write_svg(document, filename):
    with open(filename, 'wb') as output_file:
        document.writexml(output_file)

def apply_attributes(node, values):
    for (k,v) in values.items():
        node.attributes[k] = v

def delete_registration_mark(node):
    data = node.attributes['d']
    if REGISTRATION_MARK not in data.value:
        raise Exception("Registration mark not found!")
    data.value = data.value.replace(REGISTRATION_MARK, '')

if __name__ == '__main__':
    process()

