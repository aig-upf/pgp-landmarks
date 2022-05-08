#ifndef __DOMAIN_H__
#define __DOMAIN_H__

#include "common.h"
#include "state_descriptor.h"
#include "action.h"

class Domain{
public:
	explicit Domain( const string &name = "" ){
		_name = name;
		_sd = nullptr;
	}
		
	~Domain(){
        _action_name_to_idx.clear();
		for(auto & action : _actions){
			delete action;
		}
        delete _sd;
	}

	void setName( const string &name = "" ){
		_name = name;
	}

	void setStateDescriptor( StateDescriptor *sd ){
		_sd = sd;
	}
	
	void addAction( Action *a ){
		_actions.push_back( a );
        _action_name_to_idx[ a->getName() ] = int( _actions.size() ) - 1;
	}
	
	StateDescriptor* getStateDescriptor(){
		return _sd;
	}
	
	vector< Action* > getActions() const{
		return _actions;
	}
	
	Action* getAction( int id ){
		return _actions[ id ];
	}

	Action* getAction( const string &name = "" ){
	    auto it = _action_name_to_idx.find( name );
	    if( it == _action_name_to_idx.end() )
	        return nullptr;
	    return getAction( it->second );
	}
	
	int getNumActions() const{
		return (int)_actions.size();
	}
	
	string getName() const{
		return _name;
	}
	
	string toString() const{
		string ret = "[DOMAIN]: " + _name + "\n";
		if( _sd )
			ret += "\n" + _sd->toString();
		for(auto action : _actions){
			ret += "\n" + action->toString(true);
		}
		return ret;
	}
	
private:
	string _name;
	StateDescriptor *_sd;
	vector< Action* > _actions;
	map< string, int > _action_name_to_idx;
}; 

#endif
