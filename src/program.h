#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "program_state.h"
#include "instruction.h"
#include "generalized_planning_problem.h"
#include "landmark_graph.h"

class Program{
public:
	explicit Program( int program_lines = 1, Instruction* end_instruction = nullptr ){
        assert( program_lines >= 1 );
		_instructions = vector<Instruction*>( program_lines, nullptr );
        setInstruction( program_lines - 1, end_instruction );
        _num_of_steps = 0;
        _lm_count = -1;
        _failed_instance_id = -1;
	}
	
	explicit Program( Program* p ){
		vector< Instruction* > vi = p->getInstructions();
		_instructions.resize( vi.size() );
		for( int i = 0; i < int( _instructions.size() ); i++ )
			_instructions[ i ] = vi[ i ];
        _num_of_steps = 0;
        _lm_count = -1;
        _failed_instance_id = -1;
        /*if( _landmark_graphs.size() > 0u ) {
            _num_visited_landmarks = 0;
            setLandmarkGraphs(p->getLandmarks());
            assert( p->getLandmarks().size() > 0u );
            assert( getLandmarks().size() > 0u );
        }*/
	}
	
	~Program()= default;
	
	Program* copy(){
		return new Program( this );
	}
	
	vector< Instruction* > getInstructions() const{
		return _instructions;
	}
	
	void setInstruction( int pl, Instruction* ins ){
	    assert( pl < int( _instructions.size() ) );
		_instructions[ pl ] = ins;
	}
	
	Instruction* getInstruction( int pl ){
	    assert( pl < int( _instructions.size() ) );
		return _instructions[ pl ];
	}
	
	int getNumInstructions() const{
		return int( _instructions.size() );
	}
	
	bool haltingCondition( ProgramState* ps, int &error ){
		int line = ps->getLine();
		// EMPTY line is a halting condition (no transition defined)
		if( _instructions[ line ] == nullptr ){
			return true;
		}

		// Either True or False evaluation of End instructions is a halting condition
		End* ins_end = dynamic_cast< End* > ( _instructions[ line ] );
		if( ins_end ) return true;

        // Conditional effects, actions do not have to be necessarily applicable
		// (OLD) If it's a planning action, it must be applicable
		/*PlanningAction* ins_act = dynamic_cast< PlanningAction* > ( _instructions[ line ] );
		if( ins_act && !ins_act->isApplicable( ps ) ){
            #ifdef VALIDATOR
		    cout << ps->toString() << endl;
		    cout << ins_act->toString() << endl;
            #endif
			error = -2; // ERROR 2: inapplicable instruction
			return true;
		}*/
		
		return false;
	}
	
	bool checkGoal( ProgramState *ps, Instance *ins, int &error ){
		int line = ps->getLine();
		if( _instructions[ line ] == nullptr ) return false;
		End *end = dynamic_cast<End*>( _instructions[ line ] );
		if( end and not end->isGoalState( ins, ps ) ){
            #ifdef DEBUG
			//cout << ps->toString() << endl;
            #endif
			error = -1; // ERROR 1: Incorrect program
			return false;
		}
		return false;
	}

	static bool checkDeadEnd(StateDescriptor *sd, ProgramState *ps, LandmarkGraph *lm, int &error ){
	    unique_ptr<RelaxedState> rs = make_unique<RelaxedState>( sd );
	    auto reg = ps->getState()->getTypedRegisters();
	    for( int pred_idx = 0; pred_idx < (int)reg.size(); pred_idx++ ) {
	        for( const auto& m : reg[ pred_idx ] )
                rs->addFact(pred_idx, m.first, m.second );
        }
	    bool res = lm->isDeadEnd( rs );
	    if( res ) error = -4;

	    return res;
	}
	
