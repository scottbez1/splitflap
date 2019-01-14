#!/usr/bin/python3
#
# This file is part of Tin's Kicad Tools.
# Copyright (c) 2016, Martin Furter <mf@borg.ch>
# All rights reserved.
#
# Homepage:  https://projects.borg.ch/electronics/kicad/panelize.html
# SVN Repos: https://repos.borg.ch/projects/kicad/trunk/tools
#
# ANY-LICENSE:
# You can use this software under any license approved by the
# Open Source Initiative as long as the license you choose is
# compatible to the dependencies of Tin's Kicad Tools.
#
# See http://www.opensource.org/licenses/ for a list of
# approved licenses.
#
#-------------------------------------------------------------------------------

# Scott's modifications:
# 2018-08-03 - Changed "page" from Text to List, to support page definitions like "User 111 123" instead of "A4"
# 2018-12-18 - Added \n to the set of characters that will trigger a text element to be quoted when written

import math
import sys
import traceback

__version__ = "3.1.4"
__revision__ = "$Rev: 953 $ unkown".split()[1]


kicad_versions = (
	"4.0.5",
	"latest"
)

compat_4_0_5 = kicad_versions.index( "4.0.5" )


def check_instance( inst, cls ):
	if isinstance( inst, cls ):
		return
	cname = cls.__name__
	iname = type(inst).__name__
	raise Exception( "Expected class '%s' but got an instance of '%s'." %
			( cname, iname ) )


class KicadObj:

	def __init__( self, name ):
		self.obj_name = name

	def clone( self ):
		raise Exception( "KicadObj.clone() has not been overridden by '%s'" % self.__class__.__name__ )

	def tag_start( self, factory, name ):
		factory.parse_error( "unexpected tag '%s' in '%s'" % ( name, self.obj_name ) )

	def tag_end( self, factory, obj ):
		factory.parse_error( "unexpected tag end in '%s'" % ( self.obj_name ) )

	def tag_value( self, factory, value ):
		factory.parse_error( "unexpected value '%s' for '%s'" % ( value, self.obj_name ) )

	def is_set( self ):
		return True

	def write( self, writer ):
		writer.begin( self.obj_name )
		self.write_contents( writer )
		writer.end()

	def write_contents( self, writer ):
		pass


class KicadFatObj( KicadObj ):

	def __init__( self, name, object_types ):
		KicadObj.__init__( self, name )
		# self.object_types = object_types
		self.object_type_names = []
		self.object_type_dict = {}
		for ot in object_types:
			if type(ot) is tuple:
				typename = ot[0]
			else:
				typename = ot.obj_name
				setattr( self, typename, ot )
			self.object_type_names.append( typename )
			self.object_type_dict[typename] = ot

	def clone( self ):
		obj = type(self)( self.obj_name )
		for typename, ot in self.object_type_dict.items():
			if type(ot) == tuple:
				v = ot[1]
			else:
				v = getattr( self, typename )
			v = self.clone_obj( v )
			setattr( obj, typename, v )
		return obj

	def clone_obj( self, obj ):
		if obj == None:
			return obj
		if isinstance( obj, KicadObj ):
			return obj.clone()
		t = type(obj)
		if t in ( int, str ):
			return obj
		if t is list:
			l = []
			for o in obj:
				l.append( self.clone_obj( o ) )
			return l
		raise Exception( "don't know how to clone a '%s'." % type(obj).__name__ )

	def tag_start( self, factory, name ):
		if name in self.object_type_dict:
			ot = self.object_type_dict[name]
			if type(ot) is tuple:
				obj = ot[2]( name )
				# ot[1].append( obj )
				return obj
			else:
				return ot
		KicadObj.tag_start( self, factory, name )

	def tag_end( self, factory, obj ):
		name = obj.obj_name
		if name in self.object_type_dict:
			ot = self.object_type_dict[name]
			if type(ot) is tuple:
				ot[1].append( obj )

	def set( self, name, value ):
		if name in self.object_type_dict:
			ot = self.object_type_dict[name]
			check_instance( ot, KicadObj )
			self.object_type_dict[name] = value
		setattr( self, name, value )

	def write_contents( self, writer ):
		self.write_objects( writer, self.object_type_names, False )

	def write_obj( self, writer, name, newline=True ):
		ot = self.object_type_dict[name]
		if type(ot) is tuple:
			for obj in ot[1]:
				obj.write( writer )
				if newline:
					writer.newline()
		else:
			obj = getattr( self, name )
			if obj.is_set():
				obj.write( writer )
				if newline:
					writer.newline()

	def write_objects( self, writer, keys, newline=True ):
		for key in keys:
			if type(key) is bool:
				newline = key
			else:
				self.write_obj( writer, key, newline )


class Int( KicadObj ):

	def __init__( self, name, value=None ):
		KicadObj.__init__( self, name )
		self.value = value

	def tag_value( self, factory, value ):
		self.value = int(value)

	def clone( self ):
		return Int( self.obj_name, self.value )

	def is_set( self ):
		return self.value != None

	def write_contents( self, writer ):
		writer.write_int( self.value )

	def __str__( self ):
		if self.value == None:
			return "<<<NONE>>>"
		return "%d" % self.value


class Float( KicadObj ):

	def __init__( self, name, value=None ):
		KicadObj.__init__( self, name )
		self.value = value

	def tag_value( self, factory, value ):
		self.value = float(value)

	def clone( self ):
		return Float( self.obj_name, self.value )

	def is_set( self ):
		return self.value != None

	def write_contents( self, writer ):
		writer.write_float( self.value )

	def __str__( self ):
		if self.value == None:
			return "<<<NONE>>>"
		return "%f" % self.value


class Text( KicadObj ):

	def __init__( self, name, value=None ):
		KicadObj.__init__( self, name )
		self.value = value

	def tag_value( self, factory, value ):
		self.value = value

	def clone( self ):
		return Text( self.obj_name, self.value )

	def is_set( self ):
		return self.value != None

	def write_contents( self, writer ):
		writer.write_text( self.value )

	def __str__( self ):
		if self.value == None:
			return "<<<NONE>>>"
		return self.value


class Bool( KicadObj ):

	def __init__( self, name ):
		KicadObj.__init__( self, name )
		self.value = None

	def tag_value( self, factory, value ):
		if value == "true":
			self.value = True
		elif value == "false":
			self.value = False
		else:
			factory.parse_error( "invalid value '%s' for bool '%s'" % ( value, self.obj_name ) )

	def clone( self ):
		obj = Bool( self.obj_name )
		obj.value = self.value
		return obj

	def is_set( self ):
		return self.value != None

	def write_contents( self, writer ):
		if self.value:
			writer.write_text( "true" )
		else:
			writer.write_text( "false" )

	def __str__( self ):
		if self.value == None:
			return "<<<NONE>>>"
		if self.value:
			return "true"
		else:
			return "false"


class YesNo( KicadObj ):

	def __init__( self, name ):
		KicadObj.__init__( self, name )
		self.value = None

	def tag_value( self, factory, value ):
		if value == "yes":
			self.value = True
		elif value == "no":
			self.value = False
		else:
			factory.parse_error( "invalid value '%s' for bool '%s'" % ( value, self.obj_name ) )

	def clone( self ):
		obj = YesNo( self.obj_name )
		obj.value = self.value
		return obj

	def is_set( self ):
		return self.value != None

	def write_contents( self, writer ):
		if self.value:
			writer.write_text( "yes" )
		else:
			writer.write_text( "no" )

	def __str__( self ):
		if self.value == None:
			return "<<<NONE>>>"
		if self.value:
			return "yes"
		else:
			return "no"


class Vector( KicadObj ):

	def __init__( self, name, x=None, y=None ):
		KicadObj.__init__( self, name )
		self.x = x
		self.y = y

	def tag_value( self, factory, value ):
		if self.x == None:
			self.x = float(value)
		elif self.y == None:
			self.y = float(value)
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def clone( self ):
		return Vector( self.obj_name, self.x, self.y )

	def is_set( self ):
		return self.y != None

	def write_contents( self, writer ):
		writer.write_float( self.x )
		writer.write_float( self.y )

	def __add__( self, v ):
		return Vector( self.obj_name, self.x+v.x, self.y+v.y )

	def __sub__( self, v ):
		return Vector( self.obj_name, self.x-v.x, self.y-v.y )

	def __str__( self ):
		if self.y == None:
			return "<<<NONE>>>"
		return "%f/%f" % ( self.x, self.y )

	def unrot( self ):
		return self

	def get_angle( self ):
		return 0

	def rotate( self, angle ):
		x = self.x
		y = self.y
		if angle == 0:
			pass
		elif angle == 90:
			x, y = y, -x
		elif angle == 180:
			x, y = -x, -y
		elif angle == 270:
			x, y = -y, x
		else:
			# +++ sin/cos???
			# raise Exception( "unsupported angle" )
			ra = math.radians( angle )
			sa = math.sin( ra )
			ca = math.cos( ra )
			x, y = x * ca + y * sa, x * sa - y * ca
		return Vector( self.obj_name, x, y )


class VectorAngle( Vector ):

	def __init__( self, name, x=None, y=None, a=0 ):
		Vector.__init__( self, name, x, y )
		self.a = a

	def tag_value( self, factory, value ):
		if self.x == None:
			self.x = float(value)
		elif self.y == None:
			self.y = float(value)
		#elif self.a == None:
		#	self.a = float(value)
		#else:
		#	factory.parse_error( "too many values for '%s'" % self.obj_name )
		else:
			self.a = float(value)

	def clone( self ):
		return VectorAngle( self.obj_name, self.x, self.y, self.a )

	def write_contents( self, writer ):
		writer.write_float( self.x )
		writer.write_float( self.y )
		if self.a != None and self.a != 0:
			writer.write_float( self.a )

	def __add__( self, v ):
		# +++ what about the angle???
		return VectorAngle( self.obj_name, self.x+v.x, self.y+v.y, self.a )

	def __sub__( self, v ):
		# +++ what about the angle???
		return VectorAngle( self.obj_name, self.x-v.x, self.y-v.y, self.a )

	def __str__( self ):
		if self.y == None:
			return "<<<NONE>>>"
		if self.a == 0:
			return Vector.__str__( self )
		return "%f/%f@%f" % ( self.x, self.y, self.a )

	def unrot( self ):
		#x = self.x
		#y = self.y
		#if self.a == 0:
		#	pass
		#elif self.a == 90:
		#	x, y = y, -x
		#elif self.a == 180:
		#	x, y = -x, -y
		#elif self.a == 270:
		#	x, y = -y, x
		#else:
		#	# +++ sin/cos???
		#	raise Exception( "unsupported angle" )
		#return Vector( self.obj_name, x, y )
		return self.rotate( self.a )

	def get_angle( self ):
		return self.a or 0


