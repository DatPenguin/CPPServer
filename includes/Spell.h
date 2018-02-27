#ifndef CPPSERVER_SPELL_H
#define CPPSERVER_SPELL_H

#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include "db.h"

/**
 * Contains infos about the spell.
 * Every var initialized to 1 is a multiplier to the current attribute.
 * Every var initialized to 0 is an absolute value.
 */
class Spell {
public:

    std::string spellName = "";     /// Name of the spell
    float ballSpeed = 1;            /// Speed of the ball
    float ballSpeed_o = 1;          /// Speed of the opponent's ball
    float paddleSize = 1;             /// Size of the paddle
    float paddleSize_o = 1;           /// Size of the opponent's paddle
    float paddleSpeed = 1;          /// Movement speed of the paddle
    float paddleSpeed_o = 1;        /// Movement speed of the opponent's paddle
    int damages = 0;                /// Damages received
    int damages_o = 0;              /// Damage dealt
    int shield = 0;                 /// Shield received
    int shield_o = 0;               /// Shield given to the opponent
    int ball_multiplication = 0;    /// Added balls
    int ball_multiplication_o = 0;  /// Added balls for the opponent
    bool badSpell = true;

    explicit Spell(std::string name);

    const std::string &getSpellName() const;

    bool isBadSpell();

    void print();

    std::string toString();
};

std::ostream &operator<<(std::ostream, Spell const &);

#endif //CPPSERVER_SPELL_H
