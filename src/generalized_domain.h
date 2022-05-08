#ifndef __GENERALIZED_DOMAIN_H__
#define __GENERALIZED_DOMAIN_H__

#include "common.h"
#include "state_descriptor.h"
#include "action.h"
#include "domain.h"
#include "condition.h"
#include "instruction.h"

class GeneralizedDomain{
public:
	// Instructions can be created only once but we keep the logic
	// of which actions can be programmed in each line here, so the
	// engine does not have to reason about it
	GeneralizedDomain( Domain *d, int program_lines ){
		StateDescriptor *sd = d->getStateDescriptor();
	
		_instructions_line.resize( program_lines );
		
		// Planning Actions
		auto actions = d->getActions();
		// [2] Symmetry Breaking - Only End can be programmed in last line
		for( int i = 0; i + 1 < program_lines; i++ ){
			for(auto & action : actions){
				_instructions_line[ i ].push_back( new PlanningAction( action ) );
			}
		}
		
		// Extending planning actions with pointer instructions
        auto v_pointers = sd->getPointerNames();
        // [2] Symmetry Breaking - extra actions cannot be programmed in the last two lines
        for( int line = 0; line + 2 < program_lines; line++ ){
            for( const auto& p : v_pointers ){
                // zf and cf flags are indirectly modified by other pointer math updates
                if( p == "zf" or p == "cf" )
                    continue;

                //1. inc(pointer). Increase a pointer
                auto *inc_act = new Action("inc("+p+")");

                Condition *inc_cond = new Add( sd,
                        new Variable( p, "pointer", sd->getTypeID(p) ),
                        new Variable( "1", "constant", 1 ) );
                inc_act->addCondition( inc_cond );

                Operation *add_assign_op = new AddAssign( sd,
                        new Variable( p, "pointer", sd->getTypeID( p ) ),
                        new Variable( "1", "constant", 1 ) );
                inc_act->addOperation( add_assign_op );

                _extra_actions.push_back( inc_act );
                Instruction *inc_ins = new PlanningAction( inc_act );
                _instructions_line[ line ].push_back( inc_ins );

                //2. dec(pointer). Decrease a pointer
                auto *dec_act = new Action("dec("+p+")");

                Condition *dec_cond = new Subtract( sd,
                        new Variable( p, "pointer", sd->getTypeID( p ) ),
                        new Variable( "1", "constant", 1 ) );
                dec_act->addCondition( dec_cond );

                Operation * sub_assign_op = new SubtractAssign( sd,
                        new Variable( p, "pointer", sd->getTypeID( p ) ),
                        new Variable( "1", "constant", 1 ) );
                dec_act->addOperation( sub_assign_op );

                _extra_actions.push_back( dec_act );
                Instruction *dec_ins = new PlanningAction( dec_act );
                _instructions_line[ line ].push_back( dec_ins );

                /* 3. & 4. are not needed with conditional effects
                //3. test-max(pointer). Test if a pointer is pointing the last typed element
                Action *test_act = new Action( "test-max("+p+")" );

                // NO CONDITION

                Operation *test_op = new TestMax( sd, new Variable(p, "pointer", sd->getTypeID(p)),
                                                  new Variable("","constant",0));
                test_act->addOperation( test_op );

                _extra_actions.push_back( test_act );
                Instruction *test_ins = new PlanningAction( test_act );
                _instructions_line[ line ].push_back( test_ins );

                //4. cmp(pointer,0). Test if a pointer is pointing the first typed element
                Action *cmp_act = new Action("test-min("+p+")");
                // NO CONDS
                Operation *cmp_op = new Compare( sd,
                                                 new Variable( p, "pointer", sd->getTypeID( p ) ),
                                                 new Variable( "0", "constant", 0 ) );
                cmp_act->addOperation( cmp_op );

                _extra_actions.push_back( cmp_act );
                Instruction *cmp_ins = new PlanningAction( cmp_act );
                _instructions_line[ line ].push_back( cmp_ins );
                */

                // New 3. clear a pointer (reset the pointer to the first position)
                auto *clear_act = new Action( "clear("+p+")" );
                // NO CONDITION
                Operation *clear_op = new Assign(sd, new Variable(p,"pointer",sd->getTypeID(p)),
                                                 new Variable("","constant",0));
                clear_act->addOperation(clear_op);
                _extra_actions.push_back( clear_act );
                Instruction *clear_ins = new PlanningAction( clear_act );
                _instructions_line[ line ].push_back( clear_ins );

                for( const auto& p2 : v_pointers ){
                    if( p2 == "zf" or p2 == "cf" or p == p2 or
                        sd->getPointerVarType(p) != sd->getPointerVarType(p2) )
                        continue;

                    //5. set(pointer1,pointer2). Assign pointer2 to pointer1
                    auto *set_act = new Action("set(" + p + "," + p2 + ")");
                    //NO CONDS
                    Operation *assign_op = new Assign(sd, new Variable( p, "pointer", sd->getTypeID( p )),
                                                      new Variable( p2, "pointer", sd->getTypeID( p2 ) ) );
                    set_act->addOperation(assign_op);

                    _extra_actions.push_back(set_act);
                    Instruction *set_ins = new PlanningAction(set_act);
                    _instructions_line[line].push_back(set_ins);

                    // [3] Symmetry breaking - allow to compare only in one direction
                    if( sd->getTypeID(p) >= sd->getTypeID(p2) )
                        continue;

                    if( CMP_INSTRUCTION_ALLOWED ) {
                        //6. cmp(pointer1,pointer2). CMP instruction between two pointers
                        auto *cmp_act = new Action("cmp(" + p + "," + p2 + ")");
                        // NO CONDS
                        Operation *cmp_op = new Compare(sd,
                                                        new Variable(p, "pointer", sd->getTypeID(p)),
                                                        new Variable(p2, "pointer", sd->getTypeID(p2)));
                        cmp_act->addOperation(cmp_op);

                        _extra_actions.push_back(cmp_act);
                        Instruction *cmp_ins = new PlanningAction(cmp_act);
                        _instructions_line[line].push_back(cmp_ins);
                    }
                }
            }
        }

        // Extending planning actions with register instructions
        auto v_pred_types = sd->getPredicateTypes();
        for( const auto& pred_type : v_pred_types ){
            auto pred_id = sd->getTypeID( pred_type );
            auto param_var_ids = sd->getPredicateVarTypeIDs( pred_type );
            vector< string > param_var_names( param_var_ids.size() );
            for( unsigned i = 0; i < param_var_ids.size(); i++ )
                param_var_names[i] = sd->getVarName( param_var_ids[i] );
            vector< string > pointer_params;
            vector<vector<int> > param_pointer_ids = sd->computeGroundedParamIDs( param_var_names, pointer_params );

            // Add an empty param when the predicate has no parameters
            if( param_var_ids.empty() ) {
                pointer_params.emplace_back("");
                param_pointer_ids.emplace_back( );
            }


            // [3] Symmetry breaking - allow to compare only in one direction
            for (int line = 0; line + 1 < program_lines; line++) {
                for( unsigned i = 0; i < pointer_params.size(); i++ ) {
                    auto pred1_name = pred_type + "(" + pointer_params[i] + ")";

                    /* // Not needed with conditional effects
                    //7. test-max(register). Test if a register is MAX_VAL
                    Action *test_act = new Action( "test-max("+pred1_name+")" );

                    // NO CONDITION

                    Operation *test_op = new TestMax( sd, new Variable(pred1_name, "predicate", pred_id, param_pointer_ids[i] ),
                                                      new Variable("","constant",0));
                    test_act->addOperation( test_op );

                    _extra_actions.push_back( test_act );
                    Instruction *test_ins = new PlanningAction( test_act );
                    _instructions_line[ line ].push_back( test_ins );
                    */
                    // 8. & 9. only relevant for numerical domains
                    if( TESTS_ALLOWED ) {
                        //8. cmp(register,0). Test if a register is 0
                        auto *cmp_act = new Action("test-min(" + pred1_name + ")");
                        // NO CONDS
                        Operation *cmp_op = new Compare(sd, new Variable(pred1_name, "predicate", pred_id,
                                                                         param_pointer_ids[i]),
                                                        new Variable("0", "constant", 0));
                        cmp_act->addOperation(cmp_op);

                        _extra_actions.push_back(cmp_act);
                        Instruction *cmp_ins = new PlanningAction(cmp_act);
                        _instructions_line[line].push_back(cmp_ins);
                    }

                    if( CMP_INSTRUCTION_ALLOWED ) {
                        for (unsigned j = i + 1; j < pointer_params.size(); j++) {
                            auto pred2_name = pred_type + "(" + pointer_params[j] + ")";
                            //9. cmp(register1,register2). CMP instruction between two registers
                            auto *cmp_act = new Action("cmp(" + pred1_name + "," + pred2_name + ")");
                            // NO CONDS
                            Operation *cmp_op = new Compare(sd,
                                                            new Variable(pred1_name, "predicate", pred_id,
                                                                         param_pointer_ids[i]),
                                                            new Variable(pred2_name, "predicate", pred_id,
                                                                         param_pointer_ids[j]));
                            cmp_act->addOperation(cmp_op);

                            _extra_actions.push_back(cmp_act);
                            Instruction *cmp_ins = new PlanningAction(cmp_act);
                            _instructions_line[line].push_back(cmp_ins);
                        }
                    }
                }
            }
        }
		
		// Conditional Gotos
		// Set 4 different conditions ZF, !ZF, CF, !CF
        int num_of_conditions = 4;
        if( CARRY_FLAG ) num_of_conditions = 2;
		_conds.resize( num_of_conditions );
		_conds[ 0 ] = new Equals( sd,
		        new Variable( "zf", "pointer", sd->getTypeID( "zf" ) ),
		        new Variable( "1", "constant", 1 ) );
		_conds[ 1 ] = new Equals( sd,
		        new Variable( "zf", "pointer", sd->getTypeID( "zf" ) ),
		        new Variable( "0", "constant", 0 ) );
        if( CARRY_FLAG ) {
            _conds[2] = new Equals(sd,
                                   new Variable("cf", "pointer", sd->getTypeID("cf")),
                                   new Variable("1", "constant", 1));
            _conds[3] = new Equals(sd,
                                   new Variable("cf", "pointer", sd->getTypeID("cf")),
                                   new Variable("0", "constant", 0));
            // Only End can be programmed in last line
            for( int from = 0; from + 1 < program_lines; from++ ){
                for( int to = 0; to < program_lines; to++ ){
                    // A goto cannot jump over itself or next line
                    if( from == to || from+1 == to )
                        continue;
                    for( int k1 = 0; k1 < 2; k1++ ){
                        for( int k2 = 2; k2 < 4; k2++ ){
                            Instruction *ins = new Goto( to );
                            ins->addCond( _conds[ k1 ] );
                            ins->addCond( _conds[ k2 ] );
                            _instructions_line[ from ].push_back( ins );
                        }
                    }
                }
            }
        }
        else{ // ONLY ZERO FLAG
            for( int from = 0; from + 1 < program_lines; from++ ) {
                for (int to = 0; to < program_lines; to++) {
                    // A goto cannot jump over itself or next line
                    if (from == to || from + 1 == to)
                        continue;
                    if( not IFS_ALLOWED and from < to ) continue;
                    for( int cond_id = 0; cond_id < 2; cond_id++ ){
                        Instruction *ins = new Goto( to );
                        ins->addCond( _conds[ cond_id ] );
                        _instructions_line[ from ].push_back( ins );
                    }
                }
            }
        }
		
		// End
		for( int i = 0; i < program_lines; i++ ){
			_instructions_line[ i ].push_back( new End() );
		}

		//cout << toString();
	}
	