	vector< ProgramState* > run( GeneralizedPlanningProblem *gpp, bool infinite_detection = true,
                                 bool progressive_eval = false ){
        _num_of_steps = 0;

		int num_instances = gpp->getNumInstances();
		StateDescriptor *sd = (gpp->getDomain())->getStateDescriptor();
		auto pred_names = sd->getPredicateTypes();

		// One program state per instance
		vector< ProgramState* > pss( num_instances ) ;
		for( int i = 0; i < num_instances; i++ )
			pss[ i ] = new ProgramState();


        #ifdef VALIDATOR
        time_t start_instance, end_instance;
        time( &start_instance );
        #endif
        int errors = 0;

        bool use_landmarks = (!_landmark_graph.empty() );
        if( use_landmarks ){
            _lm_count = 0;
            _unachieved_landmarks.resize(num_instances, 0 );
        }

        for( int id = 0; id < num_instances; id++ ){
            if(progressive_eval and not gpp->isInstanceActive(id) ) continue;
			// Initialize local initial state
			Instance *ins = gpp->getInstance( id );			
			ProgramState *ps = pss[ id ];
			State *s = ( ins->getInitialState() )->copy();
			int line = ps->getLine();
			ps->setState( s );

			// Process the landmarks of the initial state
			set< int > accepted;
			set< Landmark* > landmarks;
			RelaxedGraph *rg = nullptr;
			vector< int > goal_landmarks_ids;
			if( use_landmarks ) {
			    landmarks = _landmark_graph[ id ]->getLandmarks();
			    rg = _landmark_graph[ id ]->getRelaxedGraph();
			    for( auto lm_id : _landmark_graph[ id ]->getInitialLandmarksIDs() ){
                    accepted.insert( lm_id );
			    }
			    goal_landmarks_ids = _landmark_graph[ id ]->getGoalLandmarksIDs();
            }

			// For detecting infinite loops (it can be enhanced with a hash or bigint identifier)
			set< vector<int> > visited;
			int error = 0;

			while( !haltingCondition( ps, error ) ){
				if( infinite_detection ){
					// Checking infinite loop (only for backward loops)
					Goto *g = dynamic_cast<Goto*>( _instructions[ line ] );
					if( g and g->getDestinationLine() < line ){
						// Infinite loop detected
#ifdef MAX_STEPS
                        if( MAX_STEPS*num_instances < _num_of_steps ){
                            error = -3; break; // ERROR 3: Infinite program
                        }
#else
                        vector<int> state_id = ps->asVector();
                        if( visited.find( state_id ) != visited.end() ){
                            error = -3; // ERROR 3: Infinite program
                            break;
                        }
                        visited.insert( state_id );
#endif
					}
				}
				
				// Retrieving program line
				line = ps->getLine();
                // cout << ps->toString(sd) << endl;
				// Applying current instruction						
				int res = _instructions[ line ]->apply( ps );
				_num_of_steps++;

				// Mathematical planning actions update zero and carry flags
				auto *act = dynamic_cast< PlanningAction* >( _instructions[ line ] );
				if( act && act->getType() == "math" ){
					s->setPointer(sd, "zf", ( res == 0 ?1:0) ); // zero-flag
                    if( CARRY_FLAG )
					    s->setPointer(sd, "cf", ( res > 0 ? 1:0) ); // carry-flag
				}

				if( use_landmarks ){
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, false );
                    processReachedLandmarks( sd, rg, s, id, landmarks, accepted, true );
				}
			}

			if( use_landmarks ){
			    set< int > not_accepted;
			    // Add all non-accepted landmarks
			    for( auto l : landmarks ){
			        auto lm_id = l->getID();
			        if( accepted.find( lm_id ) == accepted.end() )
			            not_accepted.insert( lm_id );
			    }

			    // Add required preconditions
			    set< int > required_again;
			    for( auto lm_id : accepted ){
			        auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
			        if( not landmark_holds ){
			            // Required again if it is greedy necessary for a non-accepted landmark
			            bool is_req_again = false;
			            auto gn_landmarks = _landmark_graph[ id ]->getGreedyNecessaryLandmarkOrdering( lm_id );
			            for( auto gn_id : gn_landmarks ){
			                if( not_accepted.find( gn_id ) != not_accepted.end() ){
			                    is_req_again = true; break;
			                }
			            }
			            if( is_req_again ){
			                required_again.insert( lm_id );
			            }
			        }
			    }

			    // Add required again goals
			    for( auto lm_id : goal_landmarks_ids ){
			        // If the goal has not been accepted yet continue
			        if( accepted.find( lm_id ) == accepted.end() ) continue;
			        auto l = _landmark_graph[ id ]->getLandmark( lm_id );
			        if( not l->holds( rg, s ) )
			            required_again.insert( lm_id );
			    }
                // Alternative required again
                /*set< int > potential_required_again;
                for( auto lm_id : accepted ){
                    auto landmark = _landmark_graph[ id ]->getLandmark( lm_id );
                    bool landmark_holds;
                    if( landmark->isPointerLandmark() ) landmark_holds = landmark->holds(sd,s);
                    else landmark_holds = landmark->holds(rg, s);
                    if( not landmark_holds ){
                        potential_required_again.insert( lm_id );
                    }
                }

                // Add required again goals
                for( auto lm_id : goal_landmarks_ids ){
                    // If the goal has not been accepted yet continue
                    if( accepted.find( lm_id ) == accepted.end() ) continue;
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    if( not l->holds( rg, s ) )
                        potential_required_again.insert( lm_id );
                }

                set< int > required_again;
                int num_of_required_again = 0;
                do {
                    num_of_required_again = required_again.size();
                    for (auto lm_id: potential_required_again) {
                        // Required again if it is greedy necessary for a non-accepted landmark
                        bool is_req_again = false;
                        auto gn_landmarks = _landmark_graph[id]->getGreedyNecessaryLandmarkOrdering(lm_id);
                        for (auto gn_id: gn_landmarks) {
                            if (not_accepted.find(gn_id) != not_accepted.end()) {
                                is_req_again = true;
                                break;
                            } else if (required_again.find(gn_id) != required_again.end()) {
                                is_req_again = true;
                                break;
                            }
                        }
                        if (is_req_again) {
                            required_again.insert(lm_id);
                        }
                    }
                }while( required_again.size() > num_of_required_again );*/

                //[ToDo] Check deadend here, required again must hold in the final relaxed state
#ifdef PRINT_LANDMARKS_INFO
                cout << "[INFO] LANDMARK GRAPH " << id << endl;
                cout << "[INFO] ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] NOT ACCEPTED LANDMARKS:" << endl;
                for( auto lm_id : not_accepted ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
                cout << "[INFO] REQUIRED AGAIN:" << endl;
                for( auto lm_id : required_again ){
                    auto l = _landmark_graph[ id ]->getLandmark( lm_id );
                    cout << l->toString(sd,rg);
                }
#endif
			    _lm_count += (int)not_accepted.size() + (int)required_again.size();

                // NEW FOR NORMALIZATION
                _unachieved_landmarks[id] = (int)not_accepted.size() + (int)required_again.size();
			}

            if( error == 0 )
			    checkGoal( ps, ins, error );
			if( DEADEND_DETECTION and use_landmarks and error == 0) {
                checkDeadEnd(sd, ps, _landmark_graph[id], error );
			}
			// [ENHANCEMENT] Continue execution in validation for coverage metric
			#ifdef VALIDATOR
            time(&end_instance);
			if( error < 0 ){
			    errors++;
				cout << "INSTANCE #" << (id+1) << " ERROR " << error << "... :( [" << difftime(end_instance,start_instance) << "]" << endl;
				for( int i = 0; i < int( pss.size() ); i++ ){
				    cout << pss[i]->toString( sd ) << endl;
				}
			}
			else
				cout << "INSTANCE #" << (id+1) << " SOLVED! [" << difftime(end_instance,start_instance) <<"]" << endl;
			start_instance = end_instance;
            #else
            if( error < 0 ){
                _failed_instance_id = id;
                errors++;
                break;
                /*for( int i = 0; i < int( pss.size() ); i++ ){
                    //cout << pss[i]->toString( sd ) << endl;
                    if( pss[i] ) delete pss[i];
                }
                return vector<ProgramState*>();*/
            }
			#endif
		}

        if( errors ){
            for(auto & ps : pss){
                delete ps;
            }
            return {};
        }
		
		return pss;
	}

