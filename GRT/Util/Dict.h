/*
GRT MIT License
Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GRT_DICT_HEADER
#define GRT_DICT_HEADER

#include "GRTTypedefs.h"
#include "DynamicType.h"

GRT_BEGIN_NAMESPACE

/**
	@brief This class implements a flexible dictionary that supports multiple data types.  Elements in the dictionary
	consist of key-value pairs, where keys are std::strings and values can be any data type, as they are stored as a
	GRT::DynamicType.
*/
class Dict {
	typedef std::map< std::string, DynamicType > dict; ///<Define the map type that will be used to store the internal data
public:
	/**
	@brief Default Constructor
	*/
	Dict() {
	}

	/**
	@brief Copy Constructor, copies the keys and values from the rhs dictionary to this dictionary
	@param rhs: the right-hand-side reference dictionary containing the keys you want to copy to this dictionary
	*/
	Dict(const Dict &rhs) {
		for ( auto const& x : rhs.data ) {
			this->data[ x.first ] = x.second;
		}
	}

	/**
	@brief Default Destructor
	*/
	~Dict() {
	}

	/**
	@brief Equals operator, copies the keys and values from the rhs dictionary to this dictionary, clearing any elements in this dictionary first
	@param rhs: the right-hand-side reference dictionary containing the keys you want to copy to this dictionary
	@return returns a reference to this dictionary
	*/
	Dict& operator= (const Dict &rhs) {
		if( this != &rhs ) {
			this->clear();
			for ( auto const& x : rhs.data ) {
				this->data[ x.first ] = x.second;
			}
		}
		return *this;
	}

	/**
	@brief Addition operator, copies the keys and values from the rhs dictionary to this dictionary, adding them to any existing values in this dictionary
	@param rhs: the right-hand-side reference dictionary containing the keys you want to add to this dictionary
	@return returns a reference to this dictionary
	*/
	Dict& operator+ (const Dict &rhs) {
		if( this != &rhs ) {
			for ( auto const& x : rhs.data ) {
				this->data[ x.first ] = x.second;
			}
		}
		return *this;
	}

	/**
	@brief Subtraction operator, removes any key-value pairs from this dictionary that match the keys in the rhs dictionary to this dictionary
	@param rhs: the right-hand-side reference dictionary containing the keys you want to remove from this dictionary
	@return returns a reference to this dictionary
	*/
	Dict& operator- (const Dict &rhs) {
		if( this != &rhs ) {
			for ( auto const& x : rhs.data ) {
				if( this->exists( x.first ) ) {
					this->remove( x.first );
				}
			}
		}
		return *this;
	}

	/**
	@brief Access operator, assigns a reference of the value that matches the key, returning true if the value exists, false otherwise.
	@note the assignment is via a reference, therefore changing the value outside of the function will also change the value in the dictionary
	@param key: the key for the value you want to access
	@param value: a reference to a data instance that will store the value associated with the key
	@return returns true if the key and type match, false otherwise
	*/
	template<typename T>
	    bool operator() (const std::string &key, T &value) {
		try {
			value = get<T>(key);
			return true;
		}
		catch(...) {
			return false;
		}
	}

	/**
	@brief adds a new key-value pair to the dictionary, if the overwrite option is true then any matching key already in the dictionary will be overwritten, if
	the overwrite option is false then the function will return false indicating it could not add the new pair as a dictionary element already exists for this key.
	@param key: the key for the value you want to add
	@param value: the value that will be added to the dictionary and associated with the key
	@param overwrite: if true, then key-value pairs will always be added even if a dictionary element already exists with the same key, if false then the key must be unique
	@return returns true if the key-value pair was added, false otherwise
	*/
	template<typename T>
	    bool add( const std::string &key, const T &value, const bool overwrite = true ) {
		if( overwrite ) {
			data[ key ].set( value );
			return true;
		}
		dict::iterator it;
		it = data.find( key );
		if( it == data.end() ) {
			data[ key ].set( value );
			return true;
		}
		return false;
	}

	/**
	@brief removes a new key-value pair from the dictionary
	@param key: the key for the dictionary element you want to remove
	@return returns true if the key was found and the dictionary element was removed, false otherwise
	*/
	bool remove( const std::string &key ) {
		dict::iterator it;
		it = data.find( key );
		if( it != data.end() ) {
			data.erase( it );
			return true;
		}
		return false;
	}

	/**
	@brief returns true if the key exists in the current dictionary
	@param key: the key you want to search for in the dictionary
	@return returns true if the key exists in the current dictionary, false otherwise
	*/
	bool exists( const std::string &key ) {
		dict::iterator it;
		it = data.find( key );
		if( it != data.end() ) {
			return true;
		}
		return false;
	}

	/**
	@brief clears all elements in the dictionary
	@return returns true if the dictionary was cleared, false otherwise
	*/
	bool clear( ) {
		data.clear();
		return true;
	}

	/**
	@brief returns true if the dictionary is empty, false otherwise
	@return returns true if the dictionary is empty, false otherwise
	*/
	bool empty( ) const {
		return data.empty();
	}

	/**
	@brief returns the size of the dictionary, which is the number of elements stored in the dictionary
	@return returns the size of the dictionary, which is the number of elements stored in the dictionary
	*/
	UINT getSize() const {
		return (UINT)data.size();
	}

	/**
	@brief returns a Vector of strings containing the keys in the dictionary, the vector will be empty if the dictionary is empty
	@return returns a Vector of the keys in the dictionary
	*/
	Vector< std::string > getKeys() const {
		Vector< std::string > keys;
		keys.reserve( data.size() );
		for ( auto const& x : data ) {
			keys.push_back( x.first );
		}
		return keys;
	}

	/**
	@brief returns a reference to the value that matches the key from the dictionary, the function will throw an error if the key does not exist
	@param key: the key you want to search for in the dictionary
	@return returns a reference to the value for the corresponding key in the dictionary
	*/
	template<typename T>
	    T& get( const std::string &key ) {
		dict::iterator it;
		it = data.find( key );
		if( it == data.end() ) {
			throw "Unknown key: " + key;
		}
		return it->second.get< T >();
	}

	/**
	@brief sets the value for the key in the dictionary, if the key does not exist then the function will return false
	@param key: the key you want to search for in the dictionary
	@param value: the new value for the corresponding key element
	@return returns true if the dictionary element was updated, false otherwise
	*/
	template<typename T>
	    bool set( const std::string &key, const T &value ) {
		dict::iterator it;
		it = data.find( key );
		if( it == data.end() ) {
			return false;
		}
		return it->second.set( value );
	}

protected:
	dict data; ///<The internal buffer used to store elements in the dictionary
};

GRT_END_NAMESPACE

#endif //GRT_HEADER_GUARD
