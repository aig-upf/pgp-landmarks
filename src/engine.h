#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "common.h"
#include "node.h"
#include "heuristic.h"
#include "generalized_planning_problem.h"
#include "generalized_domain.h"

struct CmpNodes{
    bool operator()(const Node* lhs, const Node* rhs) const{
        return (lhs->f() > rhs->f()) or ( lhs->f() == rhs->f() and lhs->getID() > rhs->getID() );
    }
};

class Engine{
public:
	Engine( int program_lines, GeneralizedDomain *gd, GeneralizedPlanningProblem *gpp ){
		_evaluated_states = 0;
		_evaluated_nodes = 0;
		_expanded_nodes = 0;
        _deadends = 0;
		_program_lines = program_lines;
		_gd = gd;
		_gpp = gpp;
	}
	
	virtual ~Engine(){
		for( auto h : _heuristics ){
			delete h;
		}
		delete _gd;
		delete _gpp;
        for( auto lg : _landmark_graphs ){
            delete lg;
        }
	}
	
	virtual bool isEmpty() const = 0;
	
	// Add a node to searching data structure
	virtual void addNode( Node *node ) = 0;
	
	// Select the next Node and update the data structure
	virtual Node* selectNode() = 0;
	
	// Check if goal conditions hold in the current Node
	virtual bool isGoal( Node *node, bool incremental_eval ) = 0;
	
	// Expand the current Node, adding all non-visited childs
	virtual vector< Node* > expandNode( Node *node, bool incremental_eval ) = 0;
	
	// accumulated cost
	virtual int g( Node *node ) = 0;
	
	// heuristic functions
	virtual vector<int> h( Node *node ) = 0;
	
	// evaluation functions
	virtual vector<int> f( Node *node ) = 0;
	
	// Solve the problem starting from init Node
	virtual Node* solve(bool progressive) = 0;
	
	// Add a new heuristic to the engine
	void addHeuristic( Heuristic *new_h ){ 
		cout << "[INFO] Heuristic " << new_h->getName() << " added." <<  endl;
		_heuristics.push_back( new_h );
	}
	
	int getExpanded() const{
		return _expanded_nodes;
	}
	
	int getEvaluated() const{
		return _evaluated_nodes;
	}

    int getEvaluatedStates() const{
        return _evaluated_states;
    }

    int getDeadends() const{
        return _deadends;
    }

	virtual void setLandmarks( const vector< LandmarkGraph*> & landmarks ){
        _landmark_graphs = landmarks;
	}
	
	
protected:
	int _evaluated_states;
	int _evaluated_nodes;
	int _expanded_nodes;
    int _deadends;
	int _program_lines;
	vector< Heuristic* > _heuristics;
	GeneralizedPlanningProblem *_gpp;
	GeneralizedDomain *_gd;
	vector< LandmarkGraph* > _landmark_graphs;
};

class BFS : public Engine{
public:
	BFS(int program_lines, GeneralizedDomain *gd, GeneralizedPlanningProblem *gpp) : Engine( program_lines, gd, gpp ){
	}
	
	~BFS() override{
		while( !isEmpty() ){
			Node *n = _open.top();
			_open.pop();
			delete n;
		}
	}
	
	bool isEmpty() const override{
		return _open.empty();
	}
	
	void addNode( Node *node ) override{
		_open.push( node );
	}
	
	Node* selectNode() override{
		return _open.top();
	}
	
	static void freeProgramStates( vector< ProgramState* > &vps ){
		for( auto ps : vps ){
			delete ps;
		}
	}
	
	bool isGoal( Node *node, bool progressive_eval ) override{
		//Program *p = node->generateProgram( _program_lines );
		auto *p = node->getProgram();
		auto vps = p->run(_gpp, true, progressive_eval );
		
		// Some error ocurred during execution
		if( vps.empty() )
			return false;
		
		for( int id = 0; id < int( vps.size() ); id++ ){
		    if(progressive_eval and not _gpp->isInstanceActive(id ) ) continue;
			int line = vps[ id ]->getLine();
			Instruction *ins = p->getInstruction( line );
			if( ins == nullptr ){
				freeProgramStates( vps );
				return false;
			}
			
			End *end = dynamic_cast<End*>( ins );
			if( end ){
				if( not end->isGoalState( _gpp->getInstance( id ), vps[ id ] ) ){
					freeProgramStates( vps );
					return false;
				}
			}
			else{ // The current instruction is not an end instruction
				freeProgramStates( vps );
				return false;
			}
		}
		
		freeProgramStates( vps );
		return true;
	}
	
