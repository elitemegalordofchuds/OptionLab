#include "../Result.hpp"
#include "TestStructure.hpp"

class EuTests : public Cases<OptionStyle::Euro> {
private:
	void eu_testClosedForm();
	void eu_testFDM();
	void eu_testBinomial();
	void eu_testMonteCarlo();
	void eu_testGreeks();
public:
	static int runTests();
};

class AsianTests : public Cases<OptionStyle::Asian> {
private:
	void asian_testMonteCarlo();
	void asian_testGreeks();
public:
	static int runTests();
};

class AmerTests : public Cases<OptionStyle::American> {
private:
	void amer_testFDM();
	void amer_testBinomial();
	void amer_testGreeks();
public:
	static int runTests();
};