    void processReachedLandmarks( StateDescriptor *sd, RelaxedGraph *rg, State *s, int landmark_graph_id,
                                  set<Landmark*> &landmarks, set<int> &accepted, bool pointer_landmarks = false ){
        vector<int> reached;
        for( auto l :  landmarks ){
            if( pointer_landmarks and not l->isPointerLandmark() ) continue;
            if( not pointer_landmarks and l->isPointerLandmark() ) continue;
            int lm_id = l->getID();
            // If the landmark was already accepted continue;
            if( accepted.find( lm_id ) != accepted.end() ) continue;
            bool landmark_holds;
            if( l->isPointerLandmark() ) landmark_holds = l->holds(sd,s);
            else landmark_holds = l->holds(rg, s);
            // New landmark reached
            if( landmark_holds ){
                bool is_reached = true;
                // check if gn landmarks hold
                auto gn_landmarks = _landmark_graph[ landmark_graph_id ]->getInverseGreedyNecessaryLandmarkOrdering( lm_id );
                for( auto gn_l_id : gn_landmarks ){
                    // check if a gn condition has not been accepted yet
                    if( accepted.find( gn_l_id ) == accepted.end() ){
                        is_reached = false; break;
                    }
                }
                auto nat_landmarks = _landmark_graph[ landmark_graph_id ]->getInverseNaturalLandmarkOrdering( lm_id );
                for( auto nat_l_id : nat_landmarks ){
                    // check if a nat condition has not been accepted yet
                    if( accepted.find( nat_l_id ) == accepted.end() ){
                        is_reached = false; break;
                    }
                }
                // if all gn and nat landmarks have been accepted, include the new landmark as reached
                if( is_reached )
                    reached.push_back( lm_id );
            }
        }
        // Union of reached and accepted
        std::copy( reached.begin(), reached.end(), inserter( accepted, accepted.end() ) );
    }

