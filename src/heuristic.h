#ifndef __HEURISTIC_H__
#define __HEURISTIC_H__

#include "common.h"
#include "instruction.h"
#include "program.h"
#include "program_state.h"
#include "generalized_planning_problem.h"
#include "landmark_graph.h"

class Heuristic{
public:
	explicit Heuristic( const string &name = "heuristic" ) : _name( name ){
	}
	
	virtual ~Heuristic()= default;
	
	virtual string getName() const{
		return _name;
	}
	
	// Program-based or Performance-based heuristic
	virtual int compute( Program *p, GeneralizedPlanningProblem *gpp  ){
		return 0;
	}
private:
	string _name;
};

class H1: public Heuristic{
public:
	H1() : Heuristic( "h1" ){}
	
	~H1() override= default;
	
	// Compute number of gotos of a program
	int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
		int res = 0;
		vector< Instruction* > instructions = p->getInstructions();
		for(auto & instruction : instructions){
			Goto *g = dynamic_cast<Goto*>( instruction );
			if( g ) res++;
		}
		return res;
		//return int(instructions.size())-res;
	}
};

class H2 : public Heuristic{
public:
	H2() : Heuristic( "h2" ){}
	
	~H2() override= default;
	
	// DFS like => Compute the number of empty instructions
	int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
		int res = 0;
		vector< Instruction* > instructions = p->getInstructions();
		for(auto & instruction : instructions){
			if( instruction == nullptr )
				res++;
		}
		return res;		
	}
};

class H3 : public Heuristic{
public:
    H3() : Heuristic( "h3" ){}

    ~H3() override= default;

    static bool isValidInstruction( Instruction *ins ){
        auto *pa = dynamic_cast<PlanningAction*>( ins );
        Goto *g = dynamic_cast<Goto*>( ins );
        End *end = dynamic_cast<End*>( ins );
        return ((pa != nullptr) or (g != nullptr) or (end != nullptr));
    }

    // Compute the number of repeated actions
    int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
        int res = 0;
        vector< Instruction* > instructions = p->getInstructions();
        vector< bool > affected_lines( instructions.size(), false );
        for( int line = 0; line < int( instructions.size() ); line++ ){
            if( affected_lines[ line ] ){
                res++;
                continue;
            }
            if( !isValidInstruction( instructions[ line ] ) ) continue;
            for( int next_lines = line+1; next_lines < int( instructions.size() ); next_lines++ ){
                if( affected_lines[ next_lines ] ) continue;
                if( !isValidInstruction( instructions[ next_lines ] ) ) continue;
                if( instructions[ line ]->toString( false ) == instructions[ next_lines ]->toString( false ) ){
                    affected_lines[ next_lines ] = true;
                }
            }
        }

        return res;
    }
};

class H4 : public Heuristic{
public:
	H4() : Heuristic( "h4" ){}
	
	~H4() override= default;
	
	// DFS like => Run the program over instances and return the last empty line found
	int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
		int res = p->getNumInstructions(), pc_max = -1;
		auto vps = p->run( gpp, true, gpp->getProgressive() );
        // Error when running the program over active instances
        if( vps.empty() ){
            return INF;
        }
		for( int i = 0; i < int( vps.size() ); i++ ){
            if( gpp->getProgressive() and not gpp->isInstanceActive( i ) ) continue;
			int line = vps[ i ]->getLine();
			if( p->getInstruction( line ) == nullptr )
				pc_max = max( pc_max, line );
			delete vps[i];
		}
		if( pc_max == -1 ) return 0; // OLD: return res;
		return res - pc_max;
	}
};

class H5 : public Heuristic{
public:
	H5() : Heuristic( "h5" ){}

    ~H5() override= default;
	
	// Greedy Search => F = H. Computing squared distance to goal after each execution.
	// This assumes state variables are in the same domain and goals as equalities
	int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
	    StateDescriptor *sd = gpp->getDomain()->getStateDescriptor();
		long long res = 0;
		auto vps = p->run( gpp, true, gpp->getProgressive() );
        // Error when running the program over active instances
        if( vps.empty() ){
            return INF;
        }
		for( int i = 0; i < int( vps.size() ); i++ ){
            if( gpp->getProgressive() and not gpp->isInstanceActive( i ) ) continue;
			State *s = vps[ i ]->getState();
			Instance *ins = gpp->getInstance( i );
			vector< Condition* > goal = ins->getGoalCondition();
			long long ins_res = 0;
			for(auto & g : goal){
				long long lhs = g->getLHS(sd, s );
				long long rhs = g->getRHS(sd, s );
				ins_res += ((lhs-rhs)*(lhs-rhs));
			}
			res += ins_res;
			delete vps[i];
		}
		return int(res);
	}
};

