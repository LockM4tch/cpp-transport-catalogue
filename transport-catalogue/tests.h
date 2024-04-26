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
		"Stop Tolstopaltsevo: 55.611087, 37.20829\n"
		"Stop Marushkino: 55.595884, 37.209755\n"
		"Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
		"Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
		"Stop Rasskazovka: 55.632761, 37.333324\n"
		"Stop Biryulyovo Zapadnoye: 55.574371, 37.6517\n"
		"Stop Biryusinka: 55.581065, 37.64839\n"
		"Stop Universam: 55.587655, 37.645687\n"
		"Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
		"Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
		"Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
		"Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
		"Stop Prazhskaya: 55.611678, 37.603831\n"
	);

	string line;
	while (getline(input, line))
	{	
		ir.ParseLine(line);
	}
		ir.ApplyCommands(catalogue);

		ostringstream out;
		line = "Bus 256";
		ParseAndPrintStat(catalogue, line, out);
		assert(out.str() == "Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n");

		out.str("");
		line = "Bus 750";
		ParseAndPrintStat(catalogue, line, out);
		string str = out.str();
		assert(out.str() == "Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length\n");

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

		cout << "test passed"<<endl;
}