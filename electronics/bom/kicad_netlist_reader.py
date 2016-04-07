# NOTE: This file is originally from the KiCad source:
# scripts/bom-in-python/kicad_netlist_reader.py
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#
# KiCad python module for interpreting generic netlists which can be used
# to generate Bills of materials, etc.
#
# Remember these files use UTF8 encoding
#
# No string formatting is used on purpose as the only string formatting that
# is current compatible with python 2.4+ to 3.0+ is the '%' method, and that
# is due to be deprecated in 3.0+ soon
#

"""
    @package
    Helper module for interpreting generic netlist and build custom
    bom generators or netlists in foreign format
"""


from __future__ import print_function
import sys
import xml.sax as sax
import re
import pdb

#-----<Configure>----------------------------------------------------------------

# excluded_fields is a list of regular expressions.  If any one matches a field
# from either a component or a libpart, then that will not be included as a
# column in the BOM.  Otherwise all columns from all used libparts and components
# will be unionized and will appear.  Some fields are impossible to blacklist, such
# as Ref, Value, Footprint, and Datasheet.  Additionally Qty and Item are supplied
# unconditionally as columns, and may not be removed.
excluded_fields = [
    #'Price@1000'
    ]


# You may exlude components from the BOM by either:
#
# 1) adding a custom field named "Installed" to your components and filling it
# with a value of "NU" (Normally Uninstalled).
# See netlist.getInterestingComponents(), or
#
# 2) blacklisting it in any of the three following lists:


# regular expressions which match component 'Reference' fields of components that
# are to be excluded from the BOM.
excluded_references = [
    'TP[0-9]+'              # all test points
    ]


# regular expressions which match component 'Value' fields of components that
# are to be excluded from the BOM.
excluded_values = [
    'MOUNTHOLE',
    'SCOPETEST',
    'MOUNT_HOLE',
    'SOLDER_BRIDGE.*'
    ]


# regular expressions which match component 'Footprint' fields of components that
# are to be excluded from the BOM.
excluded_footprints = [
    #'MOUNTHOLE'
    ]

#-----</Configure>---------------------------------------------------------------


