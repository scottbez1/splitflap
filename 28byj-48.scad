// 28BYJ-48 from RGriffoGoes 
// at http://www.thingiverse.com/thing:204734
// named and measured dimensions, and
// added details by J.Beale 21 July 2014

MBH = 18.8;   // motor body height
MBD = 28.25;  // motor body OD
SBD = 9.1;    // shaft boss OD
SBH = 1.45;   // shaft boss height above motor body
SBO = 7.875;  // offset of shaft/boss center from motor center
SHD = 4.93;   // motor shaft OD
SHDF = 3.0;   // motor shaft diameter, across flats
SHHF = 6.0;   // motor shaft flats height, or depth in from end
SHH = 9.75;   // height of shaft above motor body 

MBFH = 0.7;   // height of body edge flange protruding above surface
MBFW = 1.0;   // width of edge flange
MBFNW = 8;    // width of notch in edge flange
MBFNW2 = 17.8;  // width of edge flange notch above wiring box

MHCC = 35.0;  // mounting hole center-to-center
MTH  = 0.8;   // mounting tab thickness
MTW  = 7.0;   // mounting tab width
WBH  = 16.7;  // plastic wiring box height
WBW  = 14.6;  // plastic wiring box width
WBD  = 31.3;  // body diameter to outer surface of wiring box

WRD = 1.0;     // diameter of electrical wires
WRL = 30;      // length of electrical wires
WRO = 2.2;		// offset of wires below top motor surface

// =========================================================
eps = 0.05;   // small number to avoid coincident faces


module wires() {
 
  color([0.9,0.9,0]) translate([0,WRD*2,0]) cylinder(h=WRL,r=WRD/2, $fn = 7);
  color([.9,.45,0]) translate([0,WRD*1,0]) cylinder(h=WRL,r=WRD/2, $fn = 7);   
  color([1,0,0]) translate([0,0,0]) cylinder(h=WRL,r=WRD/2, $fn = 7);
  color([1,.6,.6]) translate([0,-WRD*1,0]) cylinder(h=WRL,r=WRD/2, $fn = 7);
  color([.2,.2,1]) translate([0,-WRD*2,0]) cylinder(h=WRL,r=WRD/2, $fn = 7);
}

module StepMotor28BYJ()
{
  difference(){
    union(){
	   color("gray") translate([0,0,-(MBH+MBFH)/2])
		  difference() {  // flange at top rim
         cylinder(h = MBFH+eps, r = MBD/2, center = true, $fn = 50);
         cylinder(h = MBFH+2*eps, r = (MBD-MBFW)/2, center = true, $fn = 32);
			cube([MBFNW,MHCC,MBFH*2],center=true); // notches in rim
			cube([MBD+2*MBFW,SBD,MBFH*2],center=true);
		   translate([-MBD/2,0,0]) cube([MBD,MBFNW2,MBFH*2],center=true);
        }
		color("gray") // motor body
			cylinder(h = MBH, r = MBD/2, center = true, $fn = 50);
		color("gray") translate([SBO,0,-SBH])	// shaft boss
			cylinder(h = MBH, r = SBD/2, center = true, $fn = 32);

		translate([SBO,0,-SHH])	// motor shaft
        difference() {
			color("gold") cylinder(h = MBH, r = SHD/2, center = true, $fn = 32);
				// shaft flats
		   color("red") translate([(SHD+SHDF)/2,0,-(eps+MBH-SHHF)/2]) 
				cube([SHD,SHD,SHHF], center = true);
		   color("red") translate([-(SHD+SHDF)/2,0,-(eps+MBH-SHHF)/2]) 
				cube([SHD,SHD,SHHF], center = true);
        }


		color("Silver") translate([0,0,-(MBH-MTH-eps)/2]) // mounting tab 
			cube([MTW,MHCC,MTH], center = true);				
		color("Silver") translate([0,MHCC/2,-(MBH-MTH)/2]) // mt.tab rounded end
			cylinder(h = MTH, r = MTW/2, center = true, $fn = 32);
		color("Silver") translate([0,-MHCC/2,-(MBH-MTH)/2]) // mt.tab rounded end
			cylinder(h = MTH, r = MTW/2, center = true, $fn = 32);


		color("blue") translate([-(WBD-MBD),0,eps-(MBH-WBH)/2]) // plastic wire box
			cube([MBD,WBW,WBH], center = true);
	   color("blue") translate([-2,0,0])	
			cube([24.5,16,15], center = true);
		}

				// mounting holes in tabs on side
		color("red") translate([0,MHCC/2,-MBH/2])	
				cylinder(h = 2, r = 2, center = true, $fn = 32);
		color("red") translate([0,-MHCC/2,-MBH/2])	
				cylinder(h = 2, r = 2, center = true, $fn = 32);
		}
	}

rotate([180,0,0]) {

  StepMotor28BYJ();  // motor body (without wires)

  // 5 colored hookup wires
  translate([0,0,-(MBH/2 - WRO)]) rotate([0,-90,0])  
	  wires();
}