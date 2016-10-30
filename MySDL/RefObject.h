#pragma once
class RefObject
{
	int _RefCount;

public:
	inline RefObject() : _RefCount(0) {};
	inline virtual ~RefObject() {};
	inline void _getref() { _RefCount++; };
	inline void _freeref()
	{
		_RefCount--;
		if (0 == _RefCount)
		{
			delete this;
		}
	};
};
template <class T> class RefPtr
{
	T *_tpointer;

public:
	inline RefPtr() : _tpointer(nullptr) {};
	inline RefPtr(T&&p) {
		_tpointer = p._tpointer;
		if (_tpointer)
		{
			p._tpointer = nullptr;
		}
	}
	inline RefPtr(T&p) {
		_tpointer = p._tpointer;
		if (_tpointer)
		{
			_tpointer->_getref();
		}
	}
	inline RefPtr(T *p) : _tpointer(p)
	{
		if (_tpointer)
			_tpointer->_getref();
	}
	inline RefPtr &operator=(T *p)
	{
		if (_tpointer)
			_tpointer->_freeref();
		_tpointer = p;
		if (_tpointer)
		{
			_tpointer->_getref();
		}
		return *this;
	}
	inline RefPtr &operator=(const RefPtr &p)
	{
		if (_tpointer)
			_tpointer->_freeref();
		_tpointer = p._tpointer;
		if (_tpointer)
		{
			_tpointer->_getref();
		}
		return *this;
	}
	inline bool operator==(const T*p)
	{
		return _tpointer == p;
	}
	inline bool operator==(RefPtr p)
	{
		return _tpointer == p._tpointer;
	}
	inline T& operator *()
	{
		return *_tpointer;
	}
	inline T*operator ->()
	{
		return _tpointer;
	}
	operator bool()
	{
		return _tpointer!=nullptr;
	}
	inline ~RefPtr()
	{
		if (_tpointer)
			_tpointer->_freeref();
	}
};