	~GeneralizedDomain(){
		for(auto & instructions : _instructions_line){
            for( auto instruction : instructions ){
                delete instruction;
			}
		}
		
		for(auto & cond : _conds){
            delete cond;
		}
		
		for(auto & extra_action : _extra_actions){
			delete extra_action;
		}
	}
	
	Instruction* searchInstruction( int program_line, const string &name ){
		if( program_line >= int( _instructions_line.size() ) )
			return nullptr;
		for(auto & instruction : _instructions_line[ program_line ]){
			if(instruction->toString(false ) == (name + "\n") )
				return instruction;
		}
		return nullptr;
	}
	
	vector< vector< Instruction* > > getInstructions(){
		return _instructions_line;
	}
	
	vector< Instruction* > getInstructions( int program_line ){
		if( program_line >= int( _instructions_line.size() ) )
			return {};
		return _instructions_line[ program_line ];
	}
	
	Instruction* getInstruction( int program_line, int id ){
		return _instructions_line[ program_line ][ id ];
	}

	Instruction* getInstruction( int program_line, const string &name ){
	    assert( program_line < (int)_instructions_line.size() );
	    for(auto & instruction : _instructions_line[ program_line ]){
	        if(instruction->toString(false).find(name ) != string::npos ){
	            return instruction;
	        }
	    }
	    return nullptr;
	}
	
	string toString(){
		string ret = "[GENERALIZED DOMAIN]:\n";
		for( int i = 0; i < int( _instructions_line.size() ); i++ ){
			ret += "[LINE " + to_string( i ) +"]:\n";
			for(auto & instruction : _instructions_line[ i ]){
				//ret += "ID: " + to_string(instruction) + " ";
				ret += instruction->toString(true) + "\n";
			}
		}
		return ret;
	}
	
	
private:
	vector< vector< Instruction* > > _instructions_line;
	vector< Condition* > _conds;
	vector< Action* > _extra_actions;
};

#endif