class Vector3D( Vector ):

	def __init__( self, name, x=None, y=None, z=None ):
		Vector.__init__( self, name, x, y )
		self.z = z

	def tag_value( self, factory, value ):
		if self.x == None:
			self.x = float(value)
		elif self.y == None:
			self.y = float(value)
		elif self.z == None:
			self.z = float(value)
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def clone( self ):
		return Vector3D( self.obj_name, self.x, self.y, self.z )

	def write_contents( self, writer ):
		writer.write_float( self.x )
		writer.write_float( self.y )
		if self.a != None:
			writer.write_float( self.a )

	def __str__( self ):
		if self.z == None:
			return "<<<NONE>>>"
		return "%f/%f/%f" % ( self.x, self.y, self.z )


class Area( KicadObj ):

	#def __init__( self, name, v1=Vector("area-v1"), v2=Vector("area-v2") ):
	#	KicadObj.__init__( self, name )
	#	self.vector1 = v1
	#	self.vector2 = v2

	def __init__( self, name, v1=None, v2=None ):
		KicadObj.__init__( self, name )
		if v1 != None:
			self.vector1 = v1
		else:
			self.vector1 = Vector("area-v1")
		if v2 != None:
			self.vector2 = v2
		else:
			self.vector2 = Vector("area-v2")

	def tag_value( self, factory, value ):
		# self.values.append( float(value) )
		if not self.vector1.is_set():
			self.vector1.tag_value( factory, value )
		else:
			self.vector2.tag_value( factory, value )

	def clone( self ):
		a = Area( self.obj_name )
		a.vector1.x = self.vector1.x
		a.vector1.y = self.vector1.y
		a.vector2.x = self.vector2.x
		a.vector2.y = self.vector2.y
		return a

	def is_set( self ):
		return self.vector1.is_set() and self.vector2.is_set()

	def write_contents( self, writer ):
		writer.write_float( self.vector1.x )
		writer.write_float( self.vector1.y )
		writer.write_float( self.vector2.x )
		writer.write_float( self.vector2.y )

	def normalize( self ):
		if self.vector1.x > self.vector2.x:
			self.vector1.x, self.vector2.x = self.vector2.x, self.vector1.x
		if self.vector1.y > self.vector2.y:
			self.vector1.y, self.vector2.y = self.vector2.y, self.vector1.y
		return self

	def is_inside( self, vect ):
		return (vect.x >= self.vector1.x and
				vect.x <= self.vector2.x and
				vect.y >= self.vector1.y and
				vect.y <= self.vector2.y)

	def __add__( self, v ):
		if isinstance( v, Vector ):
			return Area( self.obj_name, self.vector1 + v, self.vector2 + v )
		elif isinstance( v, Area ):
			return Area( self.obj_name, self.vector1 + v, self.vector2 + v )
		else:
			raise Exception( "invalid class" )

	def __sub__( self, v ):
		if isinstance( v, Vector ):
			return Area( self.obj_name, self.vector1 - v, self.vector2 - v )
		elif isinstance( v, Area ):
			return Area( self.obj_name, self.vector1 - v.vector1, self.vector2 - v.vector2 )
		else:
			raise Exception( "invalid class" )

	def __str__( self ):
		return "%s..%s" % ( self.vector1, self.vector2 )


class List( KicadObj ):

	def __init__( self, name ):
		KicadObj.__init__( self, name )
		self.values = []

	def tag_value( self, factory, value ):
		self.values.append( value )

	def clone( self ):
		obj = List( self.obj_name )
		for v in self.values:
			obj.values.append( v )
		return obj

	def is_set( self ):
		return len(self.values) > 0

	def write_contents( self, writer ):
		for value in self.values:
			writer.write_text( value )

class Keepout( KicadFatObj ):
	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Text( "tracks" ),
			Text( "vias" ),
			Text( "copperpour" ),
		) )
		self.opt = None
	
	def tag_value( self, factory, value ):
		if self.opt == None:
			self.opt = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write_contents( self, writer ):
		if self.opt != None:
			writer.write_text( self.opt )
		self.write_objects( writer, (
			"tracks",
			"vias",
			"copperpour",
		), False )

	def is_set( self ):
		return self.tracks.is_set() and self.vias.is_set() and self.copperpour.is_set()

class Fill( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Int( "arc_segments" ),
			Float( "thermal_gap" ),
			Float( "thermal_bridge_width" ),
		) )
		self.opt = None

	def tag_value( self, factory, value ):
		if self.opt == None:
			self.opt = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write_contents( self, writer ):
		if self.opt != None:
			writer.write_text( self.opt )
		self.write_objects( writer, (
			"arc_segments",
			"thermal_gap",
			"thermal_bridge_width",
		), False )


class ConnectPads( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Float( "clearance" ),
		) )
		self.opt = None

	def tag_value( self, factory, value ):
		if self.opt == None:
			self.opt = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write_contents( self, writer ):
		if self.opt != None:
			writer.write_text( self.opt )
		self.write_objects( writer, (
			"clearance",
		), False )


class Zone( KicadFatObj ):

	def __init__( self, name ):
		self.polygons = []
		KicadFatObj.__init__( self, name, (
			Int( "net" ),
			Text( "net_name" ),
			Text( "layer" ),
			Text( "tstamp" ),
			List( "hatch" ),
			ConnectPads( "connect_pads" ),
			Float( "min_thickness" ),
			Keepout( "keepout" ),
			Fill( "fill" ),
			( "polygon", self.polygons, ListOfPoints ),
			( "filled_polygon", self.polygons, ListOfPoints ),
		) )

	def write_contents( self, writer ):
		self.write_objects( writer, (
			"net",
			"net_name",
			"layer",
			"tstamp",
			True,
			"hatch",
			"connect_pads",
			"min_thickness",
			"fill",
			"polygon",
		), False )
		if self.keepout.is_set():
			writer.newline()
			self.write_objects( writer, ( "keepout", ), False)

	def is_inside( self, area ):
		for poly in self.polygons:
			if poly.obj_name == "polygon":
				if not poly.is_inside( area ):
					return False
		return True

	def copy( self, trans ):
		obj = Zone( self.obj_name )
		obj.set( "net", trans.net( self.net ) )
		obj.set( "net_name", trans.net( self.net_name ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "tstamp", self.tstamp.clone() )
		obj.set( "hatch", self.hatch.clone() )
		obj.set( "connect_pads", self.connect_pads.clone() )
		obj.set( "min_thickness", self.min_thickness.clone() )
		obj.set( "keepout", self.keepout.clone() )
		obj.set( "fill", self.fill.clone() )
		for p in self.polygons:
			obj.polygons.append( p.copy( trans ) )
		return obj


class Via( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "at" ),
			Float( "size" ),
			Float( "drill" ),
			List( "layers" ),
			Int( "net" ),
			Text( "tstamp" ),
			Text( "status" ),
		) )

	def is_inside( self, area ):
		return area.is_inside( self.at )

	def copy( self, trans ):
		obj = Via( self.obj_name )
		obj.set( "at", trans.vector( self.at ) )
		obj.set( "size", self.size.clone() )
		obj.set( "drill", self.drill.clone() )
		obj.set( "layers", self.layers.clone() )
		obj.set( "net", trans.net( self.net ) )
		obj.set( "tstamp", self.tstamp.clone() )
		obj.set( "status", self.status.clone() )
		return obj


class Segment( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "start" ),
			Vector( "end" ),
			Float( "width" ),
			Text( "layer" ),
			Int( "net" ),
			Text( "tstamp" ),
			Text( "status" ),
		) )

	def is_inside( self, area ):
		return area.is_inside( self.start ) and area.is_inside( self.end )

	def copy( self, trans ):
		obj = Segment( self.obj_name )
		obj.set( "start", trans.vector( self.start ) )
		obj.set( "end", trans.vector( self.end ) )
		obj.set( "width", self.width.clone() )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "net", trans.net( self.net ) )
		obj.set( "tstamp", self.tstamp.clone() )
		obj.set( "status", self.status.clone() )
		return obj


class DimLinePoints( KicadFatObj ):

	def __init__( self, name ):
		self.lines = []
		KicadFatObj.__init__( self, name, (
			DimLinePoints( "arrow2b" ),
		) )
		self.size = None


class ListOfPoints( KicadObj ):

	def __init__( self, name ):
		self.points = []
		KicadObj.__init__( self, name )
		self.pts_started = False

	def tag_start( self, factory, name ):
		if not self.pts_started:
			self.pts_started = True
			return self
		elif name == "xy":
			obj = Vector( name )
			self.points.append( obj )
			return obj
		else:
			KicadObj.tag_start( self, factory, name )

	def tag_end( self, factory, obj ):
		pass

	def write( self, writer ):
		n = len(self.points)
		writer.begin( self.obj_name )
		if n > 2:
			writer.newline()
		writer.begin( "pts" )
		if n > 2:
			writer.newline()
		for i in range( 0, n ):
			self.points[i].write( writer )
			nl = ((i+1) % 5) == 0
			if nl:
				writer.newline()
		if n > 2 and not nl:
			writer.newline()
		writer.end()
		if n > 2:
			writer.newline()
		writer.end()
		if n > 2:
			writer.newline()

	def is_inside( self, area ):
		for p in self.points:
			if not area.is_inside( p ):
				return False
		return True

	def copy( self, trans ):
		obj = ListOfPoints( self.obj_name )
		obj.pts_started = False
		for p in self.points:
			obj.points.append( trans.vector( p ) )
		return obj