class xmlElement():
    """xml element which can represent all nodes of the netlist tree.  It can be
    used to easily generate various output formats by propogating format
    requests to children recursively.
    """
    def __init__(self, name, parent=None):
        self.name = name
        self.attributes = {}
        self.parent = parent
        self.chars = ""
        self.children = []

    def __str__(self):
        """String representation of this netlist element

        """
        return self.name + "[" + self.chars + "]" + " attr_count:" + str(len(self.attributes))

    def formatXML(self, nestLevel=0, amChild=False):
        """Return this element formatted as XML

        Keywords:
        nestLevel -- increases by one for each level of nesting.
        amChild -- If set to True, the start of document is not returned.

        """
        s = ""

        indent = ""
        for i in range(nestLevel):
            indent += "    "

        if not amChild:
            s = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"

        s += indent + "<" + self.name
        for a in self.attributes:
            s += " " + a + "=\"" + self.attributes[a] + "\""

        if (len(self.chars) == 0) and (len(self.children) == 0):
            s += "/>"
        else:
            s += ">" + self.chars

        for c in self.children:
            s += "\n"
            s += c.formatXML(nestLevel+1, True)

        if (len(self.children) > 0):
            s += "\n" + indent

        if (len(self.children) > 0) or (len(self.chars) > 0):
            s += "</" + self.name + ">"

        return s

    def formatHTML(self, amChild=False):
        """Return this element formatted as HTML

        Keywords:
        amChild -- If set to True, the start of document is not returned

        """
        s = ""

        if not amChild:
            s = """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
                "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
                <html xmlns="http://www.w3.org/1999/xhtml">
                <head>
                <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
                <title></title>
                </head>
                <body>
                <table>
                """

        s += "<tr><td><b>" + self.name + "</b><br>" + self.chars + "</td><td><ul>"
        for a in self.attributes:
            s += "<li>" + a + " = " + self.attributes[a] + "</li>"

        s += "</ul></td></tr>\n"

        for c in self.children:
            s += c.formatHTML(True)

        if not amChild:
            s += """</table>
                </body>
                </html>"""

        return s

    def addAttribute(self, attr, value):
        """Add an attribute to this element"""
        if type(value) != str: value = value.encode('utf-8')
        self.attributes[attr] = value

    def setAttribute(self, attr, value):
        """Set an attributes value - in fact does the same thing as add
        attribute

        """
        self.attributes[attr] = value

    def setChars(self, chars):
        """Set the characters for this element"""
        self.chars = chars

    def addChars(self, chars):
        """Add characters (textual value) to this element"""
        self.chars += chars

    def addChild(self, child):
        """Add a child element to this element"""
        self.children.append(child)
        return self.children[len(self.children) - 1]

    def getParent(self):
        """Get the parent of this element (Could be None)"""
        return self.parent

    def getChild(self, name):
        """Returns the first child element named 'name'

        Keywords:
        name -- The name of the child element to return"""
        for child in self.children:
            if child.name == name:
                return child
        return None

    def getChildren(self, name=None):
        if name:
            # return _all_ children named "name"
            ret = []
            for child in self.children:
                if child.name == name:
                    ret.append(child)
            return ret
        else:
            return self.children

    def get(self, elemName, attribute="", attrmatch=""):
        """Return the text data for either an attribute or an xmlElement
        """
        if (self.name == elemName):
            if attribute != "":
                try:
                    if attrmatch != "":
                        if self.attributes[attribute] == attrmatch:
                            ret = self.chars
                            if type(ret) != str: ret = ret.encode('utf-8')
                            return ret
                    else:
                        ret = self.attributes[attribute]
                        if type(ret) != str: ret = ret.encode('utf-8')
                        return ret
                except AttributeError:
                    ret = ""
                    if type(ret) != str: ret = ret.encode('utf-8')
                    return ret
            else:
                ret = self.chars
                if type(ret) != str: ret = ret.encode('utf-8')
                return ret

        for child in self.children:
            ret = child.get(elemName, attribute, attrmatch)
            if ret != "":
                if type(ret) != str: ret = ret.encode('utf-8')
                return ret

        ret = ""
        if type(ret) != str: ret = ret.encode('utf-8')
        return ret



class libpart():
    """Class for a library part, aka 'libpart' in the xml netlist file.
    (Components in eeschema are instantiated from library parts.)
    This part class is implemented by wrapping an xmlElement with accessors.
    This xmlElement instance is held in field 'element'.
    """
    def __init__(self, xml_element):
        #
        self.element = xml_element

    #def __str__(self):
        # simply print the xmlElement associated with this part
        #return str(self.element)

    def getLibName(self):
        return self.element.get("libpart", "lib")

    def getPartName(self):
        return self.element.get("libpart", "part")

    def getDescription(self):
        return self.element.get("description")

    def getField(self, name):
        return self.element.get("field", "name", name)

    def getFieldNames(self):
        """Return a list of field names in play for this libpart.
        """
        fieldNames = []
        fields = self.element.getChild('fields')
        if fields:
            for f in fields.getChildren():
                fieldNames.append( f.get('field','name') )
        return fieldNames

    def getDatasheet(self):
        return self.getField("Datasheet")

    def getFootprint(self):
        return self.getField("Footprint")

    def getAliases(self):
        """Return a list of aliases or None"""
        aliases = self.element.getChild("aliases")
        if aliases:
            ret = []
            children = aliases.getChildren()
            # grab the text out of each child:
            for child in children:
                ret.append( child.get("alias") )
            return ret
        return None


