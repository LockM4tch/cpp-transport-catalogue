#pragma once
#include "input_reader.h"
#include "stat_reader.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

void test() {
	using namespace std;

	TransportCatalogue catalogue;
	InputReader ir;
	istringstream input(
		"13\n"
		"Stop Marushkino : 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\n"
		"Stop Tolstopaltsevo : 55.611087, 37.20829, 3900m to Marushkino\n"
		"Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
		"Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\n"
		"Stop Rasskazovka : 55.632761, 37.333324, 9500m to Marushkino\n"
		"Stop Biryulyovo Zapadnoye : 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
		"Stop Biryusinka : 55.581065, 37.64839, 750m to Universam\n"
		"Stop Universam : 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
		"Stop Biryulyovo Tovarnaya : 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
		"Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
		"Bus 828 : Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
		"Stop Rossoshanskaya ulitsa : 55.595579, 37.605757\n"
		"Stop Prazhskaya : 55.611678, 37.603831\n"
	);

	FillCatalogue(input, catalogue);

	ostringstream out;
	string line = "Bus 256";
	ParseAndPrintStat(catalogue, line, out);
	line = out.str();
	assert(line == "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature\n");

	out.str("");
	line = "Bus 750";
	ParseAndPrintStat(catalogue, line, out);
	line = out.str();
	assert(line == "Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature\n");

	out.str("");
	line = "Bus 751";
	ParseAndPrintStat(catalogue, line, out);
	assert(out.str() == "Bus 751: not found\n");

	out.str("");
	line = "Stop Samara";
	ParseAndPrintStat(catalogue, line, out);
	assert(out.str() == "Stop Samara: not found\n");

	out.str("");
	line = "Stop Prazhskaya";
	ParseAndPrintStat(catalogue, line, out);
	assert(out.str() == "Stop Prazhskaya: no buses\n");

	out.str("");
	line = "Stop Biryulyovo Zapadnoye";
	ParseAndPrintStat(catalogue, line, out);
	assert(out.str() == "Stop Biryulyovo Zapadnoye: buses 256 828\n");

	cout << "test passed" << endl;
}