class Dimension( KicadFatObj ):

	def __init__( self, name ):
		self.lines = []
		KicadFatObj.__init__( self, name, (
			Float( "width" ),
			Text( "layer" ),
			GrText( "gr_text" ),
			Vector( "end" ),
			ListOfPoints( "feature1" ),
			ListOfPoints( "feature2" ),
			ListOfPoints( "crossbar" ),
			ListOfPoints( "arrow1a" ),
			ListOfPoints( "arrow1b" ),
			ListOfPoints( "arrow2a" ),
			ListOfPoints( "arrow2b" ),
		) )
		self.size = None

	def tag_value( self, factory, value ):
		if self.size == None:
			self.size = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def is_inside( self, area ):
		return False # +++

	def write_contents( self, writer ):
		writer.write_text( self.size )
		self.write_objects( writer, (
			"width",
			True,
			"layer",
			"gr_text",
			"feature1",
			"feature2",
			"crossbar",
			"arrow1a",
			"arrow1b",
			"arrow2a",
			"arrow2b",
		), False )


class GrCircle( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "center" ),
			Vector( "end" ),
			Text( "layer" ),
			Float( "width" ),
		) )

	def is_inside( self, area ):
		return area.is_inside( self.center )

	def copy( self, trans ):
		obj = GrCircle( self.obj_name )
		obj.set( "center", trans.vector( self.center ) )
		obj.set( "end", trans.vector( self.end ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "width", self.width.clone() )
		return obj


class GrArc( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "start" ),
			Vector( "end" ),
			Float( "angle" ),
			Text( "layer" ),
			Float( "width" ),
			Text( "tstamp" ),
		) )

	def is_inside( self, area ):
		return area.is_inside( self.start ) and area.is_inside( self.end )

	def copy( self, trans ):
		obj = GrArc( self.obj_name )
		obj.set( "start", trans.vector( self.start ) )
		obj.set( "end", trans.vector( self.end ) )
		obj.set( "angle", trans.arc_angle( self.angle ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "width", self.width.clone() )
		obj.set( "tstamp", self.tstamp.clone() )
		return obj


class GrLine( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "start" ),
			Vector( "end" ),
			Int( "angle" ),
			Text( "layer" ),
			Float( "width" ),
			Text( "tstamp" ),
		) )

	def is_inside( self, area ):
		return area.is_inside( self.start ) and area.is_inside( self.end )

	def copy( self, trans ):
		obj = GrLine( self.obj_name )
		obj.set( "start", trans.vector( self.start ) )
		obj.set( "end", trans.vector( self.end ) )
		obj.set( "angle", self.angle.clone() )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "width", self.width.clone() )
		obj.set( "tstamp", self.tstamp.clone() )
		return obj


class GrText( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			VectorAngle( "at" ),
			Text( "layer" ),
			Text( "tstamp" ),
			Effects( "effects" ),
		) )
		self.text = None

	def tag_value( self, factory, value ):
		if self.text == None:
			self.text = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write( self, writer ):
		writer.begin( self )
		writer.write_text( self.text )
		self.write_objects( writer, (
			"at", "layer",
			"tstamp",
		), False )
		writer.newline()
		self.write_obj( writer, "effects", False )
		writer.newline()
		writer.end()

	def is_inside( self, area ):
		return area.is_inside( self.at )

	def copy( self, trans ):
		obj = GrText( self.obj_name )
		obj.text = self.text
		obj.set( "at", trans.vector( self.at ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "tstamp", self.tstamp.clone() )
		obj.set( "effects", self.effects.copy( trans ) )
		return obj


class FpArc( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "start" ),
			Vector( "end" ),
			Float( "angle" ),
			Text( "layer" ),
			Float( "width" ),
		) )

	def write( self, writer ):
		writer.begin( self )
		self.write_objects( writer, (
			"start", "end", "angle",
			"layer", "width",
		), False )
		writer.end()

	def copy( self, trans ):
		obj = FpArc( self.obj_name )
		obj.set( "start", trans.add_flip_rot( self.start ) )
		obj.set( "end", trans.add_flip_rot( self.end ) )
		obj.set( "angle", trans.arc_angle( self.angle ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "width", self.width.clone() )
		return obj


class FpCircle( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "center" ),
			Vector( "end" ),
			Text( "layer" ),
			Float( "width" ),
		) )

	def write( self, writer ):
		writer.begin( self )
		self.write_objects( writer, (
			"center", "end",
			"layer", "width",
		), False )
		writer.end()

	def copy( self, trans ):
		obj = FpCircle( self.obj_name )
		obj.set( "center", trans.add_flip_rot( self.center ) )
		obj.set( "end", trans.add_flip_rot( self.end ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "width", self.width.clone() )
		return obj


class FpLine( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "start" ),
			Vector( "end" ),
			Text( "layer" ),
			Float( "width" ),
		) )

	def write( self, writer ):
		writer.begin( self )
		self.write_objects( writer, (
			"start", "end",
			"layer", "width",
		), False )
		writer.end()

	def copy( self, trans ):
		obj = FpLine( self.obj_name )
		obj.set( "start", trans.add_flip_rot( self.start ) )
		obj.set( "end", trans.add_flip_rot( self.end ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "width", self.width.clone() )
		return obj


class FpText( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			VectorAngle( "at" ),
			Text( "layer" ),
			Effects( "effects" ),
		) )
		self.kind = None
		self.name = None
		self.hide = False

	def tag_value( self, factory, value ):
		if self.kind == None:
			self.kind = value
		elif self.name == None:
			self.name = value
		elif value == "hide":
			self.hide = True
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write( self, writer ):
		writer.begin( self )
		writer.write_text( self.kind )
		writer.write_text( self.name )
		self.write_obj( writer, "at", False )
		self.write_obj( writer, "layer", False )
		if self.hide:
			writer.write_text( "hide" )
		writer.newline()
		self.write_obj( writer, "effects" )
		writer.newline()
		writer.end()

	def copy( self, trans ):
		obj = FpText( self.obj_name )
		obj.set( "kind", self.kind )
		obj.set( "name", self.name )
		obj.set( "hide", self.hide )
		obj.set( "at", trans.add_flip_rot( self.at ) )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "effects", self.effects.copy( trans ) )
		return obj


class Font( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "size" ),
			Float( "thickness" ),
			# Flag( "italic" ),
		) )
		self.italic = False

	def tag_value( self, factory, value ):
		if value == "italic":
			self.italic = True
		else:
			raise Exception( "Font.tag_value( '%s' )." % value )

	def write( self, writer ):
		writer.begin( self )
		self.write_objects( writer, ( "size", "thickness" ), False )
		if self.italic:
			writer.write_text( "italic" )
		writer.end()

	def copy( self, trans ):
		obj = Font( self.obj_name )
		obj.set( "size", self.size.clone() )
		obj.set( "thickness", self.thickness.clone() )
		obj.italic = self.italic
		return obj


class Justify( List ):

	def __init__( self, name ):
		List.__init__( self, name )

	def clone( self ):
		obj = Justify( self.obj_name )
		for v in self.values:
			obj.values.append( v )
		return obj

	def copy( self, trans ):
		obj = self.clone()
		if trans.flip:
			if "mirror" in obj.values:
				obj.values.remove( "mirror" )
			else:
				obj.values.append( "mirror" )
		return obj


class Effects( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Font( "font" ),
			Justify( "justify" ),
		) )

	def write( self, writer ):
		writer.begin( self )
		self.write_objects( writer, ( "font", "justify" ), False )
		writer.end()

	def copy( self, trans ):
		obj = Effects( self.obj_name )
		obj.set( "font", self.font.copy( trans ) )
		obj.set( "justify", self.justify.copy( trans ) )
		return obj


class Drill( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector( "offset" ),
		) )
		self.args = []

	def tag_value( self, factory, value ):
		self.args.append(value)

	def write( self, writer ):
		if len(self.args) or self.offset.is_set():
			writer.begin( self )
			for arg in self.args:
				writer.write_text( arg )
			if self.offset.is_set():
				self.write_obj( writer, "offset", False )
			writer.end()

	def copy( self, trans ):
		obj = Drill( self.obj_name )
		if self.offset.x != None:
			obj.set( "offset", trans.add_flip_rot( self.offset ) )
		for arg in self.args:
			obj.args.append(arg)
		return obj


class Pad( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			VectorAngle( "at" ),
			Vector( "size" ),
			Vector( "rect_delta" ),
			Drill( "drill" ),
			List( "layers" ),
			Float( "solder_mask_margin" ),
			Float( "clearance" ),
			Net( "net" ),
			Int( "zone_connect" ),
		) )
		self.nr = None
		self.kind = None
		self.shape = None

	def tag_value( self, factory, value ):
		if self.nr == None:
			self.nr = value
		elif self.kind == None:
			self.kind = value
		elif self.shape == None:
			self.shape = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write( self, writer ):
		writer.begin( self )
		writer.write_text( self.nr )
		writer.write_text( self.kind )
		writer.write_text( self.shape )
		self.write_objects( writer, (
			"at",
			"size",
			"drill",
			"layers",
		), False )
		if self.rect_delta.is_set():
			self.write_objects( writer, ("rect_delta",), False )
		# +++++ if net.is_set() or zone_connect.is_set()
		#if self.get_obj( "net" ).is_set() or self.get_obj( "zone_connect" ).is_set()
		#if self.obj_is_set( "net" ) or self.obj_is_set( "zone_connect" ):
		if self.clearance.is_set() or self.net.is_set() or self.zone_connect.is_set():
			writer.newline()
			self.write_objects( writer, (
				"solder_mask_margin",
				"clearance",
				"net",
				"zone_connect",
			), False )
		writer.end()

	def copy( self, trans ):
		obj = Pad( self.obj_name )
		obj.set( "nr", self.nr )
		obj.set( "kind", self.kind )
		obj.set( "shape", self.shape )
		obj.set( "at", trans.add_flip_rot( self.at ) )
		obj.set( "size", self.size.clone() )
		obj.set( "drill", self.drill.copy( trans ) )
		obj.set( "rect_delta", self.rect_delta.clone() )
		for layer in self.layers.values:
			obj.layers.values.append( trans.layer( layer ) )
		obj.set( "clearance", self.clearance.clone() )
		obj.set( "net", trans.net( self.net ) )
		obj.set( "zone_connect", self.zone_connect.clone() )
		return obj


