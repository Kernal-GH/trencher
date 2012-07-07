#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <trench/Benchmarking.h>
#include <trench/Foreach.h>
#include <trench/FenceInsertion.h>
#include <trench/NaiveParser.h>
#include <trench/Program.h>
#include <trench/RobustnessChecking.h>
#include <trench/State.h>

void help() {
	std::cout << "Usage: trencher [-b|-nb] [-r|-f|-trf|-ftrf] file..." << std::endl;
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		help();
		return 1;
	}

	try {
		enum {
			ROBUSTNESS,
			FENCES,
			TRIANGULAR_RACE_FREEDOM,
			TRF_FENCES
		} action = FENCES;

		bool benchmarking = false;

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];

			if (arg == "-r") {
				action = ROBUSTNESS;
			} else if (arg == "-f") {
				action = FENCES;
			} else if (arg == "-trf") {
				action = TRIANGULAR_RACE_FREEDOM;
			} else if (arg == "-ftrf") {
				action = TRF_FENCES;
			} else if (arg == "-b") {
				benchmarking = true;
			} else if (arg == "-nb") {
				benchmarking = false;
			} else if (arg.size() >= 1 && arg[0] == '-') {
				throw std::runtime_error("unknown option: " + arg);
			} else {
				trench::Program program;

				trench::Statistics::instance().reset();
				{
					trench::NaiveParser parser;
					std::ifstream in(argv[i]);
					if (!in) {
						throw std::runtime_error("can't open file: " + arg);
					}
					parser.parse(in, program);

					foreach (trench::Thread *thread, program.threads()) {
						trench::Statistics::instance().incStatesCount(thread->states().size());
						trench::Statistics::instance().incTransitionsCount(thread->transitions().size());
					}
				}

				auto startTime = std::chrono::system_clock::now();

				switch (action) {
					case ROBUSTNESS: {
						bool feasible = trench::isAttackFeasible(program, false);
						if (!benchmarking) {
							if (feasible) {
								std::cout << "Program IS NOT robust." << std::endl;
							} else {
								std::cout << "Program IS robust." << std::endl;
							}
						}
						break;
					}
					case FENCES: {
						auto fences = trench::computeFences(program, false);
						if (!benchmarking) {
							std::cout << "Computed fences for enforcing robustness (" << fences.size() << " total):";
							foreach (const auto &fence, fences) {
								std::cout << " (" << fence.first->name() << "," << fence.second->name() << ')';
							}
							std::cout << std::endl;
						}
						break;
					}
					case TRIANGULAR_RACE_FREEDOM: {
						bool feasible = trench::isAttackFeasible(program, true);
						if (!benchmarking) {
							if (feasible) {
								std::cout << "Program IS NOT free from triangular data races." << std::endl;
							} else {
								std::cout << "Program IS free from triangular data races." << std::endl;
							}
						}
						break;
					}
					case TRF_FENCES: {
						auto fences = trench::computeFences(program, false);
						if (!benchmarking) {
							std::cout << "Computed fences for enforcing triangular race freedom (" << fences.size() << " total):";
							foreach (const auto &fence, fences) {
								std::cout << " (" << fence.first->name() << "," << fence.second->name() << ')';
							}
							std::cout << std::endl;
						}
						break;
					}
					default: {
						assert(!"NEVER REACHED");
					}
				}

				auto endTime = std::chrono::system_clock::now();
				trench::Statistics::instance().addRealTime(
					std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());

				if (benchmarking) {
					std::cout << "filename " << arg << " action " << action <<
						trench::Statistics::instance() << std::endl;
				}
			}
		}
	} catch (const std::exception &exception) {
		std::cerr << "trencher: " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}