class comp():
    """Class for a component, aka 'comp' in the xml netlist file.
    This component class is implemented by wrapping an xmlElement instance
    with accessors.  The xmlElement is held in field 'element'.
    """

    def __init__(self, xml_element):
        self.element = xml_element
        self.libpart = None

        # Set to true when this component is included in a component group
        self.grouped = False

    def __eq__(self, other):
        """ Equivalency operator, remember this can be easily overloaded
            2 components are equivalent ( i.e. can be grouped
            if they have same value and same footprint

            Override the component equivalence operator must be done before
            loading the netlist, otherwise all components will have the original
            equivalency operator.

            You have to define a comparison module (for instance named myEqu)
            and add the line;
                kicad_netlist_reader.comp.__eq__ = myEqu
            in your bom generator script before calling the netliste reader by something like:
                net = kicad_netlist_reader.netlist(sys.argv[1])
        """
        result = False
        if self.getValue() == other.getValue():
            if self.getFootprint() == other.getFootprint():
                result = True
        return result

    def setLibPart(self, part):
        self.libpart = part

    def getLibPart(self):
        return self.libpart

    def getPartName(self):
        return self.element.get("libsource", "part")

    def getLibName(self):
        return self.element.get("libsource", "lib")

    def setValue(self, value):
        """Set the value of this component"""
        v = self.element.getChild("value")
        if v:
            v.setChars(value)

    def getValue(self):
        return self.element.get("value")

    def getField(self, name, libraryToo=True):
        """Return the value of a field named name. The component is first
        checked for the field, and then the components library part is checked
        for the field. If the field doesn't exist in either, an empty string is
        returned

        Keywords:
        name -- The name of the field to return the value for
        libraryToo --   look in the libpart's fields for the same name if not found
                        in component itself
        """

        field = self.element.get("field", "name", name)
        if field == "" and libraryToo:
            field = self.libpart.getField(name)
        return field

    def getFieldNames(self):
        """Return a list of field names in play for this component.  Mandatory
        fields are not included, and they are: Value, Footprint, Datasheet, Ref.
        The netlist format only includes fields with non-empty values.  So if a field
        is empty, it will not be present in the returned list.
        """
        fieldNames = []
        fields = self.element.getChild('fields')
        if fields:
            for f in fields.getChildren():
                fieldNames.append( f.get('field','name') )
        return fieldNames

    def getRef(self):
        return self.element.get("comp", "ref")

    def getFootprint(self, libraryToo=True):
        ret = self.element.get("footprint")
        if ret =="" and libraryToo:
            ret = self.libpart.getFootprint()
        return ret

    def getDatasheet(self, libraryToo=True):
        ret = self.element.get("datasheet")
        if ret == '' and libraryToo:
            ret = self.libpart.getDatasheet()
        return ret

    def getTimestamp(self):
        return self.element.get("tstamp")

    def getDescription(self):
        return self.libpart.getDescription()