class NamedVector3D( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Vector3D( "xyz" ),
		) )


class Model( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			NamedVector3D( "at" ),
			NamedVector3D( "scale" ),
			NamedVector3D( "rotate" ),
		) )
		self.filename = None

	def tag_value( self, factory, value ):
		if self.filename == None:
			self.filename = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write( self, writer ):
		writer.begin( self )
		self.write_objects( writer, ( "font", "justify" ), False )
		writer.end()

	def copy( self, trans ):
		obj = Model( self.obj_name )
		obj.set( "font", self.font.copy( trans ) )
		obj.set( "justify", self.justify.copy( trans ) )
		return obj


class Module( KicadFatObj ):

	def __init__( self, name ):
		self.graphics = []
		self.pads = []
		KicadFatObj.__init__( self, name, (
			Text( "layer" ),
			Text( "tedit" ),
			Text( "tstamp" ),
			VectorAngle( "at" ),
			Text( "descr" ),
			Text( "tags" ),
			Text( "path" ),
			Float( "solder_mask_margin" ),
			Float( "clearance" ),
			Text( "attr" ),
			( "fp_text", self.graphics, FpText ),
			( "fp_line", self.graphics, FpLine ),
			( "fp_circle", self.graphics, FpCircle ),
			( "fp_arc", self.graphics, FpArc ),
			( "pad", self.pads, Pad ),
			Model( "model" ),
		) )
		self.name = None
		self.locked = None

	def tag_value( self, factory, value ):
		if self.name == None:
			self.name = value
		elif self.locked == None:
			self.locked = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write( self, writer ):
		writer.begin( self )
		writer.write_text( self.name )
		if self.locked != None:
			writer.write_text( self.locked )
		self.write_objects( writer, (
			"layer",
			"tedit",
			True,
			"tstamp",
			"at",
			"descr",
			"tags",
			"path",
			"solder_mask_margin",
			"clearance",
			"fp_text",
			"pad",
		), False )
		writer.end()
		writer.newline( True )
		writer.newline( True )

	def is_inside( self, area ):
		#p = False
		#if self.name == "p_gen_conn:P_16_EDGE_PIN_HEADER":
		#	p = True
		#if p:
		#	print( "%s\nArea: %s\nAt:   %s" % ( self.name, area, self.at ) )
		area = area - self.at
		#if p:
		#	print( "A2:   %s" % area )
		for pad in self.pads:
			# +++++ I believe unrot() is not needed here, at least removing
			# it solves a bug. needs more testing...
			# if not area.is_inside( pad.at.unrot() ):
			#if p:
			#	print( "Pad:  %s  (%s)" % ( pad.at, pad.nr ) )
			pat = pad.at.rotate( self.at.get_angle() )
			#if p:
			#	print( "pat:  %s" % pat )
			# if not area.is_inside( pad.at ):
			if not area.is_inside( pat ):
				#if p:
				#	print( "False" )
				return False
		#if p:
		#	print( "True" )
		return True

	def copy( self, trans ):
		obj = Module( self.obj_name )
		obj.set( "name", self.name )
		obj.set( "locked", self.locked )
		obj.set( "layer", trans.layer( self.layer ) )
		obj.set( "tedit", self.tedit.clone() )
		obj.set( "tstamp", self.tstamp.clone() )
		obj.set( "at", trans.vector( self.at ) )
		obj.set( "descr", self.descr.clone() )
		obj.set( "tags", self.tags.clone() )
		obj.set( "path", self.path.clone() )
		for graphic in self.graphics:
			obj.graphics.append( graphic.copy( trans ) )
		for pad in self.pads:
			obj.pads.append( pad.copy( trans ) )
		return obj


class NetClass( KicadFatObj ):

	def __init__( self, name ):
		self.nets = []
		KicadFatObj.__init__( self, name, (
			Float( "clearance" ),
			Float( "trace_width" ),
			Float( "via_dia" ),
			Float( "via_drill" ),
			Float( "uvia_dia" ),
			Float( "uvia_drill" ),
			Float( "diff_pair_gap" ), # after 4.0.5
			Float( "diff_pair_width" ), # after 4.0.5
			( "add_net", self.nets, Text ),
		) )
		self.name = None
		self.comment = None

	def tag_value( self, factory, value ):
		if self.name == None:
			self.name = value
		elif self.comment == None:
			self.comment = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write( self, writer ):
		writer.begin( self )
		writer.write_text( self.name )
		writer.write_text( self.comment )
		writer.newline()
		if writer.compat( compat_4_0_5 ):
			self.write_objects( writer, (
				"clearance", "trace_width", "via_dia", "via_drill",
				"uvia_dia", "uvia_drill", "add_net",
			) )
		else:
			self.write_objects( writer, (
				"clearance", "trace_width", "via_dia", "via_drill",
				"uvia_dia", "uvia_drill", "diff_pair_gap", "diff_pair_width",
				"add_net",
			) )
		writer.newline()
		writer.end()
		writer.newline()
		writer.newline( True )


class Net( KicadObj ):

	def __init__( self, name, nr=None, netname=None ):
		KicadObj.__init__( self, name )
		self.nr = nr
		self.name = netname

	def tag_value( self, factory, value ):
		if self.nr == None:
			self.nr = int(value)
		elif self.name == None:
			self.name = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def is_set( self ):
		return self.name != None

	def write( self, writer ):
		writer.begin( self )
		writer.write_int( self.nr )
		writer.write_text( self.name )
		writer.end()

	def clone( self ):
		return self

	def copy( self, trans ):
		return self


class PcbPlotParams( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Text( "layerselection" ),
			Bool( "usegerberextensions" ),
			Bool( "excludeedgelayer" ),
			Float( "linewidth" ),
			Bool( "plotframeref" ),
			Bool( "viasonmask" ),
			Int( "mode" ),
			Bool( "useauxorigin" ),
			Int( "hpglpennumber" ),
			Int( "hpglpenspeed" ),
			Int( "hpglpendiameter" ),
			Int( "hpglpenoverlay" ),
			Bool( "psnegative" ),
			Bool( "psa4output" ),
			Bool( "plotreference" ),
			Bool( "plotvalue" ),
			Bool( "plotinvisibletext" ),
			Bool( "padsonsilk" ),
			Bool( "subtractmaskfromsilk" ),
			Int( "outputformat" ),
			Bool( "mirror" ),
			Int( "drillshape" ),
			Int( "scaleselection" ),
			Text( "outputdirectory" ),
		) )

	def init_defaults( self ):
		self.layerselection.value = "0x010fc_80000001"
		self.usegerberextensions.value = False
		self.excludeedgelayer.value = True
		self.linewidth.value = 0.1
		self.plotframeref.value = False
		self.viasonmask.value = False
		self.mode.value = 1
		self.useauxorigin.value = False
		self.hpglpennumber.value = 1
		self.hpglpenspeed.value = 20
		self.hpglpendiameter.value = 15
		self.hpglpenoverlay.value = 2
		self.psnegative.value = False
		self.psa4output.value = False
		self.plotreference.value = True
		self.plotvalue.value = True
		self.plotinvisibletext.value = False
		self.padsonsilk.value = False
		self.subtractmaskfromsilk.value = False
		self.outputformat.value = 1.0
		self.mirror.value = False
		self.drillshape.value = 0
		self.scaleselection.value = 1.0
		self.outputdirectory.value = "production"

	def write( self, writer ):
		writer.begin( self )
		writer.newline()
		self.write_objects( writer, (
			"layerselection", "usegerberextensions", "excludeedgelayer",
			"linewidth", "plotframeref", "viasonmask", "mode",
			"useauxorigin", "hpglpennumber", "hpglpenspeed",
			"hpglpendiameter", "hpglpenoverlay", "psnegative",
			"psa4output", "plotreference", "plotvalue",
			"plotinvisibletext", "padsonsilk", "subtractmaskfromsilk",
			"outputformat", "mirror", "drillshape", "scaleselection",
			"outputdirectory",
		) )
		# writer.newline()
		writer.end()


class Setup( KicadFatObj ):

	def __init__( self, name ):
		self.user_trace_width = []
		self.user_via = []
		KicadFatObj.__init__( self, name, (
			Float( "last_trace_width" ),
			( "user_trace_width", self.user_trace_width, Float ),
			( "user_via", self.user_via, Vector ),
			Float( "trace_clearance" ),
			Float( "zone_clearance" ),
			YesNo( "zone_45_only" ),
			Float( "trace_min" ),
			Float( "segment_width" ),
			Float( "edge_width" ),
			Float( "via_size" ),
			Float( "via_drill" ),
			Float( "via_min_size" ),
			Float( "via_min_drill" ),
			Float( "uvia_size" ),
			Float( "uvia_drill" ),
			YesNo( "uvias_allowed" ),
			Float( "uvia_min_size" ),
			Float( "uvia_min_drill" ),
			Float( "pcb_text_width" ),
			Vector( "pcb_text_size" ),
			Float( "mod_edge_width" ),
			Vector( "mod_text_size" ),
			Float( "mod_text_width" ),
			Vector( "pad_size" ),
			Float( "pad_drill" ),
			Float( "pad_to_mask_clearance" ),
			Float( "pad_to_paste_clearance" ),
			Vector( "aux_axis_origin" ),
			Vector( "grid_origin" ),
			Text( "visible_elements" ),
			PcbPlotParams( "pcbplotparams" ),
		) )

	def init_defaults( self ):
		self.last_trace_width.value = 0.25
		self.user_trace_width.append( Float( "user_trace_width", 0.25 ) )
		self.user_via.append( Vector( "user_via", 0.6, 0.3 ) )
		self.trace_clearance.value = 0.2
		self.zone_clearance.value = 0.3
		self.zone_45_only.value = False
		self.trace_min.value = 0.2
		self.segment_width.value = 0.1
		self.edge_width.value = 0.1
		self.via_size.value = 0.6
		self.via_drill.value = 0.4
		self.via_min_size.value = 0.4
		self.via_min_drill.value = 0.3
		self.uvia_size.value = 0.3
		self.uvia_drill.value = 0.1
		self.uvias_allowed.value = False
		self.uvia_min_size.value = 0.2
		self.uvia_min_drill.value = 0.1
		self.pcb_text_width.value = 0.3
		self.pcb_text_size.x = 1.5
		self.pcb_text_size.y = 1.5
		self.mod_edge_width.value = 0.15
		self.mod_text_size.x = 1.0
		self.mod_text_size.y = 1.0
		self.mod_text_width.value = 0.15
		self.pad_size.x = 1.5
		self.pad_size.y = 1.5
		self.pad_drill.value = 0.6
		self.pad_to_mask_clearance.value = 0.0
		self.pad_to_paste_clearance.value = 0.0
		self.aux_axis_origin.x = 0.0
		self.aux_axis_origin.y = 0.0
		self.grid_origin.x = 0.0
		self.grid_origin.y = 0.0
		self.visible_elements.value = "FFFFFF7F"
		self.pcbplotparams.init_defaults()

	def write( self, writer ):
		writer.begin( self )
		writer.newline()
		self.write_objects( writer, (
			"last_trace_width", "user_trace_width", "user_via",
			"trace_clearance", "zone_clearance", "zone_45_only",
			"trace_min", "segment_width", "edge_width", "via_size",
			"via_drill", "via_min_size", "via_min_drill", "uvia_size",
			"uvia_drill", "uvias_allowed", "uvia_min_size",
			"uvia_min_drill", "pcb_text_width", "pcb_text_size",
			"mod_edge_width", "mod_text_size", "mod_text_width",
			"pad_size", "pad_drill", "pad_to_mask_clearance",
			"pad_to_paste_clearance", "aux_axis_origin", "grid_origin",
			"visible_elements", "pcbplotparams"
		) )
		writer.newline()
		writer.end()


