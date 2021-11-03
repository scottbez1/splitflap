EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "Splitflap Chainlink Buddy - Breadboard"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_02x04_Odd_Even J1
U 1 1 61A3A5EC
P 2400 1900
F 0 "J1" H 2450 2217 50  0000 C CNN
F 1 "Output" H 2450 2126 50  0000 C CNN
F 2 "ModifiedSymbols:IDC-Header_2x04_P2.54mm_Vertical" H 2400 1900 50  0001 C CNN
F 3 "~" H 2400 1900 50  0001 C CNN
	1    2400 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 61A3AEE0
P 2700 2250
F 0 "#PWR01" H 2700 2000 50  0001 C CNN
F 1 "GND" H 2705 2077 50  0000 C CNN
F 2 "" H 2700 2250 50  0001 C CNN
F 3 "" H 2700 2250 50  0001 C CNN
	1    2700 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 2100 2700 2250
Wire Wire Line
	2700 1800 2700 1900
Connection ~ 2700 2100
Connection ~ 2700 1900
Wire Wire Line
	2700 1900 2700 2000
Connection ~ 2700 2000
Wire Wire Line
	2700 2000 2700 2100
Text Label 1600 1800 0    50   ~ 0
CLOCK
Text Label 1600 1900 0    50   ~ 0
MOTOR_DATA
Text Label 1600 2000 0    50   ~ 0
SENSOR_DATA
Text Label 1600 2100 0    50   ~ 0
LATCH
Wire Wire Line
	1600 1800 2200 1800
Wire Wire Line
	2200 1900 1600 1900
Wire Wire Line
	1600 2000 2200 2000
Wire Wire Line
	2200 2100 1600 2100
$Comp
L Connector:Conn_01x05_Male J2
U 1 1 617FD64C
P 4300 2000
F 0 "J2" H 4408 2381 50  0000 C CNN
F 1 "Conn_01x05_Male" H 4408 2290 50  0000 C CNN
F 2 "ModifiedSymbols:PinHeader_1x05_P2.54mm_Vertical_NoSilk" H 4300 2000 50  0001 C CNN
F 3 "~" H 4300 2000 50  0001 C CNN
	1    4300 2000
	-1   0    0    1   
$EndComp
Text Label 3500 1800 0    50   ~ 0
CLOCK
Text Label 3500 1900 0    50   ~ 0
MOTOR_DATA
Text Label 3500 2000 0    50   ~ 0
SENSOR_DATA
Text Label 3500 2100 0    50   ~ 0
LATCH
Wire Wire Line
	3500 1800 4100 1800
Wire Wire Line
	4100 1900 3500 1900
Wire Wire Line
	3500 2000 4100 2000
Wire Wire Line
	4100 2100 3500 2100
$Comp
L power:GND #PWR0101
U 1 1 618021FD
P 3750 2200
F 0 "#PWR0101" H 3750 1950 50  0001 C CNN
F 1 "GND" H 3755 2027 50  0000 C CNN
F 2 "" H 3750 2200 50  0001 C CNN
F 3 "" H 3750 2200 50  0001 C CNN
	1    3750 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 2200 3750 2200
$EndSCHEMATC
