EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Splitflap sensor board"
Date ""
Rev ""
Comp "Scott Bezek"
Comment1 "https://github.com/scottbez1/splitflap"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L hall_effect:hall_effect U100
U 1 1 5B63B9BC
P 4450 3700
F 0 "U100" H 4600 4000 60  0000 C CNN
F 1 "AH3391Q" V 4350 3700 60  0000 C CNN
F 2 "hall_effect:hall_effect" H 4450 3700 60  0001 C CNN
F 3 "" H 4450 3700 60  0001 C CNN
F 4 "AH3391Q-P-BDI-ND (sensor), 469-1027-ND (magnet)" H 4450 3700 60  0001 C CNN "DK#"
F 5 "Many hall effect sensors will work. AH3391Q is recommended, but other options that have been tested are AH3369Q, AH3366Q, AH3365Q, A1122LUA-T. If using another part, MAKE SURE it doesn't have a 'sleep' or 'low power' mode, as those generally sample the magnetic field too infrequently for this purpose." H 4450 3700 60  0001 C CNN "BOM_Note"
	1    4450 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3600 4850 3600
Wire Wire Line
	4850 3600 4850 3700
Wire Wire Line
	5250 3700 5050 3700
Wire Wire Line
	5050 3700 5050 3250
Wire Wire Line
	5050 3250 4450 3250
Wire Wire Line
	4450 4150 4850 4150
Wire Wire Line
	4850 4150 4850 3800
Wire Wire Line
	4850 3800 5250 3800
$Comp
L Connector:Conn_01x03_Male P100
U 1 1 56499725
P 5450 3700
F 0 "P100" H 5450 3900 50  0000 C CNN
F 1 "SENSOR" V 5550 3700 50  0000 C CNN
F 2 "PinHeaders:Pin_Header_Straight_1x03" H 5450 3700 60  0001 C CNN
F 3 "" H 5450 3700 60  0000 C CNN
F 4 "S1112EC-03-ND" H 5450 3700 60  0001 C CNN "DK#"
F 5 "Consider ordering 1xN headers and breaking into multiple 1x3 headers, rather than ordering exact 1x3 headers" H 5450 3700 60  0001 C CNN "BOM_Note"
	1    5450 3700
	-1   0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 6052ED0C
P 4800 2500
F 0 "H1" H 4900 2549 50  0000 L CNN
F 1 "MountingHole_Pad" H 4900 2458 50  0000 L CNN
F 2 "MountingHoles:M4_mount" H 4800 2500 50  0001 C CNN
F 3 "~" H 4800 2500 50  0001 C CNN
	1    4800 2500
	1    0    0    -1  
$EndComp
NoConn ~ 4800 2600
$EndSCHEMATC
