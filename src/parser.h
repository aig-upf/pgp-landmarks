#ifndef __PARSER_H__
#define __PARSER_H__

#include "common.h"
#include "variable.h"
#include "state_descriptor.h"
#include "state.h"
#include "condition.h"
#include "operation.h"
#include "action.h"
#include "domain.h"
#include "instance.h"

class Parser{
public:
	Parser()= default;
	
	~Parser()= default;

	static string errorMessage( const string &expected_word = "", const string &readed_word = "" ) {
		return "Expected ''" + expected_word + "'' but ''" + readed_word + "'' read.";
	}

	static bool isDigit( const string &s ) {
		return (s.find_first_not_of("0123456789") == string::npos );
	}

    static vector< string > split( const string &s, char delimiter = ',' ){
        vector< string > splitted_s;
        size_t l = 0, n = 0;
        while( (n = s.find(delimiter, l)) != string::npos ){
            splitted_s.push_back( s.substr( l, n-l ) );
            l = n+1;
        }
        if( !s.empty() )
            splitted_s.push_back( s.substr(l) );
        return splitted_s;
    }

    static Variable* readGroundedVar( StateDescriptor *sd, const string &name, Instance *ins ){
        string var_name = name, var_type;
        int var_id;

        bool var_constant = isDigit( name );
        vector< int > param_ids;
        size_t pos;
        if( var_constant ) {
            var_type = "constant";
            var_id = atoi( name.c_str() );
        }
        else if( (pos = name.find('(')) != string::npos ) {
            var_type = "predicate";
            var_name = name.substr( 0, pos );
            var_id = sd->getTypeID( var_name );
            var_name += "(";
            auto param_names = split( name.substr( pos+1, (name.length() - 1 ) - (pos+1) ) );
            for (unsigned i = 0; i < param_names.size(); i++) {
                auto obj_address = ins->getObjectAddress(param_names[i]);
                param_ids.push_back(-(obj_address + 1)); // const objects have ids < 0;
                //var_name += param_name;
                var_name += (i ? "," : "") + to_string(obj_address);
            }
            var_name += ")";
        }
        else {
            var_type = "pointer";
            var_id = sd->getTypeID( name );
        }

        return (new Variable(var_name,var_type,var_id,param_ids));
	}

	static Variable* groundingVar( StateDescriptor *sd, const string &name, const map< string, int > &par_name_to_idx,
                       const vector< string > &grounded_params ){
		string var_name = name, var_type;
        int var_id;

        bool var_constant = isDigit( name );
        vector< int > param_ids;
        size_t pos;
		if( var_constant ) {
            var_type = "constant";
            var_id = atoi( name.c_str() );
        }
		else if( (pos = name.find('(')) != string::npos ) {
		    var_type = "predicate";
		    var_name = name.substr( 0, pos );
		    var_id = sd->getTypeID( var_name );
		    var_name += "(";
            auto param_names = split( name.substr( pos+1, (name.length() - 1 ) - (pos+1) ) );
            for( unsigned i = 0; i < param_names.size(); i++){
                //cout << name << " => " << var_name << ": " << param_names[i] << endl;
                auto it = par_name_to_idx.find(param_names[i]);
                assert(it != par_name_to_idx.end());
                auto idx = it->second;
                assert(idx < (int)grounded_params.size());
                auto param_id = sd->getTypeID(grounded_params[idx]); // get pointer ID
                param_ids.push_back(param_id);
                var_name += (i?",":"") + sd->getPointerName( param_id );
            }
            var_name += ")";
		}
		else {
		    var_type = "pointer";
		    var_id = sd->getTypeID( name );
		}

		return (new Variable( var_name, var_type, var_id, param_ids  ));
	}

	static Condition* readCondition( StateDescriptor *sd, const string &name, Variable *lhs, Variable *rhs ){
		if( ">=" == name ) return new GreaterEqual( sd, lhs, rhs );
		else if( "<=" == name ) return new LesserEqual( sd, lhs, rhs );
		else if( ">" == name ) return new Greater( sd, lhs, rhs );
		else if( "<" == name ) return new Lesser( sd, lhs, rhs );
		else if( "=" == name ) return new Equals( sd, lhs, rhs );
		else if( "+" == name ) return new Add( sd, lhs, rhs );
		else if( "-" == name ) return new Subtract( sd, lhs, rhs );
		else if( "!=" == name ) return new Different( sd, lhs, rhs );
		
		return nullptr;
	}

	static Operation* readOperation( StateDescriptor *sd, const string &name, Variable *lhs, Variable *rhs ){
		if( "+=" == name ) return new AddAssign( sd, lhs, rhs );
		else if( "-=" == name ) return new SubtractAssign( sd, lhs, rhs );
		else if( "=" == name ) return new Assign( sd, lhs, rhs );
		else if( "-" == name ) return new Compare( sd, lhs, rhs );
		
		return nullptr;
	}

