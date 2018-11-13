#pragma once

// Class has been referenced by Emilian Cioca
// https://github.com/EmilianC/Jewel3D/blob/master/Engine/Jewel3D/Utilities/Singleton.h

/*
Inherit from this class to create a singleton.
You must declare your class in this way when doing so:

static class MyClass : public Singleton<class MyClass>
{
//...
} &MyClass = Singleton<class MyClass>::instanceRef;
*/

// End of class shortcut
#define SingletonInstance(x) &x = Singleton<class x>::instanceRef

template<typename T>
class Singleton
{
public:
	static T& instanceRef;
private:
	static T instance;

protected:
	explicit	Singleton() {}
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

};

template<class T>
T Singleton<T>::instance;

template<class T>
T& Singleton<T>::instanceRef = Singleton<T>::instance;