class Layer( KicadObj ):

	def __init__( self, nr, name=None, kind=None, hide=None ):
		KicadObj.__init__( self, nr )
		self.nr = int(nr)
		self.name = name
		self.kind = kind
		self.hide = hide

	def tag_value( self, factory, value ):
		if self.name == None:
			self.name = value
		elif self.kind == None:
			self.kind = value
		elif self.hide == None:
			self.hide = value
		else:
			factory.parse_error( "too many values for '%s'" % self.obj_name )

	def write( self, writer ):
		writer.begin( "%d" % self.nr )
		writer.write_text( self.name )
		writer.write_text( self.kind )
		writer.end()


class Layers( KicadObj ):

	def __init__( self, name ):
		KicadObj.__init__( self, name )
		self.layers = {}
		self.names = {}
		self.internal_layer_count = 0

	def clone( self ):
		obj = Layers( self.obj_name )
		obj.layers = self.layers.copy()
		obj.names = self.names.copy()
		for layer in self.layers.values():
			self.layer_added( layer )
		return obj

	def tag_start( self, factory, name ):
		obj = Layer( name )
		self.layers[obj.nr] = obj
		return obj

	def tag_end( self, factory, obj ):
		self.names[obj.name] = obj
		self.layer_added( obj )

	def init_defaults( self ):
		for nr, name, kind in (
			( 0, "F.Cu", "signal" ),
			( 31, "B.Cu", "signal" ),
			( 32, "B.Adhes", "user" ),
			( 33, "F.Adhes", "user" ),
			( 34, "B.Paste", "user" ),
			( 35, "F.Paste", "user" ),
			( 36, "B.SilkS", "user" ),
			( 37, "F.SilkS", "user" ),
			( 38, "B.Mask", "user" ),
			( 39, "F.Mask", "user" ),
			( 40, "Dwgs.User", "user" ),
			( 41, "Cmts.User", "user" ),
			( 42, "Eco1.User", "user" ),
			( 43, "Eco2.User", "user" ),
			( 44, "Edge.Cuts", "user" ),
			( 45, "Margin", "user" ),
			( 46, "B.CrtYd", "user" ),
			( 47, "F.CrtYd", "user" ),
			( 48, "B.Fab", "user" ),
			( 49, "F.Fab", "user" ),
		):
			obj = Layer( nr, name, kind )
			self.layers[obj.nr] = obj
			self.names[obj.name] = obj
			self.layer_added( obj )

	def layer_added( self, obj ):
		if obj.name.startswith( "In" ) and obj.name.endswith( ".Cu" ):
			n = int( obj.name[2:-3] )
			if n > self.internal_layer_count:
				self.internal_layer_count = n

	def get_internal_layer_count( self ):
		return self.internal_layer_count

	def write( self, writer ):
		writer.begin( self )
		writer.newline()
		# nrs = self.layers.keys()[:]
		nrs = list( self.layers.keys() )
		nrs.sort()
		for nr in nrs:
			self.layers[nr].write( writer )
			writer.newline()
		writer.end()
		writer.newline()


class TitleBlock( KicadFatObj ):

	def __init__( self, name ):
		self.comments = [ None, None, None, None ]
		KicadFatObj.__init__( self, name, (
			Text( "title" ),
			Text( "date" ),
			Text( "rev" ),
			Text( "company" ),
			( "comment", self.comments, List ),
		) )

	def append( self, obj ):
		i = int(obj.values[0])
		self.comments[i-1] = obj.values[1]

	def write( self, writer ):
		writer.begin( self )
		writer.newline()
		self.write_obj( writer, "title" )
		self.write_obj( writer, "rev" )
		self.write_obj( writer, "company" )
		for i in range( 0, 4 ):
			if self.comments[i] != None:
				writer.begin( "comment" )
				writer.write_int( i+1 )
				writer.write_text( self.comments[i] )
				writer.end()
				writer.newline()
		writer.newline()
		writer.end()


class General( KicadFatObj ):

	def __init__( self, name ):
		KicadFatObj.__init__( self, name, (
			Int( "links" ),
			Int( "no_connects" ),
			Area( "area" ),
			Float( "thickness" ),
			Int( "drawings" ),
			Int( "tracks" ),
			Int( "zones" ),
			Int( "modules" ),
			Int( "nets" ),
		) )

	def init_defaults( self ):
		self.links.value = 0
		self.no_connects.value = 0
		self.area.vector1.x = 0.0
		self.area.vector1.y = 0.0
		self.area.vector2.x = 0.0
		self.area.vector2.y = 0.0
		self.thickness.value = 1.6
		self.drawings.value = 0
		self.tracks.value = 0
		self.zones.value = 0
		self.modules.value = 0
		self.nets.value = 0

	def write( self, writer ):
		# maybe these values have to be updated before writing? +++++
		writer.begin( self )
		writer.newline()
		self.write_objects( writer, ( "links", "no_connects", "area", "thickness", "drawings", "tracks", "zones", "modules", "nets" ) )
		writer.newline()
		writer.end()
		writer.newline()


class NetsList:

	def __init__( self ):
		self.by_nr = {}
		self.by_name = {}
		self.max_nr = -1

	def append( self, obj ):
		self.by_nr[obj.nr] = obj
		self.by_name[obj.name] = obj
		if obj.nr > self.max_nr:
			self.max_nr = obj.nr

	def create( self, name ):
		if self.max_nr < 0:
			self.append( Net( "net", 0, "" ) )
		net = Net( "net", self.max_nr+1, name )
		self.append( net )
		return net.nr

	def __iter__( self ):
		def elements( d ):
			l = list( d.keys() )
			l.sort()
			for k in l:
				yield d[k]
		return elements( self.by_nr )


class KicadPcb( KicadFatObj ):

	def __init__( self, name, template=None ):
		self.nets = NetsList()
		self.net_classes = []
		self.modules = []
		self.graphics = []
		self.segments = []
		self.zones = []
		if template == None:
			version = Int( "version" )
			host = List( "host" )
			general = General( "general" )
			page = List( "page" )
			title_block = TitleBlock( "title_block" )
			layers = Layers( "layers" )
			setup = Setup( "setup" )
		else:
			version = template.version.clone()
			host = template.host.clone()
			general = template.general.clone()
			page = template.page.clone()
			title_block = template.title_block.clone()
			layers = template.layers.clone()
			setup = template.setup.clone()
		KicadFatObj.__init__( self, name, (
			version,
			host,
			general,
			page,
			title_block,
			layers,
			setup,
			( "net", self.nets, Net ),
			( "net_class", self.net_classes, NetClass ),
			( "module", self.modules, Module ),
			( "gr_circle", self.graphics, GrCircle ),
			( "gr_arc", self.graphics, GrArc ),
			( "gr_line", self.graphics, GrLine ),
			( "gr_text", self.graphics, GrText ),
			( "dimension", self.graphics, Dimension ),
			( "segment", self.segments, Segment ),
			( "via", self.segments, Via ),
			( "zone", self.zones, Zone ),
		) )

	def finished_loading( self ):
		pass

	def get_internal_layer_count( self ):
		return self.layers.get_internal_layer_count()

	def write( self, writer ):
		writer.begin( self )
		self.write_obj( writer, "version", False )
		self.write_obj( writer, "host" )
		writer.newline( True )
		self.write_obj( writer, "general" )
		writer.newline( True )
		self.write_obj( writer, "page" )
		self.write_obj( writer, "title_block" )
		writer.newline( True )
		self.write_obj( writer, "layers" )
		writer.newline( True )
		self.write_obj( writer, "setup" )
		writer.newline( True )
		self.write_obj( writer, "net" )
		writer.newline( True )
		self.write_obj( writer, "net_class" )
		# writer.newline( True )
		self.write_obj( writer, "module" )
		self.write_obj( writer, "gr_line" )
		writer.newline( True )
		self.write_obj( writer, "segment" )
		writer.newline( True )
		self.write_obj( writer, "zone" )
		writer.end()
		writer.newline()

	def copy( self, trans, dst_pcb ):
		for obj in self.modules[:]:
			if trans.accepts_layer( obj.layer ):
				if obj.is_inside( trans.src_area ):
					dst_pcb.modules.append( obj.copy( trans ) )
		for obj in self.graphics[:]:
			if trans.accepts_layer( obj.layer ):
				if obj.is_inside( trans.src_area ):
					dst_pcb.graphics.append( obj.copy( trans ) )
		for obj in self.segments[:]:
			if isinstance( obj, Segment ):
				if trans.accepts_layer( obj.layer ):
					if obj.is_inside( trans.src_area ):
						dst_pcb.segments.append( obj.copy( trans ) )
			else:
				if trans.accepts_layers( obj.layers ):
					if obj.is_inside( trans.src_area ):
						dst_pcb.segments.append( obj.copy( trans ) )
		for obj in self.zones[:]:
			if trans.accepts_layer( obj.layer ):
				if obj.is_inside( trans.src_area ):
					dst_pcb.zones.append( obj.copy( trans ) )


