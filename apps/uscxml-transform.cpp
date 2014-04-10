#include "uscxml/config.h"
#include "uscxml/Interpreter.h"
#include "uscxml/transform/ChartToFSM.h"
#include "uscxml/DOMUtils.h"
#include <glog/logging.h>

#ifdef HAS_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAS_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef HAS_DLFCN_H
#include <dlfcn.h>
#endif

class VerboseMonitor : public uscxml::InterpreterMonitor {
	void onStableConfiguration(uscxml::Interpreter interpreter) {
		printConfig(interpreter.getConfiguration());
	}

	void beforeProcessingEvent(uscxml::Interpreter interpreter, const uscxml::Event& event) {
		std::cout << "Event: " << event.name << std::endl;
	}

	void beforeCompletion(uscxml::Interpreter interpreter) {
		printConfig(interpreter.getConfiguration());
	}

	void printConfig(const Arabica::XPath::NodeSet<std::string>& config) {
		std::string seperator;
		std::cout << "Config: {";
		for (int i = 0; i < config.size(); i++) {
			std::cout << seperator << ATTR(config[i], "id");
			seperator = ", ";
		}
		std::cout << "}" << std::endl;
	}
};

#ifdef CMAKE_BUILD_TYPE_DEBUG

#ifdef HAS_EXECINFO_H
void printBacktrace(void** array, int size) {
	char** messages = backtrace_symbols(array, size);
	for (int i = 0; i < size && messages != NULL; ++i) {
		std::cerr << "\t" << messages[i] << std::endl;
	}
	std::cerr << std::endl;
	free(messages);
}

#ifdef HAS_DLFCN_H
// see https://gist.github.com/nkuln/2020860
typedef void (*cxa_throw_type)(void *, void *, void (*) (void *));
cxa_throw_type orig_cxa_throw = 0;

void load_orig_throw_code() {
	orig_cxa_throw = (cxa_throw_type) dlsym(RTLD_NEXT, "__cxa_throw");
}

extern "C"
void __cxa_throw (void *thrown_exception, void *pvtinfo, void (*dest)(void *)) {
	std::cerr << __FUNCTION__ << " will throw exception from " << std::endl;
	if (orig_cxa_throw == 0)
		load_orig_throw_code();

	void *array[50];
	size_t size = backtrace(array, 50);
	printBacktrace(array, size);
	orig_cxa_throw(thrown_exception, pvtinfo, dest);
}
#endif
#endif


// see http://stackoverflow.com/questions/2443135/how-do-i-find-where-an-exception-was-thrown-in-c
void customTerminate() {
	static bool tried_throw = false;

	try {
		// try once to re-throw currently active exception
		if (!tried_throw) {
			tried_throw = true;
			throw;
		} else {
			tried_throw = false;
		}
	} catch (const std::exception &e) {
		std::cerr << __FUNCTION__ << " caught unhandled exception. what(): "
		          << e.what() << std::endl;
	} catch (const uscxml::Event &e) {
		std::cerr << __FUNCTION__ << " caught unhandled exception. Event: "
		          << e << std::endl;
	} catch (...) {
		std::cerr << __FUNCTION__ << " caught unknown/unhandled exception."
		          << std::endl;
	}

#ifdef HAS_EXECINFO_H
	void * array[50];
	int size = backtrace(array, 50);

	printBacktrace(array, size);
#endif
	abort();
}
#endif

int main(int argc, char** argv) {
	using namespace uscxml;

	bool verbose = false;
	bool toFlat = false;
	bool toPromela = false;
	std::string pluginPath;
	std::string inputFile;
	std::string outputFile;
	
#if defined(HAS_SIGNAL_H) && !defined(WIN32)
	signal(SIGPIPE, SIG_IGN);
#endif

#ifdef CMAKE_BUILD_TYPE_DEBUG
	std::set_terminate(customTerminate);
#endif

	// setup logging
	google::LogToStderr();
	google::InitGoogleLogging(argv[0]);

	optind = 0;
	opterr = 0;

	struct option longOptions[] = {
		{"verbose",       no_argument,       0, 'v'},
		{"to-flat",       no_argument, 0, 'f'},
		{"to-promela",    no_argument, 0, 's'},
		{"plugin-path",   required_argument, 0, 'p'},
		{"input-file",    required_argument, 0, 'i'},
		{"output-file",    required_argument, 0, 'o'},
		{"loglevel",      required_argument, 0, 'l'},
		{0, 0, 0, 0}
	};

	// parse global options
	int optionInd = 0;
	int option;
	for (;;) {
		option = getopt_long_only(argc, argv, "+vfs:p:i:o:l:", longOptions, &optionInd);
		if (option == -1) {
			break;
		}
		switch(option) {
				// cases without short option
			case 0: {
				break;
			}
				// cases with short-hand options
			case 'v':
				verbose = true;
				break;
			case 'f':
				toFlat = true;
				break;
			case 's':
				toPromela = true;
				break;
			case 'p':
				pluginPath = optarg;
				break;
			case 'i':
				inputFile = optarg;
				break;
			case 'o':
				outputFile = optarg;
				break;
			case 'l':
				break;
			case '?': {
				break;
			}
			default:
				break;
		}
	}

	// register plugins
	if (pluginPath.length() > 0) {
		Factory::pluginPath = pluginPath;
	}

	// start HTTP server
	HTTPServer::getInstance(30444, 30445, NULL);
	
	Interpreter interpreter;
	if (inputFile.size() == 0 || inputFile == "-") {
		LOG(INFO) << "Reading SCXML from STDIN";
		std::stringstream ss;
		std::string line;
		while (std::getline(std::cin, line)) {
			ss << line;
		}
		interpreter = Interpreter::fromXML(ss.str());
	} else {
		interpreter = Interpreter::fromURI(inputFile);
	}
	if (!interpreter) {
		LOG(ERROR) << "Cannot create interpreter from " << inputFile;
		exit(EXIT_FAILURE);
	}
	
	if (toFlat) {
		std::cout << ChartToFSM::flatten(interpreter.getDocument(), interpreter.getNameSpaceInfo());
		exit(EXIT_SUCCESS);
	}

	if (toPromela) {
		std::cout << ChartToFSM::flatten(interpreter.getDocument(), interpreter.getNameSpaceInfo());
		exit(EXIT_SUCCESS);
	}
	
	
	return EXIT_SUCCESS;
}