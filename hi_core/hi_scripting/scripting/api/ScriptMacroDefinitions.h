/*
  ==============================================================================

    ScriptMacroDefinitions.h
    Created: 6 Jul 2016 11:24:36am
    Author:  Christoph

  ==============================================================================
*/

#ifndef SCRIPTMACRODEFINITIONS_H_INCLUDED
#define SCRIPTMACRODEFINITIONS_H_INCLUDED


// Macros for APIClass objects

#define ADD_API_METHOD_0(name) addFunction(Identifier(#name), &Wrapper::name)
#define ADD_API_METHOD_1(name) addFunction1(Identifier(#name), &Wrapper::name)
#define ADD_API_METHOD_2(name) addFunction2(Identifier(#name), &Wrapper::name)
#define ADD_API_METHOD_3(name) addFunction3(Identifier(#name), &Wrapper::name)

#define API_METHOD_WRAPPER_0(className, name)	inline static var name(ApiClass *m) { return static_cast<className*>(m)->name(); };
#define API_METHOD_WRAPPER_1(className, name)	inline static var name(ApiClass *m, var value1) { return static_cast<className*>(m)->name(value1); };
#define API_METHOD_WRAPPER_2(className, name)	inline static var name(ApiClass *m, var value1, var value2) { return static_cast<className*>(m)->name(value1, value2); };
#define API_METHOD_WRAPPER_3(className, name)	inline static var name(ApiClass *m, var value1, var value2, var value3) { return static_cast<className*>(m)->name(value1, value2, value3); };
#define API_METHOD_WRAPPER_4(className, name) inline static var name(ApiClass *m, var value1, var value2, var value3, var value4) { return static_cast<className*>(m)->name(value1, value2, value3, value4); };


// Macros for DynamicObject objects

#define RETURN_STATIC_IDENTIFIER(name) static const Identifier id(name); return id;

#define SET_MODULE_NAME(x) static Identifier getName() {static const Identifier id(x); return id; };

/** Quick way of accessing the argument of a function call. */
#define ARG(index) args.arguments[index]

/** Quick way of getting a casted object of an argument. */
#define GET_ARGUMENT_OBJECT(classType, argumentIndex) dynamic_cast<classType*>(args.arguments[argumentIndex].getObject())

/** Adds a wrapper function with a variable amount of arguments. */
#define DYNAMIC_METHOD_WRAPPER(className, functionName, ...) static var functionName(const var::NativeFunctionArgs& args) \
{ \
	if (className* thisObject = dynamic_cast<className*>(args.thisObject.getObject())) \
		thisObject->functionName(__VA_ARGS__); \
	return var::undefined(); \
};

/** Adds a wrapper function with a variable amount of arguments that return the result of the function. */
#define DYNAMIC_METHOD_WRAPPER_WITH_RETURN(className, functionName, ...) static var functionName(const var::NativeFunctionArgs& args) \
{ \
	if (className* thisObject = dynamic_cast<className*>(args.thisObject.getObject())) \
		return thisObject->functionName(__VA_ARGS__); \
	return var::undefined(); \
};

/** Quick way of adding a method to a Dynamic Object. Use this in combination with */
#define ADD_DYNAMIC_METHOD(name) setMethod(#name, Wrapper::name);

#endif  // SCRIPTMACRODEFINITIONS_H_INCLUDED