class Writer:

	def __init__( self, filename, compat_index ):
		self.filename = filename
		self.compat_index = compat_index
		print( "writing %s..." % self.filename )
		self.ofd = open( filename, "w" )
		self.indent = ""
		self.need_indent = True

	def compat( self, compat_index ):
		return compat_index >= self.compat_index

	def close( self ):
		self.ofd.close()
		print( "wrote %s." % self.filename )

	def begin( self, name ):
		if isinstance( name, KicadObj ):
			name = name.obj_name
		if not self.do_indent():
			self.ofd.write( " " )
		self.ofd.write( "(%s" % name )
		self.indent += "  "

	def end( self ):
		self.indent = self.indent[2:]
		self.do_indent()
		self.ofd.write( ")" )

	def newline( self, double=False ):
		if double or not self.need_indent:
			self.ofd.write( "\n" )
			self.need_indent = True

	def do_indent( self ):
		if self.need_indent:
			self.need_indent = False
			self.ofd.write( self.indent )
			return True
		return False

	def write_int( self, value ):
		self.ofd.write( " %d" % value )

	def write_float( self, value ):
		value = " %f" % value
		if "." in value:
			value = value.rstrip( "0" ).rstrip( "." )
		self.ofd.write( value )

	def write_text( self, value ):
		#if value == "" or ' ' in value or '\t' in value:
		#	value = "\"%s\"" % value.replace( "\"", "\\\"" )
		quote = False
		if value == "" or "\\n" in value:
			quote = True
		else:
			for c in value:
				if c in " \t()":
					quote=True
					break
		if quote:
			value = "\"%s\"" % value
		self.ofd.write( " %s" % value )


quote_chars = {
	"n": "\n",
	"r": "\r",
	"t": "\t",
}

def unquote( s ):
	i = s.find( '\\' )
	while i >= 0:
		j = i + 1
		if j >= len(s):
			# trailing single backslash
			break
		c = s[j]
		if c in quote_chars:
			c = quote_chars[c]
		s = s[:i] + c + s[j+1:]
		i = s.find( '\\', i )
	return s


class ParseError( Exception ):

	def __init__( self, msg, factory ):
		self.msg = msg
		self.factory = factory
		self.args = ( factory.filename, factory.line, factory.column, msg )

	def __repr__( self ):
		return "ParseError(%r, %r, %r, %r)" % self.args

	def __str__( self ):
		return "%s:%d.%d: %s" % self.args


class KicadFactory( KicadObj ):

	def __init__( self ):
		KicadObj.__init__( self, "KicadFactory" )
		self.filename = ""
		self.line = 0
		self.column = 0

	def parse_file( self, filename ):
		self.filename = filename
		print( "loading %s." % filename )
		ifd = open( self.filename, "r" )
		data = ifd.read( 16*1024*1024 )
		ifd.close()
		print( "parsing %s." % filename )
		obj = self.parse_data( data )
		print( "parsed %s." % filename )
		return obj

	def parse_print( self, s ):
		x = False
		# x = True
		if x:
			print( s )

	def parse_data( self, data ):
		l = len(data)
		i = 0
		self.line = 1
		lineoff = 0
		objs = [ self ]
		while i < l:
			c = data[i]
			if c == '(':
				# start of a tuple
				i += 1
				start = i
				while i < l and not data[i] in " \t\r\n()":
					i += 1
				self.column = i-lineoff
				if i == start:
					self.parse_error( "no word at start of tuple" )
				name = data[start:i]
				self.parse_print( "(" + name )
				obj = objs[-1].tag_start( self, name )
				if obj == None:
					raise ParseError( "tag_start for '%s' in '%s' returned None" % ( name, objs[-1].obj_name ), self )
				objs.append( obj )
			elif c == ')':
				# end of a tuple
				self.column = i-lineoff
				self.parse_print( ")" )
				obj = objs.pop()
				objs[-1].tag_end( self, obj )
				i += 1
			elif c == '"':
				# start of a quoted string
				i += 1
				start = i
				c = ""
				while i < l and ( data[i] != '"' or c == '\\' ):
					if data[i] == '\n':
						self.line += 1
						lineoff = i
					c = data[i]
					i += 1
				if i >= l:
					self.column = i-lineoff
					self.parse_error( "unterminated quoted string" )
				value = data[start:i]
				self.parse_print( "  '%s'" % value )
				self.column = i-lineoff
				objs[-1].tag_value( self, value )
				i += 1
			elif not c.isspace():
				# start of a word
				start = i
				i += 1
				while i < l and not data[i] in " \t\r\n()":
					i += 1
				value = data[start:i]
				self.parse_print( "  '%s'" % value )
				self.column = i-lineoff
				objs[-1].tag_value( self, value )
			else:
				# skip whitespace
				while i < l and data[i].isspace():
					if data[i] == '\n':
						self.line += 1
						lineoff = i
					i += 1
		return self.object

	def parse_error( self, text ):
		raise ParseError( text, self )

	def tag_start( self, factory, name ):
		if name == "kicad_pcb":
			self.object = KicadPcb( name )
			return self.object
		raise ParseError( "unterminated quoted string", self.line, i-lineoff )

	def tag_end( self, factory, obj ):
		obj.finished_loading()

	def create_pcb( self ):
		pcb = KicadPcb( "kicad_pcb" )
		pcb.version.value = 4
		v = "%s-r%s" % ( __version__, __revision__ )
		pcb.host.values = [ "panelize.py", v ]
		pcb.general.init_defaults()
		pcb.page.value = "A4"
		pcb.layers.init_defaults()
		pcb.setup.init_defaults()
		return pcb


