#ifndef __OPERATION_H__
#define __OPERATION_H__

#include "common.h"
#include "state.h"
#include "expression.h"

class Operation : public Expression{
public:
	// Non-parameterized actions
	explicit Operation( StateDescriptor *sd, const string &name = "",
                        Variable *lhs = nullptr, Variable *rhs = nullptr ) : Expression( name, lhs, rhs ) {
	    _sd = sd;
	}

	virtual ~Operation()= default;
	
	int update( State *s ) const override{
		int effect = getEffect( s );
		setEffect( s, effect ); 
		return effect; 
	}
	
	virtual int getEffect( State *s ) const{ return 0; }
	
	virtual void setEffect( State *s, int val ) const{
		if( val >= 0 )
			setLeftVarNum(_sd, s, val);
	}

	string toString( bool titled = true ) const override {
		string ret = "";
		if( titled ){
			ret += "[OPERATION]: ";
		}
		ret += "( " + _lhs->toString() + " " + _name + " " + _rhs->toString() + " )\n";
		return ret;
	}

protected:
    StateDescriptor *_sd;
};

class AddAssign : public Operation{
public:
	AddAssign( StateDescriptor *sd, Variable *lhs = 0, Variable *rhs = 0 ) : Operation( sd, "+=", lhs, rhs ) {}
	
	virtual ~AddAssign(){}
	
	/*virtual int update( State *s ) const{
		int effect = getEffect( s );
		setEffect( s, effect );
		return effect;
	}*/
	
	virtual int getEffect( State *s ) const{
		int lhs = getLHS( _sd, s );
		int rhs = getRHS( _sd, s );
		return lhs + rhs;
	}

}; 

class SubtractAssign : public Operation{
public:
	SubtractAssign( StateDescriptor *sd, Variable *lhs = 0, Variable *rhs = 0 ) : Operation( sd, "-=", lhs, rhs ) {}
	
	virtual ~SubtractAssign(){}
	
	virtual int getEffect( State *s ) const{
		int lhs = getLHS( _sd, s );
		int rhs = getRHS( _sd, s );
		return lhs - rhs;
	}

}; 

class Compare : public Operation{
public:
	Compare( StateDescriptor *sd, Variable *lhs = 0, Variable *rhs = 0 ) : Operation( sd, "-", lhs, rhs ) {}
	
	virtual ~Compare(){}
	
	virtual int getEffect( State *s ) const{
		int lhs = getLHS( _sd, s );
		int rhs = getRHS( _sd, s );
		assert( lhs >= 0 and rhs >= 0);
		return lhs - rhs;
	}
	
	virtual void setEffect( State *s, int val ) const{
		// no updates for cmp
	}
}; 

class Assign : public Operation{
public:
	Assign( StateDescriptor *sd, Variable *lhs = 0, Variable *rhs = 0 ) : Operation( sd, "=", lhs, rhs ) {}
	
	virtual ~Assign(){}
	
	virtual int getEffect( State *s ) const{
		//int lhs = getLHS( _sd, s );
		int rhs = getRHS( _sd, s );
		return rhs;
	}

};

// ToDo test this class
class TestMax : public Operation{
public:
    TestMax( StateDescriptor *sd, Variable *lhs = 0, Variable *rhs = 0 ) : Operation( sd, "?", lhs, rhs ){}

    virtual ~TestMax(){}

    virtual  int getEffect( State *s ) const{
        int lhs = getLHS( _sd, s );
        auto bound = _sd->getBound( _lhs->getID(), s->getInstanceID() );
        return bound - (lhs+1);
    }

    virtual void setEffect( State *s, int val ) const{
        // no updates for test
    }
};

#endif