	static bool parseAction( ifstream &ifs, Domain *dom ){
	    StateDescriptor *sd = dom->getStateDescriptor();
        string full_name, act_name, act_type, word;
        ifs >> full_name;  // action name and parameters must be without spaces

        // 1. Parse action local parameters (name, type and index)
        // e.g. drop(b:ball,r:room,g:gripper)
        vector< string > param_type_list;
        map< string, string > act_param_type;
        map< string, int > act_param_idx;
        size_t p = full_name.find( '(' ) ;
        if( p == string::npos ) {
            cerr << "No parenthesis found in the action name: " << full_name << endl;
            return false;
        }
        act_name = full_name.substr(0, p );
        string str_params = full_name.substr( p+1 , (full_name.length()-1)-(p+1) );
        auto raw_params = split( str_params );
        for( unsigned i = 0; i < raw_params.size(); i++ ){
            auto param_info = split( raw_params[i], ':' );
            act_param_type[ param_info[0] ] = param_info[1];
            act_param_idx[ param_info[0] ] = (int)i;
            param_type_list.push_back( param_info[1] );
        }

        // 2. Parse action type, e.g. "math" or "memory"
        ifs >> word;
        if( word != "TYPE:" ){
            cerr << errorMessage( "TYPE:", word );
            return false;
        }
        ifs >> act_type; // action type must be either "math" or "memory"
        if( act_type != "math" and act_type != "memory" ){
            cerr << "Action type must be either math or memory, and not " << act_type << endl;
            return false;
        }

        // 3. Parse preconditions, e.g. ( at-robby(r) = 1 )
        vector< vector< string > > conditions;
        ifs >> word;
        if( word != "PRECONDITIONS:" ){
            cerr << errorMessage( "PRECONDITIONS:", word );
            return false;
        }
        // Parsing new condition expressions
        ifs >> word;
        while( !ifs.eof() && ( word == "(" ) ){
            //string var_lhs, cond_op, var_rhs;
            vector< string > cond( 3, "" );
            for( unsigned i = 0; i < 3; i++ )
                ifs >> cond[i];

            conditions.push_back( cond );

            ifs >> word;
            if( word != ")" ){
                cerr << errorMessage( ")", word );
                return false;
            }

            ifs >> word;
        }

        // 4. Parse effects
        vector< vector< string > > effects;
        if( word != "EFFECTS:" ){
            cerr << errorMessage( "EFFECTS:", word );
            return false;
        }

        // Parsing new effect expressions
        ifs >> word;
        while( !ifs.eof() && word == "(" ){
            if( word != "(" ){
                cerr << errorMessage( "(", word );
                return false;
            }

            //string var_lhs, op, var_rhs;
            vector< string > eff( 3, "" );
            for( unsigned i = 0; i < 3; i++ )
                ifs >> eff[i];

            effects.push_back( eff );

            ifs >> word;
            if( word != ")" ){
                cerr << errorMessage( "(", word );
                return false;
            }
            ifs >> word;
        }

        // GROUNDING ACTIONS
        // Compute grounded params and add grounded actions to the domain
        vector< string > grounded_params;
        sd->computeGroundedParams(param_type_list,grounded_params);

        for( const auto& gp : grounded_params ){
            vector< string > pointer_names = split( gp );
            string grounded_name = act_name + "(" + gp + ")";

            // Only create grounded actions
            dom->addAction( new Action( grounded_name, act_type ) );
            Action *act = dom->getAction( grounded_name );

            for( auto c : conditions ){
                Variable *lhs = groundingVar( sd, c[0], act_param_idx, pointer_names );
                Variable *rhs = groundingVar( sd, c[2], act_param_idx, pointer_names );
                Condition *cond = readCondition( sd, c[1], lhs, rhs );
                act->addCondition( cond );
            }

            for( auto eff : effects ){
                Variable *lhs = groundingVar( sd, eff[0], act_param_idx, pointer_names );
                Variable *rhs = groundingVar( sd, eff[2], act_param_idx, pointer_names );
                Operation *oper = readOperation( sd, eff[1], lhs, rhs );
                act->addOperation( oper );
            }
        }

        return true;
	}
	