class Transformer:

	def __init__( self ):
		self.src_pcb = None
		self.src_area = None
		self.dst_pcb = None
		self.dst_vector = None
		self.rotate = None
		self.flip = None
		self.clone_nets = True
		self.swap_internal_layers = False
		self.layer_filter_list = None
		self.layer_filter_include = False
		#
		self.net_pfx_nr = 0
		#
		self.dx  = 0
		self.dy  = 0
		self.fxx = 0
		self.fxy = 0
		self.fyx = 0
		self.fyy = 0
		self.da  = 0
		self.fa  = 1

	def exclude_layer( self, name ):
		if self.layer_filter_list == None or self.layer_filter_include:
			self.layer_filter_include = False
			self.layer_filter_list = set()
		self.layer_filter_list.add( name )

	def include_layer( self, name ):
		if self.layer_filter_list == None or not self.layer_filter_include:
			self.layer_filter_include = True
			self.layer_filter_list = set()
		self.layer_filter_list.add( name )

	def include_all_layers( self ):
		self.layer_filter_list = None

	def set_src_pcb( self, pcb ):
		self.src_pcb = pcb
		if  self.dst_pcb == None:
			self.dst_pcb = pcb

	def set_dst_pcb( self, pcb ):
		self.dst_pcb = pcb

	def create_template( self ):
		if self.src_pcb == None:
			return False
		self.dst_pcb = KicadPcb( self.src_pcb.obj_name, self.src_pcb )
		return True

	def set_src_area( self, area ):
		self.src_area = area.clone().normalize()

	def set_dst_vector( self, vect ):
		self.dst_vector = vect

	def set_rotate( self, rotate ):
		self.rotate = rotate

	def set_flip( self, flip ):
		self.flip = flip

	def set_swap_internal_layers( self, swap_internal_layers ):
		self.swap_internal_layers = swap_internal_layers

	def recalculate( self ):
		self.dx  = 0
		self.dy  = 0
		self.fxx = 0
		self.fxy = 0
		self.fyx = 0
		self.fyy = 0
		self.da  = self.rotate
		self.fa  = 1

		if self.flip:
			self.fa  = -1
		x = None
		y = None
		if self.rotate == 0:
			if not self.flip:
				# +x  --\  +x
				# y   --/  y
				self.fxx = 1
				self.fyy = 1
				x = self.src_area.vector1.x
				y = self.src_area.vector1.y
			else:
				# +x  --\  x+
				# y   --/   y
				self.fxx = -1
				self.fyy = 1
				x = self.src_area.vector2.x
				y = self.src_area.vector1.y
		elif self.rotate == 90:
			if not self.flip:
				# +x  --\  y+
				# y   --/   x
				self.fyx = 1
				self.fxy = -1
				x = self.src_area.vector1.x
				y = self.src_area.vector2.y
			else:
				# +x  --\   x
				# y   --/  y+
				self.fyx = 1
				self.fxy = 1
				x = self.src_area.vector1.x
				y = self.src_area.vector1.y
		elif self.rotate == 180:
			if not self.flip:
				# +x  --\   y
				# y   --/  x+
				self.fxx = -1
				self.fyy = -1
				x = self.src_area.vector2.x
				y = self.src_area.vector2.y
			else:
				# +x  --\  y
				# y   --/  +x
				self.fxx = 1
				self.fyy = -1
				x = self.src_area.vector1.x
				y = self.src_area.vector2.y
		elif self.rotate == 270:
			if not self.flip:
				# +x  --\  x
				# y   --/  +y
				self.fyx = -1
				self.fxy = 1
				x = self.src_area.vector2.x
				y = self.src_area.vector1.y
			else:
				# +x  --\   x
				# y   --/  y+
				self.fyx = -1
				self.fxy = -1
				x = self.src_area.vector2.x
				y = self.src_area.vector2.y
		else:
			raise Exception( "invalid angle" )
		v = self.dst_vector - self.vector( Vector( "", x, y ) )
		self.dx = v.x
		self.dy = v.y

	def copy( self ):
		self.src_pcb.copy( self, self.dst_pcb )
		self.net_pfx_nr += 1

	def is_inside( self, vect ):
		return self.src_area.is_inside( vect )

	def rel_vector( self, vect ):
		check_instance( vect, Vector )
		x = self.fxx * vect.x + self.fxy * vect.y
		y = self.fyx * vect.x + self.fyy * vect.y
		if isinstance( vect, VectorAngle ):
			a = None
			if vect.a != None:
				a = self.fa * (vect.a - self.da)
				a = (a + 720) % 360
			return VectorAngle( vect.obj_name, x, y, a )
		return Vector( vect.obj_name, x, y )

	def vector( self, vect ):
		check_instance( vect, Vector )
		x = self.fxx * vect.x + self.fxy * vect.y + self.dx
		y = self.fyx * vect.x + self.fyy * vect.y + self.dy
		if isinstance( vect, VectorAngle ):
			a = None
			if vect.a != None:
				a = self.fa * (vect.a - self.da)
				a = (a + 720) % 360
			return VectorAngle( vect.obj_name, x, y, a )
		return Vector( vect.obj_name, x, y )

	def add_rotation( self, vect ):
		check_instance( vect, Vector )
		if not isinstance( vect, VectorAngle ):
			return vect.clone()
		a = self.fa * (vect.a - self.da)
		a = (a + 720) % 360
		return VectorAngle( vect.obj_name, vect.x, vect.y, a )

	def add_flip_rot( self, vect ):
		check_instance( vect, Vector )
		v = vect.clone()
		if isinstance( vect, VectorAngle ):
			a = self.fa * (vect.a - self.da)
			a = (a + 720) % 360
			v.a = a
		if self.flip:
			v.x = -v.x
		return v

	def arc_angle( self, f ):
		if not isinstance( f, Float ):
			raise Exception( "unexpected class" )
		f = f.clone()
		if self.flip:
			f.value = -f.value
		return f

	def accepts_layer( self, layer ):
		if self.layer_filter_list == None:
			return True
		name = None
		if isinstance( layer, Text ):
			name = layer.value
		else:
			raise Exception( "unhandled class" )
		if name in self.layer_filter_list:
			return self.layer_filter_include
		return not self.layer_filter_include

	def accepts_layers( self, layers ):
		if self.layer_filter_list == None:
			return True
		for layer in layers.values:
			if layer in self.layer_filter_list:
				return self.layer_filter_include
		return not self.layer_filter_include

	def layer( self, layer ):
		numeric = False
		text = False
		if type(layer) is int:
			layer = self.src_pcb.layers.layers[layer].name
			numeric = True
		elif isinstance( layer, Text ):
			layer = layer.value
			text = True
		if self.flip:
			if layer.startswith( "F." ):
				layer = "B." + layer[2:]
			elif layer.startswith( "B." ):
				layer = "F." + layer[2:]
			elif layer.startswith( "In" ) and layer.endswith( ".Cu" ):
				if self.swap_internal_layers:
					n = int( layer[2:-3] )
					n = self.src_pcb.get_internal_layer_count() - n + 1
					layer = "In%d.Cu" % n
		if numeric:
			layer = self.dst_pcb.layers.names[layer].nr
		elif text:
			l = Text( "layer" )
			l.tag_value( None, layer )
			layer = l
		return layer

	def net( self, net ):
		#if not self.clone_nets:
		#	return net.clone()
		if isinstance( net, Int ):
			nr = net.value
		elif isinstance( net, Net ):
			nr = net.nr
		elif isinstance( net, Text ):
			nr = self.src_pcb.nets.by_name[net.value].nr
		else:
			raise Exception( "unexpected class" )
		if nr == None:
			return net.clone()
		name = self.src_pcb.nets.by_nr[nr].name
		newname = name
		if self.clone_nets:
			newname = "C%d_%s" % ( self.net_pfx_nr, name )
		if newname in self.dst_pcb.nets.by_name:
			nr = self.dst_pcb.nets.by_name[newname].nr
		else:
			nr = self.dst_pcb.nets.create( newname )
		if isinstance( net, Int ):
			return Int( net.obj_name, nr )
		elif isinstance( net, Net ):
			return Net( net.obj_name, nr, newname )
		elif isinstance( net, Text ):
			return Text( net.obj_name, newname )


class Variable:

	def __init__( self, value ):
		self.value = value

	def __str__( self ):
		return str(self.value)


