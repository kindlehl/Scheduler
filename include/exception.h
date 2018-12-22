#ifndef EXCEPTION_H
#define EXCEPTION_H 

#include <exception>
#include "logger.h"
#include <string>

namespace Ex{
	template <typename ExType = std::exception, typename Callable> 
	auto run(const std::string message, Callable fn) -> decltype(fn()){
		try{
			return fn();
		}
		catch(ExType& e){
			logDetail((std::string() + std::string("Exception raised at EVENT[") + message + "]").c_str());
			logError(e.what());
			return (decltype(fn()))(nullptr);
		}
	}
}

#endif 