	vector< Node* > expandNode( Node *node, bool incremental_eval ) override{
		int pc_max = -1; 
		//Program *p = node->generateProgram( _program_lines );
		auto *p = node->getProgram();
		vector< ProgramState* > vps;
        if( IFS_ALLOWED ) {
            // find the next line to program
            vps = p->run(_gpp, true, incremental_eval);

            if (vps.empty()) // error while executing the program
                return {};

            for (int id = 0; id < int(vps.size()); id++) {
                if (incremental_eval and not _gpp->isInstanceActive(id)) continue;
                int line = vps[id]->getLine();
                if (p->getInstruction(line) == nullptr) {
                    pc_max = max(pc_max, line);
                }
            }

            freeProgramStates(vps);
        }
        else{
            auto instructions = p->getInstructions();
            for( int ins_idx = 0; ins_idx < (int)instructions.size(); ins_idx++ ){
                if( instructions[ ins_idx ] == nullptr ){
                    pc_max = ins_idx; break;
                }
            }
        }
		
		// Failure case when next valid line not found
		if( pc_max == -1 or p->getInstruction( pc_max ) != nullptr ){
			return {};
		}
			
		vector< Instruction* > vi = _gd->getInstructions( pc_max );
		
		// [1] Symmetry Breaking - only Gotos after CMP or TEST
		bool only_gotos = false;
		bool gotos_enabled = false;
		if( pc_max > 0 ){
			auto *pa = dynamic_cast<PlanningAction*>( p->getInstruction( pc_max-1 ) );
			if( pa ){
			    auto act_name = pa->getName();
			    size_t pos = act_name.find( '(' );
			    if( pos != string::npos ){
                    auto name = act_name.substr(0, pos );
			        only_gotos = ( name == "cmp" or name == "test-max" or name == "test-min" );
			    }
			    gotos_enabled = (pa->getType() == "math");
			}
		}
		
		vector< Node* > childs;
		int maxi = max( 1, int(_heuristics.size() ) );
		for(auto & instruction : vi){
			// [1] S.B.
			if( only_gotos ){
				Goto *g = dynamic_cast<Goto*>( instruction );
				if( !g ) continue;
			}
			if( !gotos_enabled ){
                Goto *g = dynamic_cast<Goto*>( instruction );
                if( g ) continue;
			}

			Program *p2 = p->copy();
			p2->setInstruction(pc_max, instruction );
            p2->setLandmarkGraphs(_landmark_graphs);

            childs.push_back( new Node(p2,vector<int>(maxi,0)));
		}
		
		return childs;
	}
	
	int g( Node *node ) override{
		return 0;
	}
	
	vector<int> h( Node *node ) override{
		auto *p = node->getProgram();
		auto val_h = vector<int>( _heuristics.size(), 0 );
		for( int i = 0; i < int( _heuristics.size() ); i++ ){
			val_h[ i ] = _heuristics[ i ]->compute( p, _gpp );
		}
		return val_h;
	}
	
	vector<int> f( Node *node ) override{
        _evaluated_states += _gpp->getNumActiveInstances(); // all active instances evaluate state
		return h( node );
	}