class H6 : public Heuristic{
public:
    H6() : Heuristic( "h6" ){}

    ~H6() override= default;

    // Cost function => F = G. Computing number of execution steps.
    // This assumes state variables are in the same domain and goals as equalities
    int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
        auto vps = p->run( gpp, true, gpp->getProgressive() );
        // Error when running the program over active instances
        if( vps.empty() ){
            return INF;
        }
        int num_of_steps = p->getNumOfSteps();

        for(auto & ps : vps) {
            delete ps;
        }

        return num_of_steps;
    }
};

// OLD H1
class H7 : public Heuristic{
public:
    H7() : Heuristic( "h7" ){}

    ~H7() override= default;

    // Compute nested gotos (there could be multiple strategies)
    int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
        int res = 0;
        auto instructions = p->getInstructions();
        vector< int > affected_lines( instructions.size(), 0 );
        for( int line = 0; line < int( instructions.size() ); line++ ){
            Goto *g = dynamic_cast<Goto*>( instructions[ line ] );
            if( g ){
                int aux_line = g->getDestinationLine();
                int dir = (aux_line > line ? -1:1);
                while( aux_line != line ){
                    affected_lines[ aux_line ]++;
                    aux_line += dir;
                }
                affected_lines[ line ]++;
            }
        }
        res = *max_element(affected_lines.begin(), affected_lines.end());
        return res;
    }
};

class H8 : public Heuristic{
public:
    H8() : Heuristic( "h8" ){}

    ~H8() override= default;

    // A* like => F = G + Sum of squared differences to goal
    int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
        auto *sd = gpp->getDomain()->getStateDescriptor();
        auto vps = p->run( gpp, true, gpp->getProgressive() );
        // Error when running the program over active instances
        if( vps.empty() ){
            return INF;
        }
        long long res = p->getNumOfSteps(); // G function

        for( int i = 0; i < int( vps.size() ); i++ ){
            if( gpp->getProgressive() and not gpp->isInstanceActive( i ) ) continue;
            auto *s = vps[ i ]->getState();
            auto *ins = gpp->getInstance( i );
            auto goal = ins->getGoalCondition();
            for( auto g : goal ){
                long long lhs = g->getLHS( sd, s );
                long long rhs = g->getRHS( sd, s );
                res += ((lhs-rhs)*(lhs-rhs)); // H function
            }
            delete vps[i];
        }
        return (int)res;
    }
};

class H9 : public Heuristic{
public:
    H9() : Heuristic( "h9" ){}

    ~H9() override= default;

    // WA* like => F = G + W * Sum of squared differences to goal
    int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
        auto *sd = gpp->getDomain()->getStateDescriptor();
        auto vps = p->run( gpp, true, gpp->getProgressive() );
        // Error when running the program over active instances
        if( vps.empty() ){
            return INF;
        }
        long long res = p->getNumOfSteps(); // G function
        long long W = 5; // default value
        for( int i = 0; i < int( vps.size() ); i++ ){
            if( gpp->getProgressive() and not gpp->isInstanceActive( i ) ) continue;
            auto *s = vps[ i ]->getState();
            auto *ins = gpp->getInstance( i );
            auto goal = ins->getGoalCondition();
            for( auto g : goal ){
                long long lhs = g->getLHS( sd, s );
                long long rhs = g->getRHS( sd, s );
                res += W * ((lhs-rhs)*(lhs-rhs)); // W * H function
            }
            delete vps[i];
        }
        return (int)res;
    }
};

class LandmarksHeuristic : public Heuristic{
public:
    LandmarksHeuristic( ) : Heuristic("landmarks" ){ }

    ~LandmarksHeuristic() override = default;

    // Counts the number of unvisited landmarks
    int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
        return p->getLMCount();
    }
};

class NormalizedLandmarksHeuristic : public Heuristic{
public:
    NormalizedLandmarksHeuristic( ) : Heuristic("normlandmarks" ){ }

    ~NormalizedLandmarksHeuristic() override = default;

    // Counts the number of unvisited landmarks
    int compute( Program *p, GeneralizedPlanningProblem *gpp ) override{
        //p->getLMCount();
        int res = 0;
        int num_of_instances = gpp->getNumInstances();
        for( int id = 0; id < num_of_instances; id++ ){
            if( gpp->isInstanceActive(id) ){
                double tlm = (double)p->getTotalLandmarks(id);
                double ulm = (double)p->getUnachieveLandmarks(id);
                res += int( (ulm / tlm)*10000.0 );
            }
        }
        return res;
    }
};


#endif
