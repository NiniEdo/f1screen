#ifndef RACETRACKS_H
#define RACETRACKS_H

#include <Arduino.h>
#include <map>

extern const unsigned char vegas[];
extern const unsigned char yas_marina[];
extern const unsigned char albert_park[];
extern const unsigned char red_bull_ring[];
extern const unsigned char baku[];
extern const unsigned char bahrain[];
extern const unsigned char spa[];
extern const unsigned char interlagos[];
extern const unsigned char villeneuve[];
extern const unsigned char shanghai[];
extern const unsigned char imola[];
extern const unsigned char silverstone[];
extern const unsigned char hungaroring[];
extern const unsigned char monza[];
extern const unsigned char suzuka[];
extern const unsigned char rodriguez[];
extern const unsigned char miami[];
extern const unsigned char monaco[];
extern const unsigned char zandvoort[];
extern const unsigned char losail[];
extern const unsigned char jeddah[];
extern const unsigned char marina_bay[];
extern const unsigned char catalunya[];
extern const unsigned char americas[];


extern const std::map<String, const unsigned char*> circuitImageMap;

#endif