class Main:

	def __init__( self ):
		self.factory = KicadFactory()
		self.trans = Transformer()
		self.debugging = True
		self.var_layer_name = Variable( "Edge.Cuts" )
		self.var_line_thickness = Variable( 0.15 )
		self.var_text_height = Variable( 1.5 )
		self.var_text_width = Variable( 1.2 )
		self.var_text_thickness = Variable( 0.15 )
		self.cmd_defs = (
			# 1. name
			# 2. function
			# 3. description of command
			# 4. fixed arguments prepended to specified aguments
			# 5. arguments, tuples of type, descr and default
			(
				"new",
				self.new,
				"Create a new empty destination PCB.",
				None,
				None,
			),
			(
				"load",
				self.load,
				"Load the PCB from the specified file and use it as "
				"\nsource. If no destination PCB exists this PCB also "
				"\nbecomes the destination",
				None,
				(
					( "string", "Filename", None ),
				),
			),
			(
				"save",
				self.save,
				"Save the destination PCB to the specified file.",
				None,
				(
					( "string", "Filename", None ),
				),
			),
			(
				"create-template",
				self.create_template,
				"Create a template from the source PCB and set it"
				"\nas destination PCB. The template will contain"
				"\nonly the global configuration data of the source"
				"\nPCB. Modules, traces, drawings. etc. will not"
				"\nbe copied.",
				None,
				None,
			),
			(
				"compat",
				self.compat,
				"Save files compatible to the specified kicad version.",
				None,
				(
					( "string", "'%s'" % "', '".join( kicad_versions ), None ),
				),
			),
			(
				"source-area",
				self.source_area,
				"Copy set the source area.",
				None,
				(
					( "float", "Left (X1)", None ),
					( "float", "Top (Y1)", None ),
					( "float", "Right (X2)", None ),
					( "float", "Bottom (Y2)", None ),
				),
			),
			(
				"set-title",
				self.set_title_block,
				"The title of the PCB title block.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"set-date",
				self.set_title_block,
				"The date of the PCB title block in the format YYYY-MM-DD.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"set-rev",
				self.set_title_block,
				"The rev of the PCB title block.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"set-company",
				self.set_title_block,
				"The company of the PCB title block.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"set-comment-1",
				self.set_title_block,
				"The comment 1 of the PCB title block.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"set-comment-2",
				self.set_title_block,
				"The comment 2 of the PCB title block.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"set-comment-3",
				self.set_title_block,
				"The comment 3 of the PCB title block.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"set-comment-4",
				self.set_title_block,
				"The comment 4 of the PCB title block.",
				None,
				(
					( "string", "string", None ),
				),
			),
			(
				"clone-nets",
				self.clone_nets,
				"Clone nets when set to true.",
				None,
				(
					( "bool", "bool", None ),
				),
			),
			(
				"exclude-layer",
				self.exclude_layer,
				"Exclude the specified layer from copies."
				"\nThis removes all included layers and adds"
				"\nthe specified exclude.",
				None,
				(
					( "string", "layer name", None ),
				),
			),
			(
				"include-layer",
				self.include_layer,
				"Include the specified layer in copies."
				"\nThis removes all excluded layers and adds"
				"\nthe specified include. The special name 'all'"
				"\nincludes all layers.",
				None,
				(
					( "string", "layer name", None ),
				),
			),
			(
				"swap-internal-layers",
				self.swap_internal_layers,
				"Swap the internal copper layers in flipped copies.",
				None,
				(
					( "bool", "swap", None ),
				),
			),
			(
				"copy",
				self.copy,
				"Copy the source area to the destination.",
				( 0, False ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"rotate-right",
				self.copy,
				"Copy and rotate right.",
				(  90, False ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"rotate-180",
				self.copy,
				"Copy and rotate 180 degrees.",
				( 180, False ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"rotate-left",
				self.copy,
				"Copy and rotate left.",
				( 270, False ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"flip-copy",
				self.copy,
				"Copy and flip.",
				( 0, True ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"flip-rotate-right",
				self.copy,
				"Copy, flip and rotate right.",
				(  90, True ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"flip-rotate-180",
				self.copy,
				"Copy, flip and rotate 180 degrees.",
				( 180, True ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"flip-rotate-left",
				self.copy,
				"Copy, flip and rotate left.",
				( 270, True ),
				(
					( "float", "destination X", None ),
					( "float", "destination Y", None ),
				),
			),
			(
				"draw-line",
				self.draw_line,
				"Draw a line on the given layer.",
				None,
				(
					( "float", "from (X1)", None ),
					( "float", "from (Y1)", None ),
					( "float", "to (X2)", None ),
					( "float", "to (Y2)", None ),
					( "string", "layer", self.var_layer_name ),
					( "float", "thickness", self.var_line_thickness ),
				),
			),
			(
				"draw-text",
				self.draw_text,
				"Draw text on the given layer.",
				None,
				(
					( "string", "text", None ),
					( "float", "(X1)", None ),
					( "float", "(Y1)", None ),
					( "float", "angle", 0.0 ),
					( "string", "layer", self.var_layer_name ),
					( "float", "height", self.var_text_height ),
					( "float", "width", self.var_text_width ),
					( "float", "thickness", self.var_text_thickness ),
				),
			),
			(
				"set-layer",
				self.set_layer,
				"Sets the layer used for drawing when"
				"\nnot explicitely specified.",
				None,
				(
					( "string", "layer", None ),
				),
			),
			(
				"set-line-thickness",
				self.set_line_thickness,
				"Sets the line thickness used for drawing"
				"\nwhen not explicitely specified.",
				None,
				(
					( "float", "thickness", None ),
				),
			),
			(
				"set-text-font",
				self.set_text_font,
				"Sets the text font used for drawing when"
				"\nnot explicitely specified.",
				None,
				(
					( "float", "height", None ),
					( "float", "width", None ),
					( "float", "thickness", None ),
				),
			),
		)
		self.cmd_names = {}
		for i in range( 0, len(self.cmd_defs) ):
			name = self.cmd_defs[i][0]
			self.cmd_names[name] = i
		self.compat_index = kicad_versions.index( "latest" )

	def run( self ):
		if len(sys.argv) == 2 and sys.argv[1] not in ( '-h', '--help' ):
			self.run_file( sys.argv[1] )
		else:
			self.usage()

	def print_exc( self ):
		if self.debugging:
			traceback.print_exc()

	def usage( self ):
		print( "panelize.py V%s rev %s" % ( __version__, __revision__ ) )
		print( "\nUsage:" )
		print( "  panelize.py commandfile" )
		print( "\nCommandfile:" )
		print( "  Empty lines and lines starting with # are ignored." )
		print( "  Command lines must start with the command followed by its arguments." )
		print( "  All coordinates are in millimeters." )
		print( "\nCommands:" )
		for cmd_def in self.cmd_defs:
			name, func, descr, fixed_args, argdefs = cmd_def
			lines = descr.split( "\n" )
			print( "  %-20s%s" % ( name, lines[0] ) )
			for i in range( 1, len(lines) ):
				print( "  %-20s%s" % ( "", lines[i] ) )
			if argdefs != None:
				print( "    Arguments:" )
				n_max = len(argdefs)
				n_min = 0
				while n_min < n_max:
					if argdefs[n_min][2] != None:
						break
					n_min += 1
				optval = ""
				for argtype, argdesc, argdefault in argdefs:
					if argdefault != None:
						optval = " (optional, default=%s)" % argdefault
					print( "      %-16s%s%s" % ( argtype, argdesc, optval ) )

	def run_file( self, filename ):
		ifd = None
		try:
			ifd = open( filename, "r" )
		except:
			print( "failed to open '%s' for reading." % filename )
			return 1
		linenr = 0
		commands = []
		errors = False
		for line in ifd:
			linenr += 1
			line = line.strip()
			if len(line) == 0 or line.startswith( "#" ):
				pass
			else:
				# words = line.split()
				words = self.split_line( line, linenr )
				if words == None:
					errors = True
				else:
					cmd = self.make_cmd( words, linenr )
					if cmd == None:
						errors = True
					else:
						commands.append( cmd )
		if not errors:
			for func, cmd, args in commands:
				for i in range( 0, len(args) ):
					if isinstance( args[i], Variable ):
						args[i] = args[i].value
				if not func( cmd, args ):
					print( "Command execution aborted." )
					break

	def split_line( self, line, linenr ):
		words = []
		start = None
		char = None
		for i in range( 0, len(line) ):
			c = line[i]
			if start == None:
				if c == '"' or c == "'":
					char = c
					start = i+1
				elif not c.isspace():
					char = None
					start = i
			else:
				if char == None:
					if c.isspace():
						words.append( line[start:i] )
						start = None
				else:
					if c == char:
						words.append( line[start:i] )
						start = None
		if start != None:
			if char == None:
				words.append( line[start:] )
			else:
				print( "Unterminated %s quoted string on line %d." % ( char, linenr ) )
				return None
		return words

	def make_cmd( self, words, linenr ):
		cmd = words[0]
		words = words[1:]
		if not cmd in self.cmd_names:
			print( "invalid command '%s'." % cmd )
			return None
		i = self.cmd_names[cmd]
		name, func, desrc, fixed_args, argdefs = self.cmd_defs[i]
		n = len(words)
		n_min = n_max = 0
		if argdefs != None:
			n_max = len(argdefs)
			while n_min < n_max:
				if argdefs[n_min][2] != None:
					break
				n_min += 1
		if n < n_min or n > n_max:
			if n_min == n_max:
				print( "Command %s expects %d arguments on line %d." % ( cmd, n_max, linenr ) )
			else:
				print( "Command %s expects between %d and %d arguments on line %d." % ( cmd, n_min, n_max, linenr ) )
			return None
		args = []
		if fixed_args != None:
			args = list( fixed_args )
		for i in range( 0, n ):
			argtype = argdefs[i][0]
			arg = words[i]
			if argtype == "string":
				pass
			elif argtype == "float":
				try:
					arg = float( arg )
				except:
					print( "Invalid float argument '%s' on line %d." % ( arg, linenr ) )
					return None
			elif argtype == "bool":
				if arg.lower() == "true":
					arg = True
				elif arg.lower() == "false":
					arg = False
				else:
					print( "Invalid bool argument '%s' on line %d." % ( arg, linenr ) )
					return None
			else:
				print( "Unhandled argument type '%s' on line %d." % ( argtype, linenr ) )
				return None
			args.append( arg )
		# append default values for unspecified args
		for i in range( n, n_max ):
			args.append( argdefs[i][2] )
		return ( func, cmd, args )

	def set_title_block( self, cmd, args ):
		if cmd == "set-title":
			self.trans.dst_pcb.title_block.title.value = args[0]
		elif cmd == "set-date":
			self.trans.dst_pcb.title_block.date.value = args[0]
		elif cmd == "set-rev":
			self.trans.dst_pcb.title_block.rev.value = args[0]
		elif cmd == "set-company":
			self.trans.dst_pcb.title_block.company.value = args[0]
		elif cmd == "set-comment-1":
			self.trans.dst_pcb.title_block.comments[0] = args[0]
		elif cmd == "set-comment-2":
			self.trans.dst_pcb.title_block.comments[1] = args[0]
		elif cmd == "set-comment-3":
			self.trans.dst_pcb.title_block.comments[2] = args[0]
		elif cmd == "set-comment-4":
			self.trans.dst_pcb.title_block.comments[3] = args[0]
		else:
			return False
		return True

	def new( self, cmd, args ):
		self.trans.set_src_pcb( self.factory.create_pcb() )
		return True

	def load( self, cmd, args ):
		try:
			pcb = self.factory.parse_file( args[0] )
			if pcb == None:
				raise Exception( "error loading PCB file" )
			self.trans.set_src_pcb( pcb )
		except:
			print( "Loading PCB failed." )
			self.print_exc()
			return False
		return True

	def create_template( self, cmd, args ):
		try:
			self.trans.create_template()
		except:
			print( "Creating template PCB failed." )
			self.print_exc()
			return False
		return True

	def save( self, cmd, args ):
		try:
			writer = Writer( args[0], self.compat_index )
			self.trans.dst_pcb.write( writer )
			writer.close()
		except:
			print( "Saving PCB failed." )
			self.print_exc()
			return False
		return True

	def compat( self, cmd, args ):
		try:
			self.compat_index = kicad_versions.index( args[0] )
		except:
			print( "Invalid kicad compat version '%s'." % args[0] )
			return False
		return True

	def source_area( self, cmd, args ):
		try:
			self.trans.set_src_area(
				Area( "src-area",
					Vector( "v1", args[0], args[1] ),
					Vector( "v2", args[2], args[3] )
				)
			)
		except:
			print( "Setting source area failed." )
			self.print_exc()
			return False
		return True

	def clone_nets( self, cmd, args ):
		try:
			self.trans.clone_nets = args[0]
		except:
			print( "Setting parameter clone-nets failed." )
			self.print_exc()
			return False
		return True

	def exclude_layer( self, cmd, args ):
		try:
			self.trans.exclude_layer( args[0] )
		except:
			print( "Setting parameter exclude-layer failed." )
			self.print_exc()
			return False
		return True

	def include_layer( self, cmd, args ):
		try:
			arg = args[0]
			if arg == "all":
				self.trans.include_all_layers()
			else:
				self.trans.include_layer( arg )
		except:
			print( "Setting parameter exclude-layer failed." )
			self.print_exc()
			return False
		return True

	def swap_internal_layers( self, cmd, args ):
		try:
			self.trans.set_swap_internal_layers( args[0] )
		except:
			print( "Setting parameter swap-internal-layers failed." )
			self.print_exc()
			return False
		return True

	def copy( self, cmd, args ):
		try:
			rotate = args[0]
			flip = args[1]
			dst_vector = Vector( "v", args[2], args[3] )
			self.trans.set_rotate( rotate )
			self.trans.set_flip( flip )
			self.trans.set_dst_vector( dst_vector )
			self.trans.recalculate()
			self.trans.copy()
		except:
			print( "Copying failed." )
			self.print_exc()
			return False
		return True

	def draw_line( self, cmd, args ):
		try:
			from_vector = Vector( "start", args[0], args[1] )
			to_vector = Vector( "end", args[2], args[3] )
			layer = args[4]
			width = args[5]
			obj = GrLine( "gr_line" )
			obj.set( "start", from_vector )
			obj.set( "end", to_vector )
			obj.set( "angle", Int( "angle", 90) )
			obj.set( "layer", Text( "layer", layer ) )
			obj.set( "width", Float( "width", width ) )
			self.trans.dst_pcb.graphics.append( obj )
		except:
			print( "Drawing line failed." )
			self.print_exc()
			return False
		return True

	def draw_text( self, cmd, args ):
		try:
			text = args[0]
			at_vector = VectorAngle( "at", args[1], args[2], args[3] )
			layer = args[4]
			height = args[5]
			width = args[6]
			thickness = args[7]
			obj = GrText( "gr_text" )
			print( "text", text )
			obj.text = text
			obj.set( "at", at_vector )
			obj.set( "layer", Text( "layer", layer ) )
			font = Font( "font" )
			font.set( "size", Vector( "size", height, width ) )
			font.set( "thickness", Float( "thickness", thickness ) )
			effects = Effects( "effects" )
			effects.set( "font", font )
			if layer.startswith( "B." ):
				effects.justify.values.append( "mirror" )
			obj.set( "effects", effects )
			self.trans.dst_pcb.graphics.append( obj )
		except:
			print( "Drawing text failed." )
			self.print_exc()
			return False
		return True

	def set_layer( self, cmd, args ):
		self.var_layer_name.value = args[0]
		return True

	def set_line_thickness( self, cmd, args ):
		self.var_line_thickness.value = args[0]
		return True

	def set_text_font( self, cmd, args ):
		self.var_text_height.value = args[0]
		self.var_text_width.value = args[1]
		self.var_text_thickness.value = args[2]
		return True


if __name__ == "__main__":
	m = Main()
	m.run()