	int getNumOfSteps() const{
	    return _num_of_steps;
	}

	void setLandmarkGraphs(const vector< LandmarkGraph* > &landmark_graph ){
        _landmark_graph = landmark_graph;
	}

    int getLMCount() const{
	    return _lm_count;
    }

    int getFailedInstanceID() const{
        return _failed_instance_id;
    }

    int getFirstEmptyLine() const {
        int line = 0;
        for( auto ins : _instructions ){
            if( ins == nullptr )
                return line;
            line++;
        }
        return -1;
    }

    int getTotalLandmarks(int id) {
        assert(id < (int)_landmark_graph.size() );
        return _landmark_graph[id]->getNumLandmarks();
    }

    int getUnachieveLandmarks(int id) {
        assert(id < (int)_unachieved_landmarks.size());
        return _unachieved_landmarks[id];
    }
	
	string toString( bool full_info = true ) const{
		string ret;
		if( full_info ){
			ret = "[PROGRAM]:\n";
			for( int i = 0; i < int( _instructions.size() ); i++ ){
				ret += "LINE " + to_string( i ) + ": ";
				if( _instructions[ i ] != nullptr )
					ret += _instructions[ i ]->toString(true);
				else ret += "[INSTRUCTION] EMPTY\n";
			}
		}
		else{
			for( int i = 0; i < int( _instructions.size() ); i++ ){
				ret += to_string( i ) + ". ";
				if( _instructions[ i ] != nullptr )
					ret += _instructions[ i ]->toString( full_info );
				else ret += "empty\n";
			}
		}
		return ret;
	}
	
private:
    vector< LandmarkGraph* > _landmark_graph;
	int _lm_count;
	vector< Instruction* > _instructions;
	int _num_of_steps;
    int _failed_instance_id;
    vector<int> _unachieved_landmarks;
};

#endif