class netlist():
    """ Kicad generic netlist class. Generally loaded from a kicad generic
    netlist file. Includes several helper functions to ease BOM creating
    scripts

    """
    def __init__(self, fname=""):
        """Initialiser for the genericNetlist class

        Keywords:
        fname -- The name of the generic netlist file to open (Optional)

        """
        self.design = None
        self.components = []
        self.libparts = []
        self.libraries = []
        self.nets = []

        # The entire tree is loaded into self.tree
        self.tree = []

        self._curr_element = None

        # component blacklist regexs, made from exluded_* above.
        self.excluded_references = []
        self.excluded_values = []
        self.excluded_footprints = []

        if fname != "":
            self.load(fname)

    def addChars(self, content):
        """Add characters to the current element"""
        self._curr_element.addChars(content)

    def addElement(self, name):
        """Add a new kicad generic element to the list"""
        if self._curr_element == None:
            self.tree = xmlElement(name)
            self._curr_element = self.tree
        else:
            self._curr_element = self._curr_element.addChild(
                xmlElement(name, self._curr_element))

        # If this element is a component, add it to the components list
        if self._curr_element.name == "comp":
            self.components.append(comp(self._curr_element))

        # Assign the design element
        if self._curr_element.name == "design":
            self.design = self._curr_element

        # If this element is a library part, add it to the parts list
        if self._curr_element.name == "libpart":
            self.libparts.append(libpart(self._curr_element))

        # If this element is a net, add it to the nets list
        if self._curr_element.name == "net":
            self.nets.append(self._curr_element)

        # If this element is a library, add it to the libraries list
        if self._curr_element.name == "library":
            self.libraries.append(self._curr_element)

        return self._curr_element

    def endDocument(self):
        """Called when the netlist document has been fully parsed"""
        # When the document is complete, the library parts must be linked to
        # the components as they are seperate in the tree so as not to
        # duplicate library part information for every component
        for c in self.components:
            for p in self.libparts:
                if p.getLibName() == c.getLibName():
                    if p.getPartName() == c.getPartName():
                        c.setLibPart(p)
                        break
                    else:
                        aliases = p.getAliases()
                        if aliases and self.aliasMatch( c.getPartName(), aliases ):
                            c.setLibPart(p)
                            break;

            if not c.getLibPart():
                print( 'missing libpart for ref:', c.getRef(), c.getPartName(), c.getLibName() )


    def aliasMatch(self, partName, aliasList):
        for alias in aliasList:
            if partName == alias:
                return True
        return False

    def endElement(self):
        """End the current element and switch to its parent"""
        self._curr_element = self._curr_element.getParent()

    def getDate(self):
        """Return the date + time string generated by the tree creation tool"""
        return self.design.get("date")

    def getSource(self):
        """Return the source string for the design"""
        return self.design.get("source")

    def getTool(self):
        """Return the tool string which was used to create the netlist tree"""
        return self.design.get("tool")

    def gatherComponentFieldUnion(self, components=None):
        """Gather the complete 'set' of unique component fields, fields found in any component.
        """
        if not components:
            components=self.components

        s = set()
        for c in components:
            s.update( c.getFieldNames() )

        # omit anything matching any regex in excluded_fields
        ret = set()
        for field in s:
            exclude = False
            for rex in excluded_fields:
                if re.match( rex, field ):
                    exclude = True
                    break
            if not exclude:
                ret.add(field)

        return ret       # this is a python 'set'

    def gatherLibPartFieldUnion(self):
        """Gather the complete 'set' of part fields, fields found in any part.
        """
        s = set()
        for p in self.libparts:
            s.update( p.getFieldNames() )

        # omit anything matching any regex in excluded_fields
        ret = set()
        for field in s:
            exclude = False
            for rex in excluded_fields:
                if re.match( rex, field ):
                    exclude = True
                    break
            if not exclude:
                ret.add(field)

        return ret       # this is a python 'set'

    def getInterestingComponents(self):
        """Return a subset of all components, those that should show up in the BOM.
        Omit those that should not, by consulting the blacklists:
        excluded_values, excluded_refs, and excluded_footprints, which hold one
        or more regular expressions.  If any of the the regular expressions match
        the corresponding field's value in a component, then the component is exluded.
        """

        # pre-compile all the regex expressions:
        del self.excluded_references[:]
        del self.excluded_values[:]
        del self.excluded_footprints[:]

        for rex in excluded_references:
            self.excluded_references.append( re.compile( rex ) )

        for rex in excluded_values:
            self.excluded_values.append( re.compile( rex ) )

        for rex in excluded_footprints:
            self.excluded_footprints.append( re.compile( rex ) )

        # the subset of components to return, considered as "interesting".
        ret = []

        # run each component thru a series of tests, if it passes all, then add it
        # to the interesting list 'ret'.
        for c in self.components:
            exclude = False
            if not exclude:
                for refs in self.excluded_references:
                    if refs.match(c.getRef()):
                        exclude = True
                        break;
            if not exclude:
                for vals in self.excluded_values:
                    if vals.match(c.getValue()):
                        exclude = True
                        break;
            if not exclude:
                for mods in self.excluded_footprints:
                    if mods.match(c.getFootprint()):
                        exclude = True
                        break;

            if not exclude:
                # This is a fairly personal way to flag DNS (Do Not Stuff).  NU for
                # me means Normally Uninstalled.  You can 'or in' another expression here.
                if c.getField( "Installed" ) == 'NU':
                    exclude = True

            if not exclude:
                ret.append(c)

        # Sort first by ref as this makes for easier to read BOM's
        def f(v):
            return re.sub(r'([A-z]+)[0-9]+', r'\1', v) + '%08i' % int(re.sub(r'[A-z]+([0-9]+)', r'\1', v))
        ret.sort(key=lambda g: f(g.getRef()))

        return ret


    def groupComponents(self, components = None):
        """Return a list of component lists. Components are grouped together
        when the value, library and part identifiers match.

        Keywords:
        components -- is a list of components, typically an interesting subset
        of all components, or None.  If None, then all components are looked at.
        """
        if not components:
            components = self.components

        groups = []

        # Make sure to start off will all components ungrouped to begin with
        for c in components:
            c.grouped = False

        # Group components based on the value, library and part identifiers
        for c in components:
            if c.grouped == False:
                c.grouped = True
                newgroup = []
                newgroup.append(c)

                # Check every other ungrouped component against this component
                # and add to the group as necessary
                for ci in components:
                    if ci.grouped == False and ci == c:
                        newgroup.append(ci)
                        ci.grouped = True

                # Add the new component group to the groups list
                groups.append(newgroup)

        # Each group is a list of components, we need to sort each list first
        # to get them in order as this makes for easier to read BOM's
        def f(v):
            return re.sub(r'([A-z]+)[0-9]+', r'\1', v) + '%08i' % int(re.sub(r'[A-z]+([0-9]+)', r'\1', v))
        for g in groups:
            g = sorted(g, key=lambda g: f(g.getRef()))

        # Finally, sort the groups to order the references alphabetically
        groups = sorted(groups, key=lambda group: f(group[0].getRef()))

        return groups

    def getGroupField(self, group, field):
        """Return the whatever is known about the given field by consulting each
        component in the group.  If any of them know something about the property/field,
        then return that first non-blank value.
        """
        for c in group:
            ret = c.getField(field, False)
            if ret != '':
                return ret
        return group[0].getLibPart().getField(field)

    def getGroupFootprint(self, group):
        """Return the whatever is known about the Footprint by consulting each
        component in the group.  If any of them know something about the Footprint,
        then return that first non-blank value.
        """
        for c in group:
            ret = c.getFootprint()
            if ret != "":
                return ret
        return group[0].getLibPart().getFootprint()

    def getGroupDatasheet(self, group):
        """Return the whatever is known about the Datasheet by consulting each
        component in the group.  If any of them know something about the Datasheet,
        then return that first non-blank value.
        """
        for c in group:
            ret = c.getDatasheet()
            if ret != "":
                return ret

        if len(group) > 0:
            return group[0].getLibPart().getDatasheet()
        else:
            print("NULL!")
        return ''

    def formatXML(self):
        """Return the whole netlist formatted in XML"""
        return self.tree.formatXML()

    def formatHTML(self):
        """Return the whole netlist formatted in HTML"""
        return self.tree.formatHTML()

    def load(self, fname):
        """Load a kicad generic netlist

        Keywords:
        fname -- The name of the generic netlist file to open

        """
        try:
            self._reader = sax.make_parser()
            self._reader.setContentHandler(_gNetReader(self))
            self._reader.parse(fname)
        except IOError as e:
            print( __file__, ":", e, file=sys.stderr )
            sys.exit(-1)



class _gNetReader(sax.handler.ContentHandler):
    """SAX kicad generic netlist content handler - passes most of the work back
    to the 'netlist' class which builds a complete tree in RAM for the design

    """
    def __init__(self, aParent):
        self.parent = aParent

    def startElement(self, name, attrs):
        """Start of a new XML element event"""
        element = self.parent.addElement(name)

        for name in attrs.getNames():
            element.addAttribute(name, attrs.getValue(name))

    def endElement(self, name):
        self.parent.endElement()

    def characters(self, content):
        # Ignore erroneous white space - ignoreableWhitespace does not get rid
        # of the need for this!
        if not content.isspace():
            self.parent.addChars(content)

    def endDocument(self):
        """End of the XML document event"""
        self.parent.endDocument()
