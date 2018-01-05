#include "includes/Spell.h"

using namespace std;
using namespace pqxx;

Spell::Spell(const string name) {
	const char *sql;
	try {
		connection c(CONNECTION_STRING);
		if (!c.is_open())
			cerr << "Can't open the database" << endl;

		sql = "SELECT * FROM spell";
		nontransaction n(c);
		result r(n.exec(sql));

		for (result::const_iterator ci = r.begin(); ci != r.end(); ++ci) {
			if (ci[0].as<string>() != name) {
				cerr << "Bad spell name : " << name << " ; " << ci[0].as<string>() << endl;
				continue;
			} else
				cout << "Added Spell : " << name << endl;
			spellName = ci[0].as<string>();
			ballSpeed = ci[1].as<float>();
			ballSpeed_o = ci[2].as<float>();
            paddleSize = ci[3].as<float>();
            paddleSize_o = ci[4].as<float>();
			paddleSpeed = ci[5].as<float>();
			paddleSpeed_o = ci[6].as<float>();
			damages = ci[7].as<int>();
			damages_o = ci[8].as<int>();
			shield = ci[9].as<int>();
			ball_multiplication = ci[10].as<int>();
			ball_multiplication_o = ci[11].as<int>();
		}

	} catch (const exception &e) {
		cerr << e.what() << endl;
	}
}

void Spell::print() {
	cout << spellName << "," << ballSpeed << "," << ballSpeed_o << "," << paddleSize << "," << paddleSize_o << ","
	     << paddleSpeed << "," << paddleSpeed_o << "," << damages << "," << damages_o << "," << shield << ","
	     << ball_multiplication << "," << ball_multiplication_o << endl;
}

const string &Spell::getSpellName() const {
	return spellName;
}

std::string Spell::toString() {
	stringstream ss;
	ss << spellName << ";" << ballSpeed << ";" << ballSpeed_o << ";" << paddleSize << ";" << paddleSize_o << ";"
	   << paddleSpeed << ";" << paddleSpeed_o << ";" << damages << ";" << damages_o << ";" << shield << ";"
	   << ball_multiplication << ";" << ball_multiplication_o << endl;
	return ss.str();
}
