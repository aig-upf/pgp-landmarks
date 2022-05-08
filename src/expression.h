#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "variable.h"
#include "state_descriptor.h"

class Expression{
public:
	explicit Expression( const string &name = "", Variable *lhs = nullptr, Variable *rhs = nullptr ){
		_name = name;
		_lhs = lhs;
		_rhs = rhs;
	}
	
	virtual ~Expression(){
		delete _lhs;
		delete _rhs;
	}

	virtual int update( State *s ) const{ return 0; }

	virtual string getName() const{
		return _name;
	}

	virtual string toString( bool titled ) const{
		string ret;
		if( titled ){
			ret += "[EXPRESSION]: ";
		}
		ret += "( " + _lhs->toString() + " " + _name + " " + _rhs->toString() + " )\n";
		return ret;
	}

	// Set the left-hand side variable of the expression
	virtual void setLHS( Variable *v ){
		_lhs = v;
	}
	
	// Set the right-hand side of the variable of the expression
	virtual void setRHS( Variable *v ){
		_rhs = v;
	}
	
	// Set the content of left-hand side variable
	virtual void setLeftVarNum( StateDescriptor *sd, State *s, int number ) const{
		int id = _lhs->getID();
		string vtype = _lhs->getVType();

		// Content of pointers cannot be modified, only pointers and grounded predicates
		assert( (vtype == "pointer" and not _lhs->accessMemory()) or vtype == "predicate" );

		if( vtype == "pointer" ){
		    s->setPointer( sd, id, number );
		}
		else if( vtype == "predicate" ){
            auto pred_type = sd->getPredicateName( id );
		    vector< int > params = _lhs->getParameterIDs();
		    vector< int > param_obj_idx ( params.size() );
		    for( unsigned i = 0; i < params.size(); i++ ){
                param_obj_idx[i] = s->getPointer(sd, params[i] ); // Assign the pointer index
		    }
		    s->addRegister( sd, pred_type, param_obj_idx, number );
		}
	}
	
	// Value of the variable in current state or constant number
    virtual int getVarNum( StateDescriptor *sd, const State* s, Variable *v ) const{
		int id = v->getID();
		string vtype = v->getVType();

		assert( vtype == "pointer" or vtype == "predicate" or vtype == "constant" );

		if( vtype == "pointer" ){
		    // ToDo alternative implementation of access to objects,
		    //  which would require the Instance as parameter
			//if( v->accessMemory() ){
			//	return s->getRegister( s->getPointer( id ) );
			//}
			return s->getPointer( sd, id );
		}	

		else if( vtype == "predicate" ){
            auto pred_type = sd->getPredicateName( id );
            vector< int > params = v->getParameterIDs();
            vector< int > param_obj_idx ( params.size() );
            for( unsigned i = 0; i < params.size(); i++ ){
                if( params[i] < 0 ){ // the param is a constant object
                    param_obj_idx[i] = -(params[i]+1); // constants are 0-indexed
                }
                else // the param is a pointer
                    param_obj_idx[i] = s->getPointer(sd, params[i] );
            }
		    return s->getRegister( sd, pred_type, param_obj_idx );
		}

		// Otherwise is a constant
		return id;
    }
        
    // Return the left-hand side value
    virtual int getLHS( StateDescriptor *sd, const State* s ) const{
		return getVarNum( sd, s, _lhs );
	}
        
	// Return the right-hand side value     
	virtual int getRHS( StateDescriptor *sd, const State* s ) const{
		return getVarNum( sd, s, _rhs );
	}

	virtual string getLeftName( StateDescriptor *sd ) const{
        int id = _lhs->getID();
        string vtype = _lhs->getVType();

        if( vtype != "predicate" ) return "";

        return sd->getPredicateName( id );
	}

	virtual vector<int> getLeftObjects( ) const{
        //int id = _lhs->getID();
        string vtype = _lhs->getVType();

        assert( vtype == "predicate"); // The var type must be a predicate

        vector< int > params = _lhs->getParameterIDs();
        vector< int > param_obj_idx ( params.size() );
        for( unsigned i = 0; i < params.size(); i++ ){
            assert( params[i] < 0 ); // the param must be a constant object
            param_obj_idx[i] = -(params[i]+1); // constants are 0-indexed
        }
        return param_obj_idx;
	}

    virtual vector<string> getLeftPointerNames( StateDescriptor *sd ) const{
        //int id = _lhs->getID();
        string vtype = _lhs->getVType();

        assert( vtype == "predicate"); // The var type must be a predicate

        vector< int > params = _lhs->getParameterIDs();
        vector< string > pointer_names ( params.size() );
        for( unsigned i = 0; i < params.size(); i++ ){
            assert( params[i] >= 0 ); // the params must be pointers
            pointer_names[ i ] = sd->getPointerName( params[i] );
        }
        return pointer_names;
    }

protected:
	string _name;
	Variable *_lhs;
	Variable *_rhs;
};

#endif