	// In charge of parsing and creating a domain
	static bool parseDomain( Domain *dom, const string &file_name = "" ){
		ifstream ifs( file_name.c_str() );

		string word;

		// DOMAIN NAME
		ifs >> word;
		
		// Transform the name to lowercase
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return std::tolower(c); });
		dom->setName( word );
		
		// STATE DESCRIPTOR
		if( ifs.eof() ){
			cerr << errorMessage( "STATE", "END OF FILE" ) << endl;
			return false;
		}
		
		ifs >> word;
		
		if( word != "STATE" ){
			cerr << errorMessage( "STATE", word ) << endl;
			return false;
		}
		
		if( ifs.eof() ) {
			cerr << errorMessage( "DESCRIPTOR:", "END OF FILE" ) << endl;
			return false;
		}
		
		ifs >> word;
		
		if( word != "DESCRIPTOR:" ){
			cerr << errorMessage( "DESCRIPTOR:", word ) << endl;
			return false;
		}
		
		dom->setStateDescriptor( new StateDescriptor() );
		
		StateDescriptor *sd = dom->getStateDescriptor();
		
		if( ifs.eof() ){
			cerr << errorMessage( "DESCRIPTOR:", "END OF FILE" ) << endl;
			return false;
		}
		
		ifs >> word;

		// Parsing STATE DESCRIPTOR
		while( !ifs.eof() && word != "ACTION:" ){
			size_t splitter = word.find(':');
			if( splitter == string::npos ) {
                cerr << "Name or type not defined in: " << word << endl;
                return false;
            }
			string v_name = word.substr(0, splitter );
			string v_type = word.substr( splitter+1 );
			if( v_type == "var_type" ){ // adding a new var type
                sd->addVar( v_name );
			}
			else if( v_type == "pred_type" ){ // adding a new predicate type
			    size_t pred_splitter = v_name.find('(' );
			    if( pred_splitter == string::npos or v_name[ (int)v_name.length() - 1 ] != ')'){
			        cerr << "Predicate: " << v_name << " is ill-defined" << endl;
			        return false;
			    }
			    vector< string > param_names;
			    string param_vars = v_name.substr( pred_splitter+1, v_name.length()-1-(pred_splitter+1) );
			    size_t next_param = 0, last_param = 0;
			    while( (next_param = param_vars.find(',',last_param)) != string::npos ){
			        param_names.push_back( param_vars.substr(last_param,next_param-last_param) );
                    last_param = next_param+1;
			    }
			    if( param_vars.size() > size_t(0) )
			        param_names.push_back( param_vars.substr( last_param ) );
			    sd->addPredicate( v_name.substr(0, pred_splitter ), param_names );
			}
			else{ // otherwise, it must be a pointer
                sd->addPointer( v_type, v_name );
			}

			ifs >> word;
		} 
		
		// Parsing ACTIONS
		while( !ifs.eof() && word == "ACTION:" ){
		    if( !parseAction( ifs, dom ) ) {
                cerr << "Error while parsing the action" << endl;
                return false;
            }
		}
		
		ifs.close();
		
		return true;
	}
	
	static bool parseInstance( Domain *dom, Instance *ins, const string &file_name = "", int instance_id = 0 ){
	    ins->setInstanceID( instance_id );
        StateDescriptor *sd = dom->getStateDescriptor();

		ifstream ifs( file_name.c_str() );
		
		// Instance name
		string word;
		ifs >> word;
		ins->setName( word );

		// 1. Parse objects
        ifs >> word;
        if( word != "OBJECTS:" ){
            cerr << errorMessage( "OBJECTS:", word ) << endl;
            return false;
        }
        ifs >> word;
		while( !ifs.eof() && word != "INIT:" ){
            auto obj_info = split( word, ':' );
            ins->addObject(sd, obj_info[0], obj_info[1] );
            ifs >> word;
		}

		// 2. Parse initial state
        if( word != "INIT:" ){
            cerr << errorMessage( "INIT:", word ) << endl;
            return false;
        }

        ins->setInitialState( new State( sd ) );

        State *init = ins->getInitialState();

        ifs >> word;
        while( !ifs.eof() && word == "(" ){
            ifs >> word; // read grounded predicate
            size_t pos = word.find( '(' );

            if( pos != string::npos ) { // it is a predicate
                string pred_type = word.substr(0, pos);
                // Get object addresses
                auto obj_names = split(word.substr(pos + 1, word.length() - 1 - (pos + 1)));
                vector<int> obj_idxs( obj_names.size() );
                for (int oname_idx = 0; oname_idx < (int)obj_names.size(); oname_idx++ ) {
                    obj_idxs[oname_idx] = ins->getObjectAddress(obj_names[oname_idx] );
                }

                // Currenty, the initial states are only assignments
                ifs >> word;
                assert(word == "=");

                int value;
                ifs >> value;
                init->addRegister(sd, pred_type, obj_idxs, value);
            }
            else{ // otherwise, it must be a pointer
                string pointer_name = word;
                assert( sd->isPointer( pointer_name ) );
                ifs >> word;
                assert( word == "=" );
                int value;
                ifs >> value;
                init->setPointer(sd, pointer_name, value );
            }

            ifs >> word;
            if( word != ")" ){
                cerr << errorMessage( "(", word );
                return false;
            }
            ifs >> word;
        }

		// 3. Parse goal condition
        if( word != "GOAL:" ){
            cerr << errorMessage( "GOAL:", word ) << endl;
            return false;
        }

        ifs >> word;

        while( !ifs.eof() && word == "(" ){
            string lhs_name, cond_op, rhs_name;

            ifs >> lhs_name >> cond_op >> rhs_name;

            Variable *lhs = readGroundedVar(sd, lhs_name, ins );
            Variable *rhs = readGroundedVar(sd, rhs_name, ins );
            Condition *cond = readCondition( sd, cond_op, lhs, rhs );

            ins->addCondition( cond );

            ifs >> word;
            if( word != ")" ){
                cerr << errorMessage( "(", word );
                return false;
            }
            ifs >> word;
        }

        ifs.close();

        return true;
	}
	
};

#endif