	Node* solve(bool progressive) override{
	    auto *root_program = new Program( _program_lines, _gd->getInstruction( _program_lines - 1, 0) );

        root_program->setLandmarkGraphs(_landmark_graphs);

        if( progressive ){
            _gpp->activateInstance( 0 ); // activate the first instance
        }

#ifdef PRINT_LANDMARKS_INFO
        // LOCK DOMAIN
        vector< string > ins_names = {"inc(ptr-object1)", "move(ptr-object2,ptr-object1)","inc(ptr-object1)",
                                      "inc(ptr-object2)", "goto(1,!(( zf = 1 )))", "pickup(ptr-object1)",
                                      "dec(ptr-object1)", "move(ptr-object2,ptr-object1)", "dec(ptr-object2)",
                                      "goto(5,!(( zf = 1 )))", "open(ptr-object1)" };
        // BAKING DOMAIN
        /*vector<string> ins_names = { "putegginpan(ptr-egg1,ptr-pan1)","putflourinpan(ptr-flour1,ptr-pan1)",
                                     "mix(ptr-egg1,ptr-flour1,ptr-pan1)", "putpaninoven(ptr-pan1,ptr-oven1)",
                                     "bakecake(ptr-oven1,ptr-pan1,ptr-cake1)", "removepanfromoven(ptr-pan1,ptr-oven1)",
                                     "cleanpan(ptr-pan1,ptr-soap1)","inc(ptr-egg1)", "inc(ptr-flour1)", "inc(ptr-cake1)",
                                     "inc(ptr-soap1)", "goto(0,!(( zf = 1 )))", "end" };*/
        for( int pl = 0; pl < (int) ins_names.size(); pl++ ) {
            root_program->setInstruction(pl, _gd->getInstruction( pl, ins_names[ pl ] ));
            auto vps = root_program->run(_gpp, true, progressive);
            freeProgramStates(vps);
            cout << root_program->toString(false);
            cout << "[INFO] LM count = " << _heuristics[0]->compute(root_program, _gpp) << endl;
        }
        //return NULL;
#endif

        _evaluated_nodes = 0;
		Node *root = new Node( root_program, vector<int>( _heuristics.size(), INF ), _evaluated_nodes++ );
        _evaluated_states += _gpp->getNumActiveInstances();

		if( isGoal( root, false ) )
			return root;
			
		if( not DEFERRED_EVALUATION ) {
		    auto vps = root_program->run(_gpp,true,progressive);
            freeProgramStates(vps);
            root->setF( f( root ) );
		}

		addNode( root );

		vector< int > best_evaluations( _heuristics.size(), INF );

		while( !isEmpty() ){		
			_expanded_nodes++;
			
			Node *current = selectNode();
			// remove current node from open
			_open.pop();
			if( DEFERRED_EVALUATION ) {
                current->setF( f(current) );
            }
			auto current_evaluations = current->f();

			vector< Node* > childs = expandNode( current, progressive );

            if( current_evaluations < best_evaluations ){
                best_evaluations = current_evaluations;
                cout << "[ENGINE] NODE ID: " << current->getID() << "; EXPANDED: " << _expanded_nodes <<
                "; EVALUATED: " << _evaluated_nodes << "; OPEN QUEUE SIZE: " << _open.size() << endl;
                cout << current->toString() << endl;
            }
            else if( _expanded_nodes % PROGRAM_FREQUENCY == 0 ){
                cout << "[ENGINE] NODE ID: " << current->getID() << "; EXPANDED: " << _expanded_nodes <<
                     "; EVALUATED: " << _evaluated_nodes << "; OPEN QUEUE SIZE: " << _open.size() << endl;
                cout << current->toString() << endl;
            }

			delete current;
			
			for( int i = 0; i < int( childs.size() ); i++ ){
                childs[ i ]->setID( _evaluated_nodes++ );

                auto vps = childs[ i ]->getProgram()->run(_gpp,true, progressive);
                freeProgramStates(vps);
				if( vps.empty() ) {
                    _deadends++;
                    delete childs[ i ];
                    continue;
                }

				if( not DEFERRED_EVALUATION ) {
                    childs[i]->setF(f(childs[i]));
                }
				else{
				    childs[i]->setF( current_evaluations );
				}
				
				if( isGoal( childs[ i ], progressive ) ){
					if( progressive ){
                        cout << "[INFO] Possible solution found at node id " << childs[i]->getID() << " when solving instance ids:";
                        for( int aux_id = 0; aux_id < _gpp->getNumInstances(); aux_id++ )
                            if( _gpp->isInstanceActive(aux_id) ) cout << " #" << aux_id;
                        cout << endl << childs[i]->toString() << endl;
					}
					// if the search is not incremental, or if the program solves all instances finish
					if( not progressive or isGoal( childs[i], false ) ){
                        childs[i]->setF(f(childs[i]));
					    addNode( childs[ i ] );
					    for( int j = i+1; j < int( childs.size() ); j++ )
					        if( childs[ j ] )
					            delete childs[ j ];
					    return childs[ i ];
					}
                    else if( progressive ){
                        // Otherwise, if it is incremental but not a goal for all instances,
                        // activate the instance where the program failed
                        int id = childs[i]->getProgram()->getFailedInstanceID();
                        _gpp->activateInstance( id );
                        childs[i]->setF(f(childs[i]));
                    }
                    // This point is only reachable from progressive method
                    // where not all instances are covered by the current solution, so
                    // the first instance where the generalized planning problem is now
                    // active and the open queue can be reevaluated
                    if( not DEFERRED_EVALUATION and REEVALUATE_OPEN_QUEUE ){
                        cout << "[INFO] Failed on instance id " << childs[i]->getProgram()->getFailedInstanceID()
                             << ". Reevaluating the queue... " << endl;
                        // This snippet is not compatible with DEFERRED_EVALUATION
                        priority_queue< Node*, vector< Node* >, CmpNodes > aux_open;
                        while( !_open.empty() ) {
                            Node *aux_current = _open.top(); _open.pop();
                            auto vps2 = aux_current->getProgram()->run(_gpp,true,progressive);
                            freeProgramStates(vps2 );
                            if( vps2.empty() ){
                                delete aux_current;
                                continue;
                            }
                            aux_current->setF( f(aux_current) );
                            aux_open.push( aux_current );
                        }
                        // Reset the new best evaluations
                        best_evaluations = vector<int>(_heuristics.size(),INF);
                        // Save the reevaluated open queue
                        swap( aux_open, _open );
                        cout << "[INFO] DONE!" << endl;
                        // Delete child and continue
                        delete childs[i];
                        continue;
                    }
				}

                // Standard A* but no chance to repeat programs
                // so nodes are always pushed into priority queue
                addNode(childs[i]);
			}
		}
		
		return nullptr;
	}
	
private:
	priority_queue< Node*, vector< Node* >, CmpNodes > _open;
};

#